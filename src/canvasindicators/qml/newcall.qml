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
import QtQuick.Layouts 1.0

ColumnLayout {
    width: 300
    spacing: 10

    Component {
        id: button
        Rectangle {
            id: bg
            height: 30
            color: "#222222"
            border.color: "#333333"
            border.width: 1
            radius: 5
            visible: currentIndividual
            Text {
                text: label
                anchors.centerIn: parent
                color: "white"
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onContainsMouseChanged: {
                    bg.color = containsMouse ? "#444444" : "#222222"
                }
                onClicked: {
                    callWithAudio()
                }
            }

            Behavior on color {
                ColorAnimation {duration: 100}
            }
        }
    }

    Loader {
        width: 300
        Layout.fillWidth: true
        property string label: "Start a video call"
        visible: availabilityTracker.canVideoCall
        sourceComponent: button
    }
    Loader {
        width: 300
        Layout.fillWidth: true
        property string label: "Start an audio call"
        visible: availabilityTracker.canCall
        sourceComponent: button
    }
    Loader {
        width: 300
        Layout.fillWidth: true
        property string label: "Start screen sharing"
        visible: availabilityTracker.canVideoCall
        sourceComponent: button
    }
}
