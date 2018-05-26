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

Item {
    id:dialPad

    property var buttonsText: Array("1","2","3","4","5","6","7","8","9","*","0","#")
    property var buttonsMap: Array("","abc","def","ghi","jkl","mno","pqrs","tuv","wxyz","","+","","")

    property var mapper: {}

    Item {
        anchors.margins: 5

        //Attributes
        anchors.fill: parent

        //Signals
        signal numbrePressed(string number)

        //Content
        Grid {
            columns: 3
            spacing: 3
            width:   (height/4)*3
            height: parent.height
            anchors.horizontalCenter: parent.horizontalCenter
            Repeater {
                model: 12
                Rectangle {
                    id: key

                    property alias state: stateGroup.state

                    //Attributes
                    width:  Math.min((parent.width-3.333)/3 -1, dialPad.height/4 - 2)
                    height: dialPad.height/4 - 2
                    color:  activePalette.highlight
                    radius: 999
                    border.width: 0
                    border.color: activePalette.highlightedText

                    Behavior on border.width {
                        NumberAnimation { duration: 200 }
                    }

                    Rectangle {
                        id: overlay
                        color: activePalette.highlightedText
                        opacity: 0
                        anchors.fill: parent
                        radius: 999

                        Behavior on opacity {
                            NumberAnimation { duration: 200 }
                        }
                    }

                    //Content
                    Item {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        height: label.height + ascii.height

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            horizontalAlignment: Text.AlignRight
                            text: dialPad.buttonsText[index]
                            color: activePalette.highlightedText
                            font.bold: true
                            id:label
                        } //Text

                        Text {
                            text: dialPad.buttonsMap[index]
                            anchors.top:label.bottom
                            color: activePalette.highlightedText
                            horizontalAlignment: Text.AlignRight
                            anchors.horizontalCenter: parent.horizontalCenter
                            id: ascii
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            var call = CallModel.selectedCall

                            if (!call)
                                return

                            call.appendText(dialPad.buttonsText[index])
                            call.playDTMF(dialPad.buttonsText[index])
                        }
                        onContainsMouseChanged: {
                            if (stateGroup.state != "played")
                                stateGroup.state = containsMouse ? "hover" : "normal"
                        }
                    } //MouseArea

                    Component.onCompleted: {
                        if (dialPad.mapper == undefined)
                            dialPad.mapper = {}
                        dialPad.mapper[dialPad.buttonsText[index]] = key
                    }

                    Timer {
                        id: animTimer
                        running: false
                        interval: 200
                        repeat: false
                        onTriggered: {
                            key.state = "normal"
                        }
                    }

                    StateGroup {
                        id: stateGroup
                        states: [
                            State {
                                name: "normal"
                                PropertyChanges {
                                    target: overlay
                                    opacity: 0
                                }
                                PropertyChanges {
                                    target: key
                                    border.width: 0
                                }
                            },
                            State {
                                name: "played"
                                PropertyChanges {
                                    target: animTimer
                                    running: true
                                }
                                PropertyChanges {
                                    target: overlay
                                    opacity: 0.5
                                }
                                PropertyChanges {
                                    target: key
                                    border.width: 2
                                }
                            },
                            State {
                                name: "hover"
                                PropertyChanges {
                                    target: overlay
                                    opacity: 0.5
                                }
                            }
                        ]
                    }

                } //Rectangle
            } //Repeater
        } //Grid
    }

    Connections {
        target: CallModel
        onDtmfPlayed: {
            dialPad.mapper[code].state = "played"
        }
    }
}
