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
import QtQuick.Layouts 1.0 as Layouts

import org.kde.kirigami 2.2 as Kirigami
import org.kde.playground.kquickitemviews 1.0 as KQuickItemViews
import org.kde.ringkde.jamitimelinebase 1.0 as JamiTimelineBase

KQuickItemViews.FlickableScrollBar {
    id: scrollbar
    property var    tmlList: null
    property alias  fullWidth: timelineOverlay.width
    property alias  model: timelineOverlay.tlModel
    property bool   bottomUp: false
    property bool   display: true
    property bool   hasContent: tmlList.count > 1
    property alias  hideTimeout: hideTimer.interval
    property color  textColor: Kirigami.Theme.textColor

    property bool overlayVisible: false
    property bool forceOverlay: false

    visible: handleVisible

    SystemPalette {
        id: inactivePalette
        colorGroup: SystemPalette.Inactive
    }

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
        if (!mouseArea.drag.active) {
            handle.y = position
        }
    }

    Timer {
        id: hideTimer
        running: false
        interval: 5000
        repeat: false
    }

    onDisplayChanged: {
        if (!display)
            hideTimer.running = true
    }

    Rectangle {
        id: handle
        radius: 99
        color: inactivePalette.text
        width: parent.width
        height: scrollbar.handleHeight
        visible: scrollbar.display || hideTimer.running || stateGroup.state == "overlay"

        onYChanged: {
            if (!tmlList)
                return

            if (mouseArea.drag.active)
                scrollbar.position = y

            // Keep a reference as this function is racy
            var oldItem = tmlList.currentItem
            var relH    = scrollbar.height - height

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

            Layouts.RowLayout {
                width: parent.width
                anchors.top   : bottomUp ? undefined     : parent.top
                anchors.bottom: bottomUp ? parent.bottom : undefined
                height: 22
                Rectangle {
                    id: mainCircle
                    y: 0
                    width: 16
                    height: 16
                    radius: 999
                    color: Kirigami.Theme.backgroundColor
                    border.width: 1
                    border.color: Kirigami.Theme.textColor

                    Rectangle {
                        id: smallCircle
                        height: 8
                        width: 8
                        radius: 99
                        anchors.centerIn: parent
                        color: Kirigami.Theme.textColor

                        Behavior on color {
                            ColorAnimation {duration: 300}
                        }
                    }

                    Behavior on border.color {
                        ColorAnimation {duration: 300}
                    }
                }
                Text {
                    id: label
                    Layouts.Layout.fillWidth: true
                    text: display
                    color: scrollbar.textColor
                    font.pointSize: Kirigami.Theme.defaultFont.pointSize*1.2
                    Behavior on font.pointSize {
                        NumberAnimation {duration: 150}
                    }
                }
            }

            states: [
                State {
                    name: ""
                    when: !selected
                    PropertyChanges {
                        target:  label
                        font.pointSize: Kirigami.Theme.defaultFont.pointSize*1.2
                    }
                },
                State {
                    name: "selected"
                    when: selected
                    PropertyChanges {
                        target: mainCircle
                        border.color:  "#298223"
                    }
                    PropertyChanges {
                        target: smallCircle
                        color:  "#298223"
                    }
                    PropertyChanges {
                        target:  label
                        font.pointSize: Kirigami.Theme.defaultFont.pointSize*1.6
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
            interactive: false
            Component.onCompleted: {
                scrollbar.tmlList = tmlList

                currentIndex = bottomUp ? count - 1 : 0
                if (currentItem)
                    currentItem.selected = true
            }

            JamiTimelineBase.TimelineDots {
                height: parent.height
                width: 16
                z: -1
                color: scrollbar.textColor
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onEntered: overlayVisible = true
        onExited: overlayVisible = drag.active

        drag.target: handle
        drag.axis: Drag.YAxis
        drag.minimumY: 0
        drag.maximumY: scrollbar.height - handleHeight

        drag.onActiveChanged: {
            overlayVisible = drag.active
        }

        Loader {
            id: timelineOverlay

            property var tlModel: null

            active: active || opacity > 0
            sourceComponent: panelComponent

            width:   150
            x:      -130
            visible: opacity > 0
            opacity: stateGroup.state == "overlay" && hasContent ? 1 : 0

            Behavior on opacity {
                NumberAnimation {duration: 200}
            }
        }
    }

    StateGroup {
        id: stateGroup

        states: [
            State {
                name: "overlay"
                when: scrollbar.overlayVisible || scrollbar.forceOverlay
                PropertyChanges {
                    target:  timelineOverlay
                    height: scrollbar.height
                    x: -150
                }
                PropertyChanges {
                    target:  handle
                    color: Kirigami.Theme.highlightColor
                }
            }
        ]

        transitions: [
            Transition {
                to: "overlay"

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
