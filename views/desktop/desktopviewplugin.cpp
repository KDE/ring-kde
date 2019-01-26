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
#include "desktopviewplugin.h"

#include <QtCore/QDebug>
#include <QQmlEngine>

#include "dockmodel.h"
#include "tipmodel.h"
#include "qmlaction.h"
#include "qactionbinder.h"
#include "mainpage.h"

#include <qrc_desktopview.cpp>

void DesktopView::registerTypes(const char *uri)
{
    qmlRegisterType<DockModel>(uri, 1, 0, "DockModel");
    qmlRegisterType<TipModel>(uri, 1, 0, "TipModel");
    qmlRegisterType<QmlAction>(uri, 1, 0, "QmlAction");
    qmlRegisterType<QActionBinder>(uri, 1, 0, "QActionBinder");
    qmlRegisterType<MainPage>(uri, 1, 0, "MainPage");

    qmlRegisterType(QStringLiteral("qrc:/desktopview/qml/desktopwindow.qml"), uri, 1, 0, "DesktopWindow");
    qmlRegisterType(QStringLiteral("qrc:/desktopview/qml/dockbar.qml"), uri, 1, 0, "DockBar");
    qmlRegisterType(QStringLiteral("qrc:/desktopview/qml/drawer.qml"), uri, 1, 0, "Drawer");
    qmlRegisterType(QStringLiteral("qrc:/desktopview/qml/viewcontact.qml"), uri, 1, 0, "ViewContact");
    qmlRegisterType(QStringLiteral("qrc:/desktopview/qml/contactheader.qml"), uri, 1, 0, "ContactHeader");
    qmlRegisterType(QStringLiteral("qrc:/desktopview/qml/mediabuttons.qml"), uri, 1, 0, "MediaButtons");
}

void DesktopView::initializeEngine(QQmlEngine* engine, const char* uri)
{
    Q_UNUSED(engine)
    Q_UNUSED(uri)
}
