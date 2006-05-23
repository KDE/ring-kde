/*
 *  Copyright (C) 2004-2006 Savoir-Faire Linux inc.
 *  Author: Yan Morin <yan.morin@savoirfairelinux.com>
 *  Author: Jean-Philippe Barrette-LaPierre
              <jean-philippe.barrette-lapierre@savoirfairelinux.com>
 *                                                                              
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *                                                                              
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "ConfigurationManagerImpl.hpp"
#include "DebugOutput.hpp"
#include "Session.hpp"
#include "taxidermy/Hunter.hpp"

ConfigurationManagerImpl::ConfigurationManagerImpl()
  : mSession(NULL)
{}

ConfigurationManagerImpl::~ConfigurationManagerImpl()
{
  delete mSession;
}

void 
ConfigurationManagerImpl::setCurrentSpeakerVolume(unsigned int )
{
}

void
ConfigurationManagerImpl::setCurrentMicrophoneVolume(unsigned int )
{
}

void
ConfigurationManagerImpl::setSession(const Session &session)
{
  mSession = new Session(session);
}

void
ConfigurationManagerImpl::save()
{
  if(mSession) {
    SectionMap::iterator pos = mEntries.begin();
    while(pos != mEntries.end()) {
      VariableMap::iterator vpos = pos->second.begin();
      while(vpos != pos->second.end()) {
        ConfigEntry entry(vpos->second);
        mSession->configSet(entry.section, entry.name, entry.value);
        vpos++;
      }

      pos++;
    }
    
    mSession->configSave();
  }
  else {
    DebugOutput::instance() << QObject::tr("ConfigurationManagerImpl::save(): "
					   "We don't have a valid session.\n");
  }
}

void
ConfigurationManagerImpl::finishSave()
{
  emit saved();
}

void
ConfigurationManagerImpl::add(const ConfigEntry &entry)
{
  mEntries[entry.section][entry.name] = entry;
}

void
ConfigurationManagerImpl::addAudioDevice(QString index, 
					 QString hostApiName, 
					 QString deviceName)
{
  AudioDevice device;
  device.index = index;
  device.hostApiName = hostApiName;
  device.deviceName = deviceName;
  add(device);
}

void
ConfigurationManagerImpl::add(const AudioDevice &entry)
{
  mAudioDevices.push_back(entry);
  // emit audioDevicesUpdated(); // <-- wrong call with success
}

void
ConfigurationManagerImpl::addAudioDeviceIn(QString index, 
					 QString hostApiName, 
					 QString deviceName)
{
  AudioDevice device;
  device.index = index;
  device.hostApiName = hostApiName;
  device.deviceName = deviceName;
  addIn(device);
}

void
ConfigurationManagerImpl::addIn(const AudioDevice &entry)
{
  mAudioDevicesIn.push_back(entry);
  //emit audioDevicesInUpdated(); // <-- wrong call with success()
}

void
ConfigurationManagerImpl::addAudioDeviceOut(QString index, 
					 QString hostApiName, 
					 QString deviceName)
{
  AudioDevice device;
  device.index = index;
  device.hostApiName = hostApiName;
  device.deviceName = deviceName;
  addOut(device);
}

void
ConfigurationManagerImpl::addOut(const AudioDevice &entry)
{
  mAudioDevicesOut.push_back(entry);
  //emit audioDevicesOutUpdated(); // <-- wrong call with success()
}

void
ConfigurationManagerImpl::addRingtone(QString index, QString filename)
{
  Ringtone tone;
  tone.index = index;
  tone.filename = filename;
  add(tone);
}


void
ConfigurationManagerImpl::add(const Ringtone &entry)
{
  mRingtones.push_back(entry);
  emit ringtonesUpdated();
}

void
ConfigurationManagerImpl::addCodec(QString index, QString codecName)
{
  Codec codec;
  codec.index = index;
  codec.codecName = codecName;
  add(codec);
}


void
ConfigurationManagerImpl::add(const Codec &entry)
{
  mCodecs.push_back(entry);
  emit codecsUpdated();
}

void
ConfigurationManagerImpl::set(const QString &section,
			      const QString &name,
			      const QString &value)
{
  SectionMap::iterator pos = mEntries.find(section);
  if(pos != mEntries.end()) {
    VariableMap::iterator vpos = pos->second.find(name);
    if(vpos != pos->second.end()) {
      vpos->second.value = value;
    }
  }
}

void
ConfigurationManagerImpl::save(const QString &section, const QString &name)
{
  QString value = get(section, name);
  mSession->configSet(section, name, value);
}

QString
ConfigurationManagerImpl::get(const QString &section,
			      const QString &name)
{
  QString value;
  SectionMap::iterator pos = mEntries.find(section);
  if(pos != mEntries.end()) {
    VariableMap::iterator vpos = pos->second.find(name);
    if(vpos != pos->second.end()) {
      value = vpos->second.value;
    }
  }

  return value;
}

