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
#include "jamisearchplugin.h"

// Qt
#include <QQmlEngine>

// QRC
#include <qrc_jamisearch.cpp>

// Ring
#include "tipmodel.h"

void JamiSearch::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QByteArray("org.kde.ringkde.jamisearch"));

    qmlRegisterType<TipModel>(uri, 1, 0, "TipModel");
    qmlRegisterType(QStringLiteral( "qrc:/search/qml/firstrun.qml"        ), uri, 1, 0, "FirstRun"        );
    qmlRegisterType(QStringLiteral( "qrc:/search/qml/searchtip.qml"       ), uri, 1, 0, "SearchTip"       );
    qmlRegisterType(QStringLiteral( "qrc:/search/qml/searchbox.qml"       ), uri, 1, 0, "SearchBox"       );
    qmlRegisterType(QStringLiteral( "qrc:/search/qml/searchoverlay.qml"   ), uri, 1, 0, "Overlay"         );
    qmlRegisterType(QStringLiteral( "qrc:/search/qml/findpeers.qml"       ), uri, 1, 0, "Results"         );
    qmlRegisterType(QStringLiteral( "qrc:/search/qml/toolbar.qml"         ), uri, 1, 0, "ToolBar"         );
    qmlRegisterType(QStringLiteral( "qrc:/search/qml/searchdelegate.qml"  ), uri, 1, 0, "Delegate"        );
    qmlRegisterType(QStringLiteral( "qrc:/search/qml/searchstate.qml"     ), uri, 1, 0, "State"           );
    qmlRegisterType(QStringLiteral( "qrc:/search/qml/matchcategories.qml" ), uri, 1, 0, "MatchCategories" );
}

void JamiSearch::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_ASSERT(uri == QByteArray("org.kde.ringkde.jamisearch"));
    Q_UNUSED(engine)
    Q_UNUSED(uri)
}
