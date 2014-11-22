/****************************************************************************
 *   Copyright (C) 2012-2014 by Savoir-Faire Linux                          *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Lesser General Public             *
 *   License as published by the Free Software Foundation; either           *
 *   version 2.1 of the License, or (at your option) any later version.     *
 *                                                                          *
 *   This library is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#include "videorenderer.h"

#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtCore/QThread>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <errno.h>


#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 0
#endif

#include <QtCore/QTimer>
#include "videomodel.h"
#include "videoresolution.h"

///Shared memory object
struct SHMHeader{
   sem_t notification;
   sem_t mutex;

   unsigned m_BufferGen;
   int m_BufferSize;
   /* The header will be aligned on 16-byte boundaries */
   char padding[8];

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-pedantic"
   char m_Data[];
#pragma GCC diagnostic pop
};

class VideoRendererPrivate : public QObject
{
   Q_OBJECT
public:
   VideoRendererPrivate(VideoRenderer* parent);

   //Attributes
   QString           m_ShmPath    ;
   int               fd           ;
   SHMHeader*        m_pShmArea   ;
   signed int        m_ShmAreaLen ;
   uint              m_BufferGen  ;
   bool              m_isRendering;
   QTimer*           m_pTimer     ;
   QByteArray        m_Frame[2]   ;
   bool              m_FrameIdx   ;
   QSize             m_pSize      ;
   QMutex*           m_pMutex     ;
   QMutex*           m_pSSMutex   ;
   QString           m_Id         ;
   int               m_fpsC       ;
   int               m_Fps        ;
   QTime             m_CurrentTime;

   //Constants
   static const int TIMEOUT_SEC = 1; // 1 second

   //Helpers
   timespec createTimeout();
   bool     shmLock      ();
   void     shmUnlock    ();
   bool     renderToBitmap();

private:
   VideoRenderer* q_ptr;

private Q_SLOTS:
   void timedEvents();
};

VideoRendererPrivate::VideoRendererPrivate(VideoRenderer* parent) : QObject(parent), q_ptr(parent),
   fd(-1),m_fpsC(0),m_Fps(0),
   m_pShmArea((SHMHeader*)MAP_FAILED), m_ShmAreaLen(0), m_BufferGen(0),
   m_isRendering(false),m_pTimer(nullptr),m_pMutex(new QMutex()),
   m_FrameIdx(false),m_pSSMutex(new QMutex())
{
}

///Constructor
VideoRenderer::VideoRenderer(const QString& id, const QString& shmPath, const QSize& res): QObject(nullptr), d_ptr(new VideoRendererPrivate(this))
{
   d_ptr->m_ShmPath = shmPath;
   d_ptr->m_Id      = id;
   d_ptr->m_pSize   = res;
   setObjectName("VideoRenderer:"+id);
}

///Destructor
VideoRenderer::~VideoRenderer()
{
   stopShm();
   //delete m_pShmArea;
}

///Get the data from shared memory and transform it into a QByteArray
bool VideoRendererPrivate::renderToBitmap()
{
#ifdef Q_OS_LINUX
   if (!m_isRendering) {
      return false;
   }

   if (!shmLock()) {
      return false;
   }

   if(!VideoModel::instance()->startStopMutex()->tryLock())
      return false;

   // wait for a new buffer
   while (m_BufferGen == m_pShmArea->m_BufferGen) {
      shmUnlock();

      int err = sem_trywait(&m_pShmArea->notification);
      // Useful for debugging
//       switch (errno ) {
//          case EINTR:
//             qDebug() << "Unlock failed: Interrupted function call (POSIX.1); see signal(7)";
//             ok = false;
//             return QByteArray();
//             break;
//          case EINVAL:
//             qDebug() << "Unlock failed: Invalid argument (POSIX.1)";
//             ok = false;
//             return QByteArray();
//             break;
//          case EAGAIN:
//             qDebug() << "Unlock failed: Resource temporarily unavailable (may be the same value as EWOULDBLOCK) (POSIX.1)";
//             ok = false;
//             return QByteArray();
//             break;
//          case ETIMEDOUT:
//             qDebug() << "Unlock failed: Connection timed out (POSIX.1)";
//             ok = false;
//             return QByteArray();
//             break;
//       }
      if ((err < 0) || (!shmLock())) {
         VideoModel::instance()->startStopMutex()->unlock();
         return false;
      }
      usleep((1/60.0)*100);
   }

   if (!q_ptr->resizeShm()) {
      qDebug() << "Could not resize shared memory";
      VideoModel::instance()->startStopMutex()->unlock();
      return false;
   }

   bool otherFrame = ! m_FrameIdx;
   if (m_Frame[otherFrame].size() != m_pShmArea->m_BufferSize)
      m_Frame[otherFrame].resize(m_pShmArea->m_BufferSize);
   memcpy(m_Frame[otherFrame].data(),m_pShmArea->m_Data,m_pShmArea->m_BufferSize);
   m_BufferGen = m_pShmArea->m_BufferGen;
   shmUnlock();
   m_FrameIdx = !m_FrameIdx;

   VideoModel::instance()->startStopMutex()->unlock();
   return true;
#else
   return false;
#endif
}

///Connect to the shared memory
bool VideoRenderer::startShm()
{
   if (d_ptr->fd != -1) {
      qDebug() << "fd must be -1";
      return false;
   }

   d_ptr->fd = shm_open(d_ptr->m_ShmPath.toAscii(), O_RDWR, 0);
   if (d_ptr->fd < 0) {
      qDebug() << "could not open shm area " << d_ptr->m_ShmPath << ", shm_open failed:" << strerror(errno);
      return false;
   }
   d_ptr->m_ShmAreaLen = sizeof(SHMHeader);
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   d_ptr->m_pShmArea = (SHMHeader*) mmap(NULL, d_ptr->m_ShmAreaLen, PROT_READ | PROT_WRITE, MAP_SHARED, d_ptr->fd, 0);
   #pragma GCC diagnostic pop
   if (d_ptr->m_pShmArea == MAP_FAILED) {
      qDebug() << "Could not map shm area, mmap failed";
      return false;
   }
   emit started();
   return true;
}

///Disconnect from the shared memory
void VideoRenderer::stopShm()
{
   if (d_ptr->fd >= 0)
      close(d_ptr->fd);
   d_ptr->fd = -1;

   if (d_ptr->m_pShmArea != MAP_FAILED)
      munmap(d_ptr->m_pShmArea, d_ptr->m_ShmAreaLen);
   d_ptr->m_ShmAreaLen = 0;
   d_ptr->m_pShmArea = (SHMHeader*) MAP_FAILED;
}

///Resize the shared memory
bool VideoRenderer::resizeShm()
{
   while (( (unsigned int) sizeof(SHMHeader) + (unsigned int) d_ptr->m_pShmArea->m_BufferSize) > (unsigned int) d_ptr->m_ShmAreaLen) {
      const size_t new_size = sizeof(SHMHeader) + d_ptr->m_pShmArea->m_BufferSize;

      d_ptr->shmUnlock();
      if (munmap(d_ptr->m_pShmArea, d_ptr->m_ShmAreaLen)) {
            qDebug() << "Could not unmap shared area:" << strerror(errno);
            return false;
      }

      #pragma GCC diagnostic push
      #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
      d_ptr->m_pShmArea = (SHMHeader*) mmap(NULL, new_size, PROT_READ | PROT_WRITE, MAP_SHARED, d_ptr->fd, 0);
      #pragma GCC diagnostic pop
      d_ptr->m_ShmAreaLen = new_size;

      if (!d_ptr->m_pShmArea) {
            d_ptr->m_pShmArea = nullptr;
            qDebug() << "Could not remap shared area";
            return false;
      }

      d_ptr->m_ShmAreaLen = new_size;
      if (!d_ptr->shmLock())
            return true;
   }
   return true;
}

///Lock the memory while the copy is being made
bool VideoRendererPrivate::shmLock()
{
#ifdef Q_OS_LINUX
   return sem_trywait(&m_pShmArea->mutex) >= 0;
#else
   return false;
#endif
}

///Remove the lock, allow a new frame to be drawn
void VideoRendererPrivate::shmUnlock()
{
   sem_post(&m_pShmArea->mutex);
}


/*****************************************************************************
 *                                                                           *
 *                                   Slots                                   *
 *                                                                           *
 ****************************************************************************/

///Update the buffer
void VideoRendererPrivate::timedEvents()
{

   bool ok = renderToBitmap();

   if (ok) {

      //Compute the FPS shown to the client
      if (m_CurrentTime.second() != QTime::currentTime().second()) {
         m_Fps = m_fpsC;
         m_fpsC=0;
         m_CurrentTime = QTime::currentTime();
      }
      m_fpsC++;

      emit q_ptr->frameUpdated();
   }
   /*else {
      qDebug() << "Frame dropped";
      usleep(rand()%1000); //Be sure it can come back in sync
   }*/
}

///Start the rendering loop
void VideoRenderer::startRendering()
{
   VideoModel::instance()->startStopMutex()->lock();
   QMutexLocker locker(d_ptr->m_pMutex);
   startShm();
   if (!d_ptr->m_pTimer) {
      d_ptr->m_pTimer = new QTimer(nullptr);

//       m_pTimer->moveToThread(thread());
      connect(d_ptr->m_pTimer,SIGNAL(timeout()),d_ptr.data(),SLOT(timedEvents()));
      d_ptr->m_pTimer->setInterval(30);
   }

   if (!d_ptr->m_pTimer->isActive()) {
      qDebug() << "Is running" << thread()->isRunning();
      d_ptr->m_pTimer->start();
   }
   else
      qDebug() << "Timer already started!";

   d_ptr->m_isRendering = true;
   VideoModel::instance()->startStopMutex()->unlock();
}

///Stop the rendering loop
void VideoRenderer::stopRendering()
{
   VideoModel::instance()->startStopMutex()->lock();
   QMutexLocker locker(d_ptr->m_pMutex);
   d_ptr->m_isRendering = false;
   qDebug() << "Stopping rendering on" << d_ptr->m_Id;
   if (d_ptr->m_pTimer)
      d_ptr->m_pTimer->stop();
   emit stopped();
   stopShm();
   VideoModel::instance()->startStopMutex()->unlock();
}


/*****************************************************************************
 *                                                                           *
 *                                 Getters                                   *
 *                                                                           *
 ****************************************************************************/

///Get the raw bytes directly from the SHM, not recommended, but optimal
const char* VideoRenderer::rawData()
{
   return d_ptr->m_isRendering?d_ptr->m_Frame[d_ptr->m_FrameIdx].data():nullptr;
}

///Is this redenrer active
bool VideoRenderer::isRendering()
{
   return d_ptr->m_isRendering;
}

///Return the current framerate
const QByteArray& VideoRenderer::currentFrame()
{
   static QByteArray empty;
   return d_ptr->m_isRendering?d_ptr->m_Frame[d_ptr->m_FrameIdx]:empty;
}

///Return the current resolution
QSize VideoRenderer::size()
{
   return d_ptr->m_pSize;
}

///Get mutex, in case renderer and views are not in the same thread
QMutex* VideoRenderer::mutex()
{
   return d_ptr->m_pMutex;
}

///Get the current frame rate of this renderer
int VideoRenderer::fps() const
{
   return d_ptr->m_Fps;
}


/*****************************************************************************
 *                                                                           *
 *                                 Setters                                   *
 *                                                                           *
 ****************************************************************************/

void VideoRenderer::setSize(const QSize& size)
{
   d_ptr->m_pSize = size;
}

void VideoRenderer::setShmPath(const QString& path)
{
   d_ptr->m_ShmPath = path;
}

#include <videorenderer.moc>

