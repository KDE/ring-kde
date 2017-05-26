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
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Ring 1.0

Item {
    id: scrollbar
    property alias model: tmlList.model

    property bool overlayVisible: false

    // This isn't correct at all, but close enough for now
    function getSectionHeight(height, total, section, index, activeCategoryCount) {
        var h = (height-(activeCategoryCount*20)) * (section/total)
        return 20 + h
    }

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        id: handle
        radius: 99
        color: "black"
        width: parent.width
        height: 65
    }

    Component {
        id: category
        Item {
            height: getSectionHeight(parent.parent.height, totalEntries, categoryEntries, index, activeCategories)
            ColumnLayout {
                anchors.fill: parent

                RowLayout {
                    Layout.fillWidth: true
                    Rectangle {
                        height: 20
                        width: 20
                        radius: 999
                        color: "#005500"
                        border.width: 2
                        border.color: "#d0d0d0"
                    }
                    Text {
                        Layout.fillWidth: true
                        text: display
                        color: "white"
                    }
                }

                Repeater { //FIXME very, very slow
                    model: Math.floor(parent.height / 10) - 2
                    clip: true
                    Layout.fillHeight: true
                    delegate: Item {
                        height: 10
                        width: 20
                        Rectangle {
                            anchors.centerIn: parent
                            height: 7.5
                            width: 7.5
                            color: "#005500"
                            radius: 8
                            border.width: 1
                            border.color: "#d0d0d0"
                        }
                    }
                }
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: overlayVisible = true
        onExited: overlayVisible = false

        Item {
            id: timelineOverlay
            width: 150
            x: -130
            height: scrollbar.height
            visible: false
            opacity: 0
            ListView {
                id: tmlList
                anchors.fill: parent
                delegate: category
            }
        }
    }

    StateGroup {
        states: [
            State {
                name: "overlay"
                when: scrollbar.overlayVisible
                PropertyChanges {
                    target:  timelineOverlay
                    height: scrollbar.height
                    visible: true
                    opacity: 1
                    x: -150
                }
                PropertyChanges {
                    target:  handle
                    color: activePalette.highlight
                }
            }
        ]

        transitions: [
            Transition {
                to: "overlay"
                NumberAnimation {
                    properties: "opacity"
                    target: timelineOverlay
                    duration: 200
                }
                NumberAnimation {
                    properties: "x"
                    target: timelineOverlay
                    easing.type: Easing.InQuad
                    duration: 200
                }
                ColorAnimation {
                    target: handle
                    duration: 100
                }
            }
        ]
    }
}
