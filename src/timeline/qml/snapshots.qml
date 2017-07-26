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
import QtQuick 2.7
import QtQml.Models 2.2

Item {
    id: snapshots

    property QtObject model_: rootIndex.model
    property var rootIndex_: rootIndex
    signal viewImage(QtObject model, int index)

    height: Math.ceil(rowCount/4)*96 + 40 // 1 inch + margins

    Thumbnail {
        id: delegateModel
        rootIndex_: snapshots.rootIndex_
        model_: snapshots.model_
        onViewImage: {
            snapshots.viewImage(model, index, path)
        }
    }

    Rectangle {
        height: parent.height - 20
        width: Math.min(4, rowCount)*96 + 40
        anchors.centerIn: parent
        color: "transparent"
        radius: 10
        border.color: activePalette.text
        border.width: 1

        GridView {
            anchors.fill: parent
            anchors.margins: 10
            model: delegateModel
        }
    }
}
