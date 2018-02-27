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
import QtGraphicalEffects 1.0
import Ring 1.0

import RingQmlWidgets 1.0

Rectangle {
    id: timelinePage
    property bool showScrollbar: true

    function focusEdit() {
        chatBox.focusEdit()
    }

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    color: activePalette.base

    property var currentContactMethod: null
    property var timelineModel: null

    // Add a blurry background
    ShaderEffectSource {
        id: effectSource
        visible: false

        sourceItem: chatView
        anchors.right: timelinePage.right
        anchors.top: timelinePage.top
        width: scrollbar.fullWidth + 15
        height: chatView.height

        sourceRect: Qt.rect(
            burryOverlay.x,
            burryOverlay.y,
            burryOverlay.width,
            burryOverlay.height
        )
    }

    ColumnLayout {
        anchors.fill: parent
        clip: true

        RowLayout {
            id: chatScrollView
            Layout.fillHeight: true
            Layout.fillWidth: true

            property bool lock: false

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true

                ChatView {
                    id: chatView
                    anchors.fill: parent
                    model: timelinePage.timelineModel

                    onPercentageChanged: {
                        chatScrollView.lock = true
                        scrollbar.position = percent
                        chatScrollView.lock = false
                    }
                }

                // It needs to be here due to z-index conflicts between
                // chatScrollView and timelinePage
                Item {
                    id: burryOverlay
                    z: 2
                    visible: false
                    opacity: 0
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.rightMargin: - 15
                    width: scrollbar.fullWidth + 15
                    height: chatView.height
                    clip: true

                    Behavior on opacity {
                        NumberAnimation {duration: 300; easing.type: Easing.InQuad}
                    }

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
                        color: activePalette.base
                        opacity: 0.75
                    }
                }
            }

            TimelineScrollbar {
                id: scrollbar
                z: 1000
                bottomUp: true
                Layout.fillHeight: true
                Layout.preferredWidth: 10
                display: chatView.moving || timelinePage.showScrollbar
                model: timelinePage.timelineModel

                onWidthChanged: {
                    burryOverlay.width = scrollbar.fullWidth + 15
                }

                onPositionChanged: {
                    if (chatScrollView.lock)
                        return;

                    chatView.contentY = (chatView.contentHeight-chatView.height)*scrollbar.position
                }

                onOverlayVisibleChanged: {
                    burryOverlay.visible = overlayVisible
                    burryOverlay.opacity = overlayVisible ? 1 : 0
                    effectSource.visible = overlayVisible
                }
            }
        }

        ChatBox {
            id: chatBox
            Layout.fillWidth: true
            height: 90
            visible: currentContactMethod ? currentContactMethod.canSendTexts == ContactMethod.AVAILABLE : false
            MessageBuilder {id: builder}

            textColor: activePalette.text
            backgroundColor: activePalette.window
            emojiColor: activePalette.highlight
        }
    }

    Connections {
        target: chatBox
        onSendMessage: {
            builder.addPayload("text/plain", message)
            builder.sendWidth(currentContactMethod)
        }
    }
}
