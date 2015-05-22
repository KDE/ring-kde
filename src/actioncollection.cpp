/***************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                         *
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
#include <QtWidgets/QAction>
#include <QPointer>

//KDE
#include <QAction>

#include <klocalizedstring.h>
#include <QDebug>
#include <kmessagebox.h>
#include <KStandardAction>
#include <KShortcutsDialog>
#include <KActionCollection>
#include <KEditToolBar>
#include <KSharedConfig>

//Ring
#include "extendedaction.h"
#include "ring.h"
#include "view.h"
#include "accessibility.h"
#include "conf/configurationdialog.h"
#include "icons/icons.h"
#include "klib/kcfg_settings.h"
#include "klib/helperfunctions.h"
#include <macromodel.h>
#include <call.h>
#include <account.h>
#include <accountmodel.h>
#include <availableaccountmodel.h>
#include <callmodel.h>
#include <implementation.h>
#include <useractionmodel.h>
#include <audio/settings.h>
#include <personmodel.h>
#include <QStandardPaths>


ActionCollection* ActionCollection::m_spInstance = nullptr;

ActionCollection* ActionCollection::instance() {
   if (!m_spInstance)
      m_spInstance = new ActionCollection();
   return m_spInstance;
}

ActionCollection::ActionCollection(QObject* parent) : QObject(parent),
action_mailBox(nullptr), action_close(nullptr), action_quit(nullptr), action_displayVolumeControls(nullptr),
action_displayDialpad(nullptr), action_displayAccountCbb(nullptr),action_displayMessageBox(nullptr), action_configureRing(nullptr),
action_configureShortcut(nullptr), action_pastenumber(nullptr),
action_showContactDock(nullptr), action_showHistoryDock(nullptr), action_showBookmarkDock(nullptr),
action_editToolBar(nullptr), action_addPerson(nullptr), action_screen(nullptr), action_new_call(nullptr)
{
   action_accept        = new ExtendedAction(this);
   action_new_call      = new ExtendedAction(this);
   action_hold          = new ExtendedAction(this);
   action_transfer      = new ExtendedAction(this);
   action_record        = new ExtendedAction(this);
   action_mute_capture  = new ExtendedAction(this);
   action_mute_playback = new ExtendedAction(this);
   action_hangup        = new ExtendedAction(this);
   action_mailBox       = new QAction(Ring::app());

   action_transfer->setAltIcon(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "ring-kde/light/transfert.svg"   ));
   action_record  ->setAltIcon(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "ring-kde/light/rec_call.svg"    ));
   action_hold    ->setAltIcon(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "ring-kde/light/hold.svg"        ));
   action_mute_capture    ->setAltIcon(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "ring-kde/light/mic.svg" ));
   action_hangup  ->setAltIcon(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "ring-kde/light/refuse.svg"      ));
   action_accept  ->setAltIcon(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "ring-kde/light/accept.svg"      ));
   action_new_call->setAltIcon(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "ring-kde/light/call.svg"      ));

   action_new_call->setIcon(QIcon(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "ring-kde/call.svg"      )));
   action_hold->setIcon(QIcon(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "ring-kde/hold.svg"      )));
   action_transfer->setIcon(QIcon(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "ring-kde/transfert.svg"      )));
   action_mailBox->setIcon(QIcon(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "ring-kde/mailbox.svg"      )));

   action_transfer->setText ( i18n( "Transfer" ) );
   action_record  ->setText ( i18n( "Record"   ) );
   action_hold    ->setText ( i18n( "Hold"     ) );
   action_hangup  ->setText ( i18n( "Hang up"  ) );
   action_mute_capture    ->setText ( i18nc("Mute the current audio capture device", "Mute"     ) );
   action_mute_playback    ->setText ( i18nc("Mute the current audio playback device", "Mute Playback"     ) );
   action_accept  ->setText ( i18n("Dial"      ) );
   action_new_call  ->setText ( i18n("New Call"      ) );

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
   action_video_rotate_left     ->setAltIcon(QIcon::fromTheme("object-rotate-left"    ));
   action_video_rotate_right    ->setAltIcon(QIcon::fromTheme("object-rotate-right"   ));
   action_video_flip_horizontal ->setAltIcon(QIcon::fromTheme("object-flip-horizontal"));
   action_video_flip_vertical   ->setAltIcon(QIcon::fromTheme("object-flip-vertical"  ));
   action_video_mute            ->setAltIcon(QIcon::fromTheme("camera-web"            ));
   action_video_preview         ->setAltIcon(QIcon::fromTheme("view-preview"          ));
   action_video_scale           ->setAltIcon(QIcon::fromTheme("transform-scale"       ));
   action_video_fullscreen      ->setAltIcon(QIcon::fromTheme("view-fullscreen"       ));
   action_video_rotate_left     ->setIcon(QIcon::fromTheme("object-rotate-left"    ));
   action_video_rotate_right    ->setIcon(QIcon::fromTheme("object-rotate-right"   ));
   action_video_flip_horizontal ->setIcon(QIcon::fromTheme("object-flip-horizontal"));
   action_video_flip_vertical   ->setIcon(QIcon::fromTheme("object-flip-vertical"  ));
   action_video_mute            ->setIcon(QIcon::fromTheme("camera-web"            ));
   action_video_preview         ->setIcon(QIcon::fromTheme("view-preview"          ));
   action_video_scale           ->setIcon(QIcon::fromTheme("transform-scale"       ));
   action_video_fullscreen      ->setIcon(QIcon::fromTheme("view-fullscreen"       ));
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
   delete action_new_call              ;
   delete action_hold                  ;
   delete action_transfer              ;
   delete action_record                ;
   delete action_mailBox               ;
   delete action_close                 ;
   delete action_quit                  ;
   delete action_displayVolumeControls ;
   delete action_displayDialpad        ;
   delete action_displayAccountCbb     ;
   delete action_displayMessageBox     ;
   delete action_configureRing         ;
   delete action_configureShortcut     ;
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
   qDebug() << "setupActions";

   action_accept->setShortcut      ( Qt::CTRL + Qt::Key_A );
   action_new_call->setShortcut    ( Qt::CTRL + Qt::Key_N );
   action_hold->setShortcut        ( Qt::CTRL + Qt::Key_H );
   action_transfer->setShortcut    ( Qt::CTRL + Qt::Key_T );
   action_record->setShortcut      ( Qt::CTRL + Qt::Key_R );
   action_mailBox->setShortcut     ( Qt::CTRL + Qt::Key_M );

   action_screen = new QActionGroup(Ring::app());
   action_screen->setExclusive(true);

   action_close = KStandardAction::close(Ring::app(), SLOT(close()), Ring::app());
   action_quit  = KStandardAction::quit(Ring::app(), SLOT(quitButton()), Ring::app());

   action_configureRing = KStandardAction::preferences(this, SLOT(configureRing()), Ring::app());
   action_configureRing->setText(i18n("Configure Ring-KDE"));

   action_displayDialpad        = new QAction(QIcon(RingIcons::DISPLAY_DIALPAD), i18n("Display dialpad")                 , this);
   action_displayAccountCbb        = new QAction(i18n("Display account selector")                 , this);
   action_displayMessageBox     = new QAction(QIcon::fromTheme("mail-message-new"), i18n("Display text message box")                 , this);
   action_displayVolumeControls = new QAction(QIcon(RingIcons::DISPLAY_VOLUME_CONSTROLS), i18n("Display volume controls"), this);
   action_pastenumber           = new QAction(QIcon::fromTheme("edit-paste"), i18n("Paste")                                          , this);
   action_showContactDock       = new QAction(QIcon::fromTheme("edit-find-user")   , i18n("Display Person")                         , this);
   action_showHistoryDock       = new QAction(QIcon::fromTheme("view-history")     , i18n("Display history")                         , this);
   action_showBookmarkDock      = new QAction(QIcon::fromTheme("bookmark-new-list"), i18n("Display bookmark")                        , this);
   action_editToolBar           = new QAction(QIcon::fromTheme("configure-toolbars"), i18n("Configure Toolbars")                     , this);
   action_addPerson             = new QAction(QIcon::fromTheme("contact-new"),i18n("Add new contact")                                                     , this);
   action_configureShortcut     = new QAction(QIcon::fromTheme("configure-shortcuts"), i18n("Configure Shortcut"), this);

   action_addPerson->setShortcut ( Qt::CTRL + Qt::Key_N );

   action_displayDialpad->setCheckable( true );
   action_displayDialpad->setChecked  ( ConfigurationSkeleton::displayDialpad() );
   action_configureRing->setText(i18n("Configure Ring-KDE"));

   action_displayAccountCbb->setCheckable( true );
   action_displayAccountCbb->setChecked  ( ConfigurationSkeleton::displayAccountBox() );

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
   connect(action_mute_capture,SIGNAL(toggled(bool)),Audio::Settings::instance(),SLOT(muteCapture(bool)));
   connect(Audio::Settings::instance(),SIGNAL(captureMuted(bool)),action_mute_capture,SLOT(setChecked(bool)));
   action_mute_playback->setCheckable(true);
   connect(action_mute_playback,SIGNAL(toggled(bool)),Audio::Settings::instance(),SLOT(mutePlayback(bool)));
   connect(Audio::Settings::instance(),SIGNAL(playbackMuted(bool)),action_mute_playback,SLOT(setChecked(bool)));

   //Bind actions to the useractionmodel
   UserActionModel* uam = CallModel::instance()->userActionModel();
   QHash<int, ExtendedAction*> actionHash;
   actionHash[ (int)UserActionModel::Action::ACCEPT          ] = action_accept;
   actionHash[ (int)UserActionModel::Action::ADD_NEW         ] = action_new_call;
   actionHash[ (int)UserActionModel::Action::HOLD            ] = action_hold;
   actionHash[ (int)UserActionModel::Action::MUTE_AUDIO      ] = action_mute_capture;
   actionHash[ (int)UserActionModel::Action::SERVER_TRANSFER ] = action_transfer;
   actionHash[ (int)UserActionModel::Action::RECORD          ] = action_record;
   actionHash[ (int)UserActionModel::Action::HANGUP          ] = action_hangup;

   for (QHash<int,ExtendedAction*>::const_iterator i = actionHash.begin(); i != actionHash.end(); ++i) {
      ExtendedAction* ea = i.value();
      UserActionModel::Action a = static_cast<UserActionModel::Action>(i.key());
      connect(ea, &QAction::triggered, [uam,a](bool) {uam << a;});
   }

   connect(uam,&UserActionModel::dataChanged, [actionHash,uam](const QModelIndex& tl, const QModelIndex& br) {
      const int first(tl.row()),last(br.row());
      for(int i = first; i <= last;i++) {
         const QModelIndex& idx = uam->index(i,0);
         ExtendedAction* a = actionHash[(int)qvariant_cast<UserActionModel::Action>(idx.data(UserActionModel::Role::ACTION))];
         if (a) {
            a->setText   ( idx.data(Qt::DisplayRole).toString()                 );
            a->setEnabled( idx.flags() & Qt::ItemIsEnabled                      );
            a->setChecked( idx.data(Qt::CheckStateRole) == Qt::Checked          );
            a->setAltIcon( qvariant_cast<QPixmap>(idx.data(Qt::DecorationRole)) );
         }
      }
   });

   /**/connect(MacroModel::instance()           ,       SIGNAL(addAction(QVariant)),  this    , SLOT(addMacro(QVariant))          );
   /**/connect(action_mailBox                   ,               SIGNAL(triggered()),           this    , SLOT(mailBox())                   );
   /**/connect(action_displayVolumeControls     ,   SIGNAL(toggled(bool)),         Ring::view() , SLOT(displayVolumeControls(bool)) );
   /**/connect(action_displayDialpad            ,        SIGNAL(toggled(bool)),         Ring::view() , SLOT(displayDialpad(bool))        );
   /**/connect(action_displayAccountCbb         ,        SIGNAL(toggled(bool)),         Ring::app() , SLOT(displayAccountCbb(bool))        );
   /**/connect(action_displayMessageBox         ,     SIGNAL(toggled(bool)),         Ring::view() , SLOT(displayMessageBox(bool))     );
   /**/connect(action_pastenumber               ,           SIGNAL(triggered()),           Ring::view() , SLOT(paste())            );
   /**/connect(action_configureShortcut         ,     SIGNAL(triggered()),           this    , SLOT(showShortCutEditor())        );
   /**/connect(action_editToolBar               ,           SIGNAL(triggered()),           this    , SLOT(editToolBar())               );
   /**/connect(action_addPerson                 ,             SIGNAL(triggered()),           this    , SLOT(slotAddPerson())            );
   /*                                                                                                                   */

   connect(Audio::Settings::instance(),SIGNAL(captureVolumeChanged(int)),this,SLOT(updateRecordButton()));
   connect(Audio::Settings::instance(),SIGNAL(playbackVolumeChanged(int)),this,SLOT(updateVolumeButton()));


//    Ring::app()->actionCollection()->setConfigGlobal(true);
   Ring::app()->actionCollection()->addAction("action_accept"                , action_accept                );
   Ring::app()->actionCollection()->addAction("action_new_call"              , action_new_call              );
   Ring::app()->actionCollection()->addAction("action_hold"                  , action_hold                  );
   Ring::app()->actionCollection()->addAction("action_transfer"              , action_transfer              );
   Ring::app()->actionCollection()->addAction("action_record"                , action_record                );
   Ring::app()->actionCollection()->addAction("action_mailBox"               , action_mailBox               );
   Ring::app()->actionCollection()->addAction("action_close"                 , action_close                 );
   Ring::app()->actionCollection()->addAction("action_quit"                  , action_quit                  );
   Ring::app()->actionCollection()->addAction("action_displayVolumeControls" , action_displayVolumeControls );
   Ring::app()->actionCollection()->addAction("action_displayDialpad"        , action_displayDialpad        );
   Ring::app()->actionCollection()->addAction("action_displayAccountCbb"     , action_displayAccountCbb     );
   Ring::app()->actionCollection()->addAction("action_displayMessageBox"     , action_displayMessageBox     );
   Ring::app()->actionCollection()->addAction("action_configureRing"         , action_configureRing         );
   Ring::app()->actionCollection()->addAction("action_configureShortcut"     , action_configureShortcut     );
   Ring::app()->actionCollection()->addAction("action_pastenumber"           , action_pastenumber           );
   Ring::app()->actionCollection()->addAction("action_showContactDock"       , action_showContactDock       );
   Ring::app()->actionCollection()->addAction("action_showHistoryDock"       , action_showHistoryDock       );
   Ring::app()->actionCollection()->addAction("action_showBookmarkDock"      , action_showBookmarkDock      );
   Ring::app()->actionCollection()->addAction("action_editToolBar"           , action_editToolBar           );
   Ring::app()->actionCollection()->addAction("action_addPerson"             , action_addPerson             );
   Ring::app()->actionCollection()->addAction("action_mute_capture"          , action_mute_capture          );
   Ring::app()->actionCollection()->addAction("action_mute_playback"         , action_mute_playback         );

   KDEShortcutDelegate::setInstance(new KDEShortcutDelegate());
   MacroModel::instance()->initMacros();

   QList<QAction *> acList = *Accessibility::instance();

   foreach(QAction * ac,acList) {
      Ring::app()->actionCollection()->addAction(ac->objectName() , ac);
   }
//    qDebug() << "\n\n\nGlobal" << KSharedConfig::openConfig()->groupList();
//    KConfigGroup g = KSharedConfig::openConfig()->group("KShortcutsDialog Settings");
//    Ring::app()->actionCollection()->exportGlobalShortcuts(&g);

   updateRecordButton();
   updateVolumeButton();
}

/*
///Call
void ActionCollection::accept() //TODO dead code?
{
   Call* call = Ring::view()->currentCall();// Ring::view()->currentCall();
   if(!call) {
      qDebug() << "Calling when no item is selected. Opening an item.";
      CallModel::instance()->dialingCall();
      Ring::view()->selectDialingCall();
      Ring::view()->updateWindowCallState();
      Ring::app()->selectCallTab();
   }
   else {
      const Call::State state = call->state();
      //TODO port to lifeCycle code
      if (state == Call::State::RINGING || state == Call::State::CURRENT || state == Call::State::HOLD
         || state == Call::State::BUSY || state == Call::State::FAILURE || state == Call::State::ERROR) {
         qDebug() << "Calling when item currently ringing, current, hold or busy. Opening an item.";
         CallModel::instance()->dialingCall();
         Ring::view()->selectDialingCall();
         Ring::view()->updateWindowCallState();
      }
      else {
         try {
            call->performAction(Call::Action::ACCEPT);
         }
         catch(const char * msg) {
            KMessageBox::error(Ring::app(),i18n(msg));
         }
         emit windowStateChanged();
      }
   }
} //accept

///Call
void ActionCollection::hangup()
{
   Call* call = Ring::view()->currentCall();
   if (call) {
      try {
         call->performAction(Call::Action::REFUSE);
      }
      catch(const char * msg) {
         KMessageBox::error(Ring::app(),i18n(msg));
      }
      emit windowStateChanged();
   }
} //hangup

///Refuse call
void ActionCollection::refuse()
{
   Call* call = Ring::view()->currentCall();
   if(!call) {
      qDebug() << "Error : Hanging up when no item selected. Should not happen.";
   }
   else {
      try {
         call->performAction(Call::Action::REFUSE);
      }
      catch(const char * msg) {
         KMessageBox::error(Ring::app(),i18n(msg));
      }
      emit windowStateChanged();
   }
}

///Put call on hold
void ActionCollection::hold()
{
   Call* call = Ring::view()->currentCall();
   if(!call) {
      qDebug() << "Error : Holding when no item selected. Should not happen.";
   }
   else {
      try {
         call->performAction(Call::Action::HOLD);
      }
      catch(const char * msg) {
         KMessageBox::error(Ring::app(),i18n(msg));
      }
      emit windowStateChanged();
   }
}

///Transfer a call
void ActionCollection::transfer()
{
   Call* call = Ring::view()->currentCall();
   if(!call) {
      qDebug() << "Error : Transferring when no item selected. Should not happen.";
   }
   else {
      try {
         call->performAction(Call::Action::TRANSFER);
      }
      catch(const char * msg) {
         KMessageBox::error(Ring::app(),i18n(msg));
      }
      emit windowStateChanged();
   }
}

///Record a call
void ActionCollection::record()
{
   Call* call = Ring::view()->currentCall();
   if(!call) {
      qDebug() << "Error : Recording when no item selected. Should not happen.";
   }
   else {
      try {
         call->performAction(Call::Action::RECORD);
      }
      catch(const char * msg) {
         KMessageBox::error(Ring::app(),i18n(msg));
      }
      emit windowStateChanged();
   }
}*/

///Access the voice mail list
void ActionCollection::mailBox()
{
   Account* account = AvailableAccountModel::currentDefaultAccount();
   const QString mailBoxNumber = account->mailbox();
   Call* call = CallModel::instance()->dialingCall();
   if (call) {
      call->reset();
      call->appendText(mailBoxNumber);
      try {
         call->performAction(Call::Action::ACCEPT);
      }
      catch(const char * msg) {
         KMessageBox::error(Ring::app(),i18n(msg));
      }
      emit windowStateChanged();
   }
   else {
      HelperFunctions::displayNoAccountMessageBox(Ring::view());
   }
}

///Show the configuration dialog
void ActionCollection::configureRing()
{
   QPointer<ConfigurationDialog> configDialog = new ConfigurationDialog(Ring::view());
   configDialog->setModal(true);

   connect(configDialog, SIGNAL(changesApplied()), Ring::view(), SLOT(loadWindow()));
   configDialog->exec();
   disconnect(configDialog, SIGNAL(changesApplied()), Ring::view(), SLOT(loadWindow()));
   delete configDialog;
}

///Display the shortcuts dialog
void ActionCollection::showShortCutEditor() {
   KShortcutsDialog::configure( Ring::app()->actionCollection() );
}

///Show the toolbar editor
void ActionCollection::editToolBar()
{
   QPointer<KEditToolBar> toolbareditor = new KEditToolBar(Ring::app()->guiFactory());
   toolbareditor->setModal(true);
   toolbareditor->exec();
   toolbareditor->setDefaultToolBar("mainToolBar");
   delete toolbareditor;
}

///Add a new dynamic action (macro)
void ActionCollection::addMacro(const QVariant& newAction)
{
   qDebug() << "\n\n\nNEW ACTION" << newAction;

   if (qvariant_cast<QAction*>(newAction))
      Ring::app()->actionCollection()->addAction(qvariant_cast<QAction*>(newAction)->objectName() , qvariant_cast<QAction*>(newAction) );
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

ExtendedAction* ActionCollection::transferAction()
{
   return action_transfer;
}

ExtendedAction* ActionCollection::acceptAction  ()
{
   return action_accept;
}

ExtendedAction* ActionCollection::newCallAction  ()
{
   return action_new_call;
}

QAction * ActionCollection::displayVolumeControlsAction()
{
   return action_displayVolumeControls;
}

QAction * ActionCollection::displayDialpadAction       ()
{
   return action_displayDialpad;
}

QAction * ActionCollection::displayAccountCbbAction   ()
{
   return action_displayAccountCbb;
}

QAction * ActionCollection::displayMessageBoxAction    ()
{
   return action_displayMessageBox;
}

QAction * ActionCollection::mailBoxAction              ()
{
   return action_mailBox;
}

QAction * ActionCollection::showContactDockAction      ()
{
   return action_showContactDock;
}

QAction * ActionCollection::showHistoryDockAction      ()
{
   return action_showHistoryDock;
}

QAction * ActionCollection::showBookmarkDockAction     ()
{
   return action_showBookmarkDock;
}

QAction * ActionCollection::quitAction                 ()
{
   return action_quit;
}


QAction * ActionCollection::addPerson()
{
   return action_addPerson;
}

void ActionCollection::slotAddPerson()
{
   Person* aPerson = new Person();
   PersonModel::instance()->addNewPerson(aPerson);
}

///Change icon of the record button
void ActionCollection::updateRecordButton()
{
   double recVol = Audio::Settings::instance()->captureVolume();
   static const QIcon icons[4] = {QIcon(":/images/icons/mic.svg"),QIcon(":/images/icons/mic_25.svg"),
      QIcon(":/images/icons/mic_50.svg"),QIcon(":/images/icons/mic_75.svg")};
   const int idx = (recVol/26 < 0 || recVol/26 >= 4)?0:recVol/26;
   ActionCollection::instance()->muteCaptureAction()->setIcon(icons[idx]);
   Ring::view()->updateVolumeControls();
}

///Update the colunm button icon
void ActionCollection::updateVolumeButton()
{
   double sndVol = Audio::Settings::instance()->playbackVolume();
   static const QIcon icons[4] = {QIcon(":/images/icons/speaker.svg"),QIcon(":/images/icons/speaker_25.svg"),
      QIcon(":/images/icons/speaker_50.svg"),QIcon(":/images/icons/speaker_75.svg")};
   const int idx = (sndVol/26 < 0 || sndVol/26 >= 4)?0:sndVol/26;
   ActionCollection::instance()->mutePlaybackAction()->setIcon(icons[idx]);
   Ring::view()->updateVolumeControls();
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
