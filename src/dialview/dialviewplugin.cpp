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
#include "dialviewplugin.h"

// Qt
#include <QQmlEngine>
#include <QQmlContext>

// QRC
#include <qrc_dialview.cpp>

void JamiDialView::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QByteArray("org.kde.ringkde.jamidialview"));

    qmlRegisterType(QStringLiteral("qrc:/dialview/qml/dialview.qml"),         uri, 1, 0, "DialView"        );
    qmlRegisterType(QStringLiteral("qrc:/dialview/qml/dialpad.qml"),          uri, 1, 0, "DialPad"         );
    qmlRegisterType(QStringLiteral("qrc:/dialview/qml/calllist.qml"),         uri, 1, 0, "CallList"        );
    qmlRegisterType(QStringLiteral("qrc:/dialview/qml/calldelegate.qml"),     uri, 1, 0, "CallDelegate"    );
    qmlRegisterType(QStringLiteral("qrc:/dialview/qml/calldelegateitem.qml"), uri, 1, 0, "CallDelegateItem");
    qmlRegisterType(QStringLiteral("qrc:/dialview/qml/addcall.qml"),          uri, 1, 0, "AddCall"         );
    qmlRegisterType(QStringLiteral("qrc:/dialview/qml/mergecall.qml"),        uri, 1, 0, "MergeCall"       );
    qmlRegisterType(QStringLiteral("qrc:/dialview/qml/callerror.qml"),        uri, 1, 0, "CallError"       );
    qmlRegisterType(QStringLiteral("qrc:/dialview/qml/missedcall.qml"),       uri, 1, 0, "MissedCall"      );
    qmlRegisterType(QStringLiteral("qrc:/dialview/qml/outgoingcall.qml"),     uri, 1, 0, "OutgoingCall"    );
    qmlRegisterType(QStringLiteral("qrc:/dialview/qml/incomingcall.qml"),     uri, 1, 0, "IncomingCall"    );
    qmlRegisterType(QStringLiteral("qrc:/dialview/qml/finishedcall.qml"),     uri, 1, 0, "FinishedCall"    );
    qmlRegisterType(QStringLiteral("qrc:/dialview/qml/currentcall.qml"),      uri, 1, 0, "CurrentCall"     );
    qmlRegisterType(QStringLiteral("qrc:/dialview/qml/completiondelegate.qml"),      uri, 1, 0, "CompletionDelegate");

    qmlRegisterType(QStringLiteral("qrc:/dialview/qml/actiontoolbar.qml"), uri, 1, 0, "ActionToolbar");
}

void JamiDialView::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(engine)
    Q_UNUSED(uri)
}
