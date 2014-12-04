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
#include "resolution.h"

#include "../dbus/videomanager.h"
#include "../private/videodevice_p.h"
#include "channel.h"
#include "rate.h"
#include "device.h"

#include <QtCore/QStringList>

Video::Resolution::Resolution(const QString& size, Video::Channel* chan)
: QAbstractListModel(chan), m_pCurrentRate(nullptr),m_pChannel(chan)
{
   Q_ASSERT(chan != nullptr);
   if (size.split('x').size() == 2) {
      setWidth(size.split('x')[0].toInt());
      setHeight(size.split('x')[1].toInt());
   }
}

const QString Video::Resolution::name() const
{
   return QString::number(width())+'x'+QString::number(height());
}


QVariant Video::Resolution::data( const QModelIndex& index, int role) const
{
   if (index.isValid() && role == Qt::DisplayRole && index.row() < m_lValidRates.size()) {
      return m_lValidRates[index.row()]->name();
   }
   return QVariant();
}

int Video::Resolution::rowCount( const QModelIndex& parent) const
{
   return (parent.isValid())?0:m_lValidRates.size();
}

Qt::ItemFlags Video::Resolution::flags( const QModelIndex& idx) const
{
   if (idx.column() == 0)
      return QAbstractItemModel::flags(idx) | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   return QAbstractItemModel::flags(idx);
}

bool Video::Resolution::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}


const QList<Video::Rate*> Video::Resolution::validRates() const {
   return m_lValidRates;
}


bool Video::Resolution::setActiveRate(Video::Rate* rate) {
   if (!rate || (m_lValidRates.indexOf(rate) == -1)) {
      qWarning() << "Trying to set an invalid rate" << rate;
      return false;
   }
   m_pCurrentRate = rate;
   m_pChannel->device()->save();
   return true;
}


bool Video::Resolution::setActiveRate(int idx)
{
   if (idx >= m_lValidRates.size() || idx < 0) return false;
   return setActiveRate(m_lValidRates[idx]);
}

Video::Rate* Video::Resolution::activeRate()
{
   if (!m_pChannel) {
      qWarning() << "Trying to get the active rate of an unattached resolution";
      return nullptr;
   }
   if (!m_pCurrentRate && m_pChannel && m_pChannel->device()->isActive()) {
      VideoManagerInterface& interface = DBus::VideoManager::instance();
      const QString rate = QMap<QString,QString>(
         interface.getSettings(m_pChannel->device()->id()))[VideoDevicePrivate::PreferenceNames::RATE];
      foreach(Video::Rate* r, m_lValidRates) {
         if (r->name() == rate) {
            m_pCurrentRate = r;
            break;
         }
      }
   }
   if ((!m_pCurrentRate) && m_lValidRates.size())
      m_pCurrentRate = m_lValidRates[0];

   return m_pCurrentRate;
}

int Video::Resolution::relativeIndex() const
{
   return m_pChannel?m_pChannel->validResolutions().indexOf(const_cast<Video::Resolution*>(this)):-1;
}

int Video::Resolution::width() const
{
   return m_Size.width();
}

int Video::Resolution::height() const
{
   return m_Size.height();
}

QSize Video::Resolution::size() const
{
   return m_Size;
}

void Video::Resolution::setWidth(int width)
{
   m_Size.setWidth(width);
}

void Video::Resolution::setHeight(int height)
{
   m_Size.setHeight(height);
}
