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
import QtQuick.Controls 2.2

import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.2 as Kirigami
import org.kde.playground.kquickitemviews 1.0 as KQuickItemViews
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import org.kde.ringkde.jamitimeline 1.0 as JamiTimeline
import org.kde.ringkde.jamicontactview 1.0 as JamiContactView
import org.kde.ringkde.jamitroubleshooting 1.0 as JamiTroubleShooting
import org.kde.ringkde.jamitimelinebase 1.0 as JamiTimelineBase

Rectangle {
    color: activePalette.base
    id: recentDock
    visible: true

    property alias overlayVisible: scrollBar.overlayVisible
    signal contactMethodSelected(var cm)
    signal individualSelected(var ind)
    signal contextMenuRequested(var cm, int index)

    function setCurrentIndex(index) {
        recentView.setCurrentIndex(index)
    }

    FontMetrics {
        id: fontMetrics
    }

    Component {
        id: contactDelegate
        JamiTimeline.ContactMethodDelegate {}
    }

    Component {
        id: sectionDelegate
        JamiTimeline.PeersTimelineCategories {}
    }

    Item {
        anchors.fill: parent

        Text {
            visible: recentView.empty
            anchors.centerIn: parent
            width: parent.width
            anchors.margins: 10
            wrapMode: Text.WordWrap
            color: activePalette.text
            text: i18n("To begin using Ring-KDE, enter an username in the box above and press enter")
        }

        JamiTroubleShooting.GlobalTroubleshoot {
            id: accountError
            active: displayActionHeader

            width: parent.width -20 - scrollBar.width
            x: 10
            y: 10
            height: item ? item.implicitHeight : 0
        }

        KQuickItemViews.ListView {
            id: recentView
            clip: true
            anchors.fill: parent
            anchors.topMargin: (!accountError.active) ? 0 : accountError.height + 20
            delegate: contactDelegate
            section.delegate: sectionDelegate
            section.property: "formattedLastUsed" // indexedLastUsed
            section.model: RingSession.peersTimelineModel.timelineSummaryModel

            highlight: Item {

                anchors.topMargin: 5
                anchors.bottomMargin: 5
                anchors.leftMargin: 30
                anchors.rightMargin: 40
                Rectangle {
                    anchors.fill: parent
                    anchors.topMargin: 5
                    anchors.bottomMargin: 5
                    anchors.leftMargin: 30
                    anchors.rightMargin: 40
                    radius: 10
                    color: activePalette.highlight
                }
            }

            model: KQuickItemViews.SizeHintProxyModel {
                id: proxyModel

                /*invalidationRoles: [
                    "object",
                    "unreadTextMessageCount",
                    "isRecording",
                    "hasActiveVideo",
                ]*/

                constants: ({
                    fmh: fontMetrics.height,
                    fmh2: fontMetrics.height,
                })

                function getRowCount(obj) {
                    var activeCM = obj ? obj.activeContactMethod : null

                    return 2 + ((obj != null) && (obj.hasActiveCall
                        || obj.unreadTextMessageCount > 0
                        || (activeCM && activeCM.isRecording)
                        || (activeCM && activeCM.hasActiveVideo)
                    ) ? 1 : 0)
                }

                widthHint: recentView.width
                heightHint: (proxyModel.getRowCount(object)*2+1)*fmh + 13

                sourceModel: RingSession.peersTimelineModel
            }
        }

        JamiTimelineBase.Scrollbar {
            id: scrollBar
            height: parent.height
            anchors.top: parent.top
            anchors.right: parent.right
            width: 10
            model: RingSession.peersTimelineModel.timelineSummaryModel
            z: 100
            display: recentView.moving || recentDock.state == ""
            view: recentView

            onWidthChanged: {
                burryOverlay.width = scrollBar.fullWidth + 15
            }
        }

        // Add a blurry background
        ShaderEffectSource {
            id: effectSource
            visible: false

            sourceItem: recentView
            anchors.right: recentView.right
            anchors.top: recentView.top
            width: scrollBar.fullWidth + 15
            height: recentView.height

            sourceRect: Qt.rect(
                parent.width - scrollBar.fullWidth - 15,
                0,
                scrollBar.fullWidth + 15,
                recentView.height
            )
        }

        Item {
            id: burryOverlay
            visible: false
            opacity: 0
            anchors.right: recentView.right
            anchors.top: recentView.top
            width: scrollBar.fullWidth + 15
            height: recentView.height
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
                color: inactivePalette.highlight
                opacity: 0.75
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

    states: [
        State {
            name: ""
        },
        State {
            name: "mobile"

        }
    ]

}
