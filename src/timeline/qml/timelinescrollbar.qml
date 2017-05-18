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
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Ring 1.0

Item {
    id: scrollbar

    Rectangle {
        radius: 99
        color: "black"
        width: parent.width
        height: 65
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            timelineOverlay.height = height
            timelineOverlay.visible = true
        }
        onExited:  timelineOverlay.visible = false

        Rectangle {
            id: timelineOverlay
            color: "orange"
            width: 100
            x: -100
            height: scrollbar.height
            visible: false
        }
    }
}
