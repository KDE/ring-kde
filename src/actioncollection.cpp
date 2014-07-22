/***************************************************************************
 *   Copyright (C) 2009-2014 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/
#include "actioncollection.h"

//Qt
#include <QtGui/QAction>
#include <QtCore/QPointer>

//KDE
#include <KAction>
#include <KStandardDirs>
#include <KLocale>
#include <KDebug>
#include <KMessageBox>
#include <KStandardAction>
#include <KShortcutsDialog>
#include <KActionCollection>
#include <KEditToolBar>
#include <KGlobal>
#include <KSharedConfig>

//SFLPhone
#include "extendedaction.h"
#include "sflphone.h"
#include "accountwizard.h"
#include "sflphoneview.h"
#include "sflphoneaccessibility.h"
#include "conf/configurationdialog.h"
#include "klib/kcfg_settings.h"
#include "klib/helperfunctions.h"
#include "klib/macromodel.h"
#include <lib/call.h>
#include <lib/account.h>
#include <lib/accountlistmodel.h>
#include <lib/callmodel.h>
#include <lib/audiosettingsmodel.h>
#include <lib/contactmodel.h>


ActionCollection* ActionCollection::m_spInstance = nullptr;

ActionCollection* ActionCollection::instance() {
   if (!m_spInstance)
      m_spInstance = new ActionCollection();
   return m_spInstance;
}

ActionCollection::ActionCollection(QObject* parent) : QObject(parent),m_pWizard(nullptr),
action_mailBox(nullptr), action_close(nullptr), action_quit(nullptr), action_displayVolumeControls(nullptr),
action_displayDialpad(nullptr), action_displayMessageBox(nullptr), action_configureSflPhone(nullptr),
action_configureShortcut(nullptr), action_accountCreationWizard(nullptr), action_pastenumber(nullptr),
action_showContactDock(nullptr), action_showHistoryDock(nullptr), action_showBookmarkDock(nullptr),
action_editToolBar(nullptr), action_addContact(nullptr), action_screen(nullptr)
{
   action_accept   = new ExtendedAction(this);
   action_refuse   = new ExtendedAction(this);
   action_hold     = new ExtendedAction(this);
   action_transfer = new ExtendedAction(this);
   action_record   = new ExtendedAction(this);
   action_mute_capture     = new ExtendedAction(this);
   action_mute_playback     = new ExtendedAction(this);
   action_hangup   = new ExtendedAction(this);
   action_unhold   = new ExtendedAction(this);
   action_pickup   = new ExtendedAction(this);

   action_transfer->setAltIcon(KStandardDirs::locate("data" , "sflphone-client-kde/transfer_grayscale.png" ));
   action_record  ->setAltIcon(KStandardDirs::locate("data" , "sflphone-client-kde/record_grayscale.png"   ));
   action_hold    ->setAltIcon(KStandardDirs::locate("data" , "sflphone-client-kde/hold_grayscale.png"     ));
   action_refuse  ->setAltIcon(KStandardDirs::locate("data" , "sflphone-client-kde/refuse_grayscale.png"   ));
   action_mute_capture    ->setAltIcon(KStandardDirs::locate("data" , "sflphone-client-kde/mutemic_grayscale.png"  ));
   action_hangup  ->setAltIcon(KStandardDirs::locate("data" , "sflphone-client-kde/hangup_grayscale.png"   ));
   action_unhold  ->setAltIcon(KStandardDirs::locate("data" , "sflphone-client-kde/unhold_grayscale.png"   ));
   action_pickup  ->setAltIcon(KStandardDirs::locate("data" , "sflphone-client-kde/pickup_grayscale.png"   ));
   action_accept  ->setAltIcon(KStandardDirs::locate("data" , "sflphone-client-kde/pickup_grayscale.png"   ));

   action_transfer->setText ( i18n( "Transfer" ) );
   action_record  ->setText ( i18n( "Record"   ) );
   action_hold    ->setText ( i18n( "Hold"     ) );
   action_refuse  ->setText ( i18n( "Refuse"   ) );
   action_hangup  ->setText ( i18n( "Hang up"  ) );
   action_unhold  ->setText ( i18n( "Unhold"   ) );
   action_pickup  ->setText ( i18n( "Pickup"   ) );
   action_mute_capture    ->setText ( i18nc("Mute the current audio capture device", "Mute"     ) );
   action_mute_playback    ->setText ( i18nc("Mute the current audio playback device", "Mute Playback"     ) );
   action_accept  ->setText ( i18n("Dial"      ) );

   #ifdef ENABLE_VIDEO
   action_video_rotate_left     = new ExtendedAction(this);
   action_video_rotate_right    = new ExtendedAction(this);
   action_video_flip_horizontal = new ExtendedAction(this);
   action_video_flip_vertical   = new ExtendedAction(this);
   action_video_mute            = new ExtendedAction(this);
   action_video_preview         = new ExtendedAction(this);
   action_video_scale           = new ExtendedAction(this);
   action_video_fullscreen      = new ExtendedAction(this);
   action_video_rotate_left     ->setText ( i18n( "Rotate left"  ) );
   action_video_rotate_right    ->setText ( i18n( "Rotate right" ) );
   action_video_flip_horizontal ->setText ( i18n( "Flip"         ) );
   action_video_flip_vertical   ->setText ( i18n( "Flip"         ) );
   action_video_mute            ->setText ( i18n( "Mute"         ) );
   action_video_preview         ->setText ( i18n( "Preview"      ) );
   action_video_scale           ->setText ( i18n( "Keep aspect ratio"      ));
   action_video_fullscreen      ->setText ( i18n( "Fullscreen"             ));
   action_video_rotate_left     ->setAltIcon(KIcon("object-rotate-left"    ));
   action_video_rotate_right    ->setAltIcon(KIcon("object-rotate-right"   ));
   action_video_flip_horizontal ->setAltIcon(KIcon("object-flip-horizontal"));
   action_video_flip_vertical   ->setAltIcon(KIcon("object-flip-vertical"  ));
   action_video_mute            ->setAltIcon(KIcon("camera-web"            ));
   action_video_preview         ->setAltIcon(KIcon("view-preview"          ));
   action_video_scale           ->setAltIcon(KIcon("transform-scale"       ));
   action_video_fullscreen      ->setAltIcon(KIcon("view-fullscreen"       ));
   action_video_rotate_left     ->setIcon(KIcon("object-rotate-left"    ));
   action_video_rotate_right    ->setIcon(KIcon("object-rotate-right"   ));
   action_video_flip_horizontal ->setIcon(KIcon("object-flip-horizontal"));
   action_video_flip_vertical   ->setIcon(KIcon("object-flip-vertical"  ));
   action_video_mute            ->setIcon(KIcon("camera-web"            ));
   action_video_preview         ->setIcon(KIcon("view-preview"          ));
   action_video_scale           ->setIcon(KIcon("transform-scale"       ));
   action_video_fullscreen      ->setIcon(KIcon("view-fullscreen"       ));
   action_video_preview         ->setCheckable(true);
   action_video_scale           ->setCheckable(true);
   action_video_fullscreen      ->setCheckable(true);
   action_video_mute            ->setCheckable(true);

   action_video_scale           ->setChecked(ConfigurationSkeleton::keepVideoAspectRatio());
   action_video_preview         ->setChecked(ConfigurationSkeleton::displayVideoPreview());
   #endif
}

ActionCollection::~ActionCollection()
{
   delete action_accept                ;
   delete action_refuse                ;
   delete action_hold                  ;
   delete action_transfer              ;
   delete action_record                ;
   delete action_mailBox               ;
   delete action_close                 ;
   delete action_quit                  ;
   delete action_displayVolumeControls ;
   delete action_displayDialpad        ;
   delete action_displayMessageBox     ;
   delete action_configureSflPhone     ;
   delete action_configureShortcut     ;
   delete action_accountCreationWizard ;
   delete action_pastenumber           ;
   delete action_showContactDock       ;
   delete action_showHistoryDock       ;
   delete action_showBookmarkDock      ;
   delete action_editToolBar           ;
   delete action_mute_playback         ;
   delete action_mute_capture          ;
#ifdef ENABLE_VIDEO
   delete action_video_fullscreen      ;
#endif
}

void ActionCollection::setupAction()
{
   kDebug() << "setupActions";

   action_mailBox  = new KAction(SFLPhone::app());
   action_accept->setShortcut      ( Qt::CTRL + Qt::Key_A );
   action_refuse->setShortcut      ( Qt::CTRL + Qt::Key_D );
   action_hold->setShortcut        ( Qt::CTRL + Qt::Key_H );
   action_transfer->setShortcut    ( Qt::CTRL + Qt::Key_T );
   action_record->setShortcut      ( Qt::CTRL + Qt::Key_R );
   action_mailBox->setShortcut     ( Qt::CTRL + Qt::Key_M );

   action_screen = new QActionGroup(SFLPhone::app());
   action_screen->setExclusive(true);

   action_close = KStandardAction::close(SFLPhone::app(), SLOT(close()), SFLPhone::app());
   action_quit  = KStandardAction::quit(SFLPhone::app(), SLOT(quitButton()), SFLPhone::app());

   action_configureSflPhone = KStandardAction::preferences(this, SLOT(configureSflPhone()), SFLPhone::app());
   action_configureSflPhone->setText(i18n("Configure SFLPhone-KDE"));

   action_displayDialpad        = new KAction(KIcon(QIcon(ICON_DISPLAY_DIALPAD)), i18n("Display dialpad")                 , this);
   action_displayMessageBox     = new KAction(KIcon("mail-message-new"), i18n("Display text message box")                 , this);
   action_displayVolumeControls = new KAction(KIcon(QIcon(ICON_DISPLAY_VOLUME_CONSTROLS)), i18n("Display volume controls"), this);
   action_pastenumber           = new KAction(KIcon("edit-paste"), i18n("Paste")                                          , this);
   action_showContactDock       = new KAction(KIcon("edit-find-user")   , i18n("Display Contact")                         , this);
   action_showHistoryDock       = new KAction(KIcon("view-history")     , i18n("Display history")                         , this);
   action_showBookmarkDock      = new KAction(KIcon("bookmark-new-list"), i18n("Display bookmark")                        , this);
   action_editToolBar           = new KAction(KIcon("configure-toolbars"), i18n("Configure Toolbars")                     , this);
   action_accountCreationWizard = new KAction(i18n("Account creation wizard")                                             , this);
   action_addContact            = new KAction(KIcon("contact-new"),i18n("Add new contact")                                                     , this);

   action_addContact->setShortcut ( Qt::CTRL + Qt::Key_N );

   action_displayDialpad->setCheckable( true );
   action_displayDialpad->setChecked  ( ConfigurationSkeleton::displayDialpad() );
   action_configureSflPhone->setText(i18n("Configure SFLphone"));

   action_displayMessageBox->setCheckable( true );
   action_displayMessageBox->setChecked  ( ConfigurationSkeleton::displayMessageBox() );

   action_displayVolumeControls->setCheckable( true );
   action_displayVolumeControls->setChecked  ( ConfigurationSkeleton::displayVolume() );

   action_pastenumber->setShortcut ( Qt::CTRL + Qt::Key_V );

   action_showContactDock->setCheckable( true );
   action_showContactDock->setChecked(ConfigurationSkeleton::displayContactDock());

   action_showHistoryDock->setCheckable( true );
   action_showHistoryDock->setChecked(ConfigurationSkeleton::displayHistoryDock());

   action_showBookmarkDock->setCheckable( true );
   action_showBookmarkDock->setChecked(ConfigurationSkeleton::displayBookmarkDock());

   action_mute_capture->setCheckable(true);
   connect(action_mute_capture,SIGNAL(toggled(bool)),AudioSettingsModel::instance(),SLOT(muteCapture(bool)));
   connect(AudioSettingsModel::instance(),SIGNAL(captureMuted(bool)),action_mute_capture,SLOT(setChecked(bool)));
   action_mute_playback->setCheckable(true);
   connect(action_mute_playback,SIGNAL(toggled(bool)),AudioSettingsModel::instance(),SLOT(mutePlayback(bool)));
   connect(AudioSettingsModel::instance(),SIGNAL(playbackMuted(bool)),action_mute_playback,SLOT(setChecked(bool)));



   action_configureShortcut = new KAction(KIcon(KIcon("configure-shortcuts")), i18n("Configure Shortcut"), this);
   //                    SENDER                        SIGNAL               RECEIVER                 SLOT               /
   /**/connect(action_accept,                SIGNAL(triggered()),           this    , SLOT(accept())                    );
   /**/connect(action_hangup,                SIGNAL(triggered()),           this    , SLOT(hangup())                    );
   /**/connect(action_refuse,                SIGNAL(triggered()),           this    , SLOT(refuse())                    );
   /**/connect(action_hold,                  SIGNAL(triggered()),           this    , SLOT(hold())                      );
   /**/connect(action_unhold,                SIGNAL(triggered()),           this    , SLOT(unhold())                    );
   /**/connect(action_transfer,              SIGNAL(triggered()),           this    , SLOT(transfer())                  );
   /**/connect(action_record,                SIGNAL(triggered()),           this    , SLOT(record())                    );
   /**/connect(action_mailBox,               SIGNAL(triggered()),           this    , SLOT(mailBox())                   );
   /**/connect(action_pickup,                SIGNAL(triggered()),           this    , SLOT(accept())                    );
   /**/connect(action_displayVolumeControls, SIGNAL(toggled(bool)),         SFLPhone::view() , SLOT(displayVolumeControls(bool)) );
   /**/connect(action_displayDialpad,        SIGNAL(toggled(bool)),         SFLPhone::view() , SLOT(displayDialpad(bool))        );
   /**/connect(action_displayMessageBox,     SIGNAL(toggled(bool)),         SFLPhone::view() , SLOT(displayMessageBox(bool))     );
   /**/connect(action_accountCreationWizard, SIGNAL(triggered()),           this    , SLOT(accountCreationWizard())     );
   /**/connect(action_pastenumber,           SIGNAL(triggered()),           SFLPhone::view() , SLOT(paste())            );
   /**/connect(action_configureShortcut,     SIGNAL(triggered()),           this    , SLOT(showShortCutEditor())        );
   /**/connect(action_editToolBar,           SIGNAL(triggered()),           this    , SLOT(editToolBar())               );
   /**/connect(action_addContact,            SIGNAL(triggered()),           this    , SLOT(slotAddContact())            );
   /**/connect(MacroModel::instance(),       SIGNAL(addAction(KAction*)),   this    , SLOT(addMacro(KAction*))          );
   /*                                                                                                                   */

   connect(AudioSettingsModel::instance(),SIGNAL(captureVolumeChanged(int)),this,SLOT(updateRecordButton()));
   connect(AudioSettingsModel::instance(),SIGNAL(playbackVolumeChanged(int)),this,SLOT(updateVolumeButton()));

//    SFLPhone::app()->actionCollection()->setConfigGlobal(true);
   SFLPhone::app()->actionCollection()->addAction("action_accept"                , action_accept                );
   SFLPhone::app()->actionCollection()->addAction("action_refuse"                , action_refuse                );
   SFLPhone::app()->actionCollection()->addAction("action_hold"                  , action_hold                  );
   SFLPhone::app()->actionCollection()->addAction("action_transfer"              , action_transfer              );
   SFLPhone::app()->actionCollection()->addAction("action_record"                , action_record                );
   SFLPhone::app()->actionCollection()->addAction("action_mailBox"               , action_mailBox               );
   SFLPhone::app()->actionCollection()->addAction("action_close"                 , action_close                 );
   SFLPhone::app()->actionCollection()->addAction("action_quit"                  , action_quit                  );
   SFLPhone::app()->actionCollection()->addAction("action_pickup"                , action_pickup                );
   SFLPhone::app()->actionCollection()->addAction("action_displayVolumeControls" , action_displayVolumeControls );
   SFLPhone::app()->actionCollection()->addAction("action_displayDialpad"        , action_displayDialpad        );
   SFLPhone::app()->actionCollection()->addAction("action_displayMessageBox"     , action_displayMessageBox     );
   SFLPhone::app()->actionCollection()->addAction("action_configureSflPhone"     , action_configureSflPhone     );
   SFLPhone::app()->actionCollection()->addAction("action_accountCreationWizard" , action_accountCreationWizard );
   SFLPhone::app()->actionCollection()->addAction("action_configureShortcut"     , action_configureShortcut     );
   SFLPhone::app()->actionCollection()->addAction("action_pastenumber"           , action_pastenumber           );
   SFLPhone::app()->actionCollection()->addAction("action_showContactDock"       , action_showContactDock       );
   SFLPhone::app()->actionCollection()->addAction("action_showHistoryDock"       , action_showHistoryDock       );
   SFLPhone::app()->actionCollection()->addAction("action_showBookmarkDock"      , action_showBookmarkDock      );
   SFLPhone::app()->actionCollection()->addAction("action_editToolBar"           , action_editToolBar           );
   SFLPhone::app()->actionCollection()->addAction("action_addContact"            , action_addContact            );
   SFLPhone::app()->actionCollection()->addAction("action_mute_capture"          , action_mute_capture          );
   SFLPhone::app()->actionCollection()->addAction("action_mute_playback"         , action_mute_playback         );

   MacroModel::instance()->initMacros();

   QList<KAction*> acList = *SFLPhoneAccessibility::instance();

   foreach(KAction* ac,acList) {
      SFLPhone::app()->actionCollection()->addAction(ac->objectName() , ac);
   }
//    qDebug() << "\n\n\nGlobal" << KGlobal::config()->groupList();
//    KConfigGroup g = KGlobal::config()->group("KShortcutsDialog Settings");
//    SFLPhone::app()->actionCollection()->exportGlobalShortcuts(&g);

   updateRecordButton();
   updateVolumeButton();
}

///Call
void ActionCollection::accept() //TODO dead code?
{
   Call* call = SFLPhone::view()->currentCall();// SFLPhone::view()->currentCall();
   if(!call) {
      kDebug() << "Calling when no item is selected. Opening an item.";
      CallModel::instance()->dialingCall();
      SFLPhone::view()->selectDialingCall();
      SFLPhone::view()->updateWindowCallState();
      SFLPhone::app()->selectCallTab();
   }
   else {
      const Call::State state = call->state();
      //TODO port to lifeCycle code
      if (state == Call::State::RINGING || state == Call::State::CURRENT || state == Call::State::HOLD
         || state == Call::State::BUSY || state == Call::State::FAILURE || state == Call::State::ERROR) {
         kDebug() << "Calling when item currently ringing, current, hold or busy. Opening an item.";
         CallModel::instance()->dialingCall();
         SFLPhone::view()->selectDialingCall();
         SFLPhone::view()->updateWindowCallState();
      }
      else {
         try {
            call->performAction(Call::Action::ACCEPT);
         }
         catch(const char * msg) {
            KMessageBox::error(SFLPhone::app(),i18n(msg));
         }
         emit windowStateChanged();
      }
   }
} //accept

///Call
void ActionCollection::hangup()
{
   Call* call = SFLPhone::view()->currentCall();
   if (call) {
      try {
         call->performAction(Call::Action::REFUSE);
      }
      catch(const char * msg) {
         KMessageBox::error(SFLPhone::app(),i18n(msg));
      }
      emit windowStateChanged();
   }
} //hangup

///Refuse call
void ActionCollection::refuse()
{
   Call* call = SFLPhone::view()->currentCall();
   if(!call) {
      kDebug() << "Error : Hanging up when no item selected. Should not happen.";
   }
   else {
      try {
         call->performAction(Call::Action::REFUSE);
      }
      catch(const char * msg) {
         KMessageBox::error(SFLPhone::app(),i18n(msg));
      }
      emit windowStateChanged();
   }
}

///Put call on hold
void ActionCollection::hold()
{
   Call* call = SFLPhone::view()->currentCall();
   if(!call) {
      kDebug() << "Error : Holding when no item selected. Should not happen.";
   }
   else {
      try {
         call->performAction(Call::Action::HOLD);
      }
      catch(const char * msg) {
         KMessageBox::error(SFLPhone::app(),i18n(msg));
      }
      emit windowStateChanged();
   }
}

///Remove call from hold
void ActionCollection::unhold()
{
   Call* call = SFLPhone::view()->currentCall();
   if(!call) {
      kDebug() << "Error : Un-Holding when no item selected. Should not happen.";
   }
   else {
      try {
         call->performAction(Call::Action::HOLD);
      }
      catch(const char * msg) {
         KMessageBox::error(SFLPhone::app(),i18n(msg));
      }
      emit windowStateChanged();
   }
}

///Transfer a call
void ActionCollection::transfer()
{
   Call* call = SFLPhone::view()->currentCall();
   if(!call) {
      kDebug() << "Error : Transferring when no item selected. Should not happen.";
   }
   else {
      try {
         call->performAction(Call::Action::TRANSFER);
      }
      catch(const char * msg) {
         KMessageBox::error(SFLPhone::app(),i18n(msg));
      }
      emit windowStateChanged();
   }
}

///Record a call
void ActionCollection::record()
{
   Call* call = SFLPhone::view()->currentCall();
   if(!call) {
      kDebug() << "Error : Recording when no item selected. Should not happen.";
   }
   else {
      try {
         call->performAction(Call::Action::RECORD);
      }
      catch(const char * msg) {
         KMessageBox::error(SFLPhone::app(),i18n(msg));
      }
      emit windowStateChanged();
   }
}

///Access the voice mail list
void ActionCollection::mailBox()
{
   Account* account = AccountListModel::currentAccount();
   const QString mailBoxNumber = account->mailbox();
   Call* call = CallModel::instance()->dialingCall();
   if (call) {
      call->reset();
      call->appendText(mailBoxNumber);
      try {
         call->performAction(Call::Action::ACCEPT);
      }
      catch(const char * msg) {
         KMessageBox::error(SFLPhone::app(),i18n(msg));
      }
      emit windowStateChanged();
   }
   else {
      HelperFunctions::displayNoAccountMessageBox(SFLPhone::view());
   }
}

///Show the configuration dialog
void ActionCollection::configureSflPhone()
{
   QPointer<ConfigurationDialog> configDialog = new ConfigurationDialog(SFLPhone::view());
   configDialog->setModal(true);

   connect(configDialog, SIGNAL(changesApplied()), SFLPhone::view(), SLOT(loadWindow()));
   configDialog->exec();
   disconnect(configDialog, SIGNAL(changesApplied()), SFLPhone::view(), SLOT(loadWindow()));
   delete configDialog;
}

///Show the account creation wizard
void ActionCollection::accountCreationWizard()
{
   if (!m_pWizard) {
      m_pWizard = new AccountWizard(SFLPhone::view());
      m_pWizard->setModal(false);
   }
   m_pWizard->show();
}

///Display the shortcuts dialog
void ActionCollection::showShortCutEditor() {
   KShortcutsDialog::configure( SFLPhone::app()->actionCollection() );
}

///Show the toolbar editor
void ActionCollection::editToolBar()
{
   QPointer<KEditToolBar> toolbareditor = new KEditToolBar(SFLPhone::app()->guiFactory());
   toolbareditor->setModal(true);
   toolbareditor->exec();
   toolbareditor->setDefaultToolBar("mainToolBar");
   delete toolbareditor;
}

///Add a new dynamic action (macro)
void ActionCollection::addMacro(KAction* newAction)
{
   SFLPhone::app()->actionCollection()->addAction(newAction->objectName() , newAction );
}


/*****************************************************************************
 *                                                                           *
 *                                  Getters                                  *
 *                                                                           *
 ****************************************************************************/

ExtendedAction* ActionCollection::holdAction    ()
{
   return action_hold;
}

ExtendedAction* ActionCollection::recordAction  ()
{
   return action_record;
}

ExtendedAction* ActionCollection::refuseAction  ()
{
   return action_refuse;
}

ExtendedAction* ActionCollection::muteCaptureAction()
{
   return action_mute_capture;
}

ExtendedAction* ActionCollection::mutePlaybackAction()
{
   return action_mute_playback;
}

ExtendedAction* ActionCollection::hangupAction  ()
{
   return action_hangup;
}

ExtendedAction* ActionCollection::unholdAction  ()
{
   return action_unhold;
}

ExtendedAction* ActionCollection::transferAction()
{
   return action_transfer;
}

ExtendedAction* ActionCollection::pickupAction  ()
{
   return action_pickup;
}

ExtendedAction* ActionCollection::acceptAction  ()
{
   return action_accept;
}

KAction* ActionCollection::displayVolumeControlsAction()
{
   return action_displayVolumeControls;
}

KAction* ActionCollection::displayDialpadAction       ()
{
   return action_displayDialpad;
}

KAction* ActionCollection::displayMessageBoxAction    ()
{
   return action_displayMessageBox;
}

KAction* ActionCollection::mailBoxAction              ()
{
   return action_mailBox;
}

KAction* ActionCollection::showContactDockAction      ()
{
   return action_showContactDock;
}

KAction* ActionCollection::showHistoryDockAction      ()
{
   return action_showHistoryDock;
}

KAction* ActionCollection::showBookmarkDockAction     ()
{
   return action_showBookmarkDock;
}

KAction* ActionCollection::quitAction                 ()
{
   return action_quit;
}


KAction* ActionCollection::addContact()
{
   return action_addContact;
}

void ActionCollection::slotAddContact()
{
   Contact* aContact = new Contact();
   ContactModel::instance()->addNewContact(aContact);
}

///Change icon of the record button
void ActionCollection::updateRecordButton()
{
   double recVol = AudioSettingsModel::instance()->captureVolume();
   static const QIcon icons[4] = {QIcon(":/images/icons/mic.svg"),QIcon(":/images/icons/mic_25.svg"),
      QIcon(":/images/icons/mic_50.svg"),QIcon(":/images/icons/mic_75.svg")};
   const int idx = (recVol/26 < 0 || recVol/26 >= 4)?0:recVol/26;
   ActionCollection::instance()->muteCaptureAction()->setIcon(icons[idx]);
   SFLPhone::view()->updateVolumeControls();
}

///Update the colunm button icon
void ActionCollection::updateVolumeButton()
{
   double sndVol = AudioSettingsModel::instance()->playbackVolume();
   static const QIcon icons[4] = {QIcon(":/images/icons/speaker.svg"),QIcon(":/images/icons/speaker_25.svg"),
      QIcon(":/images/icons/speaker_50.svg"),QIcon(":/images/icons/speaker_75.svg")};
   const int idx = (sndVol/26 < 0 || sndVol/26 >= 4)?0:sndVol/26;
   ActionCollection::instance()->mutePlaybackAction()->setIcon(icons[idx]);
   SFLPhone::view()->updateVolumeControls();
}

//Video actions
#ifdef ENABLE_VIDEO
ExtendedAction* ActionCollection::videoRotateLeftAction     ()
{
   return action_video_rotate_left;
}

ExtendedAction* ActionCollection::videoRotateRightAction    ()
{
   return action_video_rotate_right;
}

ExtendedAction* ActionCollection::videoFlipHorizontalAction ()
{
   return action_video_flip_horizontal;
}

ExtendedAction* ActionCollection::videoFlipVerticalAction   ()
{
   return action_video_flip_vertical;
}

ExtendedAction* ActionCollection::videoMuteAction           ()
{
   return action_video_mute;
}

ExtendedAction* ActionCollection::videoPreviewAction        ()
{
   return action_video_preview;
}

ExtendedAction* ActionCollection::videoScaleAction          ()
{
   return action_video_scale;
}

ExtendedAction* ActionCollection::videoFullscreenAction     ()
{
   return action_video_fullscreen;
}
#endif
