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
import QtQuick.Controls 1.4
import QtQuick.Controls 2.0 as Controls2
import Ring 1.0
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0

import RingQmlWidgets 1.0

Rectangle {
    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    SystemPalette {
        id: inactivePalette
        colorGroup: SystemPalette.Disabled
    }

    color: activePalette.base
    id: recentDock
    visible: true

    signal contactMethodSelected(var cm)

    Component {
        id: contactDelegate
        ContactMethodDelegate {}
    }

    // To allow multiple places to set the contact method without knowing
    // it's index, use a selectionModel
    PeersTimelineSelectionModel {
        id: selectionMapper
        onCurrentIndexChanged: {
            recentView.currentIndex = current.row
        }
    }

    onContactMethodSelected: {
        search.text = ""
        selectionMapper.contactMethod = cm
    }

    ColumnLayout {
        anchors.fill: parent

        Controls2.TextField {
            id: search
            Layout.fillWidth: true
            placeholderText: "Find someone"
            onTextChanged: {
                var call = CallModel.dialingCall()
                call.dialNumber = search.text
                CompletionModel.call = call
            }
            Keys.onDownPressed: {
                searchView.currentIndex = (searchView.currentIndex == searchView.count - 1) ?
                    0 : searchView.currentIndex + 1
            }
            Keys.onUpPressed: {
                searchView.currentIndex = (searchView.currentIndex == 0) ?
                    searchView.count - 1 : searchView.currentIndex - 1
            }
            Keys.onReturnPressed: {
                var cm = searchView.currentItem.contactMethod

                if (!cm)
                    return

                contactMethodSelected(cm)
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ScrollView {
                id: scrollView
                anchors.fill: parent
                flickableItem.interactive: true

                ListView {
                    id: recentView
                    anchors.fill: parent
                    highlightMoveVelocity: Infinity //HACK
                    delegate: contactDelegate
                    highlight: Rectangle {
                        color: activePalette.highlight
                    }
                    model: PeersTimelineModel.deduplicatedTimelineModel
                }
            }

            TimelineScrollbar {
                id: scrollBar
                height: parent.height
                anchors.top: parent.top
                anchors.right: parent.right
                width: 10
                model: PeersTimelineModel.timelineSummaryModel
                z: 100
            }

            // Add a blurry background
            ShaderEffectSource {
                id: effectSource
                visible: false

                sourceItem: scrollView
                anchors.right: scrollView.right
                anchors.top: scrollView.top
                width: scrollView.width/2
                height: scrollView.height

                sourceRect: Qt.rect(scrollView.width/2, 0, scrollView.width/2, scrollView.height)
            }

            Item {
                id: burryOverlay
                visible: false
                opacity: 0
                anchors.right: scrollView.right
                anchors.top: scrollView.top
                width: scrollView.width/2
                height: scrollView.height
                clip: true

                Repeater {
                    anchors.fill: parent
                    model: 5
                    FastBlur {
                        anchors.fill: parent
                        source: effectSource
                        radius: 30

                    }
                }

                Rectangle {
                    anchors.fill: parent
                    color: "black"
                    opacity: 0.75
                }
            }

            FindPeers {
                id: searchView
                visible: false
                anchors.fill: parent
            }
        }
    }

    // Timeline scrollbar
    StateGroup {
        states: [
            State {
                name: "overlay"
                when: scrollBar.overlayVisible
                PropertyChanges {
                    target:  burryOverlay
                    visible: true
                }
                PropertyChanges {
                    target:  burryOverlay
                    opacity: 1
                }
                PropertyChanges {
                    target:  effectSource
                    visible: true
                }
            }
        ]

        transitions: [
            Transition {
                to: "overlay"
                NumberAnimation {
                    properties: "opacity"
                    easing.type: Easing.InQuad
                    duration: 400
                    loops: 1
                }
            }
        ]
    }

    // Search
    StateGroup {
        states: [
            State {
                name: "searchActive"
                when: search.text != ""
                PropertyChanges {
                    target:  scrollBar
                    visible: false
                }
                PropertyChanges {
                    target:  searchView
                    visible: true
                }
                PropertyChanges {
                    target:  burryOverlay
                    visible: true
                    opacity: 1
                    width: scrollView.width
                    height: scrollView.height
                }
            }
        ]

        transitions: [
            Transition {
                to: "searchActive"
                NumberAnimation {
                    properties: "opacity"
                    easing.type: Easing.InQuad
                    duration: 200
                    loops: 1
                }
                NumberAnimation {
                    properties: "width,height"
                    easing.type: Easing.InQuad
                    duration: 0
                    loops: 1
                }
            }
        ]
    }

}
