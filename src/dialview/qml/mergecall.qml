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
import org.kde.kirigami 2.2 as Kirigami

Item {
    id: mainArea
    height: 10
    width: parent.width
    Rectangle {
        id: addCallButton
        clip: true
        anchors.fill: parent
        anchors.margins: 5
        visible: false
        radius: 5
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

        Behavior on height {
            NumberAnimation {duration: 200;  easing.type: Easing.OutQuad }
        }

        Behavior on radius {
            NumberAnimation {duration: 100;  easing.type: Easing.OutQuad }
        }

        Text {
            id: label
            opacity: 0
            color: activePalette.text
            text: i18n("Merge all calls")
            font.pointSize: Kirigami.Theme.defaultFont.pointSize*1.6
            anchors.centerIn: parent
            Behavior on opacity {
                NumberAnimation {duration: 200;  easing.type: Easing.OutQuad }
            }
        }

        MouseArea {
            id: mouseGrabber
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                if (CallModel.hasConference)
                    CallModel.detachAllCalls()
                else
                    CallModel.mergeAllCalls()
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
                    }
                    PropertyChanges {
                        target: background
                        opacity: 0.1
                    }
                },
                State {
                    name: "active"
                    when: CallModel.conferencePossible || CallModel.hasConference
                    PropertyChanges {
                        target: addCallButton
                        visible: true
                        opacity: 0.5
                        height: 48
                    }
                    PropertyChanges {
                        target: mainArea
                        height: 48
                    }
                    PropertyChanges {
                        target: label
                        opacity: 0.8
                    }
                },
                State {
                    name: "hasconference"
                    extend: "active"
                    when: CallModel.hasConference
                    PropertyChanges {
                        target: label
                        text: i18n("Detach all calls")
                    }
                    PropertyChanges {
                        target: background
                        opacity: 1
                    }
                }
            ]
        }
    }
}
