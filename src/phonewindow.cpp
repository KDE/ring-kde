/************************************** ************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                         *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>         *
 *            Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
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
#include "phonewindow.h"

//System
#include <unistd.h>

//Qt
#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtWidgets/QLabel>
#include <QtCore/QTimer>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QComboBox>
#include <QtGui/QIcon>
#include <QtCore/QStandardPaths>
#include <QQmlApplicationEngine>
#include <QQmlContext>

//KDE
#include <KNotification>
#include <KToolBar>
#include <kmessagebox.h>

#include <klocalizedstring.h>

// LRC
#include <presencestatusmodel.h>
#include <accountmodel.h>
#include <personmodel.h>
#include <availableaccountmodel.h>

//Ring
#include "media/video.h"
#include "klib/kcfg_settings.h"
#include "implementation.h"
#include "icons/icons.h"
#include "view.h"
#include "dock.h"
#include "conf/account/widgets/autocombobox.h"
#include "actioncollection.h"
#include "widgets/systray.h"
#include "widgets/presence.h"
#include "errormessage.h"
#include <video/renderer.h>
#include <video/previewmanager.h>
#include "ringapplication.h"
#include "widgets/dockbase.h"
#include "wizard/welcome.h"
#include <delegates/accountinfodelegate.h>

#ifdef HAVE_SPEECH
 #include "accessibility.h"
#endif

#ifdef ENABLE_AKONADI
 #include "klib/akonadibackend.h"
#endif

#ifdef ENABLE_VIDEO
 #include "widgets/videodock.h"
#endif

///Constructor
PhoneWindow::PhoneWindow(QWidget*)
   : FancyMainWindow()
#ifdef ENABLE_VIDEO
      ,m_pVideoDW(nullptr)
#endif
{
   setObjectName("PhoneWindow");

   //On OSX, QStandardPaths doesn't work as expected, it is better to pack the .ui in the DMG
#ifdef Q_OS_MAC
   QDir dir(QApplication::applicationDirPath());
   dir.cdUp();
   dir.cd("Resources/kxmlgui5/ring-kde/");
   setXMLFile(dir.path()+"/ring-kdeui.rc");
   setUnifiedTitleAndToolBarOnMac(true);
#endif

   setWindowIcon (QIcon::fromTheme(QStringLiteral("ring-kde")));
   setWindowTitle( i18n("Ring"                                  ) );

   m_pView = new View(this);
   ActionCollection::instance()->setupAction(this);
   ActionCollection::instance()->setupPhoneAction(this);
   m_pView->updateVolumeControls();

   ConfigurationSkeleton::self();

   // accept dnd
   setAcceptDrops(true);

   // tell the KXmlGuiWindow that this is indeed the main widget
   m_pCentralDW = new QDockWidget(this);
   m_pCentralDW->setSizePolicy  ( QSizePolicy::Expanding,QSizePolicy::Expanding );
   m_pCentralDW->setWidget      ( m_pView                                       );
   m_pCentralDW->setWindowTitle ( i18nc("Call tab","Call")                      );
   m_pCentralDW->setFeatures    ( QDockWidget::NoDockWidgetFeatures             );
   m_pView->setSizePolicy       ( QSizePolicy::Expanding,QSizePolicy::Expanding );
   m_pCentralDW->setStyleSheet  ( QStringLiteral("\
      QDockWidget::title {\
         margin:0px;\
         padding:0px;\
         spacing:0px;\
         max-height:0px;\
      }\
      \
   "));

   m_pCentralDW->setTitleBarWidget(new QWidget());
   m_pCentralDW->setContentsMargins(0,0,0,0);
   m_pView->setContentsMargins     (0,0,0,0);
   m_pCentralDW->setObjectName( QStringLiteral("callDock") );
   m_pCentralDW->show();

   addDockWidget( Qt::BottomDockWidgetArea, m_pCentralDW  );

   m_pStatusBarWidget = new QLabel  ( this                     );
   m_pTrayIcon        = new SysTray ( this->windowIcon(), this );
   m_pDock            = new Dock    ( this                     );

   connect(m_pCentralDW, &QDockWidget::visibilityChanged, this, &PhoneWindow::updateTabIcons);

   setAutoSaveSettings();

   tabifyDockWidget(m_pCentralDW, m_pDock->contactDock ());
   tabifyDockWidget(m_pCentralDW, m_pDock->historyDock ());
   tabifyDockWidget(m_pCentralDW, m_pDock->bookmarkDock());

   updateTabIcons();
   createGUI();
   selectCallTab();


   if (m_pTrayIcon) {
      m_pTrayIcon->addAction( ActionCollection::instance()->acceptAction  () );
      m_pTrayIcon->addAction( ActionCollection::instance()->mailBoxAction () );
      m_pTrayIcon->addAction( ActionCollection::instance()->holdAction    () );
      m_pTrayIcon->addAction( ActionCollection::instance()->transferAction() );
      m_pTrayIcon->addAction( ActionCollection::instance()->recordAction  () );
   }

   connect(CallModel::instance().selectionModel(),&QItemSelectionModel::currentRowChanged,this,&PhoneWindow::selectCallTab);

#ifdef ENABLE_VIDEO
   connect(&CallModel::instance(),&CallModel::rendererAdded,this,&PhoneWindow::displayVideoDock);
   connect(&CallModel::instance(),&CallModel::rendererRemoved,this,&PhoneWindow::hideVideoDock);
#endif

   statusBar()->addWidget(m_pStatusBarWidget);

   if (m_pTrayIcon) {
      m_pTrayIcon->setObjectName( QStringLiteral("m_pTrayIcon")   );
   }

   m_pView->setObjectName       ( QStringLiteral("m_pView")       );
   statusBar()->setObjectName   ( QStringLiteral("statusBar")     );

   //Add the toolbar Ring icon
   QList<KToolBar*> toolBars = this->findChildren<KToolBar*>();
   if (toolBars.size()) {
      KToolBar* tb = toolBars[0];
      QWidget* spacer = new QWidget(tb);
      spacer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
      tb->addWidget(spacer);
      QToolButton* btn = new QToolButton(tb);
      btn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);
      btn->setStyleSheet(QStringLiteral("padding:0px;spacing:0px;margin:0px;"));
      btn->setIcon(QIcon(":/appicon/icons/sc-apps-ring-kde.svgz"));
      tb->addWidget(btn);
   }

   //Statusbar
   QStatusBar* bar = statusBar();
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
   m_pPresent->setVisible(false/*AccountModel::instance().isPresenceEnabled() && AccountModel::instance().isPresencePublishSupported()*/);
//    m_pPresent->setStyleSheet("background-color:red;");
   bar->addWidget(m_pPresent);
   connect(&PresenceStatusModel::instance(),&PresenceStatusModel::currentNameChanged,this,&PhoneWindow::updatePresence);
   connect(&PresenceStatusModel::instance(),&PresenceStatusModel::currentNameChanged,this,&PhoneWindow::hidePresenceDock);
   connect(&AccountModel::instance(),&AccountModel::presenceEnabledChanged,this,&PhoneWindow::slotPresenceEnabled);

   //Add the Ring hash
   bar->addWidget(new QLabel(i18n("Your Ring ID:"),bar));
   QLabel* ringId = new QLabel(bar);
   QFont f = ringId->font();
   f.setStyleHint(QFont::Monospace);
   ringId->setFont(f);
   ringId->setTextInteractionFlags(Qt::TextSelectableByMouse|Qt::TextSelectableByKeyboard);
   Account* a = AvailableAccountModel::instance().currentDefaultAccount(URI::SchemeType::RING);
   ringId->setText(a ? a->username() : i18n("None"));
   bar->addWidget(ringId);

   m_pPresenceDock = new QDockWidget(this);
   m_pPresenceDock->setObjectName(QStringLiteral("presence-dock"));
   m_pPresenceDock->setWidget(new Presence(m_pPresenceDock));
   m_pPresenceDock->setAllowedAreas(Qt::TopDockWidgetArea);
   m_pPresenceDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
   addDockWidget( Qt::TopDockWidgetArea, m_pPresenceDock );
   m_pPresenceDock->setVisible(false);
   if (m_pPresenceDock->titleBarWidget())
      m_pPresenceDock->titleBarWidget()->setVisible(false);

   m_pCurAccL = new QLabel(i18n("Account:"));
   bar->addPermanentWidget(m_pCurAccL);

   auto pollProxy = new AccountInfoProxy(&AvailableAccountModel::instance());
   m_pAccountStatus = new AutoComboBox(bar);
   m_pAccountStatus->setVisible(ConfigurationSkeleton::displayAccountBox());
   m_pCurAccL->setVisible(ConfigurationSkeleton::displayAccountBox());
   m_pAccountStatus->bindToModel(pollProxy, AvailableAccountModel::instance().selectionModel());
   m_pAccountStatus->setMinimumSize(100, 0);

   auto delegate = new AccountInfoDelegate();
   m_pAccountStatus->setItemDelegate(delegate);

   bar->addPermanentWidget(m_pAccountStatus);
   connect(m_pPresent,&QAbstractButton::toggled,m_pPresenceDock,&QWidget::setVisible);

   QToolButton* m_pReloadButton = new QToolButton(this);
   m_pReloadButton->setIcon(QIcon::fromTheme(QStringLiteral("view-refresh")));
   bar->addPermanentWidget(m_pReloadButton);
   connect(m_pReloadButton,&QAbstractButton::clicked,&AccountModel::instance(),&AccountModel::registerAllAccounts);

   if (ConfigurationSkeleton::displayOnStart()
      && !RingApplication::instance()->startIconified())
      show();
   else
      close();

   //setupGui + default size doesn't really, use this for now
   resize(QSize(1024,768));

   if (!ConfigurationSkeleton::autoStartOverride())
      setAutoStart(true);
} //Ring

///Destructor
PhoneWindow::~PhoneWindow()
{
   removeEventFilter(this);
   disconnect();

   disconnect(m_pCentralDW, &QDockWidget::visibilityChanged, this, &PhoneWindow::updateTabIcons);

   if(m_pVideoDW)
      disconnect(m_pVideoDW ,&QDockWidget::visibilityChanged, this, &PhoneWindow::updateTabIcons);

   m_pDock->deleteLater();

   delete m_pView            ;
   delete m_pStatusBarWidget ;
   delete m_pCentralDW       ;
   delete m_pPresent         ;
   delete m_pPresenceDock    ;

   if (m_pTrayIcon)
      delete m_pTrayIcon     ;

   //saveState();
}

/*****************************************************************************
 *                                                                           *
 *                                  Getters                                  *
 *                                                                           *
 ****************************************************************************/

///Singleton
PhoneWindow* PhoneWindow::app()
{
   return RingApplication::instance()->phoneWindow();
}

///Get the view (to be used with the singleton)
View* PhoneWindow::view()
{
   return m_pView;
}

/*****************************************************************************
 *                                                                           *
 *                                  Mutator                                  *
 *                                                                           *
 ****************************************************************************/

///[Action]Hide Ring
bool PhoneWindow::queryClose()
{
   if (!isHidden()) {
      ConfigurationSkeleton::setDisplayContactDock ( m_pDock->contactDock()->isVisible()  );
      ConfigurationSkeleton::setDisplayHistoryDock ( m_pDock->historyDock()->isVisible()  );
      ConfigurationSkeleton::setDisplayBookmarkDock( m_pDock->bookmarkDock()->isVisible() );
   }
   hide();
   return false;
}

///Be sure the default size is look like a phone
QSize PhoneWindow::sizeHint() const
{
   return QSize(340,700);
}

///[Action] Quit action
void PhoneWindow::quitButton()
{
   qApp->quit();
}

void PhoneWindow::displayAccountCbb( bool checked )
{
   m_pAccountStatus->setVisible(checked);
   m_pCurAccL->setVisible(checked);
   ConfigurationSkeleton::setDisplayAccountBox(checked);
}

///Hide or show the statusbar presence widget
void PhoneWindow::slotPresenceEnabled(bool state)
{
   m_pPresent->setVisible(state && AccountModel::instance().isPresencePublishSupported());
}

///Update presence label
void PhoneWindow::updatePresence(const QString& status)
{
   m_pPresent->setText(status);
   m_pPresent->setToolTip(PresenceStatusModel::instance().currentMessage());
}

///Hide the presence dock when not required
void PhoneWindow::hidePresenceDock()
{
   m_pPresent->setChecked(false);
}

#ifdef ENABLE_VIDEO
///Display the video dock
void PhoneWindow::displayVideoDock(Call* c, Video::Renderer* r)
{
   Q_UNUSED(c)

   if (!m_pVideoDW) {
      m_pVideoDW = new VideoDock();
      connect(m_pVideoDW ,&QDockWidget::visibilityChanged, this, &PhoneWindow::updateTabIcons);
   }

   if (auto vid = c->firstMedia<Media::Video>(Media::Media::Direction::OUT))
      m_pVideoDW->setSourceModel(vid->sourceModel());

   m_pVideoDW->setCall(c);
   m_pVideoDW->addRenderer(r);
   m_pVideoDW->show();

   // Fix mute/hold/playfile
   connect(r, &Video::Renderer::started, [this, c, r]() {
      displayVideoDock(c, r);
   });
}

void PhoneWindow::hideVideoDock(Call* c, Video::Renderer* r)
{
   Q_UNUSED(c)
   Q_UNUSED(r)

   // Don't hide because the preview stopped
   if (r == Video::PreviewManager::instance().previewRenderer())
      return;

   if (m_pVideoDW) {
      m_pVideoDW->hide();
      m_pVideoDW->setCall(nullptr);
   }
}
#endif

///Select the call tab
void PhoneWindow::selectCallTab()
{
   QList<QTabBar*> tabBars = this->findChildren<QTabBar*>();
   if(tabBars.count()) {
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

bool PhoneWindow::isAutoStart() const
{
   const bool enabled = ConfigurationSkeleton::autoStart();

   const QString localPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)+"/autostart/";

   const QFile f(localPath);

   if (enabled && f.exists()) {
      ConfigurationSkeleton::setAutoStart(true);
   }

   return ConfigurationSkeleton::autoStart();
}

void PhoneWindow::setAutoStart(bool value)
{
   Q_UNUSED(value)

   if (value) {
      const QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("ring-kde/cx.ring.ring-kde.desktop"));
      QFile f(path);

      if (f.exists()) {
         if (f.copy(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)+"/autostart/cx.ring.ring-kde.desktop"))
            ConfigurationSkeleton::setAutoStart(true);
      }
      else {
         qWarning() << "Cannot enable autostart, file not found";
      }

   }
   else {
      QFile f(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)+"/autostart/cx.ring.ring-kde.desktop");
      f.remove();
      ConfigurationSkeleton::setAutoStart(false);
   }
}

QDockWidget* PhoneWindow::callDock() const
{
   return m_pCentralDW;
}

// kate: space-indent on; indent-width 3; replace-tabs on;
