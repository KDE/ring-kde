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
    property var    tmlList: null
    property alias  fullWidth: timelineOverlay.width
    property alias  handleHeight: timelineOverlay.height
    property double position: 0
    property alias  model: timelineOverlay.tlModel

    property bool overlayVisible: false

    // This isn't correct at all, but close enough for now
    function getSectionHeight(height, total, section, index, activeCategoryCount) {
        var h = (height-(activeCategoryCount*20)) * (section/total)
        return 20 + h
    }

    onPositionChanged: {
        var curPos = handle.y/(scrollbar.height - handle.height)
        var newY   = (scrollbar.height - handle.height)*position

        // Prevent infinite loops
        if (Math.abs(curPos - position) < 0.01)
            return

        handle.y = newY
    }

    SystemPalette {
        id: inactivePalette
        colorGroup: SystemPalette.Disabled
    }

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        id: handle
        radius: 99
        color: inactivePalette.text
        width: parent.width
        height: 65

        onYChanged: {
            var relH = scrollbar.height - height
            scrollbar.position = y/relH

            if (!tmlList)
                return

            // Highlight the current index
            var oldItem          = tmlList.currentItem
            tmlList.currentIndex = tmlList.indexAt(10, y)
            var item             = tmlList.currentItem

            if (oldItem != item) {
                oldItem.selected = false
                item.selected    = true
            }
        }
    }

    Component {
        id: category
        Item {
            width: parent.width
            property bool selected: false
            height: getSectionHeight(scrollbar.height, totalEntries, categoryEntries, index, activeCategories)
            ColumnLayout {
                anchors.fill: parent

                RowLayout {
                    Layout.fillWidth: true
                    Rectangle {
                        id: mainCircle
                        height: 20
                        width: 20
                        radius: 999
                        color: "#005500"
                        border.width: 2
                        border.color: "#d0d0d0"
                        Behavior on color {
                            ColorAnimation {duration: 300}
                        }
                    }
                    Text {
                        id: label
                        Layout.fillWidth: true
                        text: display
                        color: "white"
                        Behavior on font.pointSize {
                            NumberAnimation {duration: 150}
                        }
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

            states: [
                State {
                    name: "selected"
                    when: selected
                    PropertyChanges {
                        target: mainCircle
                        color:  "#00AA00"
                    }
                    PropertyChanges {
                        target:  label
                        font.pointSize: 14
                    }
                }
            ]
        }
    }

    Component {
        id: panelComponent
        ListView {
            id: tmlList
            anchors.fill: parent
            delegate: category
            model: tlModel
            Component.onCompleted: {
                scrollbar.tmlList = tmlList

                currentIndex = 0
                if (currentItem)
                    currentItem.selected = true
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: overlayVisible = true
        onExited: overlayVisible = drag.active

        drag.target: handle
        drag.axis: Drag.YAxis
        drag.minimumY: 0
        drag.maximumY: scrollbar.height - handle.height

        drag.onActiveChanged: {
            overlayVisible = drag.active
        }

        Loader {
            id: timelineOverlay

            property var tlModel: null

            active: false
            sourceComponent: panelComponent

            width:   150
            x:      -130
            visible: false
            opacity: 0
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

        // Load the timeline only once
        onStateChanged: {
            if (state == "overlay")
                timelineOverlay.active = true
        }
    }
}
