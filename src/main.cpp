/***************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                         *
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

//System
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcpp"
#include <unistd.h>
#pragma GCC diagnostic pop
#include <signal.h>

//Qt
#include <QtCore/QString>

//KDE
#include <KDebug>
#include <kaboutdata.h>
#include <klocale.h>
#include <kde_file.h>

//Ring
#include "ringapplication.h"
#include "klib/kcfg_settings.h"
#include "callmodel.h"
#include "cmd.h"

static const char version[] = "1.4.1";

RingApplication* app;
void quitOnSignal(int signal);
void quitOnSignal(int signal)
{
   Q_UNUSED(signal);
   app->quit();
}

int main(int argc, char **argv)
{
   try
   {
      KLocale::setMainCatalog("ring-kde");

      KAboutData about(
         "ring-kde"                                         ,
         "ring-kde"                                         ,
         ki18n("Ring-KDE")                                  ,
         version                                            ,
         ki18n("An enterprise grade KDE SIP and IAX phone") ,
         KAboutData::License_GPL_V3                         ,
         ki18n("(C) 2009-2015 Savoir-faire Linux")          ,
         KLocalizedString()                                 ,
         "http://www.ring.cx"                               ,
         "sflphone@lists.savoirfairelinux.net"
      );
      about.addAuthor( ki18n( "Emmanuel Lepage Vallée" ), KLocalizedString(), "emmanuel.lepage@savoirfairelinux.com" );
      about.addCredit( ki18n( "Jérémy Quentin"         ), KLocalizedString(), "jeremy.quentin@savoirfairelinux.com"  );

      Cmd::parseCmd(argc,argv,about);

      app = new RingApplication();

      //dbus configuration
      CallModel::instance();

      KDE_signal(SIGINT  , quitOnSignal);
      KDE_signal(SIGTERM , quitOnSignal);

      int retVal = app->exec();

      ConfigurationSkeleton::self()->writeConfig();

      delete app;
      return retVal;
   }
   catch(const char * msg)
   {
      kDebug() << msg;
   }
   catch(QString& msg)
   {
      kDebug() << msg;
   }
}
