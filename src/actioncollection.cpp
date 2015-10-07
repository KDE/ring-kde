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
#include "globalinstances.h"
#include "mainwindow.h"
#include "view.h"
#include "localmacrocollection.h"
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
   action_accept        = new QAction(this);
   action_new_call      = new QAction(this);
   action_hold          = new QAction(this);
   action_transfer      = new QAction(this);
   action_record        = new QAction(this);
   action_mute_capture  = new QAction(this);
   action_mute_playback = new QAction(this);
   action_hangup        = new QAction(this);
   action_mailBox       = new QAction(MainWindow::app());

   action_new_call->setIcon(QIcon(":/images/icons/call.svg"     ));
   action_hold->setIcon    (QIcon(":/images/icons/hold.svg"     ));
   action_transfer->setIcon(QIcon(":/images/icons/transfert.svg"));
   action_mailBox->setIcon (QIcon(":/images/icons/mailbox.svg"  ));

   action_transfer->setText ( i18n( "Transfer" ) );
   action_record  ->setText ( i18n( "Record"   ) );
   action_hold    ->setText ( i18n( "Hold"     ) );
   action_hangup  ->setText ( i18n( "Hang up"  ) );
   action_mute_capture    ->setText ( i18nc("Mute the current audio capture device", "Mute"     ) );
   action_mute_playback    ->setText ( i18nc("Mute the current audio playback device", "Mute Playback"     ) );
   action_accept  ->setText ( i18n("Dial"      ) );
   action_new_call  ->setText ( i18n("New Call"      ) );

   #ifdef ENABLE_VIDEO
   action_video_rotate_left     = new QAction(this);
   action_video_rotate_right    = new QAction(this);
   action_video_flip_horizontal = new QAction(this);
   action_video_flip_vertical   = new QAction(this);
   action_video_mute            = new QAction(this);
   action_video_preview         = new QAction(this);
   action_video_scale           = new QAction(this);
   action_video_fullscreen      = new QAction(this);
   action_video_rotate_left     ->setText ( i18n( "Rotate left"  ) );
   action_video_rotate_right    ->setText ( i18n( "Rotate right" ) );
   action_video_flip_horizontal ->setText ( i18n( "Flip"         ) );
   action_video_flip_vertical   ->setText ( i18n( "Flip"         ) );
   action_video_mute            ->setText ( i18n( "Mute"         ) );
   action_video_preview         ->setText ( i18n( "Preview"      ) );
   action_video_scale           ->setText ( i18n( "Keep aspect ratio"      ));
   action_video_fullscreen      ->setText ( i18n( "Fullscreen"             ));
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

   action_screen = new QActionGroup(MainWindow::app());
   action_screen->setExclusive(true);

   action_close = KStandardAction::close(MainWindow::app(), SLOT(close()), MainWindow::app());
   action_quit  = KStandardAction::quit(MainWindow::app(), SLOT(quitButton()), MainWindow::app());

   action_configureRing = KStandardAction::preferences(this, SLOT(configureRing()), MainWindow::app());
   action_configureRing->setText(i18n("Configure Ring-KDE"));

   action_displayDialpad        = new QAction(QIcon(RingIcons::DISPLAY_DIALPAD), i18n("Display dialpad")                 , this);
   action_displayAccountCbb     = new QAction(i18n("Display account selector")                 , this);
   action_displayMessageBox     = new QAction(QIcon::fromTheme("mail-message-new"), i18n("Display text message box")                 , this);
   action_displayVolumeControls = new QAction(QIcon(RingIcons::DISPLAY_VOLUME_CONSTROLS), i18n("Display volume controls"), this);
   action_pastenumber           = new QAction(QIcon::fromTheme("edit-paste"), i18n("Paste")                                          , this);
   action_showContactDock       = new QAction(QIcon::fromTheme("edit-find-user")   , i18n("Display Person")                         , this);
   action_showHistoryDock       = new QAction(QIcon::fromTheme("view-history")     , i18n("Display history")                         , this);
   action_showBookmarkDock      = new QAction(QIcon::fromTheme("bookmark-new-list"), i18n("Display bookmark")                        , this);
   action_editToolBar           = new QAction(QIcon::fromTheme("configure-toolbars"), i18n("Configure Toolbars")                     , this);
   action_addPerson             = new QAction(QIcon::fromTheme("contact-new"),i18n("Add new contact")                                                     , this);
   action_configureShortcut     = new QAction(QIcon::fromTheme("configure-shortcuts"), i18n("Configure Shortcut"), this);

#define COL(a,b) MainWindow::app()->actionCollection()->setDefaultShortcut(a,b)
   COL(action_accept      , Qt::CTRL + Qt::Key_A );
   COL(action_new_call    , Qt::CTRL + Qt::Key_N );
   COL(action_hold        , Qt::CTRL + Qt::Key_H );
   COL(action_transfer    , Qt::CTRL + Qt::Key_T );
   COL(action_record      , Qt::CTRL + Qt::Key_R );
   COL(action_mailBox     , Qt::CTRL + Qt::Key_M );
   COL(action_addPerson   , Qt::CTRL + Qt::Key_N );
   COL(action_pastenumber , Qt::CTRL + Qt::Key_V );
#undef COL

   action_displayDialpad->setCheckable( true );
   action_displayDialpad->setChecked  ( ConfigurationSkeleton::displayDialpad() );
   action_configureRing->setText(i18n("Configure Ring-KDE"));

   action_displayAccountCbb->setCheckable( true );
   action_displayAccountCbb->setChecked  ( ConfigurationSkeleton::displayAccountBox() );

   action_displayMessageBox->setCheckable( true );
   action_displayMessageBox->setChecked  ( ConfigurationSkeleton::displayMessageBox() );

   action_displayVolumeControls->setCheckable( true );
   action_displayVolumeControls->setChecked  ( ConfigurationSkeleton::displayVolume() );


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
   QHash<int, QAction*> actionHash;
   actionHash[ (int)UserActionModel::Action::ACCEPT          ] = action_accept;
   actionHash[ (int)UserActionModel::Action::ADD_NEW         ] = action_new_call;
   actionHash[ (int)UserActionModel::Action::HOLD            ] = action_hold;
   actionHash[ (int)UserActionModel::Action::MUTE_AUDIO      ] = action_mute_capture;
   actionHash[ (int)UserActionModel::Action::SERVER_TRANSFER ] = action_transfer;
   actionHash[ (int)UserActionModel::Action::RECORD          ] = action_record;
   actionHash[ (int)UserActionModel::Action::HANGUP          ] = action_hangup;

   for (QHash<int,QAction*>::const_iterator i = actionHash.begin(); i != actionHash.end(); ++i) {
      QAction* ea = i.value();
      UserActionModel::Action a = static_cast<UserActionModel::Action>(i.key());
      connect(ea, &QAction::triggered, [uam,a](bool) {uam << a;});
   }

   connect(uam,&UserActionModel::dataChanged, [actionHash,uam](const QModelIndex& tl, const QModelIndex& br) {
      const int first(tl.row()),last(br.row());
      for(int i = first; i <= last;i++) {
         const QModelIndex& idx = uam->index(i,0);
         const UserActionModel::Action action = qvariant_cast<UserActionModel::Action>(idx.data(UserActionModel::Role::ACTION));
         QAction* a = actionHash[(int)action];
         if (a) {
            a->setText   ( idx.data(Qt::DisplayRole).toString()                 );

            if (action != UserActionModel::Action::ADD_NEW) //HACK
               a->setEnabled( idx.flags() & Qt::ItemIsEnabled                   );

            a->setChecked( idx.data(Qt::CheckStateRole) == Qt::Checked          );
         }
      }
   });


   /**/connect(MacroModel::instance()           ,       SIGNAL(addAction(QVariant)),  this    , SLOT(addMacro(QVariant))          );
   /**/connect(action_mailBox                   ,               SIGNAL(triggered()),           this    , SLOT(mailBox())                   );
   /**/connect(action_displayVolumeControls     ,   SIGNAL(toggled(bool)),         MainWindow::view() , SLOT(displayVolumeControls(bool)) );
   /**/connect(action_displayDialpad            ,        SIGNAL(toggled(bool)),         MainWindow::view() , SLOT(displayDialpad(bool))        );
   /**/connect(action_displayAccountCbb         ,        SIGNAL(toggled(bool)),         MainWindow::app() , SLOT(displayAccountCbb(bool))        );
   /**/connect(action_displayMessageBox         ,     SIGNAL(toggled(bool)),         MainWindow::view() , SLOT(displayMessageBox(bool))     );
   /**/connect(action_pastenumber               ,           SIGNAL(triggered()),           MainWindow::view() , SLOT(paste())            );
   /**/connect(action_configureShortcut         ,     SIGNAL(triggered()),           this    , SLOT(showShortCutEditor())        );
   /**/connect(action_editToolBar               ,           SIGNAL(triggered()),           this    , SLOT(editToolBar())               );
   /**/connect(action_addPerson                 ,             SIGNAL(triggered()),           this    , SLOT(slotAddPerson())            );
   /*                                                                                                                   */

   connect(Audio::Settings::instance(),SIGNAL(captureVolumeChanged(int)),this,SLOT(updateRecordButton()));
   connect(Audio::Settings::instance(),SIGNAL(playbackVolumeChanged(int)),this,SLOT(updateVolumeButton()));


//    MainWindow::app()->actionCollection()->setConfigGlobal(true);
   MainWindow::app()->actionCollection()->addAction("action_accept"                , action_accept                );
   MainWindow::app()->actionCollection()->addAction("action_new_call"              , action_new_call              );
   MainWindow::app()->actionCollection()->addAction("action_hold"                  , action_hold                  );
   MainWindow::app()->actionCollection()->addAction("action_transfer"              , action_transfer              );
   MainWindow::app()->actionCollection()->addAction("action_record"                , action_record                );
   MainWindow::app()->actionCollection()->addAction("action_mailBox"               , action_mailBox               );
   MainWindow::app()->actionCollection()->addAction("action_close"                 , action_close                 );
   MainWindow::app()->actionCollection()->addAction("action_quit"                  , action_quit                  );
   MainWindow::app()->actionCollection()->addAction("action_displayVolumeControls" , action_displayVolumeControls );
   MainWindow::app()->actionCollection()->addAction("action_displayDialpad"        , action_displayDialpad        );
   MainWindow::app()->actionCollection()->addAction("action_displayAccountCbb"     , action_displayAccountCbb     );
   MainWindow::app()->actionCollection()->addAction("action_displayMessageBox"     , action_displayMessageBox     );
   MainWindow::app()->actionCollection()->addAction("action_configureRing"         , action_configureRing         );
   MainWindow::app()->actionCollection()->addAction("action_configureShortcut"     , action_configureShortcut     );
   MainWindow::app()->actionCollection()->addAction("action_pastenumber"           , action_pastenumber           );
   MainWindow::app()->actionCollection()->addAction("action_showContactDock"       , action_showContactDock       );
   MainWindow::app()->actionCollection()->addAction("action_showHistoryDock"       , action_showHistoryDock       );
   MainWindow::app()->actionCollection()->addAction("action_showBookmarkDock"      , action_showBookmarkDock      );
   MainWindow::app()->actionCollection()->addAction("action_editToolBar"           , action_editToolBar           );
   MainWindow::app()->actionCollection()->addAction("action_addPerson"             , action_addPerson             );
   MainWindow::app()->actionCollection()->addAction("action_mute_capture"          , action_mute_capture          );
   MainWindow::app()->actionCollection()->addAction("action_mute_playback"         , action_mute_playback         );

   GlobalInstances::setInterface<KDEShortcutDelegate>();
   MacroModel::instance()->addCollection<LocalMacroCollection>();
   QList<QAction *> acList = *Accessibility::instance();

   foreach(QAction * ac,acList) {
      MainWindow::app()->actionCollection()->addAction(ac->objectName() , ac);
   }

   updateRecordButton();
   updateVolumeButton();
}

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
         KMessageBox::error(MainWindow::app(),i18n(msg));
      }
      emit windowStateChanged();
   }
   else {
      HelperFunctions::displayNoAccountMessageBox(MainWindow::view());
   }
}

///Show the configuration dialog
void ActionCollection::configureRing()
{
   QPointer<ConfigurationDialog> configDialog = new ConfigurationDialog(MainWindow::view());
   configDialog->setModal(true);

   connect(configDialog, SIGNAL(changesApplied()), MainWindow::view(), SLOT(loadWindow()));
   configDialog->exec();
   disconnect(configDialog, SIGNAL(changesApplied()), MainWindow::view(), SLOT(loadWindow()));
   delete configDialog;
}

///Display the shortcuts dialog
void ActionCollection::showShortCutEditor() {
   KShortcutsDialog::configure( MainWindow::app()->actionCollection() );
}

///Show the toolbar editor
void ActionCollection::editToolBar()
{
   QPointer<KEditToolBar> toolbareditor = new KEditToolBar(MainWindow::app()->guiFactory());
   toolbareditor->setModal(true);
   toolbareditor->exec();
   toolbareditor->setDefaultToolBar("mainToolBar");
   delete toolbareditor;
}

///Add a new dynamic action (macro)
void ActionCollection::addMacro(const QVariant& newAction)
{
   if (qvariant_cast<QAction*>(newAction))
      MainWindow::app()->actionCollection()->addAction(qvariant_cast<QAction*>(newAction)->objectName() , qvariant_cast<QAction*>(newAction) );
}


/*****************************************************************************
 *                                                                           *
 *                                  Getters                                  *
 *                                                                           *
 ****************************************************************************/

QAction* ActionCollection::holdAction    ()
{
   return action_hold;
}

QAction* ActionCollection::recordAction  ()
{
   return action_record;
}

QAction* ActionCollection::muteCaptureAction()
{
   return action_mute_capture;
}

QAction* ActionCollection::mutePlaybackAction()
{
   return action_mute_playback;
}

QAction* ActionCollection::hangupAction  ()
{
   return action_hangup;
}

QAction* ActionCollection::transferAction()
{
   return action_transfer;
}

QAction* ActionCollection::acceptAction  ()
{
   return action_accept;
}

QAction* ActionCollection::newCallAction  ()
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
   MainWindow::view()->updateVolumeControls();
}

///Update the colunm button icon
void ActionCollection::updateVolumeButton()
{
   double sndVol = Audio::Settings::instance()->playbackVolume();
   static const QIcon icons[4] = {QIcon(":/images/icons/speaker.svg"),QIcon(":/images/icons/speaker_25.svg"),
      QIcon(":/images/icons/speaker_50.svg"),QIcon(":/images/icons/speaker_75.svg")};
   const int idx = (sndVol/26 < 0 || sndVol/26 >= 4)?0:sndVol/26;
   ActionCollection::instance()->mutePlaybackAction()->setIcon(icons[idx]);
   MainWindow::view()->updateVolumeControls();
}

//Video actions
#ifdef ENABLE_VIDEO
QAction* ActionCollection::videoRotateLeftAction     ()
{
   return action_video_rotate_left;
}

QAction* ActionCollection::videoRotateRightAction    ()
{
   return action_video_rotate_right;
}

QAction* ActionCollection::videoFlipHorizontalAction ()
{
   return action_video_flip_horizontal;
}

QAction* ActionCollection::videoFlipVerticalAction   ()
{
   return action_video_flip_vertical;
}

QAction* ActionCollection::videoMuteAction           ()
{
   return action_video_mute;
}

QAction* ActionCollection::videoPreviewAction        ()
{
   return action_video_preview;
}

QAction* ActionCollection::videoScaleAction          ()
{
   return action_video_scale;
}

QAction* ActionCollection::videoFullscreenAction     ()
{
   return action_video_fullscreen;
}
#endif
