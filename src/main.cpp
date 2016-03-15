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
#include <QtGui/QImage>

//KDE
#include <KDBusService>
#include <kaboutdata.h>
#include <KLocalizedString>

//Ring
#include "ringapplication.h"
#include "klib/kcfg_settings.h"
#include "callmodel.h"
#include "cmd.h"

static const char version[] = "2.2.0";

static RingApplication* app;
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
      about.setProgramLogo(QImage(QStringLiteral(":appicon/icons/64-apps-ring-kde.png")));

      about.addAuthor( i18n( "Adrien Béraud"                   ), QString(), QStringLiteral("adrien.beraud@savoirfairelinux.com")        );
      about.addAuthor( i18n( "Alexandre Lision"                ), QString(), QStringLiteral("alexandre.lision@savoirfairelinux.com")     );
      about.addAuthor( i18n( "Édric Ladent-Milaret"            ), QString(), QStringLiteral("edric.ladent-milaret@savoirfairelinux.com") );
      about.addAuthor( i18n( "Éloi Bail"                       ), QString(), QStringLiteral("eloi.bail@savoirfairelinux.com")            );
      about.addAuthor( i18n( "Emmanuel Lepage-Vallée"          ), QString(), QStringLiteral("emmanuel.lepage@savoirfairelinux.com")      );
      about.addAuthor( i18n( "Guillaume Roguez"                ), QString(), QStringLiteral("guillaume.roguez@savoirfairelinux.com")     );
      about.addAuthor( i18n( "Marianne Forget"                 ), QString(), QStringLiteral("marianne.forget@savoirfairelinux.com")      );
      about.addAuthor( i18n( "Stepan Salenikovich"             ), QString(), QStringLiteral("stepan.salenikovich@savoirfairelinux.com")  );
      about.addCredit( i18n( "Based on the SFLphone teamworks" ), QString(), QString()                                   );

      KAboutData::setApplicationData(about);

      app = new RingApplication ( argc, argv          );
      Cmd::parseCmd(argc, argv, &about);

      app->setApplicationName   ( about.productName() );
      app->setApplicationVersion( about.version    () );
      app->setOrganizationDomain( QStringLiteral("ring.cx")           );

      //Only start the application once
#ifdef Q_OS_LINUX
      KDBusService service(KDBusService::Unique);
      QObject::connect(&service, &KDBusService::activateActionRequested, Cmd::instance(), &Cmd::slotActivateActionRequested);
      QObject::connect(&service, &KDBusService::activateRequested      , Cmd::instance(), &Cmd::slotActivateRequested      );
      QObject::connect(&service, &KDBusService::openRequested          , Cmd::instance(), &Cmd::slotOpenRequested          );
#endif

      //The app will have quitted by now if an instance already exist
      app->newInstance();

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
