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
// import RingQmlWidgets 1.0
import ContactView 1.0

import RingQmlWidgets 1.0
import PhotoSelectorPlugin 1.0


Item {
    id: button

    property color color: undefined
    property string label: ""
    property real expandedHeight: height
    property real radius: Math.min(48, button.height) / 2
    property real topPadding: (button.height - (radius*2)) / 2
    property real sideMargin: 10
    property var alignment: Qt.AlignHCenter
    property alias icon: icn.source

    implicitWidth: height
    width: implicitWidth

    signal clicked()

    Rectangle {
        id: addCallButton
        clip: true

        /**
         * Handle both when there is a single button + alignment and when it is
         * part of a group.
         */
        anchors.horizontalCenter: button.alignment == Qt.AlignHCenter ?
            parent.horizontalCenter : undefined
        anchors.right: button.alignment == Qt.AlignRight ?
            parent.right : undefined
        anchors.left: button.alignment == Qt.AlignLeft ?
            parent.left : undefined

        visible: false
        y: button.topPadding
        height: button.radius*2
        width: button.radius*2
        radius: button.radius
        color: "transparent"
        border.width: 1
        border.color: button.color ? button.color : activePalette.text
        opacity: 0

        Rectangle {
            id: background
            anchors.fill: parent
            color: button.color ? button.color : activePalette.text
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
            id: icon
            width: button.radius*2
            height: button.radius*2
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            Rectangle {
                visible: icn.source == ""
                width: 1
                height: button.radius
                anchors.centerIn: parent
                color: button.color ? button.color : activePalette.text
            }
            Rectangle {
                visible: icn.source == ""
                height: 1
                width: button.radius
                anchors.centerIn: parent
                color: button.color ? button.color : activePalette.text
            }
            Image {
                id: icn
                width: 16//Math.sqrt(2*parent.width) // the largest square fitting in a circle
                height: width
                sourceSize.width: width
                sourceSize.height: width
                anchors.centerIn: parent
            }
        }

        Text {
            id: label
            opacity: 0
            color: button.color ? button.color : activePalette.text
            text: button.label
            font.pointSize: Kirigami.Theme.defaultFont.pointSize*1.6
            Behavior on opacity {
                NumberAnimation {duration: 200;  easing.type: Easing.OutQuad }
            }

            // Avoid overlapping the icon when the width isn't large enough
            anchors.centerIn: implicitWidth > (button.width) ?
                undefined : parent
            anchors.right: implicitWidth > (button.width) ?
                addCallButton.right : undefined
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
                        implicitWidth: label.implicitWidth + 2*height + 2
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
                    PropertyChanges {
                        target: button
                        implicitWidth: height
                    }
                }
            ]
        }
    }
}
