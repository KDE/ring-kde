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

//KDE
#include <KCmdLineArgs>
#include <KIconLoader>
#include <KStandardDirs>
#include <KSystemTrayIcon>
#include <KMainWindow>
#include <KDebug>
#include <KIcon>
#include <KMessageBox>

//Ring library
#include "lib/dbus/instancemanager.h"
#include "klib/kcfg_settings.h"
#include "cmd.h"

//Ring
#include "ring.h"
#include "errormessage.h"
#include "lib/call.h"

//Other
#include <unistd.h>

Ring* RingApplication::m_spApp = nullptr;

/**
 * The application constructor
 */
RingApplication::RingApplication()
#ifdef DISABLE_UNIQUE_APPLICATION
  : KApplication()
#else
  : KUniqueApplication()
#endif
{

#ifdef ENABLE_VIDEO
   //Necessary to draw OpenGL from a separated thread
   setAttribute(Qt::AA_X11InitThreads,true);
#endif

   try {
      InstanceInterface& instance = DBus::InstanceManager::instance();
      QDBusPendingReply<QString> reply = instance.Register(getpid(), "Ring KDE Client");
      reply.waitForFinished();
   }
   catch (...) {
      KMessageBox::error(Ring::app(),ErrorMessage::GENERIC_ERROR);
   }

   // Start remaining initialisation
   initializePaths();
   initializeMainWindow();

#ifdef DISABLE_UNIQUE_APPLICATION
   newInstance();
#endif
}

/**
 * Destructor
 */
RingApplication::~RingApplication()
{
   delete Ring::app();
   // automatically destroyed
   disableSessionManagement();
   InstanceInterface& instance = DBus::InstanceManager::instance();
   Q_NOREPLY instance.Unregister(getpid());
   instance.connection().disconnectFromBus(instance.connection().baseService());
}

/**
 * Initialisation of the main window.
 */
void RingApplication::initializeMainWindow()
{
  // Enable KDE session restore.
//   int restoredWindow = -1;
  if( kapp->isSessionRestored() ) {
    int n = 0;
    while( KMainWindow::canBeRestored( ++n ) ) {
      if( KMainWindow::classNameOfToplevel( n ) != QLatin1String( "Ring" ) ) {
        continue;
      }
      break;
    }
  }
}

/**
 * Initialize additional paths
 */
void RingApplication::initializePaths()
{
  // Add compile time paths as fallback
  KGlobal::dirs()       -> addPrefix( QString(DATA_INSTALL_DIR) );
  KIconLoader::global() -> addAppDir( QString(DATA_INSTALL_DIR) + "/share" );

}

///Parse command line arguments
int RingApplication::newInstance()
{
   static bool init = true;
   //Only call on the first instance
   if (init) {
      init = false;
      m_spApp = new Ring();
      if( ! m_spApp->initialize() ) {
         return 1;
      };

      if (ConfigurationSkeleton::displayOnStart())
         m_spApp->show();
      else
         m_spApp->hide();
   }

   KCmdLineArgs::setCwd(QDir::currentPath().toUtf8());
   KCmdLineArgs* args = KCmdLineArgs::parsedArgs();
   if(args->isSet("place-call")) {
      Cmd::placeCall(args->getOption("place-call"));
   }
   if (args->isSet("send-text")) {
      QString smsNumber  = args->getOption("send-text");
      QString smsMessage = args->getOption("message");
      if (!smsMessage.isEmpty() && !smsNumber.isEmpty()) {
         Cmd::sendText(smsNumber,smsMessage);
      }
      else {
         kDebug() << "Error: both --send-text and --message have to be set";
      }
   }

   args->clear();
#ifndef DISABLE_UNIQUE_APPLICATION
   KUniqueApplication::newInstance();
#endif
   return 0;
}

///Exit gracefully
bool RingApplication::notify (QObject* receiver, QEvent* e)
{
   try {
#ifdef DISABLE_UNIQUE_APPLICATION
      return KApplication::notify(receiver,e);
#else
      return KUniqueApplication::notify(receiver,e);
#endif
   }
   catch (const Call::State& state) {
      kDebug() << ErrorMessage::GENERIC_ERROR << "CallState" << state;
      QTimer::singleShot(2500,Ring::app(),SLOT(timeout()));
   }
   catch (const Call::Action& state) {
      kDebug() << ErrorMessage::GENERIC_ERROR << "Call Action" << state;
      QTimer::singleShot(2500,Ring::app(),SLOT(timeout()));
   }
   catch (const Call::DaemonState& state) {
      kDebug() << ErrorMessage::GENERIC_ERROR << "Call DaemonState" << state;
      QTimer::singleShot(2500,Ring::app(),SLOT(timeout()));
   }
   catch (const QString& errorMessage) {
      KMessageBox::error(Ring::app(),errorMessage);
      QTimer::singleShot(2500,Ring::app(),SLOT(timeout()));
   }
   catch (...) {
      kDebug() << ErrorMessage::GENERIC_ERROR;
      KMessageBox::error(Ring::app(),ErrorMessage::GENERIC_ERROR);
      QTimer::singleShot(2500,Ring::app(),SLOT(timeout()));
   }
   return false;
}
#include "ringapplication.moc"
