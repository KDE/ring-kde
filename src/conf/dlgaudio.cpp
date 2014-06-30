/****************************************************************************
 *   Copyright (C) 2009-2014 by Savoir-Faire Linux                          *
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
#include "klib/kcfg_settings.h"
#include "conf/configurationdialog.h"
#include "lib/sflphone_const.h"
#include "lib/audiosettingsmodel.h"

///Constructor
DlgAudio::DlgAudio(KConfigDialog *parent)
 : QWidget(parent),m_Changed(false),m_IsLoading(false)
{
   setupUi(this);

   m_pAlwaysRecordCK->setChecked(AudioSettingsModel::instance()->isAlwaysRecording());

   KUrlRequester_destinationFolder->setMode(KFile::Directory|KFile::ExistingOnly|KFile::LocalOnly);
   KUrlRequester_destinationFolder->setUrl(KUrl(AudioSettingsModel::instance()->recordPath()));
   KUrlRequester_destinationFolder->lineEdit()->setReadOnly(true);

   m_pSuppressNoise->setChecked(AudioSettingsModel::instance()->isNoiseSuppressEnabled());
   m_pCPlayDTMFCk->setChecked(AudioSettingsModel::instance()->areDTMFMuted());

   alsaInputDevice->setModel   (AudioSettingsModel::instance()->inputDeviceModel () );
   alsaOutputDevice->setModel  (AudioSettingsModel::instance()->outputDeviceModel() );
   alsaRingtoneDevice->setModel(AudioSettingsModel::instance()->outputDeviceModel() );
   m_pManager->setModel        (AudioSettingsModel::instance()->audioManagerModel() );
   box_alsaPlugin->setModel    (AudioSettingsModel::instance()->alsaPluginModel  () );
   loadAlsaSettings();

   m_pManager->setCurrentIndex (AudioSettingsModel::instance()->audioManagerModel()->currentManagerIndex().row());

   connect( box_alsaPlugin   , SIGNAL(activated(int)) , parent, SLOT(updateButtons()));
   connect( this             , SIGNAL(updateButtons()), parent, SLOT(updateButtons()));
   connect( m_pAlwaysRecordCK, SIGNAL(clicked(bool))  , this  , SLOT(changed())      );

   connect( box_alsaPlugin                  , SIGNAL(currentIndexChanged(int)) , SLOT(changed()));
   connect( m_pSuppressNoise                , SIGNAL(toggled(bool))            , SLOT(changed()));
   connect( m_pCPlayDTMFCk                  , SIGNAL(toggled(bool))            , SLOT(changed()));
   connect( alsaInputDevice                 , SIGNAL(currentIndexChanged(int)) , SLOT(changed()));
   connect( alsaOutputDevice                , SIGNAL(currentIndexChanged(int)) , SLOT(changed()));
   connect( alsaRingtoneDevice              , SIGNAL(currentIndexChanged(int)) , SLOT(changed()));
   connect( m_pManager                      , SIGNAL(currentIndexChanged(int)) , SLOT(changed()));
   connect( KUrlRequester_destinationFolder , SIGNAL(textChanged(QString))     , SLOT(changed()));
   connect( m_pManager                      , SIGNAL(currentIndexChanged(int)) , 
            AudioSettingsModel::instance()->audioManagerModel(),SLOT(setCurrentManager(int)));
   connect(AudioSettingsModel::instance()->audioManagerModel(),SIGNAL(currentManagerChanged(int)),m_pManager,
           SLOT(setCurrentIndex(int)));
   connect( m_pManager                      , SIGNAL(currentIndexChanged(int)) , SLOT(loadAlsaSettings()));
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

      AudioSettingsModel::instance()->setRecordPath(KUrlRequester_destinationFolder->lineEdit()->text());
      AudioSettingsModel::instance()->setAlwaysRecording(m_pAlwaysRecordCK->isChecked());

      AudioSettingsModel::instance()->inputDeviceModel   ()->setCurrentDevice(alsaInputDevice->currentIndex   ());
      AudioSettingsModel::instance()->outputDeviceModel  ()->setCurrentDevice(alsaOutputDevice->currentIndex  ());
      AudioSettingsModel::instance()->ringtoneDeviceModel()->setCurrentDevice(alsaRingtoneDevice->currentIndex());
      AudioSettingsModel::instance()->alsaPluginModel    ()->setCurrentPlugin(box_alsaPlugin->currentIndex());
      AudioSettingsModel::instance()->setNoiseSuppressState(m_pSuppressNoise->isChecked());
      AudioSettingsModel::instance()->setDTMFMuted         (m_pCPlayDTMFCk  ->isChecked());

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
   switch (AudioSettingsModel::instance()->audioManagerModel()->currentManager()) {
      case AudioManagerModel::Manager::PULSE:
         box_alsaPlugin->setDisabled(true);
         stackedWidget_interfaceSpecificSettings->setVisible(true);
         break;
      case AudioManagerModel::Manager::ALSA:
         box_alsaPlugin->setDisabled(false);
         stackedWidget_interfaceSpecificSettings->setVisible(true);
         break;
      case AudioManagerModel::Manager::JACK:
         box_alsaPlugin->setDisabled(true);
         stackedWidget_interfaceSpecificSettings->setVisible(false);
         break;
   };
   if (!m_IsLoading) {
      m_Changed = true;
      emit updateButtons();
   }
}

///Load alsa settings
void DlgAudio::loadAlsaSettings()
{
   m_IsLoading = true;
   AudioSettingsModel::instance()->reload();
   alsaInputDevice->setCurrentIndex    ( AudioSettingsModel::instance()->inputDeviceModel()->currentDevice().row()   );
   alsaOutputDevice->setCurrentIndex   ( AudioSettingsModel::instance()->outputDeviceModel()->currentDevice().row()  );
   alsaRingtoneDevice->setCurrentIndex ( AudioSettingsModel::instance()->ringtoneDeviceModel()->currentDevice().row());
   box_alsaPlugin->setCurrentIndex     ( AudioSettingsModel::instance()->alsaPluginModel()->currentPlugin().row()    );
   m_IsLoading = false;
}
