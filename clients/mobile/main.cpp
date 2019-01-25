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
#include <QtWidgets/QApplication>

//KDE
#include <KAboutData>
#include <KLocalizedString>
#include <KCrash>

//Ring
#include "kcfg_settings.h"
#include <QQmlDebuggingEnabler>

#include <QQmlExtensionPlugin>

#ifdef KQUICKITEMVIEWS_USE_STATIC_PLUGIN
Q_IMPORT_PLUGIN(KQuickItemViews)
#else
#include <KQuickItemViews/plugin.h>
#endif

Q_IMPORT_PLUGIN(JamiKDEIntegration)
Q_IMPORT_PLUGIN(JamiWizard)
Q_IMPORT_PLUGIN(JamiAccountView)
Q_IMPORT_PLUGIN(JamiCallView)
Q_IMPORT_PLUGIN(JamiContactView)
Q_IMPORT_PLUGIN(JamiDialView)
Q_IMPORT_PLUGIN(JamiTimelineView)
Q_IMPORT_PLUGIN(JamiCanvasIndicator)
Q_IMPORT_PLUGIN(JamiPhotoSelector)
Q_IMPORT_PLUGIN(JamiVideoView)
Q_IMPORT_PLUGIN(JamiTroubleshooting)
Q_IMPORT_PLUGIN(JamiChatView)
Q_IMPORT_PLUGIN(JamiHistoryView)
Q_IMPORT_PLUGIN(JamiTimelineBase)
Q_IMPORT_PLUGIN(JamiAudioPlayer)
Q_IMPORT_PLUGIN(GenericUtils)
Q_IMPORT_PLUGIN(RingQtQuick)

#if false
Q_IMPORT_PLUGIN(DesktopView)
#else
Q_IMPORT_PLUGIN(BasicView)
#endif

#ifndef Q_OS_ANDROID
Q_IMPORT_PLUGIN(JamiNotification)
#endif

constexpr static const char version[] = "3.1.0";

#define REGISTER_PLUGIN(name, uri) \
 qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_ ## name().instance())->registerTypes(uri); \
 qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_ ## name().instance())->initializeEngine(&engine, uri);

int main(int argc, char **argv)
{
    //QQmlDebuggingEnabler enabler;

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    KLocalizedString::setApplicationDomain("ring-kde");

    //FIXME remove
#ifdef KQUICKITEMVIEWS_USE_STATIC_PLUGIN
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_KQuickItemViews().instance())->registerTypes("org.kde.playground.kquickitemviews");
#else
    KQuickItemViews v;
    v.registerTypes("org.kde.playground.kquickitemviews");
#endif

    // The order is important
    REGISTER_PLUGIN(JamiKDEIntegration, "org.kde.ringkde.jamikdeintegration")
    REGISTER_PLUGIN(RingQtQuick, "net.lvindustries.ringqtquick")

    // The order is not important
    REGISTER_PLUGIN(GenericUtils, "org.kde.ringkde.genericutils")
    REGISTER_PLUGIN(JamiWizard, "org.kde.ringkde.jamiwizard")
    REGISTER_PLUGIN(JamiAccountView, "org.kde.ringkde.jamiaccountview")
    REGISTER_PLUGIN(JamiCallView, "org.kde.ringkde.jamicallview")
    REGISTER_PLUGIN(JamiContactView, "org.kde.ringkde.jamicontactview")
    REGISTER_PLUGIN(JamiDialView, "org.kde.ringkde.jamidialview")
    REGISTER_PLUGIN(JamiTimelineView, "org.kde.ringkde.jamitimeline")
    REGISTER_PLUGIN(JamiCanvasIndicator, "org.kde.ringkde.jamicanvasindicator")
    REGISTER_PLUGIN(JamiPhotoSelector, "org.kde.ringkde.jamiphotoselector")
    REGISTER_PLUGIN(JamiVideoView, "org.kde.ringkde.jamivideoview")
    REGISTER_PLUGIN(JamiTroubleshooting, "org.kde.ringkde.jamitroubleshooting")
    REGISTER_PLUGIN(JamiTimelineBase, "org.kde.ringkde.jamitimelinebase")
    REGISTER_PLUGIN(JamiHistoryView, "org.kde.ringkde.jamihistoryview")
    REGISTER_PLUGIN(JamiChatView, "org.kde.ringkde.jamichatview")
    REGISTER_PLUGIN(JamiAudioPlayer, "org.kde.ringkde.jamiaudioplayer")

#ifndef Q_OS_ANDROID
    REGISTER_PLUGIN(JamiNotification, "org.kde.ringkde.jaminotification")
#endif

#if false
    REGISTER_PLUGIN(DesktopView, "org.kde.ringkde.desktopview")
#else
    REGISTER_PLUGIN(BasicView, "org.kde.ringkde.basicview")
#endif

    KAboutData about(QStringLiteral("ring-kde"),
        i18n("ring-kde"),
        /*QStringLiteral(*/version/*)*/,
        i18n("RING, a secured and distributed communication software"),
        KAboutLicense::GPL_V3,
        i18n("(C) 2004-2015 Savoir-faire Linux\n2016-2017 Emmanuel Lepage Vallee\n2017-2019 Blue Systems"),
        QString(),
        QStringLiteral("http://www.ring.cx"),
        QStringLiteral("ring@gnu.org")
    );
    about.setOrganizationDomain(QByteArray("kde.org"));
    about.setProgramLogo(QImage(QStringLiteral(":appicon/icons/64-apps-ring-kde.png")));

    about.addAuthor( i18n( "Emmanuel Lepage-Vallée"          ), QString(), QStringLiteral("elv1313@gmail.com"                    ));
    about.addAuthor( i18n( "Alexandre Lision"                ), QString(), QStringLiteral("alexandre.lision@savoirfairelinux.com"));
    about.addCredit( i18n( "Based on the SFLphone teamworks" ), QString(), QString()                                              );

    KAboutData::setApplicationData(about);

    KCrash::initialize();

    app.setOrganizationDomain(QStringLiteral("kde.org"));

#if false
    engine.load(QUrl(QStringLiteral("qrc:/desktopview/qml/desktopwindow.qml")));
#else
    engine.load(QUrl(QStringLiteral("qrc:/basicview/qml/basic.qml")));
#endif

    const int retVal = app.exec();

    ConfigurationSkeleton::self()->save();

    return retVal;
}

// kate: space-indent on; indent-width 4; replace-tabs on;
