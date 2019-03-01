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
#include <qrc_jamichatview.cpp>

// ChatView
#include "bubble.h"

void JamiChatView::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QByteArray("org.kde.ringkde.jamichatview"));

    qmlRegisterType<Bubble>(uri, 1, 0, "Bubble");
    qmlRegisterType(QStringLiteral("qrc:/jamichatview/qml/chatbox.qml")         , uri, 1, 0, "ChatBox");
    qmlRegisterType(QStringLiteral("qrc:/jamichatview/qml/chatview.qml")        , uri, 1, 0, "ChatView");
    qmlRegisterType(QStringLiteral("qrc:/jamichatview/qml/textbubble.qml")      , uri, 1, 0, "TextBubble");
    qmlRegisterType(QStringLiteral("qrc:/jamichatview/qml/categoryheader.qml")  , uri, 1, 0, "CategoryHeader");
    qmlRegisterType(QStringLiteral("qrc:/jamichatview/qml/textmessagegroup.qml"), uri, 1, 0, "TextMessageGroup");
    qmlRegisterType(QStringLiteral("qrc:/jamichatview/qml/callgroup.qml")       , uri, 1, 0, "CallGroup");
    qmlRegisterType(QStringLiteral("qrc:/jamichatview/qml/groupheader.qml")     , uri, 1, 0, "GroupHeader");
    qmlRegisterType(QStringLiteral("qrc:/jamichatview/qml/groupfooter.qml")     , uri, 1, 0, "GroupFooter");
    qmlRegisterType(QStringLiteral("qrc:/jamichatview/qml/snapshots.qml")       , uri, 1, 0, "Snapshots");
    qmlRegisterType(QStringLiteral("qrc:/jamichatview/qml/slideshow.qml")       , uri, 1, 0, "Slideshow");
    qmlRegisterType(QStringLiteral("qrc:/jamichatview/qml/thumbnail.qml")       , uri, 1, 0, "Thumbnail");
    qmlRegisterType(QStringLiteral("qrc:/jamichatview/qml/chatpage.qml")        , uri, 1, 0, "ChatPage");
    qmlRegisterType(QStringLiteral("qrc:/jamichatview/qml/navigation.qml")      , uri, 1, 0, "Navigation");
}

void JamiChatView::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(engine)
    Q_UNUSED(uri)
}
