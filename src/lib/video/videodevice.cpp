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
#include "videodevice.h"
#include "../dbus/videomanager.h"
#include "../private/videochannel_p.h"
#include "../private/videodevice_p.h"
#include "videodevicemodel.h"
#include "videoresolution.h"
#include "videorate.h"
#include "videochannel.h"



VideoDevicePrivate::VideoDevicePrivate() : m_pCurrentChannel(nullptr)
{
}

///Constructor
VideoDevice::VideoDevice(const QString &id) : QAbstractListModel(nullptr),
d_ptr(new VideoDevicePrivate())
{
   d_ptr->m_DeviceId = id;
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   MapStringMapStringVectorString cap = interface.getCapabilities(id);
   QMapIterator<QString, MapStringVectorString> channels(cap);
   while (channels.hasNext()) {
      channels.next();

      VideoChannel* chan = new VideoChannel(this,channels.key());
      d_ptr->m_lChannels << chan;

      QMapIterator<QString, VectorString> resolutions(channels.value());
      while (resolutions.hasNext()) {
         resolutions.next();

         VideoResolution* res = new VideoResolution(resolutions.key(),chan);
         chan->d_ptr->m_lValidResolutions << res;

         foreach(const QString& rate, resolutions.value()) {
            VideoRate* r = new VideoRate(res,rate);
            res->m_lValidRates << r;
         }
      }
   }
   Q_UNUSED(cap)
}

///Destructor
VideoDevice::~VideoDevice()
{
   delete d_ptr;
}

QVariant VideoDevice::data( const QModelIndex& index, int role) const
{
   if (index.isValid() && role == Qt::DisplayRole) {
      return d_ptr->m_lChannels[index.row()]->name();
   }
   return QVariant();
}

int VideoDevice::rowCount( const QModelIndex& parent) const
{
   return (parent.isValid())?0:d_ptr->m_lChannels.size();
}

Qt::ItemFlags VideoDevice::flags( const QModelIndex& idx) const
{
   if (idx.column() == 0)
      return QAbstractItemModel::flags(idx) | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   return QAbstractItemModel::flags(idx);
}

bool VideoDevice::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

// int VideoDevice::relativeIndex() {
//    return m_pDevice->channelList().indexOf(this);
// }

///Get the valid channel list
QList<VideoChannel*> VideoDevice::channelList() const
{
   return d_ptr->m_lChannels;
}

///Save the current settings
void VideoDevice::save()
{
   //In case new (unsupported) fields are added, merge with existing
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   MapStringString pref = interface.getSettings(d_ptr->m_DeviceId);
   pref[VideoDevicePrivate::PreferenceNames::CHANNEL] = activeChannel()->name();
   pref[VideoDevicePrivate::PreferenceNames::SIZE   ] = activeChannel()->activeResolution()->name();
   pref[VideoDevicePrivate::PreferenceNames::RATE   ] = activeChannel()->activeResolution()->activeRate()->name();
   interface.applySettings(d_ptr->m_DeviceId,pref);
}

///Get the device id
const QString VideoDevice::id() const
{
   return d_ptr->m_DeviceId;
}

///Get the device name
const QString VideoDevice::name() const
{
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   return QMap<QString,QString>(interface.getSettings(d_ptr->m_DeviceId))[VideoDevicePrivate::PreferenceNames::NAME];;
}

///Is this device the default one
bool VideoDevice::isActive() const
{
   return VideoDeviceModel::instance()->activeDevice() == this;
}

bool VideoDevice::setActiveChannel(VideoChannel* chan)
{
   if (!chan || !d_ptr->m_lChannels.indexOf(chan)) {
      qWarning() << "Trying to set an invalid channel" << (chan?chan->name():"NULL") << "for" << id();
      return false;
   }
   d_ptr->m_pCurrentChannel = chan;
   save();
   return true;
}

bool VideoDevice::setActiveChannel(int idx)
{
   if (idx < 0 || idx >= d_ptr->m_lChannels.size()) return false;
   return setActiveChannel(d_ptr->m_lChannels[idx]);
}

VideoChannel* VideoDevice::activeChannel() const
{
   if (!d_ptr->m_pCurrentChannel) {
      VideoManagerInterface& interface = DBus::VideoManager::instance();
      const QString chan = QMap<QString,QString>(interface.getSettings(d_ptr->m_DeviceId))[VideoDevicePrivate::PreferenceNames::CHANNEL];
      foreach(VideoChannel* c, d_ptr->m_lChannels) {
         if (c->name() == chan) {
            d_ptr->m_pCurrentChannel = c;
            break;
         }
      }
   }
   if (!d_ptr->m_pCurrentChannel && d_ptr->m_lChannels.size()) {
      d_ptr->m_pCurrentChannel = d_ptr->m_lChannels[0];
   }
   return d_ptr->m_pCurrentChannel;
}
