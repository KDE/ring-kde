/************************************** ************************************
 *   Copyright (C) 2009-2014 by Savoir-Faire Linux                         *
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
#include <KLocale>
#include <KEditToolBar>
#include <KIcon>

//sflphone library
#include "lib/sflphone_const.h"
#include "lib/dbus/instancemanager.h"
#include "lib/contact.h"
#include "lib/accountlistmodel.h"
#include "lib/instantmessagingmodel.h"
#include "lib/numbercategorymodel.h"
#include "lib/legacyhistorybackend.h"
#include "klib/minimalhistorybackend.h"
#include "lib/visitors/numbercategoryvisitor.h"
#include "klib/macromodel.h"
#include "klib/akonadibackend.h"
#include "klib/kcfg_settings.h"
#include "klib/akonadicontactcollectionmodel.h"
#include "lib/presencestatusmodel.h"
#include "lib/video/videomodel.h"
#include "lib/phonenumber.h"
#include "lib/contactmodel.h"
#include "lib/itembackendmodel.h"
#include "lib/visitors/itemmodelstateserializationvisitor.h"
#include "klib/itemmodelserialization.h"
#include "lib/extensions/presenceitembackendmodelextension.h"

//sflphone
#include "accountwizard.h"
#include "sflphoneview.h"
#include "actioncollection.h"
#include "widgets/sflphonetray.h"
#include "widgets/contactdock.h"
#include "widgets/historydock.h"
#include "widgets/bookmarkdock.h"
#include "widgets/presence.h"
#include "sflphoneaccessibility.h"
#include "extendedaction.h"
#include "errormessage.h"
#ifdef ENABLE_VIDEO
#include "widgets/videodock.h"
#endif

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
      model->addCategory(i18n("Home")     ,ICN("home")     , KABC::PhoneNumber::Home ,IS_ENABLED( KABC::PhoneNumber::Home     ));
      model->addCategory(i18n("Work")     ,ICN("work")     , KABC::PhoneNumber::Work ,IS_ENABLED( KABC::PhoneNumber::Work     ));
      model->addCategory(i18n("Msg")      ,ICN("mail")     , KABC::PhoneNumber::Msg  ,IS_ENABLED( KABC::PhoneNumber::Msg      ));
      model->addCategory(i18n("Pref")     ,ICN("call")     , KABC::PhoneNumber::Pref ,IS_ENABLED( KABC::PhoneNumber::Pref     ));
      model->addCategory(i18n("Voice")    ,ICN("video")    , KABC::PhoneNumber::Voice,IS_ENABLED( KABC::PhoneNumber::Voice    ));
      model->addCategory(i18n("Fax")      ,ICN("call")     , KABC::PhoneNumber::Fax  ,IS_ENABLED( KABC::PhoneNumber::Fax      ));
      model->addCategory(i18n("Cell")     ,ICN("mobile")   , KABC::PhoneNumber::Cell ,IS_ENABLED( KABC::PhoneNumber::Cell     ));
      model->addCategory(i18n("Video")    ,ICN("call")     , KABC::PhoneNumber::Video,IS_ENABLED( KABC::PhoneNumber::Video    ));
      model->addCategory(i18n("Bbs")      ,ICN("call")     , KABC::PhoneNumber::Bbs  ,IS_ENABLED( KABC::PhoneNumber::Bbs      ));
      model->addCategory(i18n("Modem")    ,ICN("call")     , KABC::PhoneNumber::Modem,IS_ENABLED( KABC::PhoneNumber::Modem    ));
      model->addCategory(i18n("Car")      ,ICN("car")      , KABC::PhoneNumber::Car  ,IS_ENABLED( KABC::PhoneNumber::Car      ));
      model->addCategory(i18n("Isdn")     ,ICN("call")     , KABC::PhoneNumber::Isdn ,IS_ENABLED( KABC::PhoneNumber::Isdn     ));
      model->addCategory(i18n("Pcs")      ,ICN("call")     , KABC::PhoneNumber::Pcs  ,IS_ENABLED( KABC::PhoneNumber::Pcs      ));
      model->addCategory(i18n("Pager")    ,ICN("pager")    , KABC::PhoneNumber::Pager,IS_ENABLED( KABC::PhoneNumber::Pager    ));
      model->addCategory(i18n("Preferred"),ICN("preferred"), 10000                   ,IS_ENABLED( 10000                       ));
#undef ICN
#undef IS_ENABLED
   }

//    QVariant icon(QPixmap* icon )
//    {
//       return QIcon(*icon);
//    }
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

      //Start the Akonadi collection backend (contact loader)
      AkonadiContactCollectionModel::instance();
      HistoryModel::instance()->addBackend(new MinimalHistoryBackend(this),LoadOptions::FORCE_ENABLED);

      // Import all calls from the legacy backend
      if (ConfigurationSkeleton::requireLegacyHistoryImport()) {
         HistoryModel::instance()->addBackend(new LegacyHistoryBackend(this),LoadOptions::FORCE_ENABLED);
         ConfigurationSkeleton::setRequireLegacyHistoryImport(false);

         //In case the client is not quitted correctly, save now
         ConfigurationSkeleton::self()->writeConfig();
      }
      NumberCategoryModel::instance()->setVisitor(new ConcreteNumberCategoryVisitor());
      ItemModelStateSerializationVisitor::setInstance(new ItemModelStateSerialization());
      ContactModel::instance()->backendModel()->load();
      InstantMessagingModelManager::init();
      AccountListModel::instance()->setDefaultAccount(AccountListModel::instance()->getAccountById(ConfigurationSkeleton::defaultAccountId()));
      #ifdef ENABLE_VIDEO
      VideoModel::instance();
      #endif
      init = true;

      PresenceItemBackendModelExtension* ext = new PresenceItemBackendModelExtension(this);
      ContactModel::instance()->backendModel()->addExtension(ext);
   }

   //Belong to setupActions(), but is needed now
   m_sApp = this;

   m_pView = new SFLPhoneView(this);

   ConfigurationSkeleton::self();

   // accept dnd
   setAcceptDrops(true);

   // tell the KXmlGuiWindow that this is indeed the main widget
   m_pCentralDW = new QDockWidget(this);
   m_pCentralDW->setSizePolicy  ( QSizePolicy::Expanding,QSizePolicy::Expanding );
   m_pCentralDW->setWidget      ( m_pView                                       );
   m_pCentralDW->setWindowTitle ( i18nc("Call tab","Call")                                  );
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

   addDockWidget( Qt::BottomDockWidgetArea, m_pCentralDW  );
   addDockWidget( Qt::BottomDockWidgetArea, m_pContactCD  );
   addDockWidget( Qt::BottomDockWidgetArea, m_pHistoryDW  );
   addDockWidget( Qt::BottomDockWidgetArea, m_pBookmarkDW );

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

   selectCallTab();

   setWindowIcon (QIcon(ICON_SFLPHONE) );
   setWindowTitle(i18n("SFLphone")     );
   ActionCollection::instance()->setupAction();
   setAutoSaveSettings();
   createGUI();

   m_pTrayIcon->addAction( ActionCollection::instance()->acceptAction  () );
   m_pTrayIcon->addAction( ActionCollection::instance()->mailBoxAction () );
   m_pTrayIcon->addAction( ActionCollection::instance()->refuseAction  () );
   m_pTrayIcon->addAction( ActionCollection::instance()->holdAction    () );
   m_pTrayIcon->addAction( ActionCollection::instance()->transferAction() );
   m_pTrayIcon->addAction( ActionCollection::instance()->recordAction  () );
   m_pTrayIcon->addSeparator();
   m_pTrayIcon->addAction( ActionCollection::instance()->quitAction    () );

   connect(ActionCollection::instance()->showContactDockAction(), SIGNAL(toggled(bool)),m_pContactCD, SLOT(setVisible(bool)));
   connect(ActionCollection::instance()->showHistoryDockAction(), SIGNAL(toggled(bool)),m_pHistoryDW, SLOT(setVisible(bool)));
   connect(ActionCollection::instance()->showBookmarkDockAction(),SIGNAL(toggled(bool)),m_pBookmarkDW,SLOT(setVisible(bool)));

#ifdef ENABLE_VIDEO
   connect(VideoModel::instance(),SIGNAL(videoCallInitiated(VideoRenderer*)),this,SLOT(displayVideoDock(VideoRenderer*)));
#endif

   statusBar()->addWidget(m_pStatusBarWidget);

   m_pTrayIcon->show();

   setObjectNames();
   QMetaObject::connectSlotsByName(this);
   m_pView->loadWindow();

   move(QCursor::pos().x() - geometry().width()/2, QCursor::pos().y() - geometry().height()/2);

   if (AccountListModel::instance()->size() <= 1 && ConfigurationSkeleton::displayAccountWizard())
      (new AccountWizard())->show();

   m_pInitialized = true ;

   KStatusBar* bar = statusBar();
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
   m_pPresent->setText(i18nc("The presence state is \"Online\"","Online"));
   m_pPresent->setCheckable(true);
   m_pPresent->setVisible(AccountListModel::instance()->isPresenceEnabled() && AccountListModel::instance()->isPresencePublishSupported());
//    m_pPresent->setStyleSheet("background-color:red;");
   bar->addWidget(m_pPresent);
   connect(PresenceStatusModel::instance(),SIGNAL(currentNameChanged(QString)),this,SLOT(updatePresence(QString)));
   connect(PresenceStatusModel::instance(),SIGNAL(currentNameChanged(QString)),this,SLOT(hidePresenceDock()));
   connect(AccountListModel::instance(),SIGNAL(presenceEnabledChanged(bool)),this,SLOT(slotPresenceEnabled(bool)));

   m_pPresenceDock = new QDockWidget(this);
   m_pPresenceDock->setObjectName("presence-dock");
   m_pPresenceDock->setWidget(new Presence(m_pPresenceDock));
   m_pPresenceDock->setAllowedAreas(Qt::TopDockWidgetArea);
   m_pPresenceDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
   addDockWidget( Qt::TopDockWidgetArea, m_pPresenceDock );
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

   if (ConfigurationSkeleton::displayOnStart())
      show();
   else
      close();
} //SFLPhone

///Destructor
SFLPhone::~SFLPhone()
{
   if (!isHidden()) {
      ConfigurationSkeleton::setDisplayContactDock ( m_pContactCD->isVisible()  );
      ConfigurationSkeleton::setDisplayHistoryDock ( m_pHistoryDW->isVisible()  );
      ConfigurationSkeleton::setDisplayBookmarkDock( m_pBookmarkDW->isVisible() );
   }

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

   delete CallModel::instance();
   delete ContactModel::instance();
   //saveState();
}

///Init everything
bool SFLPhone::initialize() //TODO deprecate
{
   return !m_pInitialized;
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
   return app()->m_pView;
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

///Change windowtitle
void SFLPhone::on_m_pView_windowTitleChangeAsked(const QString& message)
{
   setWindowTitle(message);
}

///Enable or disable toolbar items
void SFLPhone::on_m_pView_enabledActionsChangeAsked(const bool* enabledActions)
{
   ActionCollection::instance()->acceptAction()->setVisible   ( enabledActions[SFLPhone::CallAction::Accept   ]);
   ActionCollection::instance()->refuseAction()->setVisible   ( enabledActions[SFLPhone::CallAction::Refuse   ]);
   ActionCollection::instance()->holdAction()->setVisible     ( enabledActions[SFLPhone::CallAction::Hold     ]);
   ActionCollection::instance()->transferAction()->setVisible ( enabledActions[SFLPhone::CallAction::Transfer ]);
   ActionCollection::instance()->recordAction()->setVisible   ( enabledActions[SFLPhone::CallAction::Record   ]);
   ActionCollection::instance()->mailBoxAction()->setVisible  ( enabledActions[SFLPhone::CallAction::Mailbox  ]);
}

///Change icons
void SFLPhone::on_m_pView_actionIconsChangeAsked(const QString* actionIcons)
{
   ActionCollection::instance()->acceptAction()->setIcon   ( QIcon(actionIcons[SFLPhone::CallAction::Accept   ]));
   ActionCollection::instance()->refuseAction()->setIcon   ( QIcon(actionIcons[SFLPhone::CallAction::Refuse   ]));
   ActionCollection::instance()->holdAction()->setIcon     ( QIcon(actionIcons[SFLPhone::CallAction::Hold     ]));
   ActionCollection::instance()->transferAction()->setIcon ( QIcon(actionIcons[SFLPhone::CallAction::Transfer ]));
   ActionCollection::instance()->recordAction()->setIcon   ( QIcon(actionIcons[SFLPhone::CallAction::Record   ]));
   ActionCollection::instance()->mailBoxAction()->setIcon  ( QIcon(actionIcons[SFLPhone::CallAction::Mailbox  ]));
}

///Change text
void SFLPhone::on_m_pView_actionTextsChangeAsked(const QString* actionTexts)
{
   ActionCollection::instance()->acceptAction()->setText   ( actionTexts[SFLPhone::CallAction::Accept   ]);
   ActionCollection::instance()->refuseAction()->setText   ( actionTexts[SFLPhone::CallAction::Refuse   ]);
   ActionCollection::instance()->holdAction()->setText     ( actionTexts[SFLPhone::CallAction::Hold     ]);
   ActionCollection::instance()->transferAction()->setText ( actionTexts[SFLPhone::CallAction::Transfer ]);
   ActionCollection::instance()->recordAction()->setText   ( actionTexts[SFLPhone::CallAction::Record   ]);
   ActionCollection::instance()->mailBoxAction()->setText  ( actionTexts[SFLPhone::CallAction::Mailbox  ]);
}

///Change transfer state
void SFLPhone::on_m_pView_transferCheckStateChangeAsked(bool transferCheckState)
{
   ActionCollection::instance()->transferAction()->setChecked(transferCheckState);
}

///Change record state
void SFLPhone::on_m_pView_recordCheckStateChangeAsked(bool recordCheckState)
{
   ActionCollection::instance()->recordAction()->setChecked(recordCheckState);
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

///Hide or show the statusbar presence widget
void SFLPhone::slotPresenceEnabled(bool state)
{
   m_pPresent->setVisible(state && AccountListModel::instance()->isPresencePublishSupported());
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
      addDockWidget( Qt::TopDockWidgetArea, m_pVideoDW  );
      m_pVideoDW->setFloating(true);
   }
   m_pVideoDW->addRenderer(r);
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

///Select the call tab
void SFLPhone::selectCallTab()
{
   QList<QTabBar*> tabBars = this->findChildren<QTabBar*>();
   if(tabBars.count())
   {
      foreach(QTabBar* bar, tabBars) {
         for (int i=0;i<bar->count();i++) {
            if (bar->tabText(i).replace('&',QString()) == i18n("Call")) {
               bar->setCurrentIndex(i);
               break;
            }
         }
      }
   }
}
