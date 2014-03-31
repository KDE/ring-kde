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

///Constructor
VideoRenderer::VideoRenderer(const QString& id, const QString& shmPath, Resolution res): QObject(nullptr),
   m_Width(res.width()), m_Height(res.height()), m_ShmPath(shmPath), fd(-1),
   m_pShmArea((SHMHeader*)MAP_FAILED), m_ShmAreaLen(0), m_BufferGen(0),
   m_isRendering(false),m_pTimer(nullptr),m_Res(res),m_pMutex(new QMutex()),
   m_Id(id),m_FrameIdx(false),m_pSSMutex(new QMutex())
{
   setObjectName("VideoRenderer:"+id);
}

///Destructor
VideoRenderer::~VideoRenderer()
{
   stopShm();
   //delete m_pShmArea;
}

///Get the data from shared memory and transform it into a QByteArray
bool VideoRenderer::renderToBitmap()
{
#ifdef Q_OS_LINUX
   if (!m_isRendering) {
      return false;
   }

   if (!shmLock()) {
      return false;
   }

   // wait for a new buffer
   while (m_BufferGen == m_pShmArea->m_BufferGen) {
      shmUnlock();
      const struct timespec timeout = createTimeout();
      if(!VideoModel::instance()->startStopMutex()->tryLock())
         return false;
      int err = sem_timedwait(&m_pShmArea->notification, &timeout);
      VideoModel::instance()->startStopMutex()->unlock();
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
      if (err < 0) {
         return false;
      }

      if (!shmLock()) {
         return false;
      }
   }

   if (!resizeShm()) {
      qDebug() << "Could not resize shared memory";
      return false;
   }

   bool otherFrame = ! m_FrameIdx;
   if (m_Frame[otherFrame].size() != m_pShmArea->m_BufferSize)
      m_Frame[otherFrame].resize(m_pShmArea->m_BufferSize);
   memcpy(m_Frame[otherFrame].data(),m_pShmArea->m_Data,m_pShmArea->m_BufferSize);
   m_BufferGen = m_pShmArea->m_BufferGen;
   shmUnlock();
   m_FrameIdx = !m_FrameIdx;
//    return data;
//    m_pSSMutex->unlock();
   return true;
#else
   return false;
#endif
}

///Connect to the shared memory
bool VideoRenderer::startShm()
{
   if (fd != -1) {
      qDebug() << "fd must be -1";
      return false;
   }

   fd = shm_open(m_ShmPath.toAscii(), O_RDWR, 0);
   if (fd < 0) {
      qDebug() << "could not open shm area " << m_ShmPath << ", shm_open failed:" << strerror(errno);
      return false;
   }
   m_ShmAreaLen = sizeof(SHMHeader);
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   m_pShmArea = (SHMHeader*) mmap(NULL, m_ShmAreaLen, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
   #pragma GCC diagnostic pop
   if (m_pShmArea == MAP_FAILED) {
      qDebug() << "Could not map shm area, mmap failed";
      return false;
   }
   emit started();
   return true;
}

///Disconnect from the shared memory
void VideoRenderer::stopShm()
{
   if (fd >= 0)
      close(fd);
   fd = -1;

   if (m_pShmArea != MAP_FAILED)
      munmap(m_pShmArea, m_ShmAreaLen);
   m_ShmAreaLen = 0;
   m_pShmArea = (SHMHeader*) MAP_FAILED;
}

///Resize the shared memory
bool VideoRenderer::resizeShm()
{
   while (( (unsigned int) sizeof(SHMHeader) + (unsigned int) m_pShmArea->m_BufferSize) > (unsigned int) m_ShmAreaLen) {
      const size_t new_size = sizeof(SHMHeader) + m_pShmArea->m_BufferSize;

      shmUnlock();
      if (munmap(m_pShmArea, m_ShmAreaLen)) {
            qDebug() << "Could not unmap shared area:" << strerror(errno);
            return false;
      }

      #pragma GCC diagnostic push
      #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
      m_pShmArea = (SHMHeader*) mmap(NULL, new_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
      #pragma GCC diagnostic pop
      m_ShmAreaLen = new_size;

      if (!m_pShmArea) {
            m_pShmArea = nullptr;
            qDebug() << "Could not remap shared area";
            return false;
      }

      m_ShmAreaLen = new_size;
      if (!shmLock())
            return false;
   }
   return true;
}

///Lock the memory while the copy is being made
bool VideoRenderer::shmLock()
{
#ifdef Q_OS_LINUX
   const timespec timeout = createTimeout();
   /* We need an upper limit on how long we'll wait to avoid locking the whole GUI */
   if (sem_timedwait(&m_pShmArea->mutex, &timeout) < 0) {
       if (errno == ETIMEDOUT)
           qDebug() << "Timed out before shm lock was acquired";
      return false;
   }
   return true;
#else
   return false;
#endif
}

///Remove the lock, allow a new frame to be drawn
void VideoRenderer::shmUnlock()
{
   sem_post(&m_pShmArea->mutex);
}

///Create a SHM timeout
timespec VideoRenderer::createTimeout()
{
#ifdef Q_OS_LINUX
   timespec timeout = {0, 0};
   if (clock_gettime(CLOCK_REALTIME, &timeout) == -1)
      qDebug() << "clock_gettime";
   timeout.tv_sec += TIMEOUT_SEC;
   return timeout;
#else
   return {0,0};
#endif
}


/*****************************************************************************
 *                                                                           *
 *                                   Slots                                   *
 *                                                                           *
 ****************************************************************************/

///Update the buffer
void VideoRenderer::timedEvents()
{

   bool ok = renderToBitmap();

   if (ok == true) {

      //Compute the FPS shown to the client
      if (m_CurrentTime.second() != QTime::currentTime().second()) {
         m_Fps = m_fpsC;
         m_fpsC=0;
         m_CurrentTime = QTime::currentTime();
      }
      m_fpsC++;

      emit frameUpdated();
   }
   else {
      qDebug() << "Frame dropped";
      usleep(rand()%1000); //Be sure it can come back in sync
   }
}

///Start the rendering loop
void VideoRenderer::startRendering()
{
   VideoModel::instance()->startStopMutex()->lock();
   QMutexLocker locker(m_pMutex);
   startShm();
   if (!m_pTimer) {
      m_pTimer = new QTimer(nullptr);

//       m_pTimer->moveToThread(thread());
      connect(m_pTimer,SIGNAL(timeout()),this,SLOT(timedEvents()));
      m_pTimer->setInterval(30);
   }

   if (!m_pTimer->isActive()) {
      qDebug() << "Is running" << thread()->isRunning();
      m_pTimer->start();
   }
   else
      qDebug() << "Timer already started!";

   m_isRendering = true;
   VideoModel::instance()->startStopMutex()->unlock();
}

///Stop the rendering loop
void VideoRenderer::stopRendering()
{
   VideoModel::instance()->startStopMutex()->lock();
   QMutexLocker locker(m_pMutex);
   m_isRendering = false;
   qDebug() << "Stopping rendering on" << m_Id;
   if (m_pTimer)
      m_pTimer->stop();
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
   return m_isRendering?m_Frame[m_FrameIdx].data():nullptr;
}

///Is this redenrer active
bool VideoRenderer::isRendering()
{
   return m_isRendering;
}

///Return the current framerate
QByteArray VideoRenderer::currentFrame()
{
   return m_Frame[m_FrameIdx];
}

///Return the current resolution
Resolution VideoRenderer::activeResolution()
{
   return m_Res;
}

///Get mutex, in case renderer and views are not in the same thread
QMutex* VideoRenderer::mutex()
{
   return m_pMutex;
}

///Get the current frame rate of this renderer
int VideoRenderer::fps() const
{
   return m_Fps;
}


/*****************************************************************************
 *                                                                           *
 *                                 Setters                                   *
 *                                                                           *
 ****************************************************************************/

void VideoRenderer::setResolution(QSize size)
{
   m_Res = size;
   m_Width = size.width();
   m_Height = size.height();
}

void VideoRenderer::setShmPath(QString path)
{
   m_ShmPath = path;
}
