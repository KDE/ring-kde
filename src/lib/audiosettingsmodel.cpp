/****************************************************************************
 *   Copyright (C) 2013 by Savoir-Faire Linux                               *
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
#include "audiosettingsmodel.h"
#include "dbus/configurationmanager.h"

AudioSettingsModel* AudioSettingsModel::m_spInstance = nullptr;


AudioSettingsModel::AudioSettingsModel() : QObject()
{
   m_pAlsaPluginModel     = new AlsaPluginModel     (this);
   m_pInputDeviceModel    = new InputDeviceModel    (this);
   m_pOutputDeviceModel   = new OutputDeviceModel   (this);
   m_pAudioManagerModel   = new AudioManagerModel   (this);
   m_pRingtoneDeviceModel = new RingtoneDeviceModel (this);
}

AudioSettingsModel::~AudioSettingsModel()
{
   delete m_pAlsaPluginModel    ;
   delete m_pInputDeviceModel   ;
   delete m_pOutputDeviceModel  ;
   delete m_pAudioManagerModel   ;
   delete m_pRingtoneDeviceModel;
}

AudioSettingsModel* AudioSettingsModel::instance()
{
   if (!m_spInstance)
      m_spInstance = new AudioSettingsModel();
   return m_spInstance;
}

AlsaPluginModel* AudioSettingsModel::alsaPluginModel() const
{
   return m_pAlsaPluginModel;
}

InputDeviceModel* AudioSettingsModel::inputDeviceModel() const
{
   return m_pInputDeviceModel;
}

OutputDeviceModel* AudioSettingsModel::outputDeviceModel() const
{
   return m_pOutputDeviceModel;
}

AudioManagerModel* AudioSettingsModel::audioManagerModel() const
{
   return m_pAudioManagerModel;
}

RingtoneDeviceModel* AudioSettingsModel::ringtoneDeviceModel() const
{
   return m_pRingtoneDeviceModel;
}

void AudioSettingsModel::reload()
{
   m_pAlsaPluginModel->reload();
   m_pInputDeviceModel->reload();
   m_pOutputDeviceModel->reload();
//    m_pAudioManagerModel->reload();
   m_pRingtoneDeviceModel->reload();
}

/****************************************************************
 *                                                              *
 *                        AlsaPluginModel                       *
 *                                                              *
 ***************************************************************/

AlsaPluginModel::AlsaPluginModel(QObject* parent) : QAbstractListModel(parent)
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   m_lDeviceList = configurationManager.getAudioPluginList();
}

AlsaPluginModel::~AlsaPluginModel()
{
   
}

QVariant AlsaPluginModel::data( const QModelIndex& index, int role) const
{
   if (!index.isValid())
      return QVariant();
   switch(role) {
      case Qt::DisplayRole:
         return m_lDeviceList[index.row()];
   };
   return QVariant();
}

int AlsaPluginModel::rowCount( const QModelIndex& parent ) const
{
   if (parent.isValid())
      return 0;
   return m_lDeviceList.size();
}

Qt::ItemFlags AlsaPluginModel::flags( const QModelIndex& index ) const
{
   Q_UNUSED(index)
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool AlsaPluginModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

QModelIndex AlsaPluginModel::currentPlugin() const
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   const int idx = m_lDeviceList.indexOf(configurationManager.getCurrentAudioOutputPlugin());
   qDebug() << "Invalid current audio plugin";
   if (idx == -1)
      return QModelIndex();
   else
      return index(idx,0,QModelIndex());
}

void AlsaPluginModel::setCurrentPlugin(const QModelIndex& idx)
{
   if (!idx.isValid())
      return;
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   configurationManager.setAudioPlugin(m_lDeviceList[idx.row()]);
}

void AlsaPluginModel::setCurrentPlugin(int idx)
{
   setCurrentPlugin(index(idx,0));
}

void AlsaPluginModel::reload()
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   m_lDeviceList = configurationManager.getAudioPluginList();
   emit layoutChanged();
}


/****************************************************************
 *                                                              *
 *                       InputDeviceModel                       *
 *                                                              *
 ***************************************************************/

InputDeviceModel::InputDeviceModel(QObject* parent) : QAbstractListModel(parent)
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   m_lDeviceList = configurationManager.getAudioInputDeviceList  ();
}

InputDeviceModel::~InputDeviceModel()
{
   
}

QVariant InputDeviceModel::data( const QModelIndex& index, int role) const
{
   if (!index.isValid())
      return QVariant();
   switch(role) {
      case Qt::DisplayRole:
         return m_lDeviceList[index.row()];
   };
   return QVariant();
}

int InputDeviceModel::rowCount( const QModelIndex& parent ) const
{
   if (parent.isValid())
      return 0;
   return m_lDeviceList.size();
}

Qt::ItemFlags InputDeviceModel::flags( const QModelIndex& index ) const
{
   Q_UNUSED(index)
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool InputDeviceModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

QModelIndex InputDeviceModel::currentDevice() const
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   const QStringList currentDevices = configurationManager.getCurrentAudioDevicesIndex();
   const int idx = currentDevices[AudioSettingsModel::DeviceIndex::INPUT].toInt();
   if (idx >= m_lDeviceList.size())
      return QModelIndex();
   return index(idx,0);
}

void InputDeviceModel::setCurrentDevice(const QModelIndex& index)
{
   if (index.isValid()) {
      ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
      configurationManager.setAudioInputDevice(index.row());
   }
}

void InputDeviceModel::setCurrentDevice(int idx)
{
   setCurrentDevice(index(idx,0));
}

void InputDeviceModel::reload()
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   m_lDeviceList = configurationManager.getAudioInputDeviceList  ();
   emit layoutChanged();
}


/****************************************************************
 *                                                              *
 *                       OutputDeviceModel                      *
 *                                                              *
 ***************************************************************/

OutputDeviceModel::OutputDeviceModel(QObject* parent) : QAbstractListModel(parent)
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   m_lDeviceList = configurationManager.getAudioOutputDeviceList();
}

OutputDeviceModel::~OutputDeviceModel()
{
   
}

QVariant OutputDeviceModel::data( const QModelIndex& index, int role) const
{
   if (!index.isValid())
      return QVariant();
   switch(role) {
      case Qt::DisplayRole:
         return m_lDeviceList[index.row()];
   };
   return QVariant();
}

int OutputDeviceModel::rowCount( const QModelIndex& parent ) const
{
   if (parent.isValid())
      return 0;
   return m_lDeviceList.size();
}

Qt::ItemFlags OutputDeviceModel::flags( const QModelIndex& index ) const
{
   Q_UNUSED(index)
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool OutputDeviceModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

QModelIndex OutputDeviceModel::currentDevice() const
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   const QStringList currentDevices = configurationManager.getCurrentAudioDevicesIndex();
   const int idx = currentDevices[AudioSettingsModel::DeviceIndex::OUTPUT].toInt();
   if (idx >= m_lDeviceList.size())
      return QModelIndex();
   return index(idx,0);
}

void OutputDeviceModel::setCurrentDevice(const QModelIndex& index)
{
   if (index.isValid()) {
      ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
      configurationManager.setAudioOutputDevice(index.row());
   }
}

void OutputDeviceModel::setCurrentDevice(int idx)
{
   setCurrentDevice(index(idx,0));
}

void OutputDeviceModel::reload()
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   m_lDeviceList = configurationManager.getAudioOutputDeviceList();
   emit layoutChanged();
}

/****************************************************************
 *                                                              *
 *                        AudioManagerModel                      *
 *                                                              *
 ***************************************************************/

AudioManagerModel::AudioManagerModel(QObject* parent) : QAbstractListModel(parent)
{
   m_lDeviceList << "ALSA" << "Pulse Audio";
}

AudioManagerModel::~AudioManagerModel()
{
   m_lDeviceList.clear();
}

QVariant AudioManagerModel::data( const QModelIndex& index, int role) const
{
   if (!index.isValid())
      return QVariant();
   switch(role) {
      case Qt::DisplayRole:
         return m_lDeviceList[index.row()];
   };
   return QVariant();
}

int AudioManagerModel::rowCount( const QModelIndex& parent ) const
{
   if (parent.isValid())
      return 0;
   return m_lDeviceList.size();
}

Qt::ItemFlags AudioManagerModel::flags( const QModelIndex& index ) const
{
   Q_UNUSED(index)
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool AudioManagerModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

/****************************************************************
 *                                                              *
 *                       RingtoneDeviceModel                    *
 *                                                              *
 ***************************************************************/

RingtoneDeviceModel::RingtoneDeviceModel(QObject* parent) : QAbstractListModel(parent)
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   m_lDeviceList = configurationManager.getAudioOutputDeviceList();
}

RingtoneDeviceModel::~RingtoneDeviceModel()
{
   
}

QVariant RingtoneDeviceModel::data( const QModelIndex& index, int role) const
{
   if (!index.isValid())
      return QVariant();
   switch(role) {
      case Qt::DisplayRole:
         return m_lDeviceList[index.row()];
   };
   return QVariant();
}

int RingtoneDeviceModel::rowCount( const QModelIndex& parent ) const
{
   if (parent.isValid())
      return 0;
   return m_lDeviceList.size();
}

Qt::ItemFlags RingtoneDeviceModel::flags( const QModelIndex& index ) const
{
   Q_UNUSED(index)
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool RingtoneDeviceModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

QModelIndex RingtoneDeviceModel::currentDevice() const
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   const QStringList currentDevices = configurationManager.getCurrentAudioDevicesIndex();
   const int idx = currentDevices[AudioSettingsModel::DeviceIndex::RINGTONE].toInt();
   if (idx >= m_lDeviceList.size())
      return QModelIndex();
   return index(idx,0);
}

void RingtoneDeviceModel::setCurrentDevice(const QModelIndex& index)
{
   if (index.isValid()) {
      ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
      configurationManager.setAudioRingtoneDevice(index.row());
   }
}

void RingtoneDeviceModel::setCurrentDevice(int idx)
{
   setCurrentDevice(index(idx,0));
}

void RingtoneDeviceModel::reload()
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   m_lDeviceList = configurationManager.getAudioOutputDeviceList();
   emit layoutChanged();
}
