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
import QtQuick 2.7
import QtQuick.Layouts 1.0
import Ring 1.0
import org.kde.kirigami 2.2 as Kirigami

Item {
    property QtObject call: null
    property bool displayLabels: true
    property color outlineColor: "#dddddd"
    property color backgroundColor: "black"

    width: 200
    height: displayLabels ? 50 : 20

    Component {
        id: circle
        Rectangle {
            border.width: 1
            border.color: outlineColor
            color: backgroundColor
            radius: 99
            height: 20
            width: 20


            Rectangle {
                color: circleColor
                radius: 99

                height: 10
                width: 10

                Behavior on color {
                    ColorAnimation {duration: 300}
                }

                anchors.centerIn: parent
            }

            Loader {
                active: displayLabels
                sourceComponent: Text {
                    color: "white"
                    text: label
                    font.pointSize: fontSize
                    transformOrigin: Item.Left
                    y : -Math.sin(Math.PI/4)*implicitWidth - parent.height
                    x : -(Math.cos(Math.PI/4)*implicitWidth) / 2 - parent.height/2

                    Behavior on font.pointSize {
                        NumberAnimation {duration: 150}
                    }

                    transform: Rotation {
                        origin.x: 0
                        origin.y: height/2
                        angle: 45
                    }
                }
            }
        }
    }

    Rectangle {
        id: backgroundBar
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width
        height: 2
        radius: 99
        color: outlineColor
    }

    RowLayout {
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width
        Item {
            Layout.fillWidth: true
        }
        Loader {
            id: initCircle
            property string label: i18n("Dialing")
            property string circleColor: outlineColor
            property real fontSize: Kirigami.Theme.defaultFont.pointSize*1.2
            sourceComponent: circle
            Layout.maximumWidth: 20
            Layout.maximumHeight: 20
        }
        Item {
            Layout.fillWidth: true
        }
        Loader {
            id: searchCircle
            property string label: i18n("Searching")
            property string circleColor: outlineColor
            property real fontSize: Kirigami.Theme.defaultFont.pointSize*1.2
            sourceComponent: circle
            Layout.maximumWidth: 20
            Layout.maximumHeight: 20
        }
        Item {
            Layout.fillWidth: true
        }
        Loader {
            id: ringCircle
            property string label: i18n("Ringing")
            property string circleColor: outlineColor
            property real fontSize: Kirigami.Theme.defaultFont.pointSize*1.2
            sourceComponent: circle
            Layout.maximumWidth: 20
            Layout.maximumHeight: 20
        }
        Item {
            Layout.fillWidth: true
        }
        Loader {
            property string label: i18n("Starting")
            property string circleColor: outlineColor
            property real fontSize: Kirigami.Theme.defaultFont.pointSize*1.2
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
                name: ""
                PropertyChanges {
                    target: initCircle
                    fontSize: Kirigami.Theme.defaultFont.pointSize*1.2
                }
                PropertyChanges {
                    target: searchCircle
                    fontSize: Kirigami.Theme.defaultFont.pointSize*1.2
                }
                PropertyChanges {
                    target: ringCircle
                    fontSize: Kirigami.Theme.defaultFont.pointSize*1.2
                }
            },
            State {
                name: "INITIALIZATION"
                when: call == null ||  call.state == 14
                PropertyChanges {
                    target: initCircle
                    circleColor: "#298223"
                    fontSize: Kirigami.Theme.defaultFont.pointSize*1.6
                }
            },
            State {
                name: "CONNECTED"
                when: call && call.state == Call.CONNECTED
                extend: "INITIALIZATION"
                PropertyChanges {
                    target: searchCircle
                    circleColor: "#298223"
                    fontSize: Kirigami.Theme.defaultFont.pointSize*1.6
                }
                PropertyChanges {
                    target: initCircle
                    fontSize: Kirigami.Theme.defaultFont.pointSize*1.2
                }
            },
            State {
                name: "RINGING"
                extend: "CONNECTED"
                when: call && (call.state == Call.RINGING || call.state == 1 /*Call.INCOMING*/)
                PropertyChanges {
                    target: ringCircle
                    circleColor: "#298223"
                    fontSize: Kirigami.Theme.defaultFont.pointSize*1.6
                }
                PropertyChanges {
                    target: initCircle
                    fontSize: Kirigami.Theme.defaultFont.pointSize*1.2
                }
                PropertyChanges {
                    target: searchCircle
                    fontSize: Kirigami.Theme.defaultFont.pointSize*1.2
                }
            }
        ]
    }
}
