/***************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Valle <emmanuel.lepage@savoirfairelinux.com >*
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
#include "ringapplication.h"

//Qt
#include <QtCore/QDir>
#include <QtCore/QTimer>
#include <QtCore/QCommandLineParser>
#include <QtCore/QDebug>
#include <QtCore/QStandardPaths>
#include <QQmlApplicationEngine>
#include <QQmlContext>

//KDE
#include <KIconLoader>
#include <KMainWindow>
#include <kmessagebox.h>
#include <KDeclarative/KDeclarative>

//LRC
#include <callmodel.h>
#include <accountmodel.h>
#include <account.h>
#include <categorizedhistorymodel.h>
#include <categorizedcontactmodel.h>
#include <namedirectory.h>
#include <categorizedbookmarkmodel.h>
#include <useractionmodel.h>
#include <contactmethod.h>
#include <recentmodel.h>
#include <media/recordingmodel.h>
#include <peerstimelinemodel.h>
#include <peertimelinemodel.h>
#include <video/previewmanager.h>
#include <media/recording.h>
#include <media/textrecording.h>
#include <media/media.h>

//Ring
#include "klib/kcfg_settings.h"
#include "cmd.h"
#include "mainwindow.h"
#include "errormessage.h"
#include "callmodel.h"
#include "implementation.h"
#include "wizard/welcome.h"
#include "video/videowidget.h"

//QML
#include "qmlwidgets/plugin.h"

//Other
#include <unistd.h>

/**
 * The application constructor
 */
RingApplication::RingApplication(int & argc, char ** argv) : QApplication(argc,argv),m_StartIconified(false)
{
   Q_ASSERT(argc != -1);

   qDebug() << argc << argv << this << argv[0];
#ifdef ENABLE_VIDEO
   //Necessary to draw OpenGL from a separated thread
   setAttribute(Qt::AA_X11InitThreads,true);
#endif
   setAttribute(Qt::AA_EnableHighDpiScaling);

   if ((!CallModel::instance().isConnected()) || (!CallModel::instance().isValid())) {
      QTimer::singleShot(5000,this,&RingApplication::daemonTimeout);
   }
}

/**
 * Destructor
 */
RingApplication::~RingApplication()
{
   delete MainWindow::app();
}

RingApplication* RingApplication::instance(int& argc, char** argv)
{
   static RingApplication* i = new RingApplication(argc, argv);

   return i;
}

RingApplication* RingApplication::instance()
{
   int i = 0;
   return RingApplication::instance(i, nullptr);
}

///Parse command line arguments
int RingApplication::newInstance()
{
   qDebug() << "\n\nNEW INSTANCE!!";
   static bool wizardshown = false;

   // The first run wizard
   if ((!wizardshown) && (ConfigurationSkeleton::enableWizard() || ConfigurationSkeleton::showSplash())) {
      // Also add this object
      engine()->rootContext()->setContextProperty(
         "RingApplication", this
      );

      engine()->rootContext()->setContextProperty(
         "wizardWelcomeOnly", QVariant(!ConfigurationSkeleton::enableWizard())
      );

      auto wiz = new WelcomeDialog();
      wiz->show();
      ConfigurationSkeleton::setEnableWizard(false);
      wizardshown = true;
      return 0;
   }

   static bool init = true;
   //Only call on the first instance
   if (init) {
      init = false;
      auto mw = RingApplication::phoneWindow();
      if( ! mw->initialize() ) {
         return 1;
      };

      qDebug() << "\n\nHERE" << ConfigurationSkeleton::displayOnStart();
      if (ConfigurationSkeleton::displayOnStart())
         mw->show();
      else
         mw->hide();
   }

   return 0;
}

bool RingApplication::startIconified() const
{
   return m_StartIconified;
}

void RingApplication::setIconify(bool iconify)
{
   m_StartIconified = iconify;
}

void RingApplication::setStartTimeline(bool value)
{
   m_StartTimeLine = value;
}

void RingApplication::setStartPhone(bool value)
{
   m_StartPhone = value;
}

#define QML_TYPE(name) qmlRegisterUncreatableType<name>(AppName, 1,0, #name, #name "cannot be instanciated");
#define QML_SINGLETON(name) RingApplication::engine()->rootContext()->setContextProperty(#name, &name::instance());

constexpr static const char AppName[]= "Ring";


/// Create a QML engine for various canvas widgets
QQmlApplicationEngine* RingApplication::engine()
{
   static QQmlApplicationEngine* e = nullptr;
   if (!e) {
      auto p = new RingQmlWidgets;
      p->registerTypes("RingQmlWidgets");

      QML_TYPE( Account           )
      QML_TYPE( const Account     )
      QML_TYPE( Call              )
      QML_TYPE( Person            )
      QML_TYPE( ContactMethod     )
      QML_TYPE( UserActionModel   )
      QML_TYPE( PeerTimelineModel )

      e = new QQmlApplicationEngine(QGuiApplication::instance());

      // Setup the icon theme provider and ki18n
      auto decl = new KDeclarative::KDeclarative;
      decl->setDeclarativeEngine(e);
      decl->setupBindings();

      QML_SINGLETON( CallModel                );
      QML_SINGLETON( CategorizedHistoryModel  );
      QML_SINGLETON( AccountModel             );
      QML_SINGLETON( CategorizedContactModel  );
      QML_SINGLETON( CategorizedBookmarkModel );
      QML_SINGLETON( NameDirectory            );
      QML_SINGLETON( RecentModel              );
      QML_SINGLETON( PeersTimelineModel       );

      { using namespace Media;
         QML_SINGLETON( RecordingModel        );
         QML_TYPE     ( Recording             );
         QML_TYPE     ( TextRecording         );

      }

      { using namespace Video;
         QML_SINGLETON( PreviewManager        );
      }

      qmlRegisterUncreatableType<::Media::Media>(
         AppName, 1,0, "Media", "cannot be instanciated"
      );

      VideoWidget3::initProvider();
   }
   return e;
}

#undef QML_TYPE
#undef QML_SINGLETON

MainWindow* RingApplication::phoneWindow() const
{
   if (!m_pPhone)
      m_pPhone = new MainWindow(nullptr);

   return m_pPhone;
}

MainWindow* RingApplication::timelineWindow() const
{
   if (!m_pTimeline)
      m_pTimeline = new MainWindow(nullptr);

   return m_pTimeline;
}

MainWindow* RingApplication::mainWindow() const
{
   if (m_pPhone && !m_pTimeline)
      return m_pPhone;

   return RingApplication::timelineWindow();
}

///The daemon is not found
void RingApplication::daemonTimeout()
{
   if ((!CallModel::instance().isConnected()) || (!CallModel::instance().isValid())) {
      KMessageBox::error(phoneWindow(), ErrorMessage::NO_DAEMON_ERROR);
      exit(1);
   }
}

///Exit gracefully
bool RingApplication::notify (QObject* receiver, QEvent* e)
{
   try {
      return QApplication::notify(receiver,e);
   }
   catch (const Call::State& state) {
      qDebug() << ErrorMessage::GENERIC_ERROR << "CallState" << state;
      QTimer::singleShot(2500, this, &RingApplication::daemonTimeout);
   }
   catch (const Call::Action& state) {
      qDebug() << ErrorMessage::GENERIC_ERROR << "Call Action" << state;
      QTimer::singleShot(2500, this, &RingApplication::daemonTimeout);
   }
   catch (const QString& errorMessage) {
      KMessageBox::error(MainWindow::app(),errorMessage);
      QTimer::singleShot(2500, this, &RingApplication::daemonTimeout);
   }
   catch (...) {
      qDebug() << ErrorMessage::GENERIC_ERROR;
      KMessageBox::error(MainWindow::app(),ErrorMessage::GENERIC_ERROR);
      QTimer::singleShot(2500, this, &RingApplication::daemonTimeout);
   }
   return false;
}

// kate: space-indent on; indent-width 3; replace-tabs on;
