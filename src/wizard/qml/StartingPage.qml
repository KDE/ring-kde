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
import QtQuick 2.4

StartingPageForm {
    id: startingPage

    signal quit()

    Behavior on y {
        NumberAnimation {
            easing.type: Easing.InQuad
            duration: 500
            onRunningChanged: {
                if (!running){
                    visible = false
                }
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            x = 0
            y = 0
            width = parent.width
            height = parent.height

            startingPage.anchors.fill = undefined

            startingPage.state = "hidden"
        }
    }

    Timer {
        id: createAccountTimer
        interval: 5000
        repeat: true
        running: true
        onTriggered: {
            whatsNew.currentIndex = whatsNew.count-1 == whatsNew.currentIndex ?
                        0 : whatsNew.currentIndex+1
        }
    }

    states: [
        State {
            name: "hidden"
            PropertyChanges {
                target: startingPage
                y: -startingPage.height
            }
        }
    ]
}
