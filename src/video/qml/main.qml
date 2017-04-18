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

Item {
    id: videoDock
    visible: true

    height: 480
    width: 640

    VideoWidget {
        anchors.fill: parent
        z: -100
    }

    VideoControlToolbar {
        id: controlToolbar
        y: parent.y
    }

    VideoStateToolbar {
        id: actionToolbar
        y: parent.height - height
    }

    // Hide both toolbars when the mouse isn't moving
    //TODO keep visible if the mouse if over the toolbars
    MouseArea {
        Timer {
            id: activityTimer
            interval: 3000
            running: true
            repeat: false
            onTriggered: {
                actionToolbar.visible = false
                controlToolbar.visible = false
            }
        }

        function trackActivity() {
            actionToolbar.visible = true
            controlToolbar.visible = true
            activityTimer.restart()
        }

        anchors.fill: parent
        hoverEnabled: true
        onMouseXChanged: trackActivity()
        onMouseYChanged: trackActivity()
    }
}
