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
// #include <kde_file.h>

//Ring
#include "ringapplication.h"
#include "klib/kcfg_settings.h"
#include "callmodel.h"
#include "cmd.h"

static const char version[] = "2.0.1";

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
         i18n("RING, a secured and distributed communication software"),
         KAboutLicense::GPL_V3,
         i18n("(C) 2015 Savoir-faire Linux"),
         QString(),
         QStringLiteral("http://www.ring.cx"),
         QStringLiteral("ring@lists.savoirfairelinux.net")
      );

      about.addAuthor( i18n( "Adrien Béraud"                   ), QString(), "adrien.beraud@savoirfairelinux.com"        );
      about.addAuthor( i18n( "Alexandre Lision"                ), QString(), "alexandre.lision@savoirfairelinux.com"     );
      about.addAuthor( i18n( "Édric Ladent-Milaret"            ), QString(), "edric.ladent-milaret@savoirfairelinux.com" );
      about.addAuthor( i18n( "Éloi Bail"                       ), QString(), "eloi.bail@savoirfairelinux.com"            );
      about.addAuthor( i18n( "Emmanuel Lepage-Vallée"          ), QString(), "emmanuel.lepage@savoirfairelinux.com"      );
      about.addAuthor( i18n( "Guillaume Roguez"                ), QString(), "guillaume.roguez@savoirfairelinux.com"     );
      about.addAuthor( i18n( "Marianne Forget"                 ), QString(), "marianne.forget@savoirfairelinux.com"      );
      about.addAuthor( i18n( "Stepan Salenikovich"             ), QString(), "stepan.salenikovich@savoirfairelinux.com"  );
      about.addCredit( i18n( "Based on the SFLphone teamworks" ), QString(), QString()                                   );

      Cmd::parseCmd(argc,argv,about);

      app = new RingApplication(argc, argv);

      //dbus configuration
      CallModel::instance();

//       KDE_signal(SIGINT  , quitOnSignal);
//       KDE_signal(SIGTERM , quitOnSignal);

      const int retVal = app->exec();

      ConfigurationSkeleton::self()->save();

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
