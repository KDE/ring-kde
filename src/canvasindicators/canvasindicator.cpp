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
#include "canvasindicator.h"

// Qt
#include <QQmlEngine>
#include <QQmlContext>
#include <QtGui/QIcon>

// QRC
#include <qrc_canvasindicator.cpp>

#include "ringingimageprovider.h"

void JamiCanvasIndicator::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QByteArray("org.kde.ringkde.jamicanvasindicator"));

    qmlRegisterType(QStringLiteral("qrc:/canvasassets/qml/ringing.qml"),   uri, 1, 0, "Ringing"  );
    qmlRegisterType(QStringLiteral("qrc:/canvasassets/qml/searching.qml"), uri, 1, 0, "Searching");
    qmlRegisterType(QStringLiteral("qrc:/canvasassets/qml/progress.qml"),  uri, 1, 0, "Progress" );
    qmlRegisterType(QStringLiteral("qrc:/canvasassets/qml/newcall.qml"),   uri, 1, 0, "NewCall"  );
}

void JamiCanvasIndicator::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_ASSERT(uri == QByteArray("org.kde.ringkde.jamicanvasindicator"));

    auto im = new RingingImageProvider();
    engine->addImageProvider( QStringLiteral("RingingImageProvider"), im );
}
