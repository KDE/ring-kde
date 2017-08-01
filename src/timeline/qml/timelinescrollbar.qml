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
import RingQmlWidgets 1.0

Item {
    id: scrollbar
    property var    tmlList: null
    property alias  fullWidth: timelineOverlay.width
    property alias  handleHeight: timelineOverlay.height
    property double position: 0
    property alias  model: timelineOverlay.tlModel
    property bool   bottomUp: false

    property bool overlayVisible: false

    // This isn't correct at all, but close enough for now
    function getSectionHeight(height, total, section, index, activeCategoryCount) {
        var h = (height-(activeCategoryCount*22)) * (section/total)

        // Align to the closest "hole" in the dotted bar
        var align = h%22
        h += align > 22/2 ? (22 - align) : -align

        // The minimum height (22points) prevents the text from overlapping and
        // allow the list to look unified.
        return 22 + h
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
            if (!tmlList)
                return

            // Keep a reference as this function is racy
            var oldItem = tmlList.currentItem
            var relH    = scrollbar.height - height

            // Move the list
            scrollbar.position = y/relH

            // Highlight the current index
            var point = bottomUp ? y + height : y
            var item  = tmlList.itemAt(10, point)

            if ((!item) || item == oldItem)
                return

            tmlList.currentIndex = tmlList.indexAt(10, point)
            item.selected        = true

            if (oldItem != undefined)
                oldItem.selected = false
        }
    }

    Component {
        id: category
        Item {
            width: parent.width
            property bool selected: false
            height: getSectionHeight(scrollbar.height, totalEntries, categoryEntries, index, activeCategories)

            RowLayout {
                width: parent.width
                anchors.top   : bottomUp ? undefined     : parent.top
                anchors.bottom: bottomUp ? parent.bottom : undefined
                height: 22
                Rectangle {
                    id: mainCircle
                    y: 0
                    height: 18
                    width: 18
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

                currentIndex = bottomUp ? count - 1 : 0
                if (currentItem)
                    currentItem.selected = true
            }

            TimelineDots {
                height: parent.height
                width: 18
                z: -1
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
