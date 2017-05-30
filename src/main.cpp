/***************************************************************************
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

//Qt
#include <QtCore/QString>
#include <QtGui/QImage>

//KDE
#include <KDBusService>
#include <KAboutData>
#include <KLocalizedString>

//Ring
#include "ringapplication.h"
#include "klib/kcfg_settings.h"
#include "cmd.h"

constexpr static const char version[] = "2.4.0";

int main(int argc, char **argv)
{
   try
   {
      static volatile auto app = RingApplication::instance( argc, argv );
      qDebug() << "MAIN" << RingApplication::instance()->arguments();

      KLocalizedString::setApplicationDomain("ring-kde");

      KAboutData about(QStringLiteral("ring-kde"),
         i18n("ring-kde"),
         /*QStringLiteral(*/version/*)*/,
         i18n("RING, a secured and distributed communication software"),
         KAboutLicense::GPL_V3,
         i18n("(C) 2004-2015 Savoir-faire Linux\n2016-2017 Emmanuel Lepage Vallee"),
         QString(),
         QStringLiteral("http://www.ring.cx"),
         QStringLiteral("ring@gnu.org")
      );
      about.setProgramLogo(QImage(QStringLiteral(":appicon/icons/64-apps-ring-kde.png")));

      about.addAuthor( i18n( "Emmanuel Lepage-Vallée"          ), QString(), QStringLiteral("elv1313@gmail.com"                    ));
      about.addAuthor( i18n( "Alexandre Lision"                ), QString(), QStringLiteral("alexandre.lision@savoirfairelinux.com"));
      about.addCredit( i18n( "Based on the SFLphone teamworks" ), QString(), QString()                                              );

      KAboutData::setApplicationData(about);

      Cmd::parseCmd(argc, argv, about);

      RingApplication::instance()->setOrganizationDomain( QStringLiteral("ring.cx")           );

      //Only start the application once
#ifdef Q_OS_LINUX
      KDBusService service(KDBusService::Unique);
      QObject::connect(&service, &KDBusService::activateActionRequested, Cmd::instance(), &Cmd::slotActivateActionRequested);
      QObject::connect(&service, &KDBusService::activateRequested      , Cmd::instance(), &Cmd::slotActivateRequested      );
      QObject::connect(&service, &KDBusService::openRequested          , Cmd::instance(), &Cmd::slotOpenRequested          );
#endif

      //The app will have quitted by now if an instance already exist
      RingApplication::instance()->newInstance();

      const int retVal = RingApplication::instance()->exec();

      ConfigurationSkeleton::self()->save();

      delete RingApplication::instance();
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

// kate: space-indent on; indent-width 3; replace-tabs on;
