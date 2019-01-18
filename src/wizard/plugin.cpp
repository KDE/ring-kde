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
#include <QtGui/QIcon>

// QRC
#include <qrc_wizard.cpp>

// Ring-KDE
#include "welcome.h"

void JamiWizard::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QByteArray("org.kde.ringkde.jamiwizard"));

    qmlRegisterType<WelcomeDialog>(uri, 1, 0, "WelcomeDialog");
    qmlRegisterType(QStringLiteral("qrc:/ButtonBar.qml"), uri, 1, 0, "ButtonBar");
    qmlRegisterType(QStringLiteral("qrc:/CreateRing.qml"), uri, 1, 0, "CreateRing");
    qmlRegisterType(QStringLiteral("qrc:/ImportRing.qml"), uri, 1, 0, "ImportRing");
    qmlRegisterType(QStringLiteral("qrc:/Page1.qml"), uri, 1, 0, "Page1");
    qmlRegisterType(QStringLiteral("qrc:/Page1Form.ui.qml"), uri, 1, 0, "Page1Form");
    qmlRegisterType(QStringLiteral("qrc:/ProfilePage.qml"), uri, 1, 0, "ProfilePage");
    qmlRegisterType(QStringLiteral("qrc:/StartingPage.qml"), uri, 1, 0, "StartingPage");
    qmlRegisterType(QStringLiteral("qrc:/StartingPageForm.ui.qml"), uri, 1, 0, "StartingPageForm");
    qmlRegisterType(QStringLiteral("qrc:/Wizard.qml"), uri, 1, 0, "Wizard");
}

void JamiWizard::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(engine)
    Q_UNUSED(uri)
}
