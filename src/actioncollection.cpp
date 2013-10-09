/***************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                         *
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

//SFLPhone
#include "extendedaction.h"
#include "sflphone.h"
#include "accountwizard.h"
#include "sflphoneview.h"
#include "sflphoneaccessibility.h"
#include "conf/configurationdialog.h"
#include "klib/kcfg_settings.h"
#include "klib/helperfunctions.h"
#include <lib/call.h>
#include <lib/account.h>
#include <lib/accountlistmodel.h>
#include <lib/callmodel.h>
#include <lib/dbus/callmanager.h>


ActionCollection* ActionCollection::m_spInstance = nullptr;

ActionCollection* ActionCollection::instance() {
   if (!m_spInstance)
      m_spInstance = new ActionCollection();
   return m_spInstance;
}

ActionCollection::ActionCollection(QObject* parent) : QObject(parent),m_pWizard(nullptr)
{
   action_accept   = new ExtendedAction(this);
   action_refuse   = new ExtendedAction(this);
   action_hold     = new ExtendedAction(this);
   action_transfer = new ExtendedAction(this);
   action_record   = new ExtendedAction(this);
   action_mute     = new ExtendedAction(this);
   action_hangup   = new ExtendedAction(this);
   action_unhold   = new ExtendedAction(this);
   action_pickup   = new ExtendedAction(this);

   action_transfer->setAltIcon(KStandardDirs::locate("data" , "sflphone-client-kde/transfer_grayscale.png" ));
   action_record  ->setAltIcon(KStandardDirs::locate("data" , "sflphone-client-kde/record_grayscale.png"   ));
   action_hold    ->setAltIcon(KStandardDirs::locate("data" , "sflphone-client-kde/hold_grayscale.png"     ));
   action_refuse  ->setAltIcon(KStandardDirs::locate("data" , "sflphone-client-kde/refuse_grayscale.png"   ));
   action_mute    ->setAltIcon(KStandardDirs::locate("data" , "sflphone-client-kde/mutemic_grayscale.png"  ));
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
   action_mute    ->setText ( i18nc("Mute the current audio device", "Mute"     ) );
   action_accept  ->setText ( i18n("Dial"      ) );

   #ifdef ENABLE_VIDEO
   action_video_rotate_left     = new ExtendedAction(this);
   action_video_rotate_right    = new ExtendedAction(this);
   action_video_flip_horizontal = new ExtendedAction(this);
   action_video_flip_vertical   = new ExtendedAction(this);
   action_video_mute            = new ExtendedAction(this);
   action_video_preview         = new ExtendedAction(this);
   action_video_rotate_left     ->setText ( i18n( "Rotate left"  ) );
   action_video_rotate_right    ->setText ( i18n( "Rotate right" ) );
   action_video_flip_horizontal ->setText ( i18n( "Flip"         ) );
   action_video_flip_vertical   ->setText ( i18n( "Flip"         ) );
   action_video_mute            ->setText ( i18n( "Mute"         ) );
   action_video_preview         ->setText ( i18n( "Preview"      ) );
   action_video_rotate_left     ->setAltIcon(KIcon("object-rotate-left"    ));
   action_video_rotate_right    ->setAltIcon(KIcon("object-rotate-right"   ));
   action_video_flip_horizontal ->setAltIcon(KIcon("object-flip-horizontal"));
   action_video_flip_vertical   ->setAltIcon(KIcon("object-flip-vertical"  ));
   action_video_mute            ->setAltIcon(KIcon("camera-web"            ));
   action_video_preview         ->setAltIcon(KIcon("view-preview"          ));
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
   action_configureSflPhone->setText(i18n("Configure SFLphone"));

   action_displayDialpad        = new KAction(KIcon(QIcon(ICON_DISPLAY_DIALPAD)), i18n("Display dialpad")                 , this);
   action_displayMessageBox     = new KAction(KIcon("mail-message-new"), i18n("Display text message box")                 , this);
   action_displayVolumeControls = new KAction(KIcon(QIcon(ICON_DISPLAY_VOLUME_CONSTROLS)), i18n("Display volume controls"), this);
   action_pastenumber           = new KAction(KIcon("edit-paste"), i18n("Paste")                                          , this);
   action_showContactDock       = new KAction(KIcon("edit-find-user")   , i18n("Display Contact")                         , this);
   action_showHistoryDock       = new KAction(KIcon("view-history")     , i18n("Display history")                         , this);
   action_showBookmarkDock      = new KAction(KIcon("bookmark-new-list"), i18n("Display bookmark")                        , this);
   action_editToolBar           = new KAction(KIcon("configure-toolbars"), i18n("Configure Toolbars")                     , this);
   action_accountCreationWizard = new KAction(i18n("Account creation wizard")                                             , this);

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
   
   action_mute->setCheckable(true);



   action_configureShortcut = new KAction(KIcon(KIcon("configure-shortcuts")), i18n("Configure Shortcut"), this);
   //                    SENDER                        SIGNAL               RECEIVER                 SLOT               /
   /**/connect(action_accept,                SIGNAL(triggered()),           this    , SLOT(accept())                    );
   /**/connect(action_hangup,                SIGNAL(triggered()),           this    , SLOT(hangup())                    );
   /**/connect(action_refuse,                SIGNAL(triggered()),           this    , SLOT(refuse())                    );
   /**/connect(action_hold,                  SIGNAL(triggered()),           this    , SLOT(hold())                      );
   /**/connect(action_unhold,                SIGNAL(triggered()),           this    , SLOT(unhold())                    );
   /**/connect(action_transfer,              SIGNAL(triggered()),           this    , SLOT(transfer())                  );
   /**/connect(action_record,                SIGNAL(triggered()),           this    , SLOT(record())                    );
   /**/connect(action_mute,                  SIGNAL(toggled(bool)),         this    , SLOT(mute(bool))                  );
   /**/connect(action_mailBox,               SIGNAL(triggered()),           this    , SLOT(mailBox())                   );
   /**/connect(action_displayVolumeControls, SIGNAL(toggled(bool)),         SFLPhone::view() , SLOT(displayVolumeControls(bool)) );
   /**/connect(action_displayDialpad,        SIGNAL(toggled(bool)),         SFLPhone::view() , SLOT(displayDialpad(bool))        );
   /**/connect(action_displayMessageBox,     SIGNAL(toggled(bool)),         SFLPhone::view() , SLOT(displayMessageBox(bool))     );
   /**/connect(action_accountCreationWizard, SIGNAL(triggered()),           this    , SLOT(accountCreationWizard())     );
   /**/connect(action_pastenumber,           SIGNAL(triggered()),           SFLPhone::view() , SLOT(paste())             );
   /**/connect(action_configureShortcut,     SIGNAL(triggered()),           this    , SLOT(showShortCutEditor())        );
   /**/connect(action_editToolBar,           SIGNAL(triggered()),           this    , SLOT(editToolBar())               );
   /**/connect(MacroModel::instance(),       SIGNAL(addAction(KAction*)),   this    , SLOT(addMacro(KAction*))          );
   /*                                                                                                                   */

   SFLPhone::app()->actionCollection()->addAction("action_accept"                , action_accept                );
   SFLPhone::app()->actionCollection()->addAction("action_refuse"                , action_refuse                );
   SFLPhone::app()->actionCollection()->addAction("action_hold"                  , action_hold                  );
   SFLPhone::app()->actionCollection()->addAction("action_transfer"              , action_transfer              );
   SFLPhone::app()->actionCollection()->addAction("action_record"                , action_record                );
   SFLPhone::app()->actionCollection()->addAction("action_mailBox"               , action_mailBox               );
   SFLPhone::app()->actionCollection()->addAction("action_close"                 , action_close                 );
   SFLPhone::app()->actionCollection()->addAction("action_quit"                  , action_quit                  );
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

   MacroModel::instance()->initMacros();


   QList<KAction*> acList = *SFLPhoneAccessibility::instance();

   foreach(KAction* ac,acList) {
      SFLPhone::app()->actionCollection()->addAction(ac->objectName() , ac);
   }

}

///Call
void ActionCollection::accept() //TODO dead code?
{
   Call* call = SFLPhone::view()->currentCall();// SFLPhone::view()->currentCall();
   if(!call) {
      kDebug() << "Calling when no item is selected. Opening an item.";
      Call* newCall = CallModel::instance()->addDialingCall();
      const QModelIndex& newCallIdx = CallModel::instance()->getIndex(newCall);
      if (newCallIdx.isValid()) {
         SFLPhone::view()->setCurrentIndex(newCallIdx);
      }
   }
   else {
      const Call::State state = call->state();
      if (state == Call::State::RINGING || state == Call::State::CURRENT || state == Call::State::HOLD || state == Call::State::BUSY) {
         kDebug() << "Calling when item currently ringing, current, hold or busy. Opening an item.";
         Call* newCall = CallModel::instance()->addDialingCall();
         const QModelIndex& newCallIdx = CallModel::instance()->getIndex(newCall);
         if (newCallIdx.isValid()) {
            SFLPhone::view()->setCurrentIndex(newCallIdx);
         }
      }
      else {
         try {
            call->actionPerformed(Call::Action::ACCEPT);
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
         call->actionPerformed(Call::Action::REFUSE);
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
         call->actionPerformed(Call::Action::REFUSE);
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
         call->actionPerformed(Call::Action::HOLD);
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
         call->actionPerformed(Call::Action::HOLD);
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
         call->actionPerformed(Call::Action::TRANSFER);
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
         call->actionPerformed(Call::Action::RECORD);
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
   Call* call = CallModel::instance()->addDialingCall();
   if (call) {
      call->appendText(mailBoxNumber);
      try {
         call->actionPerformed(Call::Action::ACCEPT);
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
