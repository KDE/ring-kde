/***************************************************************************
 *   Copyright (C) 2017-2018 by Bluesystems                                *
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
import QtQuick 2.0
import Ring 1.0
import org.kde.kirigami 2.2 as Kirigami

Item {
    id: button

    property string label: ""
    property real expandedHeight: height
    property real radius: Math.min(48, button.height) / 2
    property real topPadding: (button.height - (radius*2)) / 2
    property real sideMargin: 10

    signal clicked()

    Rectangle {
        id: addCallButton
        clip: true
        anchors.horizontalCenter: parent.horizontalCenter
        visible: false
        y: button.topPadding
        height: button.radius*2
        width: button.radius*2
        radius: button.radius
        color: "transparent"
        border.width: 1
        border.color: activePalette.text
        opacity: 0

        Rectangle {
            id: background
            anchors.fill: parent
            color: activePalette.text
            opacity: 0

            Behavior on opacity {
                NumberAnimation {duration: 300;  easing.type: Easing.OutQuad }
            }
        }

        Behavior on width {
            NumberAnimation {duration: 200;  easing.type: Easing.OutQuad }
        }

        Behavior on height {
            NumberAnimation {duration: 200;  easing.type: Easing.OutQuad }
        }

        Behavior on radius {
            NumberAnimation {duration: 100;  easing.type: Easing.OutQuad }
        }

        Behavior on opacity {
            NumberAnimation {duration: 200;  easing.type: Easing.OutQuad }
        }

        Item {
            width: button.radius*2
            height: button.radius*2
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            Rectangle {
                width: 1
                height: button.radius
                anchors.centerIn: parent
                color: activePalette.text
            }
            Rectangle {
                height: 1
                width: button.radius
                anchors.centerIn: parent
                color: activePalette.text
            }
        }

        Text {
            id: label
            opacity: 0
            color: activePalette.text
            text: button.label
            font.pointSize: Kirigami.Theme.defaultFont.pointSize*1.6
            Behavior on opacity {
                NumberAnimation {duration: 200;  easing.type: Easing.OutQuad }
            }
            anchors.centerIn: parent
        }

        MouseArea {
            id: mouseGrabber
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                button.clicked()
            }
        }

        StateGroup {
            id: stateGroup
            states: [
                State {
                    name: "hover"
                    when: mouseGrabber.containsMouse
                    extend: "active"
                    PropertyChanges {
                        target: addCallButton
                        opacity: 0.8
                        radius: 5
                        height: button.expandedHeight
                        width: addCallButton.parent.width - 2*button.sideMargin
                    }
                    PropertyChanges {
                        target: background
                        opacity: 0.1
                    }
                    PropertyChanges {
                        target: label
                        opacity: 0.8
                    }
                    PropertyChanges {
                        target: button
                        height: button.expandedHeight
                    }
                },
                State {
                    name: "active"
                    when: button.visible
                    PropertyChanges {
                        target: addCallButton
                        visible: true
                        opacity: 0.5
                        radius: 99
                        width: button.radius*2
                    }
                    PropertyChanges {
                        target: background
                        opacity: 0
                    }
                    PropertyChanges {
                        target: label
                        opacity: 0
                    }
                }
            ]
        }
    }
}
