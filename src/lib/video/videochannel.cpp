/****************************************************************************
 *   Copyright (C) 2014 by Savoir-Faire Linux                               *
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
#include "videochannel.h"

//SFLphone
#include "videoresolution.h"
#include "videodevice.h"
#include "dbus/videomanager.h"

VideoChannel::VideoChannel(VideoDevice* dev,const QString& name) :
   m_Name(name),m_pCurrentResolution(nullptr),m_pDevice(dev)
{
}

VideoChannel* VideoDevice::activeChannel() const
{
   if (!m_pCurrentChannel) {
      VideoManagerInterface& interface = DBus::VideoManager::instance();
      const QString chan = QMap<QString,QString>(interface.getPreferences(m_DeviceId))[VideoDevice::PreferenceNames::CHANNEL];
      foreach(VideoChannel* c, m_lChannels) {
         if (c->name() == chan) {
            const_cast<VideoDevice*>(this)->m_pCurrentChannel = c;
            break;
         }
      }
   }
   if (!m_pCurrentChannel && m_lChannels.size()) {
      const_cast<VideoDevice*>(this)->m_pCurrentChannel = m_lChannels[0];
   }
   return m_pCurrentChannel;
}

QVariant VideoChannel::data( const QModelIndex& index, int role) const
{
   if (index.isValid() && role == Qt::DisplayRole) {
      return m_lValidResolutions[index.row()]->name();
   }
   return QVariant();
}

int VideoChannel::rowCount( const QModelIndex& parent) const
{
   return (parent.isValid())?0:m_lValidResolutions.size();
}

Qt::ItemFlags VideoChannel::flags( const QModelIndex& idx) const
{
   if (idx.column() == 0)
      return QAbstractItemModel::flags(idx) | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   return QAbstractItemModel::flags(idx);
}

bool VideoChannel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

int VideoChannel::relativeIndex() {
   return m_pDevice->channelList().indexOf(this);
}

bool VideoChannel::setActiveResolution(int idx)
{
   if (idx < 0 || idx >= m_lValidResolutions.size()) return false;
   return setActiveResolution(m_lValidResolutions[idx]);
}

bool VideoChannel::setActiveResolution(VideoResolution* res) {
   if ((!res) || m_lValidResolutions.indexOf(res) == -1 || res->name().isEmpty()) {
      qWarning() << "Invalid active resolution" << (res?res->name():"NULL");
      return false;
   }
   m_pCurrentResolution = res;
   m_pDevice->save();
   return true;
}

