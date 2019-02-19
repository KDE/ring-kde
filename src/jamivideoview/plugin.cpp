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
#include <qrc_jamivideo.cpp>

// VideoView
#include "snapshotadapter.h"
#include "imageprovider.h"

void JamiVideoView::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QByteArray("org.kde.ringkde.jamivideoview"));

    qmlRegisterType<SnapshotAdapter>(uri, 1, 0, "SnapshotAdapter");

    qmlRegisterType(QStringLiteral("qrc:/jamivideoview/qml/videowidget.qml"), uri, 1, 0, "VideoWidget");
    qmlRegisterType(QStringLiteral("qrc:/jamivideoview/qml/videocontroltoolbar.qml"), uri, 1, 0, "VideoControlToolbar");
    qmlRegisterType(QStringLiteral("qrc:/jamivideoview/qml/devicesetting.qml"), uri, 1, 0, "DeviceSetting");
    qmlRegisterType(QStringLiteral("qrc:/jamivideoview/qml/settingpopup.qml"), uri, 1, 0, "SettingPopup");
    qmlRegisterType(QStringLiteral("qrc:/jamivideoview/qml/videosource.qml"), uri, 1, 0, "VideoSource");
    qmlRegisterType(QStringLiteral("qrc:/jamivideoview/qml/canvaspopup.qml"), uri, 1, 0, "CanvasPopup");
    qmlRegisterType(QStringLiteral("qrc:/jamivideoview/qml/screensharing.qml"), uri, 1, 0, "ScreenSharing");
    qmlRegisterType(QStringLiteral("qrc:/jamivideoview/qml/streaming.qml"), uri, 1, 0, "Streaming");
}

void JamiVideoView::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(engine)
    Q_UNUSED(uri)

    auto p = new ImageProvider();
    engine->addImageProvider( QStringLiteral("VideoFrame"), p );
}
