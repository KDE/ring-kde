/************************************************************************************
 *   Copyright (C) 2018 by BlueSystems GmbH                                         *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                            *
 *                                                                                  *
 *   This library is free software; you can redistribute it and/or                  *
 *   modify it under the terms of the GNU Lesser General Public                     *
 *   License as published by the Free Software Foundation; either                   *
 *   version 2.1 of the License, or (at your option) any later version.             *
 *                                                                                  *
 *   This library is distributed in the hope that it will be useful,                *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of                 *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU              *
 *   Lesser General Public License for more details.                                *
 *                                                                                  *
 *   You should have received a copy of the GNU Lesser General Public               *
 *   License along with this library; if not, write to the Free Software            *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA *
 ***********************************************************************************/
#include "plugin.h"

// Qt
#include <QQmlEngine>
#include <QQmlContext>

// QRC
#include <qrc_jamitroubleshooting.cpp>

void JamiTroubleshooting::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QByteArray("org.kde.ringkde.jamitroubleshooting"));

    qmlRegisterType(QStringLiteral("qrc:/jamitroubleshooting/qml/troubleshoot.qml")   , uri, 1, 0, "Troubleshoot");
    qmlRegisterType(QStringLiteral("qrc:/jamitroubleshooting/qml/noaccount.qml")      , uri, 1, 0, "NoAccount");
    qmlRegisterType(QStringLiteral("qrc:/jamitroubleshooting/qml/accounterror.qml")   , uri, 1, 0, "AccountError");
    qmlRegisterType(QStringLiteral("qrc:/jamitroubleshooting/qml/accountdisabled.qml"), uri, 1, 0, "AccountDisabled");
    qmlRegisterType(QStringLiteral("qrc:/jamitroubleshooting/qml/mediaavailability.qml"), uri, 1, 0, "MediaAvailability");
    qmlRegisterType(QStringLiteral("qrc:/jamitroubleshooting/qml/globaltroubleshoot.qml"), uri, 1, 0, "GlobalTroubleshoot");
}

void JamiTroubleshooting::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(engine)
    Q_UNUSED(uri)
}
