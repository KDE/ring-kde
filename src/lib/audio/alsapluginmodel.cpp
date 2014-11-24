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
#include "alsapluginmodel.h"

//SFLPhone
#include "dbus/configurationmanager.h"

///Constructor
Audio::AlsaPluginModel::AlsaPluginModel(const QObject* parent) : QAbstractListModel(const_cast<QObject*>(parent))
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   m_lDeviceList = configurationManager.getAudioPluginList();
}

///Destructor
Audio::AlsaPluginModel::~AlsaPluginModel()
{
   
}

///Re-implement QAbstractListModel data
QVariant Audio::AlsaPluginModel::data( const QModelIndex& index, int role) const
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
int Audio::AlsaPluginModel::rowCount( const QModelIndex& parent ) const
{
   if (parent.isValid())
      return 0;
   return m_lDeviceList.size();
}

///Re-implement QAbstractListModel flags
Qt::ItemFlags Audio::AlsaPluginModel::flags( const QModelIndex& index ) const
{
   Q_UNUSED(index)
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

///Setting data is disabled
bool Audio::AlsaPluginModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

///Return the current index
QModelIndex Audio::AlsaPluginModel::currentPlugin() const
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   const int idx = m_lDeviceList.indexOf(configurationManager.getCurrentAudioOutputPlugin());
   qDebug() << "Invalid current audio plugin";
   if (idx == -1)
      return QModelIndex();
   else
      return index(idx,0,QModelIndex());
}

///Set the current index
void Audio::AlsaPluginModel::setCurrentPlugin(const QModelIndex& idx)
{
   if (!idx.isValid())
      return;
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   configurationManager.setAudioPlugin(m_lDeviceList[idx.row()]);
}

///Set the current index (qcombobox compatibility shim)
void Audio::AlsaPluginModel::setCurrentPlugin(int idx)
{
   setCurrentPlugin(index(idx,0));
}

///Reload to current daemon state
void Audio::AlsaPluginModel::reload()
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   m_lDeviceList = configurationManager.getAudioPluginList();
   emit layoutChanged();
   emit dataChanged(index(0,0),index(m_lDeviceList.size()-1,0));
}