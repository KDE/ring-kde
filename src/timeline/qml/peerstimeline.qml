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
            model: PeersTimelineModel
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

}
