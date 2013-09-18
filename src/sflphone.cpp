/************************************** ************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                         *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>         *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
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

//Parent
#include "sflphone.h"

//System
#include <unistd.h>

//Qt
#include <QtCore/QString>
#include <QtGui/QActionGroup>
#include <QtGui/QLabel>
#include <QtGui/QCursor>
#include <QtCore/QPointer>

//KDE
#include <KDebug>
#include <KStandardAction>
#include <KAction>
#include <KStatusBar>
#include <KActionCollection>
#include <KNotification>
#include <KShortcutsDialog>
#include <KComboBox>
#include <KMessageBox>
#include <KStandardDirs>
#include <KEditToolBar>

//sflphone library
#include "lib/sflphone_const.h"
#include "lib/dbus/instancemanager.h"
#include "lib/dbus/configurationmanager.h"
#include "lib/contact.h"
#include "lib/accountlistmodel.h"
#include "lib/instantmessagingmodel.h"
#include "lib/numbercategorymodel.h"
#include "lib/visitors/numbercategoryvisitor.h"
#include "klib/macromodel.h"
#include "klib/akonadibackend.h"
#include "klib/kcfg_settings.h"
#include "lib/presencestatusmodel.h"
#include "lib/videomodel.h"
#include "lib/phonenumber.h"

//sflphone
#include "accountwizard.h"
#include "sflphoneview.h"
#include "widgets/sflphonetray.h"
#include "widgets/contactdock.h"
#include "widgets/historydock.h"
#include "widgets/bookmarkdock.h"
#include "widgets/videodock.h"
#include "widgets/presence.h"
#include "sflphoneaccessibility.h"
#include "extendedaction.h"
#include "errormessage.h"

SFLPhone* SFLPhone::m_sApp = nullptr;

class ConcreteNumberCategoryVisitor :public NumberCategoryVisitor {
   void serialize(NumberCategoryModel* model)
   {
      Q_UNUSED(model)
      QList<int> list;
      for(int i=0;i<model->rowCount();i++) {
         const QModelIndex& idx = model->index(i,0);
         if (idx.data(Qt::CheckStateRole) == Qt::Checked) {
            list << idx.data(NumberCategoryModel::Role::INDEX).toInt();
         }
      }
      ConfigurationSkeleton::setPhoneTypeList(list);
   }

   void load(NumberCategoryModel* model)
   {
      Q_UNUSED(model)
      QList<int> list = ConfigurationSkeleton::phoneTypeList();
      const bool isEmpty = !list.size();
#define IS_ENABLED(name) (list.indexOf(name) != -1) || isEmpty
#define ICN(name) new QPixmap(KStandardDirs::locate("data" , QString("sflphone-client-kde/mini/%1.png").arg(name)))
      model->addCategory("Home"     ,ICN("home")     , KABC::PhoneNumber::Home ,IS_ENABLED( KABC::PhoneNumber::Home     ));
      model->addCategory("Work"     ,ICN("work")     , KABC::PhoneNumber::Work ,IS_ENABLED( KABC::PhoneNumber::Work     ));
      model->addCategory("Msg"      ,ICN("mail")     , KABC::PhoneNumber::Msg  ,IS_ENABLED( KABC::PhoneNumber::Msg      ));
      model->addCategory("Pref"     ,ICN("call")     , KABC::PhoneNumber::Pref ,IS_ENABLED( KABC::PhoneNumber::Pref     ));
      model->addCategory("Voice"    ,ICN("video")    , KABC::PhoneNumber::Voice,IS_ENABLED( KABC::PhoneNumber::Voice    ));
      model->addCategory("Fax"      ,ICN("call")     , KABC::PhoneNumber::Fax  ,IS_ENABLED( KABC::PhoneNumber::Fax      ));
      model->addCategory("Cell"     ,ICN("mobile")   , KABC::PhoneNumber::Cell ,IS_ENABLED( KABC::PhoneNumber::Cell     ));
      model->addCategory("Video"    ,ICN("call")     , KABC::PhoneNumber::Video,IS_ENABLED( KABC::PhoneNumber::Video    ));
      model->addCategory("Bbs"      ,ICN("call")     , KABC::PhoneNumber::Bbs  ,IS_ENABLED( KABC::PhoneNumber::Bbs      ));
      model->addCategory("Modem"    ,ICN("call")     , KABC::PhoneNumber::Modem,IS_ENABLED( KABC::PhoneNumber::Modem    ));
      model->addCategory("Car"      ,ICN("car")      , KABC::PhoneNumber::Car  ,IS_ENABLED( KABC::PhoneNumber::Car      ));
      model->addCategory("Isdn"     ,ICN("call")     , KABC::PhoneNumber::Isdn ,IS_ENABLED( KABC::PhoneNumber::Isdn     ));
      model->addCategory("Pcs"      ,ICN("call")     , KABC::PhoneNumber::Pcs  ,IS_ENABLED( KABC::PhoneNumber::Pcs      ));
      model->addCategory("Pager"    ,ICN("pager")    , KABC::PhoneNumber::Pager,IS_ENABLED( KABC::PhoneNumber::Pager    ));
      model->addCategory("Preferred",ICN("preferred"), 10000                   ,IS_ENABLED( 10000                       ));
#undef ICN
#undef IS_ENABLED
   }

   QVariant icon(QPixmap* icon )
   {
      return QIcon(*icon);
   }
};

///Constructor
SFLPhone::SFLPhone(QWidget *parent)
    : KXmlGuiWindow(parent), m_pInitialized(false)
#ifdef ENABLE_VIDEO
      ,m_pVideoDW(nullptr)
#endif
{
    if (!DBus::InstanceManager::instance().connection().isConnected() || !DBus::InstanceManager::instance().isValid()) {
       QTimer::singleShot(5000,this,SLOT(timeout()));
    }

   static bool init = false;
   if (!init) {
      Call::setContactBackend(AkonadiBackend::instance());
      NumberCategoryModel::instance()->setVisitor(new ConcreteNumberCategoryVisitor());
      InstantMessagingModelManager::init();
      AccountListModel::instance()->setDefaultAccount(AccountListModel::instance()->getAccountById(ConfigurationSkeleton::defaultAccountId()));
      #ifdef ENABLE_VIDEO
      VideoModel::instance();
      #endif
      init = true;
    }

    //Belong to setupActions(), but is needed now
   m_sApp = this;
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
   
   action_mute->setCheckable(true);

   m_pView = new SFLPhoneView(this);

   ConfigurationSkeleton::self();

   // accept dnd
   setAcceptDrops(true);

   // tell the KXmlGuiWindow that this is indeed the main widget
   m_pCentralDW = new QDockWidget(this);
   m_pCentralDW->setSizePolicy  ( QSizePolicy::Expanding,QSizePolicy::Expanding );
   m_pCentralDW->setWidget      ( m_pView                                       );
   m_pCentralDW->setWindowTitle ( i18n("Call")                                  );
   m_pCentralDW->setFeatures    ( QDockWidget::NoDockWidgetFeatures             );
   m_pView->setSizePolicy       ( QSizePolicy::Expanding,QSizePolicy::Expanding );
   m_pCentralDW->setStyleSheet  ( "\
      QDockWidget::title {\
         margin:0px;\
         padding:0px;\
         spacing:0px;\
         max-height:0px;\
      }\
      \
   ");

   m_pCentralDW->setTitleBarWidget(new QWidget());
   m_pCentralDW->setContentsMargins(0,0,0,0);
   m_pView->setContentsMargins     (0,0,0,0);

   m_pContactCD       = new ContactDock  ( this );
   m_pHistoryDW       = new HistoryDock  ( this );
   m_pBookmarkDW      = new BookmarkDock ( this );
   m_pStatusBarWidget = new QLabel       ( this );

   //System tray
   m_pTrayIcon        = new SFLPhoneTray ( this->windowIcon(), this );

   addDockWidget( Qt::TopDockWidgetArea, m_pCentralDW  );
   addDockWidget( Qt::TopDockWidgetArea, m_pContactCD  );
   addDockWidget( Qt::TopDockWidgetArea, m_pHistoryDW  );
   addDockWidget( Qt::TopDockWidgetArea, m_pBookmarkDW );

   tabifyDockWidget(m_pCentralDW,m_pHistoryDW );
   tabifyDockWidget(m_pCentralDW,m_pContactCD );
   tabifyDockWidget(m_pCentralDW,m_pBookmarkDW);

   m_pCentralDW->setObjectName( "callDock" );

   connect(m_pContactCD ,SIGNAL(visibilityChanged(bool)),this,SLOT(updateTabIcons()));
   connect(m_pHistoryDW ,SIGNAL(visibilityChanged(bool)),this,SLOT(updateTabIcons()));
   connect(m_pBookmarkDW,SIGNAL(visibilityChanged(bool)),this,SLOT(updateTabIcons()));
   connect(m_pCentralDW ,SIGNAL(visibilityChanged(bool)),this,SLOT(updateTabIcons()));

   m_pContactCD-> setVisible(ConfigurationSkeleton::displayContactDock() );
   m_pHistoryDW-> setVisible(ConfigurationSkeleton::displayHistoryDock() );
   m_pBookmarkDW->setVisible(ConfigurationSkeleton::displayBookmarkDock());

   m_pCentralDW->show();

   QList<QTabBar*> tabBars = this->findChildren<QTabBar*>();
   if(tabBars.count())
   {
      foreach(QTabBar* bar, tabBars) {
         for (int i=0;i<bar->count();i++) {
            QString text = bar->tabText(i);
            if (text == i18n("Call")) {
               bar->setCurrentIndex(i);
               break;
            }
         }
      }
   }

   setWindowIcon (QIcon(ICON_SFLPHONE) );
   setWindowTitle(i18n("SFLphone")     );
   setupActions();

   m_pTrayIcon->addAction( action_accept   );
   m_pTrayIcon->addAction( action_mailBox  );
   m_pTrayIcon->addAction( action_refuse   );
   m_pTrayIcon->addAction( action_hold     );
   m_pTrayIcon->addAction( action_transfer );
   m_pTrayIcon->addAction( action_record   );
   m_pTrayIcon->addSeparator();
   m_pTrayIcon->addAction( action_quit     );

   connect(action_showContactDock, SIGNAL(toggled(bool)),m_pContactCD, SLOT(setVisible(bool)));
   connect(action_showHistoryDock, SIGNAL(toggled(bool)),m_pHistoryDW, SLOT(setVisible(bool)));
   connect(action_showBookmarkDock,SIGNAL(toggled(bool)),m_pBookmarkDW,SLOT(setVisible(bool)));

#ifdef ENABLE_VIDEO
   connect(VideoModel::instance(),SIGNAL(videoCallInitiated(VideoRenderer*)),this,SLOT(displayVideoDock(VideoRenderer*)));
#endif

   statusBar()->addWidget(m_pStatusBarWidget);

   m_pTrayIcon->show();

   setObjectNames();
   QMetaObject::connectSlotsByName(this);
   m_pView->loadWindow();

   move(QCursor::pos().x() - geometry().width()/2, QCursor::pos().y() - geometry().height()/2);
   show();

   if (AccountListModel::instance()->size() <= 1)
      (new AccountWizard())->show();

   m_pIconChanged = false;
   m_pInitialized = true ;

   KStatusBar* bar = statusBar();
//    bar->setStyleSheet("padding-left:0px;margin-left:0px;");
   int left,top,right,bottom;
   bar->layout()->getContentsMargins ( &left, &top, &right, &bottom );
   bar->layout()->setContentsMargins(0,top,0,bottom);

   //The presence button act as the label, remove the original one
   QList<QLabel*> labels = bar->findChildren<QLabel*>();
   for (int i=0;i<labels.size();i++) {
      bar->layout()->removeWidget(labels[i]);
      labels[i]->setHidden(true);
   }

   m_pPresent = new QToolButton(bar);
   m_pPresent->setAutoRaise(true);
   m_pPresent->setText("Present");
   m_pPresent->setCheckable(true);
//    m_pPresent->setStyleSheet("background-color:red;");
   bar->addWidget(m_pPresent);
   connect(PresenceStatusModel::instance(),SIGNAL(currentNameChanged(QString)),this,SLOT(updatePresence(QString)));
   connect(PresenceStatusModel::instance(),SIGNAL(currentNameChanged(QString)),this,SLOT(hidePresenceDock()));

   m_pPresenceDock = new QDockWidget(this);
   m_pPresenceDock->setWidget(new Presence(m_pPresenceDock));
   m_pPresenceDock->setAllowedAreas(Qt::BottomDockWidgetArea);
   m_pPresenceDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
   addDockWidget( Qt::BottomDockWidgetArea, m_pPresenceDock );
   m_pPresenceDock->setVisible(false);
   if (m_pPresenceDock->titleBarWidget())
      m_pPresenceDock->titleBarWidget()->setVisible(false);

   QLabel* curAccL = new QLabel(i18n("Account:"));
   bar->addPermanentWidget(curAccL);

   m_pAccountStatus = new KComboBox(bar);
   m_pAccountModel = new AccountListNoCheckProxyModel();
   m_pAccountStatus->setModel(m_pAccountModel);
   m_pAccountStatus->setMinimumSize(100,0);
   bar->addPermanentWidget(m_pAccountStatus);
   connect(m_pPresent,SIGNAL(toggled(bool)),m_pPresenceDock,SLOT(setVisible(bool)));

   QToolButton* m_pReloadButton = new QToolButton(this);
   m_pReloadButton->setIcon(KIcon("view-refresh"));
   bar->addPermanentWidget(m_pReloadButton);
   connect(m_pReloadButton,SIGNAL(clicked()),AccountListModel::instance(),SLOT(registerAllAccounts()));
   connect(m_pAccountStatus, SIGNAL(currentIndexChanged(int)), this, SLOT(currentAccountIndexChanged(int)) );
   connect(AccountListModel::instance(), SIGNAL(priorAccountChanged(Account*)),this,SLOT(currentPriorAccountChanged(Account*)));

   if (!CallModel::instance()->isValid()) {
      KMessageBox::error(this,i18n("The SFLPhone daemon (sflphoned) is not available. Please be sure it is installed correctly or launch it manually"));
      QTimer::singleShot(2500,this,SLOT(timeout())); //FIXME this may leave the client in an unreliable state
      //exit(1); //Don't try to exit normally, it will segfault, the application is already in a broken state if this is reached //BUG break some slow netbooks
   }
   try {
      currentPriorAccountChanged(AccountListModel::currentAccount());
   }
   catch(const char * msg) {
      KMessageBox::error(this,msg);
      QTimer::singleShot(2500,this,SLOT(timeout())); //FIXME this may leave the client in an unreliable state
      //exit(1); //Don't try to exit normally, it will segfault, the application is already in a broken state if this is reached //BUG break some slow netbooks
   }
} //SFLPhone

///Destructor
SFLPhone::~SFLPhone()
{
   if (!isHidden()) {
      ConfigurationSkeleton::setDisplayContactDock ( m_pContactCD->isVisible()  );
      ConfigurationSkeleton::setDisplayHistoryDock ( m_pHistoryDW->isVisible()  );
      ConfigurationSkeleton::setDisplayBookmarkDock( m_pBookmarkDW->isVisible() );
   }

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

   delete m_pView            ;
   delete m_pTrayIcon        ;
   delete m_pStatusBarWidget ;
   delete m_pContactCD       ;
   delete m_pCentralDW       ;
   delete m_pHistoryDW       ;
   delete m_pBookmarkDW      ;
   delete m_pAccountModel    ;
   delete m_pPresent         ;
   delete m_pPresenceDock    ;

   delete AkonadiBackend::instance();
   delete CallModel::instance();
   //saveState();
}

///Init everything
bool SFLPhone::initialize()
{
   if ( m_pInitialized ) {
      kDebug() << "Already initialized.";
      return false;
   }

   return true;
}

///Setup evry actions
void SFLPhone::setupActions()
{
   kDebug() << "setupActions";

   action_mailBox  = new KAction(this);
   action_accept->setShortcut      ( Qt::CTRL + Qt::Key_A );
   action_refuse->setShortcut      ( Qt::CTRL + Qt::Key_D );
   action_hold->setShortcut        ( Qt::CTRL + Qt::Key_H );
   action_transfer->setShortcut    ( Qt::CTRL + Qt::Key_T );
   action_record->setShortcut      ( Qt::CTRL + Qt::Key_R );
   action_mailBox->setShortcut     ( Qt::CTRL + Qt::Key_M );

   action_screen = new QActionGroup(this);
   action_screen->setExclusive(true);

   action_close = KStandardAction::close(this, SLOT(close()), this);
   action_quit  = KStandardAction::quit(this, SLOT(quitButton()), this);

   action_configureSflPhone = KStandardAction::preferences(m_pView, SLOT(configureSflPhone()), this);
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



   action_configureShortcut = new KAction(KIcon(KIcon("configure-shortcuts")), i18n("Configure Shortcut"), this);
   //                    SENDER                        SIGNAL               RECEIVER                 SLOT               /
   /**/connect(action_accept,                SIGNAL(triggered()),           m_pView , SLOT(accept())                    );
   /**/connect(action_hangup,                SIGNAL(triggered()),           m_pView , SLOT(hangup())                    );
   /**/connect(action_refuse,                SIGNAL(triggered()),           m_pView , SLOT(refuse())                    );
   /**/connect(action_hold,                  SIGNAL(triggered()),           m_pView , SLOT(hold())                      );
   /**/connect(action_unhold,                SIGNAL(triggered()),           m_pView , SLOT(unhold())                    );
   /**/connect(action_transfer,              SIGNAL(triggered()),           m_pView , SLOT(transfer())                  );
   /**/connect(action_record,                SIGNAL(triggered()),           m_pView , SLOT(record())                    );
   /**/connect(action_mute,                  SIGNAL(toggled(bool)),         m_pView , SLOT(mute(bool))                  );
   /**/connect(action_mailBox,               SIGNAL(triggered()),           m_pView , SLOT(mailBox())                   );
   /**/connect(action_displayVolumeControls, SIGNAL(toggled(bool)),         m_pView , SLOT(displayVolumeControls(bool)) );
   /**/connect(action_displayDialpad,        SIGNAL(toggled(bool)),         m_pView , SLOT(displayDialpad(bool))        );
   /**/connect(action_displayMessageBox,     SIGNAL(toggled(bool)),         m_pView , SLOT(displayMessageBox(bool))     );
   /**/connect(action_accountCreationWizard, SIGNAL(triggered()),           m_pView , SLOT(accountCreationWizard())     );
   /**/connect(action_pastenumber,           SIGNAL(triggered()),           m_pView , SLOT(paste())                     );
   /**/connect(action_configureShortcut,     SIGNAL(triggered()),           this    , SLOT(showShortCutEditor())        );
   /**/connect(action_editToolBar,           SIGNAL(triggered()),           this    , SLOT(editToolBar())               );
   /**/connect(MacroModel::instance(),    SIGNAL(addAction(KAction*)),   this    , SLOT(addMacro(KAction*))          );
   /*                                                                                                                   */

   actionCollection()->addAction("action_accept"                , action_accept                );
   actionCollection()->addAction("action_refuse"                , action_refuse                );
   actionCollection()->addAction("action_hold"                  , action_hold                  );
   actionCollection()->addAction("action_transfer"              , action_transfer              );
   actionCollection()->addAction("action_record"                , action_record                );
   actionCollection()->addAction("action_mailBox"               , action_mailBox               );
   actionCollection()->addAction("action_close"                 , action_close                 );
   actionCollection()->addAction("action_quit"                  , action_quit                  );
   actionCollection()->addAction("action_displayVolumeControls" , action_displayVolumeControls );
   actionCollection()->addAction("action_displayDialpad"        , action_displayDialpad        );
   actionCollection()->addAction("action_displayMessageBox"     , action_displayMessageBox     );
   actionCollection()->addAction("action_configureSflPhone"     , action_configureSflPhone     );
   actionCollection()->addAction("action_accountCreationWizard" , action_accountCreationWizard );
   actionCollection()->addAction("action_configureShortcut"     , action_configureShortcut     );
   actionCollection()->addAction("action_pastenumber"           , action_pastenumber           );
   actionCollection()->addAction("action_showContactDock"       , action_showContactDock       );
   actionCollection()->addAction("action_showHistoryDock"       , action_showHistoryDock       );
   actionCollection()->addAction("action_showBookmarkDock"      , action_showBookmarkDock      );
   actionCollection()->addAction("action_editToolBar"           , action_editToolBar           );

   MacroModel::instance()->initMacros();


   QList<KAction*> acList = *SFLPhoneAccessibility::instance();

   foreach(KAction* ac,acList) {
      actionCollection()->addAction(ac->objectName() , ac);
   }

   setAutoSaveSettings();
   createGUI();
}


/*****************************************************************************
 *                                                                           *
 *                                  Getters                                  *
 *                                                                           *
 ****************************************************************************/

///Singleton
SFLPhone* SFLPhone::app()
{
   return m_sApp;
}

///Get the view (to be used with the singleton)
SFLPhoneView* SFLPhone::view()
{
   return m_pView;
}

///Return the contact dock
ContactDock*  SFLPhone::contactDock()
{
   return m_pContactCD;
}

///Return the history dock
HistoryDock*  SFLPhone::historyDock()
{
   return m_pHistoryDW;
}

///Return the bookmark dock
BookmarkDock* SFLPhone::bookmarkDock()
{
   return m_pBookmarkDW;
}

void SFLPhone::showShortCutEditor() {
   KShortcutsDialog::configure( actionCollection() );
}

///Produce an actionList for auto CallBack
QList<QAction*> SFLPhone::getCallActions()
{
   QList<QAction*> callActions = QList<QAction *>();
   callActions.insert((int) Accept   , action_accept   );
   callActions.insert((int) Refuse   , action_refuse   );
   callActions.insert((int) Hold     , action_hold     );
   callActions.insert((int) Transfer , action_transfer );
   callActions.insert((int) Record   , action_record   );
   callActions.insert((int) Mailbox  , action_mailBox  );
   return callActions;
}


/*****************************************************************************
 *                                                                           *
 *                                  Setters                                  *
 *                                                                           *
 ****************************************************************************/

///Set widgets object name
void SFLPhone::setObjectNames()
{
   m_pView->setObjectName      ( "m_pView"       );
   statusBar()->setObjectName  ( "statusBar"     );
   m_pTrayIcon->setObjectName  ( "m_pTrayIcon"   );
}


/*****************************************************************************
 *                                                                           *
 *                                  Mutator                                  *
 *                                                                           *
 ****************************************************************************/

///[Action]Hide sflphone
bool SFLPhone::queryClose()
{
   if (!isHidden()) {
      ConfigurationSkeleton::setDisplayContactDock ( m_pContactCD->isVisible()  );
      ConfigurationSkeleton::setDisplayHistoryDock ( m_pHistoryDW->isVisible()  );
      ConfigurationSkeleton::setDisplayBookmarkDock( m_pBookmarkDW->isVisible() );
   }
   hide();
   return false;
}

///Be sure the default size is look like a phone
QSize SFLPhone::sizeHint() const
{
   return QSize(340,700);
}

///[Action] Quit action
void SFLPhone::quitButton()
{
   qApp->quit();
}

///Called when something happen
void SFLPhone::changeEvent(QEvent* event)
{
   if (event->type() == QEvent::ActivationChange && m_pIconChanged && isActiveWindow()) {
      m_pIconChanged = false;
   }
}

///Change status message
void SFLPhone::on_m_pView_statusMessageChangeAsked(const QString& message)
{
   Q_UNUSED(message)
//    m_pStatusBarWidget->setText(message);
}

///Change windowtitle
void SFLPhone::on_m_pView_windowTitleChangeAsked(const QString& message)
{
   setWindowTitle(message);
}

///Enable or disable toolbar items
void SFLPhone::on_m_pView_enabledActionsChangeAsked(const bool* enabledActions)
{
   action_accept->setVisible   ( enabledActions[SFLPhone::Accept   ]);
   action_refuse->setVisible   ( enabledActions[SFLPhone::Refuse   ]);
   action_hold->setVisible     ( enabledActions[SFLPhone::Hold     ]);
   action_transfer->setVisible ( enabledActions[SFLPhone::Transfer ]);
   action_record->setVisible   ( enabledActions[SFLPhone::Record   ]);
   action_mailBox->setVisible  ( enabledActions[SFLPhone::Mailbox  ]);
}

///Change icons
void SFLPhone::on_m_pView_actionIconsChangeAsked(const QString* actionIcons)
{
   action_accept->setIcon   ( QIcon(actionIcons[SFLPhone::Accept   ]));
   action_refuse->setIcon   ( QIcon(actionIcons[SFLPhone::Refuse   ]));
   action_hold->setIcon     ( QIcon(actionIcons[SFLPhone::Hold     ]));
   action_transfer->setIcon ( QIcon(actionIcons[SFLPhone::Transfer ]));
   action_record->setIcon   ( QIcon(actionIcons[SFLPhone::Record   ]));
   action_mailBox->setIcon  ( QIcon(actionIcons[SFLPhone::Mailbox  ]));
}

///Change text
void SFLPhone::on_m_pView_actionTextsChangeAsked(const QString* actionTexts)
{
   action_accept->setText   ( actionTexts[SFLPhone::Accept   ]);
   action_refuse->setText   ( actionTexts[SFLPhone::Refuse   ]);
   action_hold->setText     ( actionTexts[SFLPhone::Hold     ]);
   action_transfer->setText ( actionTexts[SFLPhone::Transfer ]);
   action_record->setText   ( actionTexts[SFLPhone::Record   ]);
   action_mailBox->setText  ( actionTexts[SFLPhone::Mailbox  ]);
}

///Change transfer state
void SFLPhone::on_m_pView_transferCheckStateChangeAsked(bool transferCheckState)
{
   action_transfer->setChecked(transferCheckState);
}

///Change record state
void SFLPhone::on_m_pView_recordCheckStateChangeAsked(bool recordCheckState)
{
   action_record->setChecked(recordCheckState);
}

///Called when a call is coming
void SFLPhone::on_m_pView_incomingCall(const Call* call)
{
   if (call) {
      const Contact* contact = call->peerPhoneNumber()->contact();
      if (contact)
         KNotification::event(KNotification::Notification, i18n("New incoming call"), i18n("New call from:\n%1",call->peerName().isEmpty() ? call->peerPhoneNumber()->uri() : call->peerName()),((contact->photo())?*contact->photo():nullptr));
      else
         KNotification::event(KNotification::Notification, i18n("New incoming call"), i18n("New call from:\n%1",call->peerName().isEmpty() ? call->peerPhoneNumber()->uri() : call->peerName()));
   }
}

///Change current account
void SFLPhone::currentAccountIndexChanged(int newIndex)
{
   if (AccountListModel::instance()->size()) {
      const Account* acc = AccountListModel::instance()->getAccountByModelIndex(AccountListModel::instance()->index(newIndex,0));
      if (acc)
         AccountListModel::instance()->setPriorAccount(acc);
   }
}

///Update the combobox index
void SFLPhone::currentPriorAccountChanged(Account* newPrior)
{
   if (DBus::InstanceManager::instance().connection().isConnected() && newPrior) {
      m_pAccountStatus->setCurrentIndex(newPrior->index().row());
   }
   else {
      kDebug() << "Daemon not responding";
   }
}

///Qt does not support dock icons by default, this is an hack around this
void SFLPhone::updateTabIcons()
{
   QList<QTabBar*> tabBars = this->findChildren<QTabBar*>();
   if(tabBars.count())
   {
      foreach(QTabBar* bar, tabBars) {
         for (int i=0;i<bar->count();i++) {
            QString text = bar->tabText(i);
            if (text == i18n("Call")) {
               bar->setTabIcon(i,KIcon("call-start"));
            }
            else if (text == i18n("Bookmark")) {
               bar->setTabIcon(i,KIcon("bookmarks"));
            }
            else if (text == i18n("Contact")) {
               bar->setTabIcon(i,KIcon("edit-find-user"));
            }
            else if (text == i18n("History")) {
               bar->setTabIcon(i,KIcon("view-history"));
            }
         }
      }
   }
} //updateTabIcons

///Add a new dynamic action (macro)
void SFLPhone::addMacro(KAction* newAction)
{
   actionCollection()->addAction(newAction->objectName() , newAction );
}

///Show the toolbar editor
void SFLPhone::editToolBar()
{
   QPointer<KEditToolBar> toolbareditor = new KEditToolBar(guiFactory());
   toolbareditor->setModal(true);
   toolbareditor->exec();
   toolbareditor->setDefaultToolBar("mainToolBar");
   delete toolbareditor;
}

///Update presence label
void SFLPhone::updatePresence(const QString& status)
{
   m_pPresent->setText(status);
   m_pPresent->setToolTip(PresenceStatusModel::instance()->currentMessage());
}

///Hide the presence dock when not required
void SFLPhone::hidePresenceDock()
{
   m_pPresent->setChecked(false);
}

#ifdef ENABLE_VIDEO
///Display the video dock
void SFLPhone::displayVideoDock(VideoRenderer* r)
{
   if (!m_pVideoDW) {
      m_pVideoDW = new VideoDock(this);
   }
   m_pVideoDW->setRenderer(r);
   m_pVideoDW->show();
}
#endif

///The daemon is not found
void SFLPhone::timeout()
{
   if (!DBus::InstanceManager::instance().connection().isConnected() || !DBus::InstanceManager::instance().isValid() || (!CallModel::instance()->isValid())) {
      KMessageBox::error(this,ErrorMessage::NO_DAEMON_ERROR);
      exit(1);
   }
}
