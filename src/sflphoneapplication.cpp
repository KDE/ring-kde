/***************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                         *
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
#include <KNotification>
#include <KSystemTrayIcon>
#include <KMainWindow>
#include <KDebug>
#include <KMessageBox>

//SFLPhone library
#include "lib/instance_interface_singleton.h"
#include "klib/configurationskeleton.h"

//SFLPhone
#include "sflphone.h"
#include "errormessage.h"


/**
 * The application constructor
 */
SFLPhoneApplication::SFLPhoneApplication()
  : KUniqueApplication()
{
   InstanceInterface& instance = InstanceInterfaceSingleton::getInstance();
   instance.Register(getpid(), APP_NAME);
   // Start remaining initialisation
   initializePaths();
   initializeMainWindow();
}



/**
 * Destructor
 */
SFLPhoneApplication::~SFLPhoneApplication()
{
   delete SFLPhone::app();
   // automatically destroyed
   disableSessionManagement();
   InstanceInterface& instance = InstanceInterfaceSingleton::getInstance();
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
      SFLPhone* sflphoneWindow_ = new SFLPhone();
      if( ! sflphoneWindow_->initialize() ) {
         return 1;
      };

      if (ConfigurationSkeleton::displayOnStart())
         sflphoneWindow_->show();
      else
         sflphoneWindow_->hide();
      init = false;
   }

   KCmdLineArgs::setCwd(QDir::currentPath().toUtf8());
   KCmdLineArgs* args = KCmdLineArgs::parsedArgs();
   if(args->isSet("place-call")) {
      Call* call = SFLPhone::model()->addDialingCall();
      call->appendText(args->getOption("place-call"));
      call->actionPerformed(CALL_ACTION_ACCEPT);
   }
   args->clear();
   KUniqueApplication::newInstance();
   return 0;
}

///Exit gracefully
bool SFLPhoneApplication::notify (QObject* receiver, QEvent* e)
{
   try {
      return KApplication::notify(receiver,e);
   }
   catch (...) {
      kDebug() << ErrorMessage::GENERIC_ERROR;
      KMessageBox::error(SFLPhone::app(),ErrorMessage::GENERIC_ERROR);
      QTimer::singleShot(2500,SFLPhone::app(),SLOT(timeout()));
   }
   return false;
}
#include "sflphoneapplication.moc"

