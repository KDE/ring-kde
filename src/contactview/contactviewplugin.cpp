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
#include "contactviewplugin.h"

// Qt
#include <QQmlEngine>
#include <QQmlContext>
#include <QtGui/QIcon>

// QRC
#include <qrc_contactview.cpp>

// CallView
#include "mainpage.h"
#include "contactphoto.h"

void JamiContactViewPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QByteArray("org.kde.ringkde.jamicontactview"));

    qmlRegisterType<MainPage>(uri, 1, 0, "MainPage");
    qmlRegisterType<ContactPhoto>(uri, 1, 0, "ContactPhoto");

    qmlRegisterType(QStringLiteral("qrc:/contactview/qml/pendingrequest.qml")   , uri, 1, 0, "PendingRequest");
    qmlRegisterType(QStringLiteral("qrc:/contactview/qml/contactlist.qml")      , uri, 1, 0, "ContactList");
    qmlRegisterType(QStringLiteral("qrc:/contactview/qml/bookmarklist.qml")     , uri, 1, 0, "BookmarkList");
    qmlRegisterType(QStringLiteral("qrc:/contactview/qml/contactcard.qml")      , uri, 1, 0, "ContactCard");
    qmlRegisterType(QStringLiteral("qrc:/contactview/qml/bookmarkcard.qml")     , uri, 1, 0, "BookmarkCard");
    qmlRegisterType(QStringLiteral("qrc:/contactview/qml/vcardform.qml")        , uri, 1, 0, "VCardForm");
    qmlRegisterType(QStringLiteral("qrc:/contactview/qml/statistics.qml")       , uri, 1, 0, "Statistics");
    qmlRegisterType(QStringLiteral("qrc:/contactview/qml/newcontactdialog.qml") , uri, 1, 0, "NewContactDialog");
    qmlRegisterType(QStringLiteral("qrc:/contactview/qml/phonenumbers.qml")              , uri, 1, 0, "PhoneNumbers");
    qmlRegisterType(QStringLiteral("qrc:/contactview/qml/contactinfo.qml")               , uri, 1, 0, "ContactInfo");
    qmlRegisterType(QStringLiteral("qrc:/contactview/qml/pendingrequests.qml")           , uri, 1, 0, "PendingRequests");
    qmlRegisterType(QStringLiteral("qrc:/contactview/qml/sendrequest.qml")               , uri, 1, 0, "SendRequest");
    qmlRegisterType(QStringLiteral("qrc:/contactview/qml/viewcontactrequests.qml")       , uri, 1, 0, "ViewContactRequests");
    qmlRegisterType(QStringLiteral("qrc:/contactview/qml/addresses.qml")                 , uri, 1, 0, "Addresses");
    qmlRegisterType(QStringLiteral("qrc:/contactview/qml/contactdialog.qml")             , uri, 1, 0, "ContactDialog");
}

void JamiContactViewPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(engine)
    Q_UNUSED(uri)
}
