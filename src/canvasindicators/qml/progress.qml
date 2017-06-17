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
import Ring 1.0

Item {
    property QtObject call: null

    width: 200
    height: 50

    Component {
        id: circle
        Rectangle {
            border.width: 1
            border.color: "white"
            color: circleColor
            radius: 99
            height: 20
            width: 20
            Text {
                anchors.right: parent.horizontalCenter
                color: "white"
                text: label
                transformOrigin: Item.Left
                transform: Rotation {
                    origin.x: 0
                    origin.y: height/2
                    angle: 45
                }
                Component.onCompleted: {
                    // 13: parent.height/2 + 3pt margin
                    y = -Math.sin(Math.PI/4)*width - 13
                }
            }
        }
    }

    Rectangle {
        id: backgroundBar
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width
        height: 7
        radius: 99
        border.width: 1
        border.color: "white"
        color: "gray"
    }

    Rectangle {
        id: progress
        anchors.verticalCenter: parent.verticalCenter
        width: 0
        height: 7
        radius: 99
        border.width: 1
        border.color: "white"
        color: "green"
    }

    RowLayout {
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width
        Item {
            Layout.fillWidth: true
        }
        Loader {
            id: initCircle
            property string label: "Dialing"
            property string circleColor: "green"
            sourceComponent: circle
            Layout.maximumWidth: 20
            Layout.maximumHeight: 20
        }
        Item {
            Layout.fillWidth: true
        }
        Loader {
            id: searchCircle
            property string label: "Searching"
            property string circleColor: "green"
            sourceComponent: circle
            Layout.maximumWidth: 20
            Layout.maximumHeight: 20
        }
        Item {
            Layout.fillWidth: true
        }
        Loader {
            id: ringCircle
            property string label: "Ringing"
            property string circleColor: "green"
            sourceComponent: circle
            Layout.maximumWidth: 20
            Layout.maximumHeight: 20
        }
        Item {
            Layout.fillWidth: true
        }
        Loader {
            property string label: "Starting"
            property string circleColor: "green"
            sourceComponent: circle
            Layout.maximumWidth: 20
            Layout.maximumHeight: 20
        }
        Item {
            Layout.fillWidth: true
        }
    }

    property var counter: 0

    onCallChanged: {
        if (call && (call.state == Call.RINGING || call.state == 1 /*Call.INCOMING*/))
            callStateGroup.state = "RINGING"
        else if (call && call.state == Call.CONNECTED)
            callStateGroup.state = "CONNECTED"
        else
            callStateGroup.state = "INITIALIZATION"
    }

    StateGroup {
        id: callStateGroup

        states: [
            State {
                name: "INITIALIZATION"
                when: call == null ||  call.state == 14
                PropertyChanges {
                    target: progress
                    width: backgroundBar.width * (0/3)
                }
                PropertyChanges {
                    target: initCircle
                    circleColor: "red"
                }
            },
            State {
                name: "CONNECTED"
                when: call && call.state == Call.CONNECTED
                extend: "INITIALIZATION"
                PropertyChanges {
                    target: progress
                    width: backgroundBar.width * (1/3)
                }
                PropertyChanges {
                    target: searchCircle
                    circleColor: "red"
                }
            },
            State {
                name: "RINGING"
                extend: "CONNECTED"
                when: call && (call.state == Call.RINGING || call.state == 1 /*Call.INCOMING*/)
                PropertyChanges {
                    target: progress
                    width: backgroundBar.width * (2/3)
                }
                PropertyChanges {
                    target: ringCircle
                    circleColor: "red"
                }
            }
        ]
    }
}
