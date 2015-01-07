/****************************************************************************
 *   Copyright (C) 2012-2015 by Savoir-Faire Linux                          *
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
#include "manager.h"

//Qt
#include <QtCore/QMutex>

//SFLPhone
#include "../dbus/videomanager.h"
#include "device.h"
#include <call.h>
#include <callmodel.h>
#include "renderer.h"
#include "devicemodel.h"
#include "channel.h"
#include "rate.h"
#include "resolution.h"

//Static member
Video::Manager* Video::Manager::m_spInstance = nullptr;

namespace Video {
class ManagerPrivate : public QObject
{
   Q_OBJECT

public:
   ManagerPrivate(Video::Manager* parent);

   //Attributes
   bool           m_PreviewState;
   uint           m_BufferSize  ;
   uint           m_ShmKey      ;
   uint           m_SemKey      ;
   QMutex*        m_SSMutex     ;
   QHash<QString,Video::Renderer*> m_lRenderers;

private:
   Video::Manager* q_ptr;

private Q_SLOTS:
   void startedDecoding(const QString& id, const QString& shmPath, int width, int height);
   void stoppedDecoding(const QString& id, const QString& shmPath);
   void deviceEvent();

};
}

Video::ManagerPrivate::ManagerPrivate(Video::Manager* parent) : QObject(parent), q_ptr(parent),
m_BufferSize(0),m_ShmKey(0),m_SemKey(0),m_PreviewState(false),m_SSMutex(new QMutex())
{
   
}

///Constructor
Video::Manager::Manager():QThread(), d_ptr(new Video::ManagerPrivate(this))
{
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   connect( &interface , SIGNAL(deviceEvent())                           , d_ptr.data(), SLOT(deviceEvent())                           );
   connect( &interface , SIGNAL(startedDecoding(QString,QString,int,int,bool)), d_ptr.data(), SLOT(startedDecoding(QString,QString,int,int)));
   connect( &interface , SIGNAL(stoppedDecoding(QString,QString,bool))        , d_ptr.data(), SLOT(stoppedDecoding(QString,QString))        );
}


Video::Manager::~Manager()
{
//    delete d_ptr;
}

///Singleton
Video::Manager* Video::Manager::instance()
{
   if (!m_spInstance) {
      m_spInstance = new Video::Manager();
   }
   return m_spInstance;
}

///Return the call renderer or nullptr
Video::Renderer* Video::Manager::getRenderer(const Call* call) const
{
   if (!call) return nullptr;
   return d_ptr->m_lRenderers[call->id()];
}

///Get the video preview renderer
Video::Renderer* Video::Manager::previewRenderer()
{
   if (!d_ptr->m_lRenderers["local"]) {
      Video::Resolution* res = Video::DeviceModel::instance()->activeDevice()->activeChannel()->activeResolution();
      if (!res) {
         qWarning() << "Misconfigured video device";
         return nullptr;
      }
      d_ptr->m_lRenderers["local"] = new Video::Renderer("local","",res->size());
   }
   return d_ptr->m_lRenderers["local"];
}

///Stop video preview
void Video::Manager::stopPreview()
{
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   interface.stopCamera();
   d_ptr->m_PreviewState = false;
}

///Start video preview
void Video::Manager::startPreview()
{
   if (d_ptr->m_PreviewState) return;
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   interface.startCamera();
   d_ptr->m_PreviewState = true;
}

///Is the video model fetching preview from a camera
bool Video::Manager::isPreviewing()
{
   return d_ptr->m_PreviewState;
}

///@todo Set the video buffer size
void Video::Manager::setBufferSize(uint size)
{
   d_ptr->m_BufferSize = size;
}

///Event callback
void Video::ManagerPrivate::deviceEvent()
{
   //TODO is there anything useful to do?
}

///A video is not being rendered
void Video::ManagerPrivate::startedDecoding(const QString& id, const QString& shmPath, int width, int height)
{
   Q_UNUSED(id)

   QSize res = QSize(width,height);
//    if (Video::DeviceModel::instance()->activeDevice()
//       && Video::DeviceModel::instance()->activeDevice()->activeChannel()->activeResolution()->width() == width) {
//       //FIXME flawed logic
//       res = Video::DeviceModel::instance()->activeDevice()->activeChannel()->activeResolution()->size();
//    }
//    else {
//       res =  QSize(width,height);
//    }

   if (m_lRenderers[id] == nullptr ) {
      m_lRenderers[id] = new Video::Renderer(id,shmPath,res);
      m_lRenderers[id]->moveToThread(q_ptr);
      if (!q_ptr->isRunning())
         q_ptr->start();
   }
   else {
      Video::Renderer* renderer = m_lRenderers[id];
      renderer->setShmPath(shmPath);
      renderer->setSize(res);
   }

   m_lRenderers[id]->startRendering();
   Video::Device* dev = Video::DeviceModel::instance()->getDevice(id);
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
void Video::ManagerPrivate::stoppedDecoding(const QString& id, const QString& shmPath)
{
   Q_UNUSED(shmPath)
   Video::Renderer* r = m_lRenderers[id];
   if ( r ) {
      r->stopRendering();
   }
   qDebug() << "Video stopped for call" << id <<  "Renderer found:" << (m_lRenderers[id] != nullptr);
//    emit videoStopped();

   Video::Device* dev = Video::DeviceModel::instance()->getDevice(id);
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

void Video::Manager::switchDevice(const Video::Device* device) const
{
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   interface.switchInput(device->id());
}

QMutex* Video::Manager::startStopMutex() const
{
   return d_ptr->m_SSMutex;
}

#include <manager.moc>
