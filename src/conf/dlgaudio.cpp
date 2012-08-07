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
 : QWidget(parent),m_Changed(false),m_IsLoading(false)
{
   setupUi(this);

   ConfigurationManagerInterface& configurationManager = ConfigurationManagerInterfaceSingleton::getInstance();
   m_pAlwaysRecordCK->setChecked(configurationManager.getIsAlwaysRecording());

   KUrlRequester_destinationFolder->setMode(KFile::Directory|KFile::ExistingOnly|KFile::LocalOnly);
   KUrlRequester_destinationFolder->setUrl(KUrl(configurationManager.getRecordPath()));
   KUrlRequester_destinationFolder->lineEdit()->setReadOnly(true);

   connect( box_alsaPlugin, SIGNAL(activated(int)),  parent, SLOT(updateButtons()));
   connect( this,           SIGNAL(updateButtons()), parent, SLOT(updateButtons()));
   connect(m_pAlwaysRecordCK, SIGNAL(clicked(bool)), this  , SLOT(changed())      );

   connect( box_alsaPlugin                  , SIGNAL(currentIndexChanged(int)) , SLOT(changed()));
   connect( alsaInputDevice            , SIGNAL(currentIndexChanged(int)) , SLOT(changed()));
   connect( alsaOutputDevice           , SIGNAL(currentIndexChanged(int)) , SLOT(changed()));
   connect( alsaRingtoneDevice         , SIGNAL(currentIndexChanged(int)) , SLOT(changed()));
   connect( kcfg_interface                  , SIGNAL(currentIndexChanged(int)) , SLOT(changed()));
   connect( KUrlRequester_destinationFolder , SIGNAL(textChanged(QString))     , SLOT(changed()));
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
   if (m_Changed) {
      m_IsLoading = true;
      ConfigurationSkeleton* skeleton = ConfigurationSkeleton::self();
      skeleton->setAlsaPlugin(box_alsaPlugin->currentText());

      ConfigurationManagerInterface& configurationManager = ConfigurationManagerInterfaceSingleton::getInstance();
      configurationManager.setRecordPath(KUrlRequester_destinationFolder->lineEdit()->text());
      configurationManager.setAudioPlugin(box_alsaPlugin->currentText());

      configurationManager.setAudioOutputDevice   ( alsaOutputDevice->currentIndex()   );
      configurationManager.setAudioInputDevice    ( alsaInputDevice->currentIndex()    );
      configurationManager.setAudioRingtoneDevice ( alsaRingtoneDevice->currentIndex() );
      configurationManager.setIsAlwaysRecording   ( m_pAlwaysRecordCK->isChecked()          );
      m_Changed   = false;
      m_IsLoading = false;
   }
}

///Have this dialog changed
bool DlgAudio::hasChanged()
{
   return m_Changed;
}

///Tag the dialog as needing saving
void DlgAudio::changed()
{
   if (!m_IsLoading) {
      m_Changed = true;
      emit updateButtons();
   }
}

///Load alsa settings
void DlgAudio::loadAlsaSettings()
{
   m_IsLoading = true;
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
      alsaInputDevice->clear             (                              );
      alsaInputDevice->addItems          ( inputDeviceList              );
      alsaInputDevice->setCurrentIndex   ( currentDevices[1].toInt()    );

      const QStringList outputDeviceList = configurationManager.getAudioOutputDeviceList ();
      alsaOutputDevice->clear            (                              );
      alsaOutputDevice->addItems         ( outputDeviceList             );
      alsaOutputDevice->setCurrentIndex  ( currentDevices[0].toInt()    );

      alsaRingtoneDevice->clear          (                              );
      alsaRingtoneDevice->addItems       ( outputDeviceList             );
      alsaRingtoneDevice->setCurrentIndex( currentDevices[2].toInt()    );

      groupBox_alsa->setEnabled(true);
   }
   else {
      groupBox_alsa->setEnabled(false);
   }
   m_IsLoading = false;
}
