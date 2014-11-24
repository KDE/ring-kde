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
#include "settings.h"

//SFLPhone
#include "dbus/configurationmanager.h"
#include "dbus/callmanager.h"
#include "ringtonedevicemodel.h"
#include "alsapluginmodel.h"
#include "managermodel.h"
#include "outputdevicemodel.h"
#include "inputdevicemodel.h"

namespace Audio {
class SettingsPrivate : public QObject
{
   Q_OBJECT
public:
   SettingsPrivate(Audio::Settings* parent);

   //Attributes
   mutable Audio::AlsaPluginModel*     m_pAlsaPluginModel    ;
   mutable Audio::InputDeviceModel*    m_pInputDeviceModel   ;
   mutable Audio::OutputDeviceModel*   m_pOutputDeviceModel  ;
   mutable Audio::ManagerModel*        m_pAudioManagerModel  ;
   mutable Audio::RingtoneDeviceModel* m_pRingtoneDeviceModel;
   bool                 m_EnableRoomTone      ;

private Q_SLOTS:
   void slotVolumeChanged(const QString& str, double volume);

private:
   Audio::Settings* q_ptr;
};
}

Audio::Settings* Audio::Settings::m_spInstance = nullptr;

Audio::SettingsPrivate::SettingsPrivate(Audio::Settings* parent) : q_ptr(parent),m_EnableRoomTone(false),
 m_pAlsaPluginModel  (nullptr), m_pInputDeviceModel   (nullptr),
 m_pAudioManagerModel(nullptr), m_pRingtoneDeviceModel(nullptr),
 m_pOutputDeviceModel(nullptr)
{

}

///Constructor
Audio::Settings::Settings() : QObject(), d_ptr(new Audio::SettingsPrivate(this))
{
   d_ptr->m_pRingtoneDeviceModel = new RingtoneDeviceModel (this);
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   connect(&configurationManager,SIGNAL(volumeChanged(QString,double)),d_ptr.data(),SLOT(slotVolumeChanged(QString,double)));
}

///Destructor
Audio::Settings::~Settings()
{
   delete d_ptr->m_pAlsaPluginModel    ;
   delete d_ptr->m_pInputDeviceModel   ;
   delete d_ptr->m_pOutputDeviceModel  ;
   delete d_ptr->m_pAudioManagerModel  ;
   delete d_ptr->m_pRingtoneDeviceModel;
}

///Singleton
Audio::Settings* Audio::Settings::instance()
{
   if (!m_spInstance)
      m_spInstance = new Settings();
   return m_spInstance;
}


///Return plugin model (alsa only for the time being)
Audio::AlsaPluginModel* Audio::Settings::alsaPluginModel() const
{
   if (!d_ptr->m_pAlsaPluginModel)
      d_ptr->m_pAlsaPluginModel = new Audio::AlsaPluginModel(this);
   return d_ptr->m_pAlsaPluginModel;
}


///Return the input device model
Audio::InputDeviceModel* Audio::Settings::inputDeviceModel() const
{
   if (!d_ptr->m_pInputDeviceModel)
      d_ptr->m_pInputDeviceModel = new Audio::InputDeviceModel(this);
   return d_ptr->m_pInputDeviceModel;
}

///Return the output device model
Audio::OutputDeviceModel* Audio::Settings::outputDeviceModel() const
{
   if (!d_ptr->m_pOutputDeviceModel)
      d_ptr->m_pOutputDeviceModel   = new Audio::OutputDeviceModel(this);
   return d_ptr->m_pOutputDeviceModel;
}

///Return audio manager
Audio::ManagerModel* Audio::Settings::managerModel() const
{
   if (!d_ptr->m_pAudioManagerModel)
      d_ptr->m_pAudioManagerModel = new Audio::ManagerModel(this);
   return d_ptr->m_pAudioManagerModel;
}

///Return the ringtone device model
Audio::RingtoneDeviceModel* Audio::Settings::ringtoneDeviceModel() const
{
   if (!d_ptr->m_pRingtoneDeviceModel)
      d_ptr->m_pRingtoneDeviceModel = new Audio::RingtoneDeviceModel (this);
   return d_ptr->m_pRingtoneDeviceModel;
}

///Is the room tone (globally) enabled
bool Audio::Settings::isRoomToneEnabled()
{
   return d_ptr->m_EnableRoomTone;
}

///Reload everything
void Audio::Settings::reload()
{
   d_ptr->m_pAlsaPluginModel->reload();
   d_ptr->m_pInputDeviceModel->reload();
   d_ptr->m_pOutputDeviceModel->reload();
//    m_pAudioManagerModel->reload();
   d_ptr->m_pRingtoneDeviceModel->reload();
}

///Play room tone
Audio::Settings::ToneType Audio::Settings::playRoomTone() const
{
   CallManagerInterface& callManager = DBus::CallManager::instance();
   callManager.startTone(true,static_cast<int>(Audio::Settings::ToneType::WITHOUT_MESSAGE));
   //TODO support voicemail
   return Audio::Settings::ToneType::WITHOUT_MESSAGE;
}

///Stop room tone if it is playing
void Audio::Settings::stopRoomTone() const
{
   CallManagerInterface& callManager = DBus::CallManager::instance();
   callManager.startTone(false,0);
}

///Set if the roomtone is (globally) enabled
void Audio::Settings::setEnableRoomTone(bool enable)
{
   d_ptr->m_EnableRoomTone = enable;
}

///Enable noise suppress code, may make things worst
void Audio::Settings::setNoiseSuppressState(bool enabled)
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   configurationManager.setNoiseSuppressState(enabled);
}

///Enable noise suppress code, may make things worst
bool Audio::Settings::isNoiseSuppressEnabled() const
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   return configurationManager.getNoiseSuppressState();
}

///Mute playback
void Audio::Settings::mutePlayback(bool m)
{
   DBus::ConfigurationManager::instance().mutePlayback(m);
   emit playbackMuted(m);
}

///Mute capture
void Audio::Settings::muteCapture(bool m)
{
   DBus::ConfigurationManager::instance().muteCapture(m);
   emit captureMuted(m);
}

///is mute playback
bool Audio::Settings::isPlaybackMuted() const
{
   return DBus::ConfigurationManager::instance().isPlaybackMuted();
}

///is mute capture
bool Audio::Settings::isCaptureMuted() const
{
   return DBus::ConfigurationManager::instance().isCaptureMuted();
}

///Set where the call recordings will be saved
void Audio::Settings::setRecordPath(const QUrl& path)
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   configurationManager.setRecordPath(path.toString());
}

///Return the path where recordings are going to be saved
QUrl Audio::Settings::recordPath() const
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   return QUrl(configurationManager.getRecordPath());
}

///are all calls recorded by default
bool Audio::Settings::isAlwaysRecording() const
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   return configurationManager.getIsAlwaysRecording();
}

///Set if all calls needs to be recorded
void Audio::Settings::setAlwaysRecording(bool record)
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   configurationManager.setIsAlwaysRecording   ( record );
}

int Audio::Settings::playbackVolume() const
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   return configurationManager.getVolume(DeviceKey::PLAYBACK)*100;
}

int Audio::Settings::captureVolume() const
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   return configurationManager.getVolume(DeviceKey::CAPTURE)*100;
}

void Audio::Settings::setPlaybackVolume(int volume)
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   configurationManager.setVolume(DeviceKey::PLAYBACK,volume/100.0f);
   emit playbackVolumeChanged(volume);
}

void Audio::Settings::setCaptureVolume(int volume)
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   configurationManager.setVolume(DeviceKey::CAPTURE,volume/100.0f);
   emit captureVolumeChanged(volume);
}

void Audio::Settings::setDTMFMuted(bool muted)
{
   //TODO
   DBus::ConfigurationManager::instance().muteDtmf(muted);
   emit DTMFMutedChanged(muted);
}

bool Audio::Settings::areDTMFMuted() const
{
   return DBus::ConfigurationManager::instance().isDtmfMuted();
}

///Called when the volume change for external reasons
void Audio::SettingsPrivate::slotVolumeChanged(const QString& str, double volume)
{
   if (str == Audio::Settings::DeviceKey::CAPTURE)
      emit q_ptr->captureVolumeChanged(static_cast<int>(volume*100));
   else if (str == Audio::Settings::DeviceKey::PLAYBACK)
      emit q_ptr->playbackVolumeChanged(static_cast<int>(volume*100));
   else
      qDebug() << "Unknown audio device" << str;
}

#include <settings.moc>
