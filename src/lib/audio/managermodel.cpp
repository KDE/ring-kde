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
#include "managermodel.h"

//SFLPhone
#include "dbus/configurationmanager.h"
#include "settings.h"

///Constructor
Audio::ManagerModel::ManagerModel(const QObject* parent) : QAbstractListModel(const_cast<QObject*>(parent))
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   const QStringList managers = configurationManager.getSupportedAudioManagers();
   foreach(const QString& m,managers) {
      if (m == ManagerName::PULSEAUDIO) {
         m_lSupportedManagers << Manager::PULSE;
         m_lDeviceList << "Pulse Audio";
      }
      else if (m == ManagerName::ALSA) {
         m_lSupportedManagers << Manager::ALSA;
         m_lDeviceList<< "ALSA";
      }
      else if (m == ManagerName::JACK) {
         m_lSupportedManagers << Manager::JACK;
         m_lDeviceList<< "Jack";
      }
      else
         qDebug() << "Unsupported audio manager" << m;
   }
}

///Destructor
Audio::ManagerModel::~ManagerModel()
{
   m_lDeviceList.clear();
}

///Re-implement QAbstractListModel data
QVariant Audio::ManagerModel::data( const QModelIndex& index, int role) const
{
   if (!index.isValid())
      return QVariant();
   switch(role) {
      case Qt::DisplayRole:
         return m_lDeviceList[index.row()];
   };
   return QVariant();
}

///Re-implement QAbstractListModel rowCount
int Audio::ManagerModel::rowCount( const QModelIndex& parent ) const
{
   if (parent.isValid())
      return 0;
   return m_lDeviceList.size();
}

///Re-implement QAbstractListModel flags
Qt::ItemFlags Audio::ManagerModel::flags( const QModelIndex& index ) const
{
   Q_UNUSED(index)
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

///This model is read only
bool Audio::ManagerModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

/**
 * Return the current audio manager
 * @warning Changes to the current index model will invalid Input/Output/Ringtone devices models
 */
QModelIndex Audio::ManagerModel::currentManagerIndex() const
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   const QString manager = configurationManager.getAudioManager();
      if (manager == ManagerName::PULSEAUDIO)
         return index((int)Manager::PULSE,0);
      else if (manager == ManagerName::ALSA)
         return index((int)Manager::ALSA,0);
      else if (manager == ManagerName::JACK)
         return index((int)Manager::JACK,0);
      return QModelIndex();
}

Audio::ManagerModel::Manager Audio::ManagerModel::currentManager() const
{
   return m_lSupportedManagers[currentManagerIndex().row()];
}

///Set current audio manager
bool Audio::ManagerModel::setCurrentManager(const QModelIndex& idx)
{
   if (!idx.isValid())
      return false;

   bool ret = true;
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   switch (m_lSupportedManagers[idx.row()]) {
      case Manager::PULSE:
         ret = configurationManager.setAudioManager(ManagerName::PULSEAUDIO);
         Audio::Settings::instance()->reload();
         break;
      case Manager::ALSA:
         ret = configurationManager.setAudioManager(ManagerName::ALSA);
         Audio::Settings::instance()->reload();
         break;
      case Manager::JACK:
         ret = configurationManager.setAudioManager(ManagerName::JACK);
         Audio::Settings::instance()->reload();
         break;
   };
   if (!ret) {
      const QModelIndex& newIdx = currentManagerIndex();
      emit currentManagerChanged(currentManager());
      emit currentManagerChanged(newIdx);
      emit currentManagerChanged(newIdx.row());
   }
   return ret;
}

///QCombobox -> QModelIndex shim
bool Audio::ManagerModel::setCurrentManager(int idx)
{
   return setCurrentManager(index(idx,0));
}