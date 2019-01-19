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
#include <QQmlApplicationEngine>

//KDE
#include <KDBusService>
#include <KAboutData>
#include <KLocalizedString>

//Ring
#include "ringapplication.h"
#include "kcfg_settings.h"
#include "cmd.h"
#include <QQmlDebuggingEnabler>

#include <QQmlExtensionPlugin>

#ifdef KQUICKITEMVIEWS_USE_STATIC_PLUGIN
Q_IMPORT_PLUGIN(KQuickItemViews)
#else
#include <KQuickItemViews/plugin.h>
#endif

#ifdef JAMIKDEINTEGRATION_USE_STATIC_PLUGIN
Q_IMPORT_PLUGIN(JamiKDEIntegration)
#else
#include <jamikdeintegration/src/plugin.h>
#endif

#ifdef JAMIWIZARD_USE_STATIC_PLUGIN
Q_IMPORT_PLUGIN(JamiWizard)
#else
#include <wizard/plugin.h>
#endif

#ifdef JAMIACCOUNTVIEW_USE_STATIC_PLUGIN
Q_IMPORT_PLUGIN(JamiAccountView)
#else
#include <accountview/accountviewplugin.h>
#endif

#ifdef JAMIACCOUNTVIEW_USE_STATIC_PLUGIN
Q_IMPORT_PLUGIN(JamiCallView)
#else
#include <callview/callviewplugin.h>
#endif

#ifdef JAMIACCOUNTVIEW_USE_STATIC_PLUGIN
Q_IMPORT_PLUGIN(JamiContactView)
#else
#include <contactview/contactviewplugin.h>
#endif

#ifdef JAMIACCOUNTVIEW_USE_STATIC_PLUGIN
Q_IMPORT_PLUGIN(JamiDialView)
#else
#include <dialview/dialviewplugin.h>
#endif

#ifdef JAMIACCOUNTVIEW_USE_STATIC_PLUGIN
Q_IMPORT_PLUGIN(JamiTimelineView)
#else
#include <timeline/timelineplugin.h>
#endif

#ifdef JAMICANVASINDICATOR_USE_STATIC_PLUGIN
Q_IMPORT_PLUGIN(JamiCanvasIndicator)
#else
#include <canvasindicators/canvasindicator.h>
#endif

#ifdef JAMIPHOTOSELECTOR_USE_STATIC_PLUGIN
Q_IMPORT_PLUGIN(JamiPhotoSelector)
#else
#include <photoselector/photoplugin.h>
#endif

#ifdef JAMINOTIFICATION_USE_STATIC_PLUGIN
Q_IMPORT_PLUGIN(JamiNotification)
#else
#include <jaminotification/plugin.h>
#endif

#ifdef JAMIVIDEOVIEW_USE_STATIC_PLUGIN
Q_IMPORT_PLUGIN(JamiVideoView)
#else
#include <jamivideoview/plugin.h>
#endif

Q_IMPORT_PLUGIN(RingQtQuick)

constexpr static const char version[] = "3.1.0";

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
      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_RingQtQuick().instance())->initializeEngine(app.engine(), "net.lvindustries.ringqtquick");

#ifdef JAMIKDEINTEGRATION_USE_STATIC_PLUGIN
      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_JamiKDEIntegration().instance())->registerTypes("org.kde.ringkde.jamikdeintegration");
      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_JamiKDEIntegration().instance())->initializeEngine(app.engine(), "org.kde.ringkde.jamikdeintegration");
#else
      JamiKDEIntegration v2;
      v2.registerTypes("org.kde.ringkde.jamikdeintegration");
      v2.initializeEngine(app.engine(), "org.kde.ringkde.jamikdeintegration");
#endif

#ifdef JAMIWIZARD_USE_STATIC_PLUGIN
      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_JamiWizard().instance())->registerTypes("org.kde.ringkde.jamiwizard");
      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_JamiWizard().instance())->initializeEngine(app.engine(), "org.kde.ringkde.jamiwizard");
#else
      JamiWizard v3;
      v3.registerTypes("org.kde.ringkde.jamiwizard");
      v3.initializeEngine(app.engine(), "org.kde.ringkde.jamiwizard");
#endif

#ifdef JAMIACCOUNTVIEW_USE_STATIC_PLUGIN
      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_JamiAccountView().instance())->registerTypes("org.kde.ringkde.jamiaccountview");
      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_JamiAccountView().instance())->initializeEngine(app.engine(), "org.kde.ringkde.jamiaccountview");
#else
      JamiAccountView v4;
      v4.registerTypes("org.kde.ringkde.jamiaccountview");
      v4.initializeEngine(app.engine(), "org.kde.ringkde.jamiaccountview");
#endif

#ifdef JAMIACCOUNTVIEW_USE_STATIC_PLUGIN
      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_JamiCallView().instance())->registerTypes("org.kde.ringkde.jamicallview");
      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_JamiCallView().instance())->initializeEngine(app.engine(), "org.kde.ringkde.jamicallview");
#else
      JamiCallViewPlugin v5;
      v5.registerTypes("org.kde.ringkde.jamicallview");
      v5.initializeEngine(app.engine(), "org.kde.ringkde.jamicallview");
#endif

#ifdef JAMICONTACTVIEW_USE_STATIC_PLUGIN
      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_JamiContactView().instance())->registerTypes("org.kde.ringkde.jamicontactview");
      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_JamiContactView().instance())->initializeEngine(app.engine(), "org.kde.ringkde.jamicontactview");
#else
      JamiContactViewPlugin v6;
      v6.registerTypes("org.kde.ringkde.jamicontactview");
      v6.initializeEngine(app.engine(), "org.kde.ringkde.jamicontactview");
#endif

#ifdef JAMIDIALVIEW_USE_STATIC_PLUGIN
      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_JamiDialView().instance())->registerTypes("org.kde.ringkde.jamidialview");
      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_JamiDialView().instance())->initializeEngine(app.engine(), "org.kde.ringkde.jamidialview");
#else
      JamiDialView v7;
      v7.registerTypes("org.kde.ringkde.jamidialview");
      v7.initializeEngine(app.engine(), "org.kde.ringkde.jamidialview");
#endif

#ifdef JAMITIMELINEVIEW_USE_STATIC_PLUGIN
      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_JamiTimelineView().instance())->registerTypes("org.kde.ringkde.jamitimelineview");
      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_JamiTimelineView().instance())->initializeEngine(app.engine(), "org.kde.ringkde.jamitimelineview");
#else
      JamiTimelineView v8;
      v8.registerTypes("org.kde.ringkde.jamitimelineview");
      v8.initializeEngine(app.engine(), "org.kde.ringkde.jamitimelineview");
#endif

#ifdef JAMICANVASINDICATOR_USE_STATIC_PLUGIN
      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_JamiCanvasIndicator().instance())->registerTypes("org.kde.ringkde.jamicanvasindicator");
      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_JamiCanvasIndicator().instance())->initializeEngine(app.engine(), "org.kde.ringkde.jamicanvasindicator");
#else
      JamiCanvasIndicator v9;
      v9.registerTypes("org.kde.ringkde.jamicanvasindicator");
      v9.initializeEngine(app.engine(), "org.kde.ringkde.jamicanvasindicator");
#endif

#ifdef JAMIPHOTOSELECTOR_USE_STATIC_PLUGIN
      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_JamiPhotoSelector().instance())->registerTypes("org.kde.ringkde.jamiphotoselector");
      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_JamiPhotoSelector().instance())->initializeEngine(app.engine(), "org.kde.ringkde.jamiphotoselector");
#else
      JamiPhotoSelector v10;
      v10.registerTypes("org.kde.ringkde.jamiphotoselector");
      v10.initializeEngine(app.engine(), "org.kde.ringkde.jamiphotoselector");
#endif

#ifdef JAMINOTIFICATION_USE_STATIC_PLUGIN
      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_JamiNotification().instance())->registerTypes("org.kde.ringkde.jaminotification");
      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_JamiNotification().instance())->initializeEngine(app.engine(), "org.kde.ringkde.jaminotification");
#else
      JamiNotification v11;
      v11.registerTypes("org.kde.ringkde.jaminotification");
      v11.initializeEngine(app.engine(), "org.kde.ringkde.jaminotification");
#endif

#ifdef JAMIVIDEOVIEW_USE_STATIC_PLUGIN
      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_JamiNotification().instance())->registerTypes("org.kde.ringkde.jamivideoview");
      qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_JamiNotification().instance())->initializeEngine(app.engine(), "org.kde.ringkde.jamivideoview");
#else
      JamiVideoView v12;
      v12.registerTypes("org.kde.ringkde.jamivideoview");
      v12.initializeEngine(app.engine(), "org.kde.ringkde.jamivideoview");
#endif

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
