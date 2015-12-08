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
#include <QtCore/QPointer>
#include <QtCore/QStandardPaths>
#include <QtGui/QKeySequence>
#include <QtWidgets/QAction>

//KDE
#include <KLocalizedString>
#include <kmessagebox.h>
#include <KStandardAction>
#include <KShortcutsDialog>
#include <KActionCollection>
#include <KEditToolBar>
#include <KSharedConfig>
#include <KNotifyConfigWidget>
#include <KGlobalAccel>

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
#include <availableaccountmodel.h>
#include <callmodel.h>
#include <implementation.h>
#include <useractionmodel.h>
#include <audio/settings.h>
#include <personmodel.h>
#include <delegates/kdepixmapmanipulation.h>

#define CREATE_ACTION(name) name = new QAction(this);name->setObjectName(#name);
#define INIT_ACTION(name, icon, text) name = new QAction(icon, text, this);name->setObjectName(#name);

ActionCollection* ActionCollection::instance()
{
   static auto i = new ActionCollection();
   return i;
}

// Return the default icon
static QIcon getIcon(const UserActionModel::Action a)
{
   return qvariant_cast<QIcon>(GlobalInstances::pixmapManipulator().userActionIcon({a, {}, Qt::Unchecked}));
}

// Return the translated default name
static QString getName(const UserActionModel::Action a)
{
   Q_UNUSED(a)
   return "FOO";
}

ActionCollection::ActionCollection(QObject* parent) : QObject(parent)
{
   // It is important to init the actions correctly for the menu and KDE global shortcuts
   INIT_ACTION(action_accept        , QIcon(":/images/icons/accept.svg"   ), i18n( "Accept"   ));
   INIT_ACTION(action_new_call      , QIcon(":/images/icons/call.svg"     ), i18n( "New Call" ));
   INIT_ACTION(action_hold          , QIcon(":/images/icons/hold.svg"     ), i18n( "Hold"     ));
   INIT_ACTION(action_transfer      , QIcon(":/images/icons/transfert.svg"), i18n( "Transfer" ));
   INIT_ACTION(action_record        , QIcon(":/images/icons/rec_call.svg" ), i18n( "Record"   ));
   INIT_ACTION(action_hangup        , QIcon(":/images/icons/hang_up.svg"  ), i18n( "Hang up"  ));
   INIT_ACTION(action_mailBox       , QIcon(":/images/icons/mailbox.svg"  ), i18n( "Mailbox"  ));

   INIT_ACTION(action_mute_video         , getIcon(UserActionModel::Action::MUTE_VIDEO         ) , getName(UserActionModel::Action::MUTE_VIDEO         ));
   INIT_ACTION(action_join               , getIcon(UserActionModel::Action::JOIN               ) , getName(UserActionModel::Action::JOIN               ));
   INIT_ACTION(action_toggle_video       , getIcon(UserActionModel::Action::TOGGLE_VIDEO       ) , getName(UserActionModel::Action::TOGGLE_VIDEO       ));
   INIT_ACTION(action_add_contact        , getIcon(UserActionModel::Action::ADD_CONTACT        ) , getName(UserActionModel::Action::ADD_CONTACT        ));
   INIT_ACTION(action_add_to_contact     , getIcon(UserActionModel::Action::ADD_TO_CONTACT     ) , getName(UserActionModel::Action::ADD_TO_CONTACT     ));
   INIT_ACTION(action_delete_contact     , getIcon(UserActionModel::Action::DELETE_CONTACT     ) , getName(UserActionModel::Action::DELETE_CONTACT     ));
   INIT_ACTION(action_email_contact      , getIcon(UserActionModel::Action::EMAIL_CONTACT      ) , getName(UserActionModel::Action::EMAIL_CONTACT      ));
   INIT_ACTION(action_copy_contact       , getIcon(UserActionModel::Action::COPY_CONTACT       ) , getName(UserActionModel::Action::COPY_CONTACT       ));
   INIT_ACTION(action_bookmark           , getIcon(UserActionModel::Action::BOOKMARK           ) , getName(UserActionModel::Action::BOOKMARK           ));
   INIT_ACTION(action_view_chat_history  , getIcon(UserActionModel::Action::VIEW_CHAT_HISTORY  ) , getName(UserActionModel::Action::VIEW_CHAT_HISTORY  ));
   INIT_ACTION(action_add_contact_method , getIcon(UserActionModel::Action::ADD_CONTACT_METHOD ) , getName(UserActionModel::Action::ADD_CONTACT_METHOD ));
   INIT_ACTION(action_call_contact       , getIcon(UserActionModel::Action::CALL_CONTACT       ) , getName(UserActionModel::Action::CALL_CONTACT       ));
   INIT_ACTION(action_edit_contact       , getIcon(UserActionModel::Action::EDIT_CONTACT       ) , getName(UserActionModel::Action::EDIT_CONTACT       ));
   INIT_ACTION(action_add_new            , getIcon(UserActionModel::Action::ADD_NEW            ) , getName(UserActionModel::Action::ADD_NEW            ));
   INIT_ACTION(action_remove_history     , getIcon(UserActionModel::Action::REMOVE_HISTORY     ) , getName(UserActionModel::Action::REMOVE_HISTORY     ));

   INIT_ACTION(action_mute_capture  , QIcon::fromTheme("player-volume-muted"), i18nc("Mute the current audio capture device" , "Mute"     ));
   INIT_ACTION(action_mute_playback , QIcon::fromTheme("player-volume-muted"), i18nc("Mute the current audio playback device", "Mute Playback"     ));

#ifdef ENABLE_VIDEO
   INIT_ACTION(action_video_rotate_left     , QIcon::fromTheme("object-rotate-left"    ), i18n( "Rotate left"       ));
   INIT_ACTION(action_video_rotate_right    , QIcon::fromTheme("object-rotate-right"   ), i18n( "Rotate right"      ));
   INIT_ACTION(action_video_flip_horizontal , QIcon::fromTheme("object-flip-horizontal"), i18n( "Flip"              ));
   INIT_ACTION(action_video_flip_vertical   , QIcon::fromTheme("object-flip-vertical"  ), i18n( "Flip"              ));
   INIT_ACTION(action_video_mute            , QIcon::fromTheme("camera-web"            ), i18n( "Mute"              ));
   INIT_ACTION(action_video_preview         , QIcon::fromTheme("view-preview"          ), i18n( "Preview"           ));
   INIT_ACTION(action_video_scale           , QIcon::fromTheme("transform-scale"       ), i18n( "Keep aspect ratio" ));
   INIT_ACTION(action_video_fullscreen      , QIcon::fromTheme("view-fullscreen"       ), i18n( "Fullscreen"        ));
#endif
}

ActionCollection::~ActionCollection()
{}

void ActionCollection::setupAction()
{
   // Import standard actions
   action_close         = KStandardAction::close      ( MainWindow::app(), SLOT(close())        , MainWindow::app());
   action_quit          = KStandardAction::quit       ( MainWindow::app(), SLOT(quitButton())   , MainWindow::app());
   action_configureRing = KStandardAction::preferences( this             , SLOT(configureRing()), MainWindow::app());

   action_configureRing->setText(i18n("Configure Ring-KDE"));

   action_close         ->setObjectName( "action_close"         );
   action_quit          ->setObjectName( "action_quit"          );
   action_configureRing ->setObjectName( "action_configureRing" );

   INIT_ACTION(action_displayDialpad        , QIcon(RingIcons::DISPLAY_DIALPAD                   ), i18n("Display dialpad"         ));
   INIT_ACTION(action_displayAccountCbb     , {}                                                  , i18n("Display account selector"));
   INIT_ACTION(action_displayMessageBox     , QIcon::fromTheme("mail-message-new"                ), i18n("Display text message box"));
   INIT_ACTION(action_displayVolumeControls , QIcon(RingIcons::DISPLAY_VOLUME_CONSTROLS          ), i18n("Display volume controls" ));
   INIT_ACTION(action_pastenumber           , QIcon::fromTheme("edit-paste"                      ), i18n("Paste"                   ));
   INIT_ACTION(action_showContactDock       , QIcon::fromTheme("edit-find-user"                  ), i18n("Display Person"          ));
   INIT_ACTION(action_showHistoryDock       , QIcon::fromTheme("view-history"                    ), i18n("Display history"         ));
   INIT_ACTION(action_showBookmarkDock      , QIcon::fromTheme("bookmark-new-list"               ), i18n("Display bookmark"        ));
   INIT_ACTION(action_editToolBar           , QIcon::fromTheme("configure-toolbars"              ), i18n("Configure Toolbars"      ));
   INIT_ACTION(action_addPerson             , QIcon::fromTheme("contact-new"                     ), i18n("Add new contact"         ));
   INIT_ACTION(action_configureShortcut     , QIcon::fromTheme("configure-shortcuts"             ), i18n("Configure Shortcut"      ));
   INIT_ACTION(action_configureNotifications, QIcon::fromTheme("preferences-desktop-notification"), i18n("Configure Notifications" ));
   INIT_ACTION(action_raise_client          , {}                                                  , i18n("Raise Ring-KDE window"   ));

   // Assign default shortcuts
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


   // Declare checkable actions
   for (QAction* a : QList<QAction*> {
      action_video_preview        , action_video_scale          , action_video_fullscreen  ,
      action_video_mute           , action_displayDialpad       , action_displayAccountCbb ,
      action_displayMessageBox    , action_displayVolumeControls, action_showContactDock   ,
      action_showHistoryDock      , action_showBookmarkDock     , action_mute_capture      ,
      action_mute_playback,
   }) {
      a->setCheckable( true );
   }

   // Load the saved check state from KConfig
   action_video_scale           ->setChecked( ConfigurationSkeleton::keepVideoAspectRatio() );
   action_video_preview         ->setChecked( ConfigurationSkeleton::displayVideoPreview () );
   action_displayDialpad        ->setChecked( ConfigurationSkeleton::displayDialpad      () );
   action_displayAccountCbb     ->setChecked( ConfigurationSkeleton::displayAccountBox   () );
   action_displayMessageBox     ->setChecked( ConfigurationSkeleton::displayMessageBox   () );
   action_displayVolumeControls ->setChecked( ConfigurationSkeleton::displayVolume       () );
   action_showContactDock       ->setChecked( ConfigurationSkeleton::displayContactDock  () );
   action_showHistoryDock       ->setChecked( ConfigurationSkeleton::displayHistoryDock  () );
   action_showBookmarkDock      ->setChecked( ConfigurationSkeleton::displayBookmarkDock () );


   //Bind actions to the useractionmodel
   UserActionModel* uam = CallModel::instance().userActionModel();
   QHash<int, QAction*> actionHash;
   actionHash[ (int)UserActionModel::Action::ACCEPT              ] = action_accept             ;
   actionHash[ (int)UserActionModel::Action::ADD_NEW             ] = action_new_call           ;
   actionHash[ (int)UserActionModel::Action::HOLD                ] = action_hold               ;
   actionHash[ (int)UserActionModel::Action::MUTE_AUDIO          ] = action_mute_capture       ;
   actionHash[ (int)UserActionModel::Action::SERVER_TRANSFER     ] = action_transfer           ;
   actionHash[ (int)UserActionModel::Action::RECORD              ] = action_record             ;
   actionHash[ (int)UserActionModel::Action::HANGUP              ] = action_hangup             ;
   actionHash[ (int)UserActionModel::Action::MUTE_VIDEO          ] = action_mute_video         ;
   actionHash[ (int)UserActionModel::Action::JOIN                ] = action_join               ;
   actionHash[ (int)UserActionModel::Action::TOGGLE_VIDEO        ] = action_toggle_video       ;
   actionHash[ (int)UserActionModel::Action::ADD_CONTACT         ] = action_add_contact        ;
   actionHash[ (int)UserActionModel::Action::ADD_TO_CONTACT      ] = action_add_to_contact     ;
   actionHash[ (int)UserActionModel::Action::DELETE_CONTACT      ] = action_delete_contact     ;
   actionHash[ (int)UserActionModel::Action::EMAIL_CONTACT       ] = action_email_contact      ;
   actionHash[ (int)UserActionModel::Action::COPY_CONTACT        ] = action_copy_contact       ;
   actionHash[ (int)UserActionModel::Action::BOOKMARK            ] = action_bookmark           ;
   actionHash[ (int)UserActionModel::Action::VIEW_CHAT_HISTORY   ] = action_view_chat_history  ;
   actionHash[ (int)UserActionModel::Action::ADD_CONTACT_METHOD  ] = action_add_contact_method ;
   actionHash[ (int)UserActionModel::Action::CALL_CONTACT        ] = action_call_contact       ;
   actionHash[ (int)UserActionModel::Action::EDIT_CONTACT        ] = action_edit_contact       ;
   actionHash[ (int)UserActionModel::Action::ADD_NEW             ] = action_add_new            ;
   actionHash[ (int)UserActionModel::Action::REMOVE_HISTORY      ] = action_remove_history     ;

   for (QHash<int,QAction*>::const_iterator i = actionHash.begin(); i != actionHash.end(); ++i) {
      QAction* ea = i.value();
      UserActionModel::Action a = static_cast<UserActionModel::Action>(i.key());
      connect(ea, &QAction::triggered, [uam,a](bool) {uam << a;});
   }

   // Refresh the action state and text
   static auto l = [actionHash,uam](const QModelIndex& tl, const QModelIndex& br) {
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
   };

   connect(uam, &UserActionModel::dataChanged, l);

   // Load the initial text
   l(uam->index(0,0), uam->index(uam->rowCount() -1));


   const auto as = &Audio::Settings::instance();

   // Connect actions
   connect(action_mute_capture           , &QAction::toggled   , as                 , &Audio::Settings::muteCapture             );
   connect(action_mute_playback          , &QAction::toggled   , as                 , &Audio::Settings::mutePlayback            );
   connect(action_displayAccountCbb      , &QAction::toggled   , MainWindow::app () , &MainWindow::displayAccountCbb            );
   connect(action_displayVolumeControls  , &QAction::toggled   , MainWindow::view() , &View::displayVolumeControls              );
   connect(action_displayDialpad         , &QAction::toggled   , MainWindow::view() , &View::displayDialpad                     );
   connect(action_displayMessageBox      , &QAction::toggled   , MainWindow::view() , &View::displayMessageBox                  );
   connect(action_pastenumber            , &QAction::triggered , MainWindow::view() , &View::paste                              );
   connect(action_mailBox                , &QAction::triggered , this               , &ActionCollection::mailBox                );
   connect(action_configureShortcut      , &QAction::triggered , this               , &ActionCollection::showShortCutEditor     );
   connect(action_configureNotifications , &QAction::triggered , this               , &ActionCollection::showNotificationEditor );
   connect(action_editToolBar            , &QAction::triggered , this               , &ActionCollection::editToolBar            );
   connect(action_addPerson              , &QAction::triggered , this               , &ActionCollection::slotAddPerson          );
   connect(action_raise_client           , &QAction::triggered , this               , &ActionCollection::raiseClient            );

   connect(&MacroModel::instance(), &MacroModel::addAction                  , this                , &ActionCollection::addMacro           );
   connect(as                     , &Audio::Settings::captureVolumeChanged  , this                , &ActionCollection::updateRecordButton );
   connect(as                     , &Audio::Settings::playbackVolumeChanged , this                , &ActionCollection::updateVolumeButton );
   connect(as                     , &Audio::Settings::captureMuted          , action_mute_capture , &QAction::setChecked                  );
   connect(as                     , &Audio::Settings::playbackMuted         , action_mute_playback, &QAction::setChecked                  );

   // Add the actions to the collection
   for (QAction* a : QList<QAction*>{
      action_accept            , action_new_call          , action_hold              ,
      action_transfer          , action_record            , action_mailBox           ,
      action_close             , action_quit              , action_displayDialpad    ,
      action_displayAccountCbb , action_displayMessageBox , action_configureRing     ,
      action_configureShortcut , action_pastenumber       , action_showContactDock   ,
      action_showHistoryDock   , action_showBookmarkDock  , action_editToolBar       ,
      action_addPerson         , action_mute_capture      , action_mute_playback     ,
      action_mute_video        , action_join              , action_toggle_video      ,
      action_add_contact       , action_add_to_contact    , action_delete_contact    ,
      action_email_contact     , action_copy_contact      , action_bookmark          ,
      action_view_chat_history , action_add_contact_method, action_call_contact      ,
      action_edit_contact      , action_add_new           , action_remove_history    ,
      action_raise_client      ,
      action_configureNotifications, action_displayVolumeControls ,
   }) {
      MainWindow::app()->actionCollection()->addAction(a->objectName(), a);
   }

   // Enable global shortcuts for relevant "current call" actions
   for (QAction* a : QList<QAction*>{
      action_accept      , action_new_call    , action_hold        ,
      action_mute_capture, action_transfer    , action_record      ,
      action_hangup      , action_raise_client,
   }) {
      KGlobalAccel::self()->setGlobalShortcut(a, QList<QKeySequence>{});
   }

   GlobalInstances::setInterface<KDEShortcutDelegate>();

   MacroModel::instance().addCollection<LocalMacroCollection>();

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
   Call* call = CallModel::instance().dialingCall();
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
void ActionCollection::showShortCutEditor()
{
   KShortcutsDialog::configure( MainWindow::app()->actionCollection() );
}

///Display the notification manager
void ActionCollection::showNotificationEditor()
{
   QPointer<QDialog> d = new QDialog(MainWindow::app());

   auto l = new QGridLayout(d);
   auto w = new KNotifyConfigWidget(d);

   auto def    = new QPushButton(QIcon::fromTheme(""             ), i18n("Defaults"), d);
   auto ok     = new QPushButton(QIcon::fromTheme("dialog-ok"    ), i18n("Ok"      ), d);
   auto cancel = new QPushButton(QIcon::fromTheme("dialog-cancel"), i18n("Cancel"  ), d);

   ok->setDefault(true);

   w->setApplication(QStringLiteral("ring-kde"));

   l->addWidget(w      , 0, 0, 1, 4);
   l->addWidget(def    , 1, 0, 1, 1);
   l->addWidget(ok     , 1, 2, 1, 1);
   l->addWidget(cancel , 1, 3, 1, 1);

   l->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding),1,1);

   connect(def   , &QPushButton::clicked, w, &KNotifyConfigWidget::revertToDefaults);
   connect(ok    , &QPushButton::clicked, w, &KNotifyConfigWidget::save            );
   connect(ok    , &QPushButton::clicked, d, &QDialog::close                       );
   connect(cancel, &QPushButton::clicked, d, &QDialog::close                       );

   d->exec();

   delete d;
}

///Show the toolbar editor
void ActionCollection::editToolBar()
{
   QPointer<KEditToolBar> toolbareditor = new KEditToolBar(MainWindow::app()->guiFactory());
   toolbareditor->setModal(true);
   toolbareditor->setDefaultToolBar("mainToolBar");
   toolbareditor->exec();
   delete toolbareditor;
}

///Add a new dynamic action (macro)
void ActionCollection::addMacro(const QVariant& newAction)
{
   if (qvariant_cast<QAction*>(newAction))
      MainWindow::app()->actionCollection()->addAction(qvariant_cast<QAction*>(newAction)->objectName() , qvariant_cast<QAction*>(newAction) );
}

void ActionCollection::slotAddPerson()
{
   Person* aPerson = new Person();
   PersonModel::instance().addNewPerson(aPerson);
}

///Change icon of the record button
void ActionCollection::updateRecordButton()
{
   double recVol = Audio::Settings::instance().captureVolume();
   static const QIcon icons[4] = {QIcon(":/images/icons/mic.svg"),QIcon(":/images/icons/mic_25.svg"),
      QIcon(":/images/icons/mic_50.svg"),QIcon(":/images/icons/mic_75.svg")};
   const int idx = (recVol/26 < 0 || recVol/26 >= 4)?0:recVol/26;
   ActionCollection::instance()->muteCaptureAction()->setIcon(icons[idx]);
   MainWindow::view()->updateVolumeControls();
}

///Update the colunm button icon
void ActionCollection::updateVolumeButton()
{
   double sndVol = Audio::Settings::instance().playbackVolume();
   static const QIcon icons[4] = {QIcon(":/images/icons/speaker.svg"),QIcon(":/images/icons/speaker_25.svg"),
      QIcon(":/images/icons/speaker_50.svg"),QIcon(":/images/icons/speaker_75.svg")};
   const int idx = (sndVol/26 < 0 || sndVol/26 >= 4)?0:sndVol/26;
   ActionCollection::instance()->mutePlaybackAction()->setIcon(icons[idx]);
   MainWindow::view()->updateVolumeControls();
}

///Raise the main window to the foreground
void ActionCollection::raiseClient()
{
   MainWindow::app()->show          ();
   MainWindow::app()->activateWindow();
   MainWindow::app()->raise         ();

   // Add a new call if there is none
   if (!CallModel::instance().rowCount())
      CallModel::instance().userActionModel() << UserActionModel::Action::ADD_NEW;
   MainWindow::view()->setFocus(Qt::OtherFocusReason);
}


#define GETTER(name, action) QAction* ActionCollection::name(){return action;}

GETTER(holdAction                   , action_hold                  )
GETTER(recordAction                 , action_record                )
GETTER(muteCaptureAction            , action_mute_capture          )
GETTER(mutePlaybackAction           , action_mute_playback         )
GETTER(hangupAction                 , action_hangup                )
GETTER(transferAction               , action_transfer              )
GETTER(acceptAction                 , action_accept                )
GETTER(newCallAction                , action_new_call              )
GETTER(displayVolumeControlsAction  , action_displayVolumeControls )
GETTER(displayDialpadAction         , action_displayDialpad        )
GETTER(displayAccountCbbAction      , action_displayAccountCbb     )
GETTER(displayMessageBoxAction      , action_displayMessageBox     )
GETTER(mailBoxAction                , action_mailBox               )
GETTER(showContactDockAction        , action_showContactDock       )
GETTER(showHistoryDockAction        , action_showHistoryDock       )
GETTER(showBookmarkDockAction       , action_showBookmarkDock      )
GETTER(quitAction                   , action_quit                  )
GETTER(addPerson                    , action_addPerson             )

//Video actions
#ifdef ENABLE_VIDEO
GETTER(videoRotateLeftAction        , action_video_rotate_left     )
GETTER(videoRotateRightAction       , action_video_rotate_right    )
GETTER(videoFlipHorizontalAction    , action_video_flip_horizontal )
GETTER(videoFlipVerticalAction      , action_video_flip_vertical   )
GETTER(videoMuteAction              , action_video_mute            )
GETTER(videoPreviewAction           , action_video_preview         )
GETTER(videoScaleAction             , action_video_scale           )
GETTER(videoFullscreenAction        , action_video_fullscreen      )
#endif

#undef GETTER


#undef CREATE_ACTION
#undef INIT_ACTION
