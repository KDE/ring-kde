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
#include "timelineplugin.h"

// Qt
#include <QQmlEngine>

// QRC
#include <qrc_recentdock.cpp>

void JamiTimelineView::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QByteArray("org.kde.ringkde.jamitimeline"));

    qmlRegisterType(QStringLiteral("qrc:/qml/peerstimeline.qml")             , uri, 1, 0, "PeersTimeline");
    qmlRegisterType(QStringLiteral("qrc:/qml/historytimeline.qml")           , uri, 1, 0, "HistoryTimeline");
    qmlRegisterType(QStringLiteral("qrc:/qml/contactdialog.qml")             , uri, 1, 0, "ContactDialog");
    qmlRegisterType(QStringLiteral("qrc:/qml/callhistory.qml")               , uri, 1, 0, "CallHistory");
    qmlRegisterType(QStringLiteral("qrc:/qml/recordinghistory.qml")          , uri, 1, 0, "RecordingHistory");
    qmlRegisterType(QStringLiteral("qrc:/qml/audioplayer.qml")               , uri, 1, 0, "AudioPlayer");
    qmlRegisterType(QStringLiteral("qrc:/qml/chatbox.qml")                   , uri, 1, 0, "ChatBox");
    qmlRegisterType(QStringLiteral("qrc:/qml/chatview.qml")                  , uri, 1, 0, "ChatView");
    qmlRegisterType(QStringLiteral("qrc:/qml/timelinepage.qml")              , uri, 1, 0, "TimelinePage");
    qmlRegisterType(QStringLiteral("qrc:/qml/timelinescrollbar.qml")         , uri, 1, 0, "TimelineScrollbar");
    qmlRegisterType(QStringLiteral("qrc:/qml/textbubble.qml")                , uri, 1, 0, "TextBubble");
    qmlRegisterType(QStringLiteral("qrc:/qml/categoryheader.qml")            , uri, 1, 0, "CategoryHeader");
    qmlRegisterType(QStringLiteral("qrc:/qml/textmessagegroup.qml")          , uri, 1, 0, "TextMessageGroup");
    qmlRegisterType(QStringLiteral("qrc:/qml/callgroup.qml")                 , uri, 1, 0, "CallGroup");
    qmlRegisterType(QStringLiteral("qrc:/qml/groupheader.qml")               , uri, 1, 0, "GroupHeader");
    qmlRegisterType(QStringLiteral("qrc:/qml/groupfooter.qml")               , uri, 1, 0, "GroupFooter");
    qmlRegisterType(QStringLiteral("qrc:/qml/contactmethoddelegate.qml")     , uri, 1, 0, "ContactMethodDelegate");
    qmlRegisterType(QStringLiteral("qrc:/qml/peerstimelinecategories.qml")   , uri, 1, 0, "PeersTimelineCategories");
    qmlRegisterType(QStringLiteral("qrc:/qml/addresses.qml")                 , uri, 1, 0, "Addresses");
    qmlRegisterType(QStringLiteral("qrc:/qml/searchdelegate.qml")            , uri, 1, 0, "SearchDelegate");
    qmlRegisterType(QStringLiteral("qrc:/qml/findpeers.qml")                 , uri, 1, 0, "FindPeers");
    qmlRegisterType(QStringLiteral("qrc:/qml/snapshots.qml")                 , uri, 1, 0, "Snapshots");
    qmlRegisterType(QStringLiteral("qrc:/qml/slideshow.qml")                 , uri, 1, 0, "Slideshow");
    qmlRegisterType(QStringLiteral("qrc:/qml/thumbnail.qml")                 , uri, 1, 0, "Thumbnail");
    qmlRegisterType(QStringLiteral("qrc:/qml/historycallicon.qml")           , uri, 1, 0, "HistoryCallIcon");
    qmlRegisterType(QStringLiteral("qrc:/qml/firstrun.qml")                  , uri, 1, 0, "FirstRun");
    qmlRegisterType(QStringLiteral("qrc:/qml/searchtip.qml")                 , uri, 1, 0, "SearchTip");
    qmlRegisterType(QStringLiteral("qrc:/qml/searchbox.qml")                 , uri, 1, 0, "SearchBox");
    qmlRegisterType(QStringLiteral("qrc:/qml/searchoverlay.qml")             , uri, 1, 0, "SearchOverlay");
}

void JamiTimelineView::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(engine)
    Q_UNUSED(uri)
}
