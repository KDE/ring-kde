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
#include "plugin.h"

#include <QtCore/QDebug>
#include <QtWidgets/QAction>
#include <QQmlEngine>

#include "treehelper.h"
#include "symboliccolorizer.h"
#include "fileloader.h"

#include "qrc_qmlwidgets.cpp"

void GenericUtils::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.kde.ringkde.genericutils"));

    qmlRegisterType<QAction>(uri, 1, 0, "QAction");

    qmlRegisterType<TreeHelper>(uri, 1, 0, "TreeHelper");
    qmlRegisterType<FileLoader>(uri, 1, 0, "FileLoader");
    qmlRegisterType(QStringLiteral("qrc:/qml/outlinebutton.qml"), uri, 1, 0, "OutlineButton");
    qmlRegisterType(QStringLiteral("qrc:/qml/outlinebuttons.qml"), uri, 1, 0, "OutlineButtons");
}

void GenericUtils::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(engine)
    Q_UNUSED(uri)

    static SymbolicColorizer p;
    engine->addImageProvider( QStringLiteral("SymbolicColorizer"), &p);
}
