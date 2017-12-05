/******************************************************************************
 *   Copyright (C) 2012 by Savoir-Faire Linux                                 *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>   *
 *                                                                            *
 *   This library is free software; you can redistribute it and/or            *
 *   modify it under the terms of the GNU Lesser General Public               *
 *   License as published by the Free Software Foundation; either             *
 *   version 2.1 of the License, or (at your option) any later version.       *
 *                                                                            *
 *   This library is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU        *
 *   Lesser General Public License for more details.                          *
 *                                                                            *
 *   You should have received a copy of the Lesser GNU General Public License *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *****************************************************************************/
import QtQuick 2.0
import Ring 1.0

Item {
    id: mainArea
    height: 58
    width: parent.width
    Rectangle {
        id: addCallButton
        clip: true
        anchors.horizontalCenter: parent.horizontalCenter
        visible: false
        y: 15
        height: 48
        width: 48
        radius: 99
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
            width: 48
            height: 48
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            Rectangle {
                width: 1
                height: 24
                anchors.centerIn: parent
                color: activePalette.text
            }
            Rectangle {
                height: 1
                width: 24
                anchors.centerIn: parent
                color: activePalette.text
            }
        }

        Text {
            id: label
            opacity: 0
            color: activePalette.text
            text: i18n("Create a call")
            font.pointSize: 14
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
                CallModel.selectDialingCall()
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
                        height: 64
                        width: addCallButton.parent.width - 20
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
                        target: mainArea
                        height: 64
                    }
                },
                State {
                    name: "active"
                    when: !CallModel.hasDialingCall
                    PropertyChanges {
                        target: addCallButton
                        visible: true
                        opacity: 0.5
                        radius: 99
                        width: 48
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
