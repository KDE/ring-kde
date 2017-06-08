/***************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
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
#include <QtCore/QTimer>
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
#include "phonewindow.h"
#include "ringapplication.h"
#include "view.h"
#include "conf/configurationdialog.h"
#include "icons/icons.h"
#include "klib/kcfg_settings.h"
#include "klib/helperfunctions.h"
#include <call.h>
#include <account.h>
#include <availableaccountmodel.h>
#include <callmodel.h>
#include <implementation.h>
#include <useractionmodel.h>
#include <audio/settings.h>
#include <personmodel.h>
#include <delegates/kdepixmapmanipulation.h>

#ifdef HAVE_SPEECH
 #include "accessibility.h"
#endif

#define INIT_ACTION(name, icon, text) name = new QAction(icon, text, this);name->setObjectName(QStringLiteral(#name));

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
   return QStringLiteral("FOO");
}

ActionCollection::ActionCollection(FancyMainWindow* parent) : QObject(parent)
{
   // It is important to init the actions correctly for the menu and KDE global shortcuts
   INIT_ACTION(action_accept        , QIcon(QStringLiteral(":/sharedassets/phone_light/accept.svg"   )), i18n( "Accept"   ));
   INIT_ACTION(action_hold          , QIcon(QStringLiteral(":/sharedassets/phone_light/hold.svg"     )), i18n( "Hold"     ));
   INIT_ACTION(action_transfer      , QIcon(QStringLiteral(":/sharedassets/phone_light/transfert.svg")), i18n( "Transfer" ));
   INIT_ACTION(action_record        , QIcon(QStringLiteral(":/sharedassets/phone_light/record_call.svg" )), i18n( "Record"   ));
   INIT_ACTION(action_hangup        , QIcon(QStringLiteral(":/sharedassets/phone_light/hang_up.svg"  )), i18n( "Hang up"  ));

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
   INIT_ACTION(action_new_call           , getIcon(UserActionModel::Action::ADD_NEW            ) , getName(UserActionModel::Action::ADD_NEW            ));
   INIT_ACTION(action_remove_history     , getIcon(UserActionModel::Action::REMOVE_HISTORY     ) , getName(UserActionModel::Action::REMOVE_HISTORY     ));

   INIT_ACTION(action_mute_capture  , QIcon::fromTheme(QStringLiteral("player-volume-muted")), i18nc("Mute the current audio capture device" , "Mute"     ));
   INIT_ACTION(action_mute_playback , QIcon::fromTheme(QStringLiteral("player-volume-muted")), i18nc("Mute the current audio playback device", "Mute Playback"     ));

#ifdef ENABLE_VIDEO
   INIT_ACTION(action_video_rotate_left     , QIcon::fromTheme(QStringLiteral("object-rotate-left"    )), i18n( "Rotate left"       ));
   INIT_ACTION(action_video_rotate_right    , QIcon::fromTheme(QStringLiteral("object-rotate-right"   )), i18n( "Rotate right"      ));
   INIT_ACTION(action_video_flip_horizontal , QIcon::fromTheme(QStringLiteral("object-flip-horizontal")), i18n( "Flip"              ));
   INIT_ACTION(action_video_flip_vertical   , QIcon::fromTheme(QStringLiteral("object-flip-vertical"  )), i18n( "Flip"              ));
   INIT_ACTION(action_video_mute            , QIcon::fromTheme(QStringLiteral("camera-web"            )), i18n( "Mute"              ));
   INIT_ACTION(action_video_preview         , QIcon::fromTheme(QStringLiteral("view-preview"          )), i18n( "Preview"           ));
   INIT_ACTION(action_video_scale           , QIcon::fromTheme(QStringLiteral("transform-scale"       )), i18n( "Keep aspect ratio" ));
   INIT_ACTION(action_video_fullscreen      , QIcon::fromTheme(QStringLiteral("view-fullscreen"       )), i18n( "Fullscreen"        ));
#endif
}

ActionCollection::~ActionCollection()
{}

void ActionCollection::setupAction(FancyMainWindow* mw)
{
   // Import standard actions
   action_close_phone   = new QAction();//KStandardAction::close      ( RingApplication::instance()->phoneWindow(), SLOT(close())        , PhoneWindow::app());
   action_close_timeline= new QAction();//KStandardAction::close      ( RingApplication::instance()->timelineWindow(), SLOT(close())        , PhoneWindow::app());
   action_quit          = KStandardAction::quit       ( RingApplication::instance(), SLOT(quit())   , this);
   action_configureRing = KStandardAction::preferences( this             , SLOT(configureRing()), this);

   action_configureRing->setText(i18n("Configure Ring-KDE"));

   action_close_phone   ->setObjectName( QStringLiteral("action_close_phone")         );
   action_close_timeline->setObjectName( QStringLiteral("action_close_timeline")         );
   action_quit          ->setObjectName( QStringLiteral("action_quit")          );
   action_configureRing ->setObjectName( QStringLiteral("action_configureRing") );

   INIT_ACTION(action_displayDialpad        , QIcon(RingIcons::DISPLAY_DIALPAD                   ), i18n("Display dialpad"         ));
   INIT_ACTION(action_displayVolumeControls , QIcon(RingIcons::DISPLAY_VOLUME_CONSTROLS          ), i18n("Display volume controls" ));
   INIT_ACTION(action_displayAccountCbb     , {}                                                  , i18n("Display account selector"));
   INIT_ACTION(action_raise_client          , {}                                                  , i18n("Raise Ring-KDE window"   ));
   INIT_ACTION(action_focus_history         , {}                                                  , i18n("Search history"          ));
   INIT_ACTION(action_focus_call            , {}                                                  , i18n("Search call"             ));
   INIT_ACTION(action_focus_contact         , {}                                                  , i18n("Search contact"          ));
   INIT_ACTION(action_focus_bookmark        , {}                                                  , i18n("Search bookmark"         ));

   INIT_ACTION(action_show_wizard           , QIcon::fromTheme(QStringLiteral("tools-wizard"                    )), i18n("New account wizard"      ));
   INIT_ACTION(action_new_contact           , QIcon::fromTheme(QStringLiteral("contact-new"                     )), i18n("New contact"             ));
   INIT_ACTION(action_pastenumber           , QIcon::fromTheme(QStringLiteral("edit-paste"                      )), i18n("Paste"                   ));
   INIT_ACTION(action_showContactDock       , QIcon::fromTheme(QStringLiteral("edit-find-user"                  )), i18n("Display Person"          ));
   INIT_ACTION(action_showHistoryDock       , QIcon::fromTheme(QStringLiteral("view-history"                    )), i18n("Display history"         ));
   INIT_ACTION(action_showBookmarkDock      , QIcon::fromTheme(QStringLiteral("bookmark-new-list"               )), i18n("Display bookmark"        ));
   INIT_ACTION(action_editToolBar           , QIcon::fromTheme(QStringLiteral("configure-toolbars"              )), i18n("Configure Toolbars"      ));
   INIT_ACTION(action_addPerson             , QIcon::fromTheme(QStringLiteral("contact-new"                     )), i18n("Add new contact"         ));
   INIT_ACTION(action_configureShortcut     , QIcon::fromTheme(QStringLiteral("configure-shortcuts"             )), i18n("Configure Shortcut"      ));
   INIT_ACTION(action_configureNotifications, QIcon::fromTheme(QStringLiteral("preferences-desktop-notification")), i18n("Configure Notifications" ));

   // Assign default shortcuts
#define COL(a,b) mw->actionCollection()->setDefaultShortcut(a,b)
   COL(action_accept      , Qt::CTRL + Qt::Key_A );
   COL(action_new_call    , Qt::CTRL + Qt::Key_N );
   COL(action_hold        , Qt::CTRL + Qt::Key_H );
   COL(action_transfer    , Qt::CTRL + Qt::Key_T );
   COL(action_record      , Qt::CTRL + Qt::Key_R );
   COL(action_pastenumber , Qt::CTRL + Qt::Key_V );
#undef COL


   // Declare checkable actions
   for (QAction* a : QList<QAction*> {
      action_video_preview        , action_video_scale          , action_video_fullscreen  ,
      action_video_mute           , action_displayDialpad       , action_displayAccountCbb ,
      action_mute_playback        , action_displayVolumeControls, action_showContactDock   ,
      action_showHistoryDock      , action_showBookmarkDock     , action_mute_capture      ,
   }) {
      a->setCheckable( true );
   }

   // Load the saved check state from KConfig
   action_video_scale           ->setChecked( ConfigurationSkeleton::keepVideoAspectRatio() );
   action_video_preview         ->setChecked( ConfigurationSkeleton::displayVideoPreview () );
   action_displayDialpad        ->setChecked( ConfigurationSkeleton::displayDialpad      () );
   action_displayAccountCbb     ->setChecked( ConfigurationSkeleton::displayAccountBox   () );
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
   actionHash[ (int)UserActionModel::Action::REMOVE_HISTORY      ] = action_remove_history     ;

   for (QHash<int,QAction*>::const_iterator i = actionHash.constBegin(); i != actionHash.constEnd(); ++i) {
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
   connect(action_show_wizard            , &QAction::triggered , RingApplication::instance(), &RingApplication::showWizard                   );
   connect(action_new_contact            , &QAction::triggered , this               , &ActionCollection::slotNewContact         );
   connect(action_configureShortcut      , &QAction::triggered , this               , &ActionCollection::showShortCutEditor     );
   connect(action_configureNotifications , &QAction::triggered , this               , &ActionCollection::showNotificationEditor );
   connect(action_editToolBar            , &QAction::triggered , this               , &ActionCollection::editToolBar            );
   connect(action_addPerson              , &QAction::triggered , this               , &ActionCollection::slotAddPerson          );
   connect(action_raise_client           , &QAction::triggered , this               , &ActionCollection::raiseClient            );

   connect(as                     , &Audio::Settings::captureVolumeChanged  , this                , &ActionCollection::updateRecordButton );
   connect(as                     , &Audio::Settings::playbackVolumeChanged , this                , &ActionCollection::updateVolumeButton );
   connect(as                     , &Audio::Settings::captureMuted          , action_mute_capture , &QAction::setChecked                  );
   connect(as                     , &Audio::Settings::playbackMuted         , action_mute_playback, &QAction::setChecked                  );

   // Add the actions to the collection
   for (QAction* a : {
      action_accept            , action_new_call          , action_hold              ,
      action_transfer          , action_record            , action_new_contact       ,
      action_close_phone       , action_quit              , action_displayDialpad    ,
      action_displayAccountCbb , action_close_timeline    , action_configureRing     ,
      action_configureShortcut , action_pastenumber       , action_showContactDock   ,
      action_showHistoryDock   , action_showBookmarkDock  , action_editToolBar       ,
      action_addPerson         , action_mute_capture      , action_mute_playback     ,
      action_mute_video        , action_join              , action_toggle_video      ,
      action_add_contact       , action_add_to_contact    , action_delete_contact    ,
      action_email_contact     , action_copy_contact      , action_bookmark          ,
      action_view_chat_history , action_add_contact_method, action_call_contact      ,
      action_edit_contact      , action_focus_history     , action_remove_history    ,
      action_raise_client      , action_focus_contact     , action_focus_call        ,
      action_focus_bookmark    , action_show_wizard       ,
      action_configureNotifications, action_displayVolumeControls ,
   }) {
      mw->actionCollection()->addAction(a->objectName(), a);
   }

   // Enable global shortcuts for relevant "current call" actions
   for (QAction* a : QList<QAction*>{
      action_accept       , action_new_call    , action_hold         ,
      action_mute_capture , action_transfer    , action_record       ,
      action_hangup       , action_raise_client, action_focus_history,
      action_focus_contact, action_focus_call  , action_focus_bookmark
   }) {
      KGlobalAccel::self()->setGlobalShortcut(a, QList<QKeySequence>{});
   }

   GlobalInstances::setInterface<KDEShortcutDelegate>();

#ifdef HAVE_SPEECH
   QList<QAction *> acList = *Accessibility::instance();

   foreach(QAction * ac,acList) {
      mw->actionCollection()->addAction(ac->objectName() , ac);
   }
#endif

   QTimer::singleShot(0, [this]() {
      updateRecordButton();
      updateVolumeButton();
   });
}


void ActionCollection::setupPhoneAction(PhoneWindow* mw)
{
   connect(action_displayAccountCbb      , &QAction::toggled   , mw         , &PhoneWindow::displayAccountCbb            );
   connect(action_displayVolumeControls  , &QAction::toggled   , mw->view() , &View::displayVolumeControls              );
   connect(action_displayDialpad         , &QAction::toggled   , mw->view() , &View::displayDialpad                     );
   connect(action_pastenumber            , &QAction::triggered , mw->view() , &View::paste                              );

}

///Show the configuration dialog
void ActionCollection::configureRing()
{
   QPointer<ConfigurationDialog> configDialog = new ConfigurationDialog(PhoneWindow::app()->view());
   configDialog->setModal(true);

   configDialog->exec();
   delete configDialog;
}

///Display the shortcuts dialog
void ActionCollection::showShortCutEditor()
{
   KShortcutsDialog::configure( RingApplication::instance()->mainWindow()->actionCollection() );
}

///Display the notification manager
void ActionCollection::showNotificationEditor()
{
   KNotifyConfigWidget::configure(RingApplication::instance()->mainWindow(), QStringLiteral("ring-kde"));
}

///Show the toolbar editor
void ActionCollection::editToolBar()
{
   QPointer<KEditToolBar> toolbareditor = new KEditToolBar(RingApplication::instance()->mainWindow()->guiFactory());
   toolbareditor->setModal(true);
   toolbareditor->setDefaultToolBar(QStringLiteral("mainToolBar"));
   toolbareditor->exec();
   delete toolbareditor;
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
   static const QIcon icons[4] = {
      QIcon(QStringLiteral(":/images/icons/mic.svg"   )),
      QIcon(QStringLiteral(":/images/icons/mic_25.svg")),
      QIcon(QStringLiteral(":/images/icons/mic_50.svg")),
      QIcon(QStringLiteral(":/images/icons/mic_75.svg"))
   };

   const int idx = (recVol/26 < 0 || recVol/26 >= 4)?0:recVol/26;
   ActionCollection::instance()->muteCaptureAction()->setIcon(icons[idx]);
   PhoneWindow::app()->view()->updateVolumeControls();
}

///Update the colunm button icon
void ActionCollection::updateVolumeButton()
{
   double sndVol = Audio::Settings::instance().playbackVolume();
   static const QIcon icons[4] = {
      QIcon(QStringLiteral(":/images/icons/speaker.svg"   )),
      QIcon(QStringLiteral(":/images/icons/speaker_25.svg")),
      QIcon(QStringLiteral(":/images/icons/speaker_50.svg")),
      QIcon(QStringLiteral(":/images/icons/speaker_75.svg"))
   };

   const int idx = (sndVol/26 < 0 || sndVol/26 >= 4)?0:sndVol/26;
   ActionCollection::instance()->mutePlaybackAction()->setIcon(icons[idx]);
   PhoneWindow::app()->view()->updateVolumeControls();
}

void ActionCollection::slotNewContact()
{
   // Find a suitable collection
   auto cols = PersonModel::instance().enabledCollections(
      CollectionInterface::SupportedFeatures::ADD |
      CollectionInterface::SupportedFeatures::EDIT
   );

   if (cols.isEmpty()) {
      qWarning() << "Failed to add a contact: no suitable backend found";
      return;
   }

   const auto col = cols.first();

   auto p = new Person();
   p->setCollection(col); //TODO have a selection widget again

   GlobalInstances::actionExtender().editPerson(p);

   // The editor wont save if the name isn't set, so it can be used to check
   // if cancel was clicked.
   col->editor<Person>()->addExisting(p);
}

///Raise the main window to the foreground
void ActionCollection::raiseClient(bool focus)
{
   PhoneWindow::app()->show          ();
   PhoneWindow::app()->activateWindow();
   PhoneWindow::app()->raise         ();

   if (focus) {
      // Add a new call if there is none
      if (!CallModel::instance().rowCount())
         CallModel::instance().userActionModel() << UserActionModel::Action::ADD_NEW;
      PhoneWindow::app()->view()->setFocus(Qt::OtherFocusReason);
   }
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
GETTER(showContactDockAction        , action_showContactDock       )
GETTER(showHistoryDockAction        , action_showHistoryDock       )
GETTER(showBookmarkDockAction       , action_showBookmarkDock      )
GETTER(quitAction                   , action_quit                  )
GETTER(addPerson                    , action_addPerson             )
GETTER(focusHistory                 , action_focus_history         )
GETTER(focusContact                 , action_focus_contact         )
GETTER(focusCall                    , action_focus_call            )
GETTER(focusBookmark                , action_focus_bookmark        )
GETTER(showWizard                   , action_show_wizard           )
GETTER(newContact                   , action_new_contact           )

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

#undef INIT_ACTION

// kate: space-indent on; indent-width 3; replace-tabs on;
