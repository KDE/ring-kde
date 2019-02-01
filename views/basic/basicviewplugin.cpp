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
#include "basicviewplugin.h"

#include <QtCore/QDebug>
#include <QQmlEngine>


#include <qrc_basic.cpp>

void BasicView::registerTypes(const char *uri)
{
    qmlRegisterType(QStringLiteral("qrc:/basicview/qml/actioncollection.qml"), uri, 1, 0, "ActionCollection");
    qmlRegisterType(QStringLiteral("qrc:/basicview/qml/chatpage.qml"), uri, 1, 0, "ChatPage");
    qmlRegisterType(QStringLiteral("qrc:/basicview/qml/callpage.qml"), uri, 1, 0, "CallPage");
    qmlRegisterType(QStringLiteral("qrc:/basicview/qml/contacts.qml"), uri, 1, 0, "Contacts");
    qmlRegisterType(QStringLiteral("qrc:/basicview/qml/desktopheader.qml"), uri, 1, 0, "DesktopHeader");
    qmlRegisterType(QStringLiteral("qrc:/basicview/qml/detailpage.qml"), uri, 1, 0, "DetailPage");
    qmlRegisterType(QStringLiteral("qrc:/basicview/qml/detail.qml"), uri, 1, 0, "Detail");
    qmlRegisterType(QStringLiteral("qrc:/basicview/qml/formpage.qml"), uri, 1, 0, "FormPage");
    qmlRegisterType(QStringLiteral("qrc:/basicview/qml/form.qml"), uri, 1, 0, "Form");
    qmlRegisterType(QStringLiteral("qrc:/basicview/qml/listpage.qml"), uri, 1, 0, "ListPage");
    qmlRegisterType(QStringLiteral("qrc:/basicview/qml/list.qml"), uri, 1, 0, "List");
    qmlRegisterType(QStringLiteral("qrc:/basicview/qml/globaldrawer.qml"), uri, 1, 0, "GlobalDrawer");
    qmlRegisterType(QStringLiteral("qrc:/basicview/qml/sidebar.qml"), uri, 1, 0, "SideBar");
    qmlRegisterType(QStringLiteral("qrc:/basicview/qml/actionswitch.qml"), uri, 1, 0, "ActionSwitch");
}

void BasicView::initializeEngine(QQmlEngine* engine, const char* uri)
{
    Q_UNUSED(engine)
    Q_UNUSED(uri)
}
