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
#include "channel.h"

//SFLphone
#include "resolution.h"
#include "device.h"
#include "../dbus/videomanager.h"
#include "../private/videochannel_p.h"
#include "../private/videodevice_p.h"

VideoChannelPrivate::VideoChannelPrivate() : m_pCurrentResolution(nullptr)
{
}

Video::Channel::Channel(Video::Device* dev,const QString& name) : QAbstractListModel(dev),
d_ptr(new VideoChannelPrivate())
{
   d_ptr->m_Name    = name;
   d_ptr->m_pDevice = dev;
}

Video::Channel::~Channel()
{
   delete d_ptr;
}

QVariant Video::Channel::data( const QModelIndex& index, int role) const
{
   if (index.isValid() && role == Qt::DisplayRole) {
      return d_ptr->m_lValidResolutions[index.row()]->name();
   }
   return QVariant();
}

int Video::Channel::rowCount( const QModelIndex& parent) const
{
   return (parent.isValid())?0:d_ptr->m_lValidResolutions.size();
}

Qt::ItemFlags Video::Channel::flags( const QModelIndex& idx) const
{
   if (idx.column() == 0)
      return QAbstractItemModel::flags(idx) | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   return QAbstractItemModel::flags(idx);
}

bool Video::Channel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

int Video::Channel::relativeIndex() {
   return d_ptr->m_pDevice->channelList().indexOf(this);
}

bool Video::Channel::setActiveResolution(int idx)
{
   if (idx < 0 || idx >= d_ptr->m_lValidResolutions.size()) return false;
   return setActiveResolution(d_ptr->m_lValidResolutions[idx]);
}

bool Video::Channel::setActiveResolution(Video::Resolution* res) {
   if ((!res) || d_ptr->m_lValidResolutions.indexOf(res) == -1 || res->name().isEmpty()) {
      qWarning() << "Invalid active resolution" << (res?res->name():"NULL");
      return false;
   }
   d_ptr->m_pCurrentResolution = res;
   d_ptr->m_pDevice->save();
   return true;
}

Video::Resolution* Video::Channel::activeResolution()
{
   //If it is the current device, then there is "current" resolution
   if ((!d_ptr->m_pCurrentResolution) && d_ptr->m_pDevice->isActive()) {
      VideoManagerInterface& interface = DBus::VideoManager::instance();
      const QString res = QMap<QString,QString>(interface.getSettings(d_ptr->m_pDevice->id()))[VideoDevicePrivate::PreferenceNames::SIZE];
      foreach(Video::Resolution* r, validResolutions()) {
         if (r->name() == res) {
            d_ptr->m_pCurrentResolution = r;
            break;
         }
      }
   }
   //If it isn't the current _or_ the current res is invalid, pick the first valid one
   if (!d_ptr->m_pCurrentResolution && validResolutions().size()) {
      d_ptr->m_pCurrentResolution = validResolutions().first();
   }

   return d_ptr->m_pCurrentResolution;
}

QString Video::Channel::name() const
{
   return d_ptr->m_Name;
}

QList<Video::Resolution*> Video::Channel::validResolutions() const
{
   return d_ptr->m_lValidResolutions;
}
Video::Device* Video::Channel::device() const
{
   return d_ptr->m_pDevice;
}

