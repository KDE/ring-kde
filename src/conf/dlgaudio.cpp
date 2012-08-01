/****************************************************************************
 *   Copyright (C) 2009 by Savoir-Faire Linux                               *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>          *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
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
#include "dlgaudio.h"

//Qt
#include <QtGui/QHeaderView>

//KDE
#include <KStandardDirs>
#include <KLineEdit>

//SFLPhone
#include "lib/configurationmanager_interface_singleton.h"
#include "klib/configurationskeleton.h"
#include "conf/configurationdialog.h"
#include "lib/sflphone_const.h"

///Constructor
DlgAudio::DlgAudio(KConfigDialog *parent)
 : QWidget(parent),m_Changed(false)
{
   setupUi(this);

   ConfigurationManagerInterface& configurationManager = ConfigurationManagerInterfaceSingleton::getInstance();
   m_pAlwaysRecordCK->setChecked(configurationManager.getIsAlwaysRecording());

   KUrlRequester_destinationFolder->setMode(KFile::Directory|KFile::ExistingOnly|KFile::LocalOnly);
   KUrlRequester_destinationFolder->setUrl(KUrl(configurationManager.getRecordPath()));
   KUrlRequester_destinationFolder->lineEdit()->setObjectName("kcfg_destinationFolder");
   KUrlRequester_destinationFolder->lineEdit()->setReadOnly(true);

   connect( box_alsaPlugin, SIGNAL(activated(int)),  parent, SLOT(updateButtons()));
   connect( this,           SIGNAL(updateButtons()), parent, SLOT(updateButtons()));
   connect(m_pAlwaysRecordCK, SIGNAL(clicked(bool)), this  , SLOT(changed())      );
}

///Destructor
DlgAudio::~DlgAudio()
{
}

///Update the widgets
void DlgAudio::updateWidgets()
{
   loadAlsaSettings();
}

///Save the settings
void DlgAudio::updateSettings()
{
   //alsaPlugin
   ConfigurationSkeleton* skeleton = ConfigurationSkeleton::self();
   skeleton->setAlsaPlugin(box_alsaPlugin->currentText());
   
   ConfigurationManagerInterface& configurationManager = ConfigurationManagerInterfaceSingleton::getInstance();
   configurationManager.setRecordPath(KUrlRequester_destinationFolder->lineEdit()->text());
   configurationManager.setAudioPlugin(box_alsaPlugin->currentText());

   configurationManager.setAudioOutputDevice   ( kcfg_alsaOutputDevice->currentIndex()   );
   configurationManager.setAudioInputDevice    ( kcfg_alsaInputDevice->currentIndex()    );
   configurationManager.setAudioRingtoneDevice ( kcfg_alsaRingtoneDevice->currentIndex() );
   configurationManager.setIsAlwaysRecording   ( m_pAlwaysRecordCK->isChecked()          );
   m_Changed = false;
}

///Have this dialog changed
bool DlgAudio::hasChanged()
{
   ConfigurationSkeleton* skeleton = ConfigurationSkeleton::self();
   return (skeleton->interface() == ConfigurationSkeleton::EnumInterface::ALSA && skeleton->alsaPlugin() != box_alsaPlugin->currentText()) || m_Changed;
}

void DlgAudio::changed()
{
   m_Changed = true;
   emit updateButtons();
}

///Load alsa settings
void DlgAudio::loadAlsaSettings()
{
   ConfigurationManagerInterface& configurationManager = ConfigurationManagerInterfaceSingleton::getInstance();
   if(QString(configurationManager.getAudioManager()) == "alsa") {
//       ConfigurationSkeleton* skeleton = ConfigurationSkeleton::self();

      const QString     curPlugin      = configurationManager.getCurrentAudioOutputPlugin();
      const QStringList currentDevices = configurationManager.getCurrentAudioDevicesIndex();
      int               index          = box_alsaPlugin->findText(curPlugin);

      if(index < 0) index = 0;
      const QStringList pluginList       = configurationManager.getAudioPluginList();
      box_alsaPlugin->clear                   (                              );
      box_alsaPlugin->addItems                ( pluginList                   );
      box_alsaPlugin->setCurrentIndex         ( index                        );

      const QStringList inputDeviceList  = configurationManager.getAudioInputDeviceList  ();
      kcfg_alsaInputDevice->clear             (                              );
      kcfg_alsaInputDevice->addItems          ( inputDeviceList              );
      kcfg_alsaInputDevice->setCurrentIndex   ( currentDevices[1].toInt()    );

      const QStringList outputDeviceList = configurationManager.getAudioOutputDeviceList ();
      kcfg_alsaOutputDevice->clear            (                              );
      kcfg_alsaOutputDevice->addItems         ( outputDeviceList             );
      kcfg_alsaOutputDevice->setCurrentIndex  ( currentDevices[0].toInt()    );

      kcfg_alsaRingtoneDevice->clear          (                              );
      kcfg_alsaRingtoneDevice->addItems       ( outputDeviceList             );
      kcfg_alsaRingtoneDevice->setCurrentIndex( currentDevices[2].toInt()    );

      groupBox_alsa->setEnabled(true);
   }
   else {
      groupBox_alsa->setEnabled(false);
   }
}
