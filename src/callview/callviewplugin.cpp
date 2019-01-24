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
#include "callviewplugin.h"

// Qt
#include <QQmlEngine>
#include <QQmlContext>
#include <QtGui/QIcon>

// QRC
#include <qrc_callview.cpp>

void JamiCallView::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QByteArray("org.kde.ringkde.jamicallview"));

    qmlRegisterType(QStringLiteral("qrc:/callview/qml/callview.qml"), uri, 1, 0, "CallView");
    qmlRegisterType(QStringLiteral("qrc:/callview/qml/cmselector.qml"), uri, 1, 0, "CMSelector");
    qmlRegisterType(QStringLiteral("qrc:/callview/qml/recordingicon.qml"), uri, 1, 0, "RecordingIcon");
    qmlRegisterType(QStringLiteral("qrc:/callview/qml/callbackground.qml"), uri, 1, 0, "CallBackground");
}

void JamiCallView::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(engine)
    Q_UNUSED(uri)
}
