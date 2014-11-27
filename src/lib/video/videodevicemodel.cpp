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
#include "videodevicemodel.h"
#include "videodevice.h"
#include <call.h>
#include <account.h>
#include "videocodec.h"
#include "../dbus/videomanager.h"

#include <QtCore/QCoreApplication>

VideoDeviceModel* VideoDeviceModel::m_spInstance = nullptr;


class VideoDeviceModelPrivate
{
public:
   VideoDeviceModelPrivate();

   //Attrbutes
   QHash<QString,VideoDevice*> m_hDevices     ;
   QList<VideoDevice*>         m_lDevices     ;
   VideoDevice*                m_pDummyDevice ;
   VideoDevice*                m_pActiveDevice;
};


VideoDeviceModelPrivate::VideoDeviceModelPrivate() : m_pDummyDevice(nullptr),m_pActiveDevice(nullptr)
{
   
}

///Constructor
VideoDeviceModel::VideoDeviceModel() : QAbstractListModel(QCoreApplication::instance()),
d_ptr(new VideoDeviceModelPrivate())
{
   m_spInstance = this;
   reload();
}

VideoDeviceModel* VideoDeviceModel::instance()
{
   if (!m_spInstance)
      m_spInstance = new VideoDeviceModel();
   return m_spInstance;
}

///Get data from the model
QVariant VideoDeviceModel::data( const QModelIndex& idx, int role) const
{
   if(idx.column() == 0 && role == Qt::DisplayRole)
      return QVariant(d_ptr->m_lDevices[idx.row()]->id());
   return QVariant();
}

///The number of codec
int VideoDeviceModel::rowCount( const QModelIndex& par ) const
{
   Q_UNUSED(par)
   return d_ptr->m_lDevices.size();
}

///Items flag
Qt::ItemFlags VideoDeviceModel::flags( const QModelIndex& idx ) const
{
   if (idx.column() == 0)
      return QAbstractItemModel::flags(idx) | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   return QAbstractItemModel::flags(idx);
}

///Set the codec data (codecs can't be added or removed that way)
bool VideoDeviceModel::setData(const QModelIndex& idx, const QVariant &value, int role)
{
   Q_UNUSED(idx)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

///Destructor
VideoDeviceModel::~VideoDeviceModel()
{
   while (d_ptr->m_lDevices.size()) {
      VideoDevice* c = d_ptr->m_lDevices[0];
      d_ptr->m_lDevices.removeAt(0);
      delete c;
   }
   delete d_ptr;
}

///Save the current model over dbus
void VideoDeviceModel::setActive(const QModelIndex& idx)
{
   if (idx.isValid()) {
      VideoManagerInterface& interface = DBus::VideoManager::instance();
      interface.setDefaultDevice(d_ptr->m_lDevices[idx.row()]->id());
      d_ptr->m_pActiveDevice = d_ptr->m_lDevices[idx.row()];
      emit changed();
      emit currentIndexChanged(idx.row());
   }
}

///Convenience
void VideoDeviceModel::setActive(const int idx)
{
   setActive(index(idx,0,QModelIndex()));
}


void VideoDeviceModel::setActive(const VideoDevice* device)
{
   VideoManagerInterface& interface = DBus::VideoManager::instance();

   interface.setDefaultDevice(device?device->id():VideoDevice::NONE);
   d_ptr->m_pActiveDevice = const_cast<VideoDevice*>(device);
   emit changed();
   const int idx = d_ptr->m_lDevices.indexOf((VideoDevice*)device);
   emit currentIndexChanged(idx);
}

void VideoDeviceModel::reload()
{
   QHash<QString,VideoDevice*> devicesHash;
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   const QStringList deviceList = interface.getDeviceList();
   if (deviceList.size() == d_ptr->m_hDevices.size()) {
      d_ptr->m_lDevices = d_ptr->m_hDevices.values();
   }

   foreach(const QString& deviceName,deviceList) {
      if (!d_ptr->m_hDevices[deviceName]) {
         devicesHash[deviceName] = new VideoDevice(deviceName);
      }
      else {
         devicesHash[deviceName] = d_ptr->m_hDevices[deviceName];
      }
   }
   foreach(VideoDevice* dev, d_ptr->m_hDevices) {
      if (dev && devicesHash.key(dev).isEmpty()) {
         delete dev;
      }
   }
   d_ptr->m_hDevices.clear();
   d_ptr->m_hDevices = devicesHash;
   d_ptr->m_lDevices = d_ptr->m_hDevices.values();

   emit layoutChanged();
//    channelModel   ()->reload();
   setActive(activeDevice());
}


VideoDevice* VideoDeviceModel::activeDevice() const
{
   if (!d_ptr->m_pActiveDevice) {
      VideoManagerInterface& interface = DBus::VideoManager::instance();
      const QString deId = interface.getDefaultDevice();
      if (!d_ptr->m_lDevices.size())
         const_cast<VideoDeviceModel*>(this)->reload();
      VideoDevice* dev =  d_ptr->m_hDevices[deId];

      //Handling null everywhere is too long, better create a dummy device and
      //log the event
      if (!dev) {
         if (!deId.isEmpty())
            qWarning() << "Requested unknown device" << deId;
         if (!d_ptr->m_pDummyDevice)
            d_ptr->m_pDummyDevice = new VideoDevice("None");
         return d_ptr->m_pDummyDevice;
      }
      d_ptr->m_pActiveDevice = dev;
   }
   return d_ptr->m_pActiveDevice;
}


int VideoDeviceModel::activeIndex() const
{
   return d_ptr->m_lDevices.indexOf(activeDevice());
}


VideoDevice* VideoDeviceModel::getDevice(const QString& devId) const
{
   return d_ptr->m_hDevices[devId];
}

QList<VideoDevice*> VideoDeviceModel::devices() const
{
   return d_ptr->m_lDevices;
}
