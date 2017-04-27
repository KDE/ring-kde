/****************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                          *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>          *
 *            Emmanuel Lepage Vallee <elv1313@gmail.com>                    *
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
#include <QtWidgets/QHeaderView>

//KDE
#include <KLineEdit>

//Ring
#include "klib/kcfg_settings.h"
#include "conf/configurationdialog.h"
#include "audio/settings.h"
#include "audio/inputdevicemodel.h"
#include "audio/outputdevicemodel.h"
#include "audio/ringtonedevicemodel.h"
#include "audio/managermodel.h"
#include "audio/alsapluginmodel.h"

///Constructor
DlgAudio::DlgAudio(KConfigDialog *parent)
 : QWidget(parent),m_Changed(false),m_IsLoading(false)
{
   setupUi(this);

   m_pSuppressNoise->setChecked(Audio::Settings::instance().isNoiseSuppressEnabled());
   m_pCPlayDTMFCk->setChecked(Audio::Settings::instance().areDTMFMuted());

   loadAlsaSettings();

   m_pManager           ->bindToModel(Audio::Settings::instance().managerModel       (), Audio::Settings::instance().managerModel       ()->selectionModel());
   alsaOutputDevice     ->bindToModel(Audio::Settings::instance().outputDeviceModel  (), Audio::Settings::instance().outputDeviceModel  ()->selectionModel());
   alsaRingtoneDevice   ->bindToModel(Audio::Settings::instance().ringtoneDeviceModel(), Audio::Settings::instance().ringtoneDeviceModel()->selectionModel());
   alsaInputDevice      ->bindToModel(Audio::Settings::instance().inputDeviceModel   (), Audio::Settings::instance().inputDeviceModel   ()->selectionModel());
   box_alsaPlugin       ->bindToModel(Audio::Settings::instance().alsaPluginModel    (), Audio::Settings::instance().alsaPluginModel    ()->selectionModel());

   connect( this             , SIGNAL(updateButtons()), parent, SLOT(updateButtons()));

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

      Audio::Settings::instance().setNoiseSuppressState(m_pSuppressNoise->isChecked());
      Audio::Settings::instance().setDTMFMuted         (m_pCPlayDTMFCk  ->isChecked());

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
   switch (Audio::Settings::instance().managerModel()->currentManager()) {
      case Audio::ManagerModel::Manager::PULSE:
         box_alsaPlugin->setDisabled(true);
         stackedWidget_interfaceSpecificSettings->setVisible(true);
         break;
      case Audio::ManagerModel::Manager::ALSA:
         box_alsaPlugin->setDisabled(false);
         stackedWidget_interfaceSpecificSettings->setVisible(true);
         break;
      case Audio::ManagerModel::Manager::JACK:
         box_alsaPlugin->setDisabled(true);
         stackedWidget_interfaceSpecificSettings->setVisible(false);
         break;
      case Audio::ManagerModel::Manager::ERROR:
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
   Audio::Settings::instance().reload();

   m_IsLoading = false;
}

// kate: space-indent on; indent-width 3; replace-tabs on;
