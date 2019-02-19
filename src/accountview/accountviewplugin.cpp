/***************************************************************************
 *   Copyright (C) 2017 by Bluesystems                                     *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
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
#include "accountviewplugin.h"

// Qt
#include <QQmlEngine>
#include <QQmlContext>
#include <QtCore/QDebug>

#include <qrc_accountview.cpp>

void JamiAccountView::registerTypes(const char *uri)
{
    static QByteArray pageUri = QByteArray(uri)+".pages";

    qmlRegisterType(QStringLiteral("qrc:/account/qml/accountlist.qml"), uri, 1, 0,           "AccountList");
    qmlRegisterType(QStringLiteral("qrc:/account/qml/presenceselector.qml"), uri, 1, 0,      "PresenceSelector");
    qmlRegisterType(QStringLiteral("qrc:/account/qml/accounts.qml"), uri, 1, 0,              "Accounts");
    qmlRegisterType(QStringLiteral("qrc:/account/qml/accountdialog.qml"), uri, 1, 0,         "AccountDialog");
    qmlRegisterType(QStringLiteral("qrc:/account/qml/securitylevel.qml"), uri, 1, 0,         "SecurityLevel");
    qmlRegisterType(QStringLiteral("qrc:/account/qml/securitytip.qml"), uri, 1, 0,           "SecurityTip");
    qmlRegisterType(QStringLiteral("qrc:/account/qml/deleter.qml"), uri, 1, 0,               "Deleter");
    qmlRegisterType(QStringLiteral("qrc:/account/qml/pages/basic.qml"), pageUri, 1, 0,       "Basic");
    qmlRegisterType(QStringLiteral("qrc:/account/qml/pages/advanced.qml"), pageUri, 1, 0,    "Advanced");
    qmlRegisterType(QStringLiteral("qrc:/account/qml/pages/network.qml"), pageUri, 1, 0,     "Network");
    qmlRegisterType(QStringLiteral("qrc:/account/qml/pages/codecs.qml"), pageUri, 1, 0,      "Codecs");
    qmlRegisterType(QStringLiteral("qrc:/account/qml/pages/credentials.qml"), pageUri, 1, 0, "Credentials");
    qmlRegisterType(QStringLiteral("qrc:/account/qml/pages/devices.qml"), pageUri, 1, 0,     "Devices");
    qmlRegisterType(QStringLiteral("qrc:/account/qml/pages/security.qml"), pageUri, 1, 0,    "Security");
    qmlRegisterType(QStringLiteral("qrc:/account/qml/pages/ringtones.qml"), pageUri, 1, 0,   "Ringtones");
    qmlRegisterType(QStringLiteral("qrc:/account/qml/pages/profiles.qml"), pageUri, 1, 0,    "Profiles");
}
