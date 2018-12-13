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
#include <QtCore/QtPlugin>
#include <QtGui/QImage>

//KDE
#include <KDBusService>
#include <KAboutData>
#include <KLocalizedString>

//Ring
#include "ringapplication.h"
#include "klib/kcfg_settings.h"
#include "cmd.h"
#include <QQmlDebuggingEnabler>

#include <QQmlExtensionPlugin>

#ifdef KQUICKITEMVIEWS_USE_STATIC_PLUGIN
Q_IMPORT_PLUGIN(KQuickItemViews)
#else
#include <KQuickItemViews/plugin.h>
#endif

Q_IMPORT_PLUGIN(RingQtQuick)

constexpr static const char version[] = "3.0.1";

int main(int argc, char **argv)
{
   try
   {
      //QQmlDebuggingEnabler enabler;

      QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

      RingApplication app( argc, argv );

      KLocalizedString::setApplicationDomain("ring-kde");

      //FIXME remove
#ifdef KQUICKITEMVIEWS_USE_STATIC_PLUGIN
      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_KQuickItemViews().instance())->registerTypes("org.kde.playground.kquickitemviews");
#else
      KQuickItemViews v;
      v.registerTypes("org.kde.playground.kquickitemviews");
#endif

      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_RingQtQuick().instance())->registerTypes("net.lvindustries.ringqtquick");

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
      about.setOrganizationDomain(QByteArray("kde.org"));
      about.setProgramLogo(QImage(QStringLiteral(":appicon/icons/64-apps-ring-kde.png")));

      about.addAuthor( i18n( "Emmanuel Lepage-Vallée"          ), QString(), QStringLiteral("elv1313@gmail.com"                    ));
      about.addAuthor( i18n( "Alexandre Lision"                ), QString(), QStringLiteral("alexandre.lision@savoirfairelinux.com"));
      about.addCredit( i18n( "Based on the SFLphone teamworks" ), QString(), QString()                                              );

      if (!Cmd::parseCmd(argc, argv, about))
         return 0;

      KAboutData::setApplicationData(about);


      app.setOrganizationDomain(QStringLiteral("ring.cx"));

      //Only start the application once
#ifdef Q_OS_LINUX
#ifndef DISABLE_KDBUS_SERVICE
      KDBusService service(KDBusService::Unique);
      QObject::connect(&service, &KDBusService::activateActionRequested, Cmd::instance(), &Cmd::slotActivateActionRequested);
      QObject::connect(&service, &KDBusService::activateRequested      , Cmd::instance(), &Cmd::slotActivateRequested      );
      QObject::connect(&service, &KDBusService::openRequested          , Cmd::instance(), &Cmd::slotOpenRequested          );
#endif
#endif

      //The app will have quitted by now if an instance already exist
      app.newInstance();

      const int retVal = app.exec();

      ConfigurationSkeleton::self()->save();

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
