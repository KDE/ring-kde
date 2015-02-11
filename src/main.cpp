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
#include <QString>

//KDE
#include <QDebug>
#include <kaboutdata.h>
#include <KLocalizedString>
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

      KAboutData about(QStringLiteral("ring-kde"),
         i18n("ring-kde"),
         /*QStringLiteral(*/version/*)*/,
         i18n("KWrite - Text Editor"),
         KAboutLicense::GPL_V3,
         i18n("(C) 2009-2015 Savoir-faire Linux"),
         QString(),
         QStringLiteral("http://www.ring.cx"),
         QStringLiteral("sflphone@lists.savoirfairelinux.net")
      );


      /*KAboutData about(
         "ring-kde"                                         ,
         "ring-kde"                                         ,
         i18n("Ring-KDE")                                  ,
         version                                            ,
         i18n("An enterprise grade KDE SIP and IAX phone") ,
         KAboutLicense::GPL_V3                         ,
         i18n("(C) 2009-2015 Savoir-faire Linux")          ,
         KLocalizedString()                                 ,
         "http://www.ring.cx"                               ,
         "sflphone@lists.savoirfairelinux.net"
      );*/
      about.addAuthor( i18n( "Emmanuel Lepage Vallée" ), QString(), "emmanuel.lepage@savoirfairelinux.com" );
      about.addCredit( i18n( "Jérémy Quentin"         ), QString(), "jeremy.quentin@savoirfairelinux.com"  );

      Cmd::parseCmd(argc,argv,about);

      app = new RingApplication(argc, argv);

      //dbus configuration
      CallModel::instance();

      KDE_signal(SIGINT  , quitOnSignal);
      KDE_signal(SIGTERM , quitOnSignal);

      const int retVal = app->exec();

      /*ConfigurationSkeleton::self()->writeConfig();*/

      delete app;
      return retVal;
   }
   catch(const char * msg)
   {
      qDebug() << msg;
   }
   catch(QString& msg)
   {
      qDebug() << msg;
   }
}
