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

#include "bubble.h"
#include "messagebuilder.h"
#include "contactbuilder.h"
#include "pixmapwrapper.h"
#include "modelscrolladapter.h"
#include "treehelper.h"
#include "treeview.h"
#include "multicall.h"
#include "bindedcombobox.h"
#include "snapshotadapter.h"

void RingQmlWidgets::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("RingQmlWidgets"));

    qmlRegisterType<Bubble>(uri, 1, 0, "Bubble");
    qmlRegisterType<MultiCall>(uri, 1, 0, "MultiCall");
    qmlRegisterType<TreeView>(uri, 1, 0, "TreeView");
    qmlRegisterType<MessageBuilder>(uri, 1, 0, "MessageBuilder");
    qmlRegisterType<ContactBuilder>(uri, 1, 0, "ContactBuilder");
    qmlRegisterType<TreeHelper>(uri, 1, 0, "TreeHelper");
    qmlRegisterType<ModelScrollAdapter>(uri, 1, 0, "ModelScrollAdapter");
    qmlRegisterType<PixmapWrapper>("Ring", 1,0, "PixmapWrapper");
    qmlRegisterType<BindedComboBox>(uri, 1, 0, "BindedComboBox");
    qmlRegisterType<SnapshotAdapter>(uri, 1, 0, "SnapshotAdapter");
}
