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
#include <QtCore/QTimer>
#include <QtGui/QKeySequence>
#include <QQuickWindow>

//KDE
#include <KLocalizedString>
#include <KStandardAction>
#include <KShortcutsDialog>
#include <KActionCollection>
#include <KSharedConfig>
#include <KNotifyConfigWidget>
#include <KGlobalAccel>
#include <KXmlGuiWindow>

//Ring
#include "globalinstances.h"
#include "windowevent.h"
#include "kcfg_settings.h"
#include <call.h>
#include <callmodel.h>
#include <session.h>
#include <useractionmodel.h>
#include <audio/settings.h>
#include <persondirectory.h>
#include "shortcutinterface.h"
#include <interfaces/pixmapmanipulatori.h>
#include <interfaces/actionextenderi.h>

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

ActionCollection::ActionCollection(QObject* parent) : QObject(parent)
{
   // It is important to init the actions correctly for the menu and KDE global shortcuts
   INIT_ACTION(action_close         , QIcon::fromTheme("window-close"), i18n( "Close"   ));

   INIT_ACTION(action_accept        , QIcon(QStringLiteral(":/sharedassets/phone_light/accept.svg"   )), i18n( "Accept"   ));
   INIT_ACTION(action_hold          , QIcon(QStringLiteral(":/sharedassets/phone_light/hold.svg"     )), i18n( "Hold"     ));
   INIT_ACTION(action_transfer      , QIcon(QStringLiteral(":/sharedassets/phone_light/transfert.svg")), i18n( "Transfer" ));
   INIT_ACTION(action_record        , QIcon(QStringLiteral(":/sharedassets/phone_light/record_call.svg" )), i18n( "Record"   ));
   INIT_ACTION(action_hangup        , QIcon(QStringLiteral(":/sharedassets/phone_light/refuse.svg"  )), i18n( "Hang up"  ));

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

   INIT_ACTION(action_video_rotate_left     , QIcon::fromTheme(QStringLiteral("object-rotate-left"    )), i18n( "Rotate left"       ));
   INIT_ACTION(action_video_rotate_right    , QIcon::fromTheme(QStringLiteral("object-rotate-right"   )), i18n( "Rotate right"      ));
   INIT_ACTION(action_video_flip_horizontal , QIcon::fromTheme(QStringLiteral("object-flip-horizontal")), i18n( "Flip"              ));
   INIT_ACTION(action_video_flip_vertical   , QIcon::fromTheme(QStringLiteral("object-flip-vertical"  )), i18n( "Flip"              ));
   INIT_ACTION(action_video_mute            , QIcon::fromTheme(QStringLiteral("camera-web"            )), i18n( "Mute"              ));
   INIT_ACTION(action_video_preview         , QIcon::fromTheme(QStringLiteral("view-preview"          )), i18n( "Preview"           ));
   INIT_ACTION(action_video_scale           , QIcon::fromTheme(QStringLiteral("transform-scale"       )), i18n( "Keep aspect ratio" ));
   INIT_ACTION(action_video_fullscreen      , QIcon::fromTheme(QStringLiteral("view-fullscreen"       )), i18n( "Fullscreen"        ));
}

ActionCollection::~ActionCollection()
{}

QObject* ActionCollection::fakeMainWindow() const
{
   static auto mw = new KXmlGuiWindow();

   static bool init = false;
   if (!init) {
      // Use a QTimer since it can enter in a recursion if there is a shortcut
      // collision creating a warning popup parented on the main window.
      #ifdef Q_OS_MAC
         QDir dir(QApplication::applicationDirPath());
         dir.cdUp();
         dir.cd("Resources/");
         QTimer::singleShot(0, [dir]() {mw->createGUI(dir.path()+"/ring-kdeui.rc");});
      #else
         QTimer::singleShot(0, []() {mw->createGUI();});
      #endif
      init = true;
   }

   return mw;
}

QObject* ActionCollection::kactionCollection() const
{
   return qobject_cast<KXmlGuiWindow*>(fakeMainWindow())->actionCollection();
}

void ActionCollection::setupAction()
{
   auto mw  = qobject_cast<KXmlGuiWindow*>(fakeMainWindow());
   auto col = mw->actionCollection();

   // Import standard actions
   action_quit = KStandardAction::quit       ( WindowEvent::instance(), SLOT(quit())   , this);

   action_quit ->setObjectName( QStringLiteral("action_quit" ) );
   action_close->setObjectName( QStringLiteral("action_close") );

   action_quit->setText(i18n("Quit"));

   INIT_ACTION(action_displayDialpad        , {}                               , i18n("Display dialpad"         ));
   INIT_ACTION(action_displayVolumeControls , {}                               , i18n("Display volume controls" ));
   INIT_ACTION(action_displayAccountCbb     , {}                               , i18n("Display account selector"));
   INIT_ACTION(action_raise_client          , {}                               , i18n("Raise Ring-KDE window"   ));
   INIT_ACTION(action_focus_history         , {}                               , i18n("Search history"          ));
   INIT_ACTION(action_focus_call            , {}                               , i18n("Search call"             ));
   INIT_ACTION(action_focus_contact         , {}                               , i18n("Search contact"          ));
   INIT_ACTION(action_focus_bookmark        , {}                               , i18n("Search bookmark"         ));

   INIT_ACTION(action_show_wizard           , QIcon::fromTheme(QStringLiteral("tools-wizard"                    )), i18n("New account wizard"      ));
   INIT_ACTION(action_show_menu             , QIcon::fromTheme(QStringLiteral("application-menu"                )), i18n("Show the menu"           ));
   INIT_ACTION(action_new_contact           , QIcon::fromTheme(QStringLiteral("contact-new"                     )), i18n("New contact"             ));
   INIT_ACTION(action_pastenumber           , QIcon::fromTheme(QStringLiteral("edit-paste"                      )), i18n("Paste"                   ));
   INIT_ACTION(action_showContactDock       , QIcon::fromTheme(QStringLiteral("edit-find-user"                  )), i18n("Display Person"          ));
   INIT_ACTION(action_showHistoryDock       , QIcon::fromTheme(QStringLiteral("view-history"                    )), i18n("Display history"         ));
   INIT_ACTION(action_showBookmarkDock      , QIcon::fromTheme(QStringLiteral("bookmark-new-list"               )), i18n("Display bookmark"        ));
   INIT_ACTION(action_showTimelineDock      , QIcon::fromTheme(QStringLiteral("bookmark-new-list"               )), i18n("Display timeline"        ));
   INIT_ACTION(action_showDialDock          , QIcon::fromTheme(QStringLiteral("bookmark-new-list"               )), i18n("Display call manager"    ));
   INIT_ACTION(action_addPerson             , QIcon::fromTheme(QStringLiteral("contact-new"                     )), i18n("Add new contact"         ));
   INIT_ACTION(action_configureShortcut     , QIcon::fromTheme(QStringLiteral("configure-shortcuts"             )), i18n("Configure Shortcuts"     ));
   INIT_ACTION(action_configureNotifications, QIcon::fromTheme(QStringLiteral("preferences-desktop-notification")), i18n("Configure Notifications" ));
   INIT_ACTION(action_configureAccount      , QIcon::fromTheme(QStringLiteral("configure"                       )), i18n("Configure Accounts"      ));

#define COL(a,b) col->setDefaultShortcut(a,b)
   // Assign default shortcuts
   COL(action_accept      , Qt::CTRL + Qt::Key_A );
   COL(action_new_call    , Qt::CTRL + Qt::Key_N );
   COL(action_hold        , Qt::CTRL + Qt::Key_H );
   COL(action_transfer    , Qt::CTRL + Qt::Key_T );
   COL(action_record      , Qt::CTRL + Qt::Key_R );
   COL(action_pastenumber , Qt::CTRL + Qt::Key_V );
   COL(action_show_menu   , Qt::CTRL + Qt::Key_M );
#undef COL


   // Declare checkable actions
   for (QAction* a : {
      action_video_preview        , action_video_scale          , action_video_fullscreen  ,
      action_video_mute           , action_displayDialpad       , action_displayAccountCbb ,
      action_mute_playback        , action_displayVolumeControls, action_showContactDock   ,
      action_showHistoryDock      , action_showBookmarkDock     , action_mute_capture      ,
      action_show_menu            , action_showTimelineDock     , action_showDialDock      ,
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
   action_showTimelineDock      ->setChecked( ConfigurationSkeleton::displayRecentDock   () );
   action_showDialDock          ->setChecked( ConfigurationSkeleton::displayDialDock     () );
   action_show_menu             ->setChecked( ConfigurationSkeleton::displayMenu         () );

#define BIND_KCFG(action, setter) connect(action, &QAction::triggered,[](bool v) {\
   ConfigurationSkeleton:: setter(v);\
});

   BIND_KCFG(action_showHistoryDock , setDisplayHistoryDock )
   BIND_KCFG(action_showContactDock , setDisplayContactDock )
   BIND_KCFG(action_showBookmarkDock, setDisplayBookmarkDock)
   BIND_KCFG(action_showDialDock    , setDisplayDialDock    )
   BIND_KCFG(action_showTimelineDock, setDisplayRecentDock  )

#undef BIND_KCFG

   //Bind actions to the useractionmodel
   UserActionModel* uam = Session::instance()->callModel()->userActionModel();
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
      connect(ea, &QAction::triggered, this, [uam,a](bool) {uam << a;});
   }

   // Refresh the action state and text
   static auto l = [actionHash,uam](const QModelIndex& tl, const QModelIndex& br) {
      const int first(tl.row()),last(br.row());
      for(int i = first; i <= last;i++) {
         const auto idx = uam->index(i,0);
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
   connect(action_show_wizard            , &QAction::triggered , WindowEvent::instance(), &WindowEvent::showWizard      );
   connect(action_configureAccount       , &QAction::triggered , WindowEvent::instance(), &WindowEvent::configureAccounts);
   connect(action_show_menu              , &QAction::toggled   , this               , &ActionCollection::slotShowMenubar        );
   connect(action_close                  , &QAction::triggered , this               , &ActionCollection::slotClose              );
   connect(action_new_contact            , &QAction::triggered , this               , &ActionCollection::slotNewContact         );
   connect(action_configureShortcut      , &QAction::triggered , this               , &ActionCollection::showShortCutEditor     );
   connect(action_configureNotifications , &QAction::triggered , this               , &ActionCollection::showNotificationEditor );
   connect(action_addPerson              , &QAction::triggered , this               , &ActionCollection::slotAddPerson          );
   connect(action_raise_client           , &QAction::triggered , this               , &ActionCollection::slotRaiseClient        );

   connect(as, &Audio::Settings::captureVolumeChanged  , this                , &ActionCollection::updateRecordButton );
   connect(as, &Audio::Settings::playbackVolumeChanged , this                , &ActionCollection::updateVolumeButton );
   connect(as, &Audio::Settings::captureMuted          , action_mute_capture , &QAction::setChecked                  );
   connect(as, &Audio::Settings::playbackMuted         , action_mute_playback, &QAction::setChecked                  );

   // Add the actions to the collection
   for (QAction* a : {
      action_accept            , action_new_call          , action_hold              ,
      action_transfer          , action_record            , action_new_contact       ,
      action_quit              , action_displayDialpad    , action_showDialDock      ,
      action_displayAccountCbb , action_close             ,
      action_configureShortcut , action_pastenumber       , action_showContactDock   ,
      action_showHistoryDock   , action_showBookmarkDock  ,
      action_addPerson         , action_mute_capture      , action_mute_playback     ,
      action_mute_video        , action_join              , action_toggle_video      ,
      action_add_contact       , action_add_to_contact    , action_delete_contact    ,
      action_email_contact     , action_copy_contact      , action_bookmark          ,
      action_view_chat_history , action_add_contact_method, action_call_contact      ,
      action_edit_contact      , action_focus_history     , action_remove_history    ,
      action_raise_client      , action_focus_contact     , action_focus_call        ,
      action_focus_bookmark    , action_show_wizard       ,
      action_show_menu         , action_showTimelineDock  , action_configureAccount  ,
      action_configureNotifications, action_displayVolumeControls ,
   }) {
      col->addAction(a->objectName(), a);
   }

   // Enable global shortcuts for relevant "current call" actions
   for (QAction* a : {
      action_accept       , action_new_call    , action_hold         ,
      action_mute_capture , action_transfer    , action_record       ,
      action_hangup       , action_raise_client, action_focus_history,
      action_focus_contact, action_focus_call  , action_focus_bookmark
   }) {
      KGlobalAccel::self()->setGlobalShortcut(a, QList<QKeySequence>{});
   }

   GlobalInstances::setInterface<ShortcutInterface>();

#ifdef HAVE_SPEECH
   QList<QAction *> acList = *Accessibility::instance();

   foreach(QAction * ac,acList) {
      col->addAction(ac->objectName() , ac);
   }
#endif

   QTimer::singleShot(0, [this]() {
      updateRecordButton();
      updateVolumeButton();
   });
}

///Display the shortcuts dialog
void ActionCollection::showShortCutEditor()
{
   KShortcutsDialog::configure(
      qobject_cast<KXmlGuiWindow*>(
         ActionCollection::instance()->fakeMainWindow()
      )->actionCollection()
   );
}

///Display the notification manager
void ActionCollection::showNotificationEditor()
{
   KNotifyConfigWidget::configure(nullptr, QStringLiteral("ring-kde"));
}

void ActionCollection::slotAddPerson()
{
   Person* aPerson = new Person();
   Session::instance()->personDirectory()->addNewPerson(aPerson);
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
}

void ActionCollection::slotNewContact()
{
   // Find a suitable collection
   auto cols = Session::instance()->personDirectory()->enabledCollections(
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

   // The editor won't save if the name isn't set, so it can be used to check
   // if cancel was clicked.
   col->editor<Person>()->addExisting(p);
}

void ActionCollection::slotClose()
{
   WindowEvent::instance()->hideWindow();
}

void ActionCollection::slotShowMenubar(bool s)
{
   Q_UNUSED(s)
//    RingApplication::instance()->timelineWindow()->showMenu(s);
}

///Raise the main window to the foreground
void ActionCollection::slotRaiseClient(bool focus)
{
   if (focus) {
      // Add a new call if there is none
      if (!Session::instance()->callModel()->rowCount())
         Session::instance()->callModel()->userActionModel() << UserActionModel::Action::ADD_NEW;
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
GETTER(showTimelineDockAction       , action_showTimelineDock      )
GETTER(showDialDockAction           , action_showDialDock          )
GETTER(quitAction                   , action_quit                  )
GETTER(closeAction                  , action_close                 )
GETTER(addPerson                    , action_addPerson             )
GETTER(focusHistory                 , action_focus_history         )
GETTER(focusContact                 , action_focus_contact         )
GETTER(focusCall                    , action_focus_call            )
GETTER(focusBookmark                , action_focus_bookmark        )
GETTER(showWizard                   , action_show_wizard           )
GETTER(showMenu                     , action_show_menu             )
GETTER(newContact                   , action_new_contact           )
GETTER(configureShortcut            , action_configureShortcut     )
GETTER(configureNotification        , action_configureNotifications)
GETTER(configureAccount             , action_configureAccount)

//Video actions
GETTER(videoRotateLeftAction        , action_video_rotate_left     )
GETTER(videoRotateRightAction       , action_video_rotate_right    )
GETTER(videoFlipHorizontalAction    , action_video_flip_horizontal )
GETTER(videoFlipVerticalAction      , action_video_flip_vertical   )
GETTER(videoMuteAction              , action_video_mute            )
GETTER(videoPreviewAction           , action_video_preview         )
GETTER(videoScaleAction             , action_video_scale           )
GETTER(videoFullscreenAction        , action_video_fullscreen      )

#undef GETTER

#undef INIT_ACTION

// kate: space-indent on; indent-width 3; replace-tabs on;
