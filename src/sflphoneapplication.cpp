/***************************************************************************
 *   Copyright (C) 2009-2012 by Savoir-Faire Linux                         *
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

//SFLPhone
#include "sflphone.h"


/**
 * The application constructor
 */
SFLPhoneApplication::SFLPhoneApplication()
  : KApplication()
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
  // Fetch the command line arguments
  //KCmdLineArgs* args = KCmdLineArgs::parsedArgs();

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

///Exit gracefully
bool SFLPhoneApplication::notify (QObject* receiver, QEvent* e)
{
   try {
      return KApplication::notify(receiver,e);
   }
   catch (...) {
      kDebug() << "Error caught!!!";
      KMessageBox::error(nullptr,i18n("An unknown error occurred. SFLPhone KDE will now exit. If the problem persist, please report a bug.\n\n"
      "It is known that this message can be caused by trying to open SFLPhone KDE while the SFLPhone daemon is exiting. If so, waiting 15 seconds and "
      "trying again will solve the issue."));
      exit(1);
   }
   return false;
}
#include "sflphoneapplication.moc"

