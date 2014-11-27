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
#include "videomanager.h"

//Qt
#include <QtCore/QMutex>

//SFLPhone
#include "../dbus/videomanager.h"
#include "videodevice.h"
#include <call.h>
#include <callmodel.h>
#include "videorenderer.h"
#include "videodevicemodel.h"
#include "videochannel.h"
#include "videorate.h"
#include "videoresolution.h"

//Static member
VideoManager* VideoManager::m_spInstance = nullptr;

class VideoManagerPrivate : public QObject
{
   Q_OBJECT

public:
   VideoManagerPrivate(VideoManager* parent);

   //Attributes
   bool           m_PreviewState;
   uint           m_BufferSize  ;
   uint           m_ShmKey      ;
   uint           m_SemKey      ;
   QMutex*        m_SSMutex     ;
   QHash<QString,VideoRenderer*> m_lRenderers;

private:
   VideoManager* q_ptr;

private Q_SLOTS:
   void startedDecoding(const QString& id, const QString& shmPath, int width, int height);
   void stoppedDecoding(const QString& id, const QString& shmPath);
   void deviceEvent();

};

VideoManagerPrivate::VideoManagerPrivate(VideoManager* parent) : QObject(parent), q_ptr(parent),
m_BufferSize(0),m_ShmKey(0),m_SemKey(0),m_PreviewState(false),m_SSMutex(new QMutex())
{
   
}

///Constructor
VideoManager::VideoManager():QThread(), d_ptr(new VideoManagerPrivate(this))
{
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   connect( &interface , SIGNAL(deviceEvent())                           , d_ptr, SLOT(deviceEvent())                           );
   connect( &interface , SIGNAL(startedDecoding(QString,QString,int,int,bool)), d_ptr, SLOT(startedDecoding(QString,QString,int,int)));
   connect( &interface , SIGNAL(stoppedDecoding(QString,QString,bool))        , d_ptr, SLOT(stoppedDecoding(QString,QString))        );
}


VideoManager::~VideoManager()
{
   delete d_ptr;
}

///Singleton
VideoManager* VideoManager::instance()
{
   if (!m_spInstance) {
      m_spInstance = new VideoManager();
   }
   return m_spInstance;
}

///Return the call renderer or nullptr
VideoRenderer* VideoManager::getRenderer(const Call* call) const
{
   if (!call) return nullptr;
   return d_ptr->m_lRenderers[call->id()];
}

///Get the video preview renderer
VideoRenderer* VideoManager::previewRenderer()
{
   if (!d_ptr->m_lRenderers["local"]) {
      VideoResolution* res = VideoDeviceModel::instance()->activeDevice()->activeChannel()->activeResolution();
      if (!res) {
         qWarning() << "Misconfigured video device";
         return nullptr;
      }
      d_ptr->m_lRenderers["local"] = new VideoRenderer("local","",res->size());
   }
   return d_ptr->m_lRenderers["local"];
}

///Stop video preview
void VideoManager::stopPreview()
{
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   interface.stopCamera();
   d_ptr->m_PreviewState = false;
}

///Start video preview
void VideoManager::startPreview()
{
   if (d_ptr->m_PreviewState) return;
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   interface.startCamera();
   d_ptr->m_PreviewState = true;
}

///Is the video model fetching preview from a camera
bool VideoManager::isPreviewing()
{
   return d_ptr->m_PreviewState;
}

///@todo Set the video buffer size
void VideoManager::setBufferSize(uint size)
{
   d_ptr->m_BufferSize = size;
}

///Event callback
void VideoManagerPrivate::deviceEvent()
{
   //TODO is there anything useful to do?
}

///A video is not being rendered
void VideoManagerPrivate::startedDecoding(const QString& id, const QString& shmPath, int width, int height)
{
   Q_UNUSED(id)

   QSize res = QSize(width,height);
//    if (VideoDeviceModel::instance()->activeDevice()
//       && VideoDeviceModel::instance()->activeDevice()->activeChannel()->activeResolution()->width() == width) {
//       //FIXME flawed logic
//       res = VideoDeviceModel::instance()->activeDevice()->activeChannel()->activeResolution()->size();
//    }
//    else {
//       res =  QSize(width,height);
//    }

   if (m_lRenderers[id] == nullptr ) {
      m_lRenderers[id] = new VideoRenderer(id,shmPath,res);
      m_lRenderers[id]->moveToThread(q_ptr);
      if (!q_ptr->isRunning())
         q_ptr->start();
   }
   else {
      VideoRenderer* renderer = m_lRenderers[id];
      renderer->setShmPath(shmPath);
      renderer->setSize(res);
   }

   m_lRenderers[id]->startRendering();
   VideoDevice* dev = VideoDeviceModel::instance()->getDevice(id);
   if (dev) {
      emit dev->renderingStarted(m_lRenderers[id]);
   }
   if (id != "local") {
      qDebug() << "Starting video for call" << id;
      emit q_ptr->videoCallInitiated(m_lRenderers[id]);
   }
   else {
      m_PreviewState = true;
      emit q_ptr->previewStateChanged(true);
      emit q_ptr->previewStarted(m_lRenderers[id]);
   }
}

///A video stopped being rendered
void VideoManagerPrivate::stoppedDecoding(const QString& id, const QString& shmPath)
{
   Q_UNUSED(shmPath)
   VideoRenderer* r = m_lRenderers[id];
   if ( r ) {
      r->stopRendering();
   }
   qDebug() << "Video stopped for call" << id <<  "Renderer found:" << (m_lRenderers[id] != nullptr);
//    emit videoStopped();

   VideoDevice* dev = VideoDeviceModel::instance()->getDevice(id);
   if (dev) {
      emit dev->renderingStopped(r);
   }
   if (id == "local") {
      m_PreviewState = false;
      emit q_ptr->previewStateChanged(false);
      emit q_ptr->previewStopped(r);
   }
//    r->mutex()->lock();
   m_lRenderers[id] = nullptr;
   delete r;
}

void VideoManager::switchDevice(const VideoDevice* device) const
{
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   interface.switchInput(device->id());
}

QMutex* VideoManager::startStopMutex() const
{
   return d_ptr->m_SSMutex;
}

#include <videomanager.moc>
