/***************************************************************************
 *   Copyright (C) 2009-2014 by Savoir-Faire Linux                         *
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
#include "sflphoneapplication.h"

//KDE
#include <KCmdLineArgs>
#include <KIconLoader>
#include <KStandardDirs>
#include <KSystemTrayIcon>
#include <KMainWindow>
#include <KDebug>
#include <KIcon>
#include <KMessageBox>

//SFLPhone library
#include "lib/dbus/instancemanager.h"
#include "klib/kcfg_settings.h"
#include "sflphonecmd.h"

//SFLPhone
#include "sflphone.h"
#include "errormessage.h"
#include "lib/sflphone_const.h"
#include "lib/call.h"

//Other
#include <unistd.h>


/**
 * The application constructor
 */
SFLPhoneApplication::SFLPhoneApplication()
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
      QDBusPendingReply<QString> reply = instance.Register(getpid(), "SFLPhone KDE Client");
      reply.waitForFinished();
   }
   catch (...) {
      KMessageBox::error(SFLPhone::app(),ErrorMessage::GENERIC_ERROR);
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
SFLPhoneApplication::~SFLPhoneApplication()
{
   delete SFLPhone::app();
   // automatically destroyed
   disableSessionManagement();
   InstanceInterface& instance = DBus::InstanceManager::instance();
   Q_NOREPLY instance.Unregister(getpid());
   instance.connection().disconnectFromBus(instance.connection().baseService());
}

/**
 * Initialisation of the main window.
 */
void SFLPhoneApplication::initializeMainWindow()
{
  // Enable KDE session restore.
//   int restoredWindow = -1;
  if( kapp->isSessionRestored() ) {
    int n = 0;
    while( KMainWindow::canBeRestored( ++n ) ) {
      if( KMainWindow::classNameOfToplevel( n ) != QLatin1String( "SFLPhone" ) ) {
        continue;
      }
      break;
    }
  }
}

/**
 * Initialize additional paths
 */
void SFLPhoneApplication::initializePaths()
{
  // Add compile time paths as fallback
  KGlobal::dirs()       -> addPrefix( QString(DATA_INSTALL_DIR) );
  KIconLoader::global() -> addAppDir( QString(DATA_INSTALL_DIR) + "/share" );

}

///Parse command line arguments
int SFLPhoneApplication::newInstance()
{
   static bool init = true;
   //Only call on the first instance
   if (init) {
      init = false;
      SFLPhone* sflphoneWindow_ = new SFLPhone();
      if( ! sflphoneWindow_->initialize() ) {
         return 1;
      };

      if (ConfigurationSkeleton::displayOnStart())
         sflphoneWindow_->show();
      else
         sflphoneWindow_->hide();
   }

   KCmdLineArgs::setCwd(QDir::currentPath().toUtf8());
   KCmdLineArgs* args = KCmdLineArgs::parsedArgs();
   if(args->isSet("place-call")) {
      SFLPhoneCmd::placeCall(args->getOption("place-call"));
   }
   if (args->isSet("send-text")) {
      QString smsNumber  = args->getOption("send-text");
      QString smsMessage = args->getOption("message");
      if (!smsMessage.isEmpty() && !smsNumber.isEmpty()) {
         SFLPhoneCmd::sendText(smsNumber,smsMessage);
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
bool SFLPhoneApplication::notify (QObject* receiver, QEvent* e)
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
      QTimer::singleShot(2500,SFLPhone::app(),SLOT(timeout()));
   }
   catch (const Call::Action& state) {
      kDebug() << ErrorMessage::GENERIC_ERROR << "Call Action" << state;
      QTimer::singleShot(2500,SFLPhone::app(),SLOT(timeout()));
   }
   catch (const Call::DaemonState& state) {
      kDebug() << ErrorMessage::GENERIC_ERROR << "Call DaemonState" << state;
      QTimer::singleShot(2500,SFLPhone::app(),SLOT(timeout()));
   }
   catch (const QString& errorMessage) {
      KMessageBox::error(SFLPhone::app(),errorMessage);
      QTimer::singleShot(2500,SFLPhone::app(),SLOT(timeout()));
   }
   catch (...) {
      kDebug() << ErrorMessage::GENERIC_ERROR;
      KMessageBox::error(SFLPhone::app(),ErrorMessage::GENERIC_ERROR);
      QTimer::singleShot(2500,SFLPhone::app(),SLOT(timeout()));
   }
   return false;
}
#include "sflphoneapplication.moc"
