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
import org.kde.kirigami 2.2 as Kirigami
import QtGraphicalEffects 1.0
import org.kde.ringkde.jamitimeline 1.0 as JamiTimeline
import org.kde.ringkde.jamitimelinebase 1.0 as JamiTimelineBase
import org.kde.ringkde.jamichatview 1.0 as JamiChatView
import org.kde.ringkde.jamicontactview 1.0 as JamiContactView
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import org.kde.playground.kquickitemviews 1.0 as KQuickItemViews

Rectangle {
    id: timelinePage
    signal disableContactRequests()
    property bool showScrollbar: true
    property bool _sendRequestOverride: true
    property var currentContactMethod: null
    property var currentIndividual: null
    property var timelineModel: null

    property bool canSendTexts: currentIndividual ? currentIndividual.canSendTexts : false

    property bool sendRequest: _sendRequestOverride && (
        sendRequestLoader.active && sendRequestLoader.item && sendRequestLoader.item.sendRequests
    )

    onDisableContactRequests: {
        if (timelinePage.setContactMethod())
            currentContactMethod.confirmationEnabled = false

        timelinePage._sendRequestOverride = send
    }

    Kirigami.Theme.colorSet: Kirigami.Theme.View

    function focusEdit() {
        chatBox.focusEdit()
    }

    function showNewContent() {
        chatView.moveTo(Qt.BottomEdge)
    }

    function setContactMethod() {
        if (currentIndividual && !currentContactMethod) {
            currentContactMethod = currentIndividual.preferredContactMethod(
                RingQtQuick.Media.TEXT
            )

            if (!currentContactMethod)
                console.log("Cannot find a valid ContactMethod for", currentIndividual)
        }

        return currentContactMethod
    }

    onCurrentIndividualChanged: {
        currentContactMethod = null
        setContactMethod()
    }

    color: Kirigami.Theme.backgroundColor

    // Scroll to the search, unread messages, bookmark, etc
    RingQtQuick.TimelineIterator {
        id: iterator
        currentIndividual: timelinePage.currentIndividual
        firstVisibleIndex: chatView.topLeft
        lastVisibleIndex: chatView.bottomLeft
        onContentAdded: {
            lastVisibleIndex = chatView.indexAt(Qt.BottomEdge)
            timelinePage.showNewContent()
        }

        onProposeIndex: {
            if (poposedIndex == newestIndex)
                timelinePage.showNewContent()
            else
                chatView.contentY = chatView.itemRect(newestIndex).y
        }
    }

    // Add a blurry background
    ShaderEffectSource {
        id: effectSource
        visible: chatView.displayExtraTime

        sourceItem: chatView
        anchors.right: timelinePage.right
        anchors.top: timelinePage.top
        width: scrollbar.fullWidth + 15
        height: chatView.height

        sourceRect: Qt.rect(
            blurryOverlay.x,
            blurryOverlay.y,
            blurryOverlay.width,
            blurryOverlay.height
        )
    }

    ColumnLayout {
        anchors.fill: parent
        clip: true
        spacing: 0

        Loader {
            id: sendRequestLoader
            height: active && item ? item.implicitHeight : 0
            Layout.fillWidth: true
            active: chatBox.requireContactRequest
            Layout.minimumHeight: active && item ? item.implicitHeight : 0
            Layout.maximumHeight: active && item ? item.implicitHeight : 0
            sourceComponent: JamiContactView.SendRequest {
                width: sendRequestLoader.width
            }
        }

        RowLayout {
            id: chatScrollView
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.bottomMargin: 0

            property bool lock: false

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true

                // Buttons to navigate to relevant content
                JamiChatView.Navigation {
                    timelineIterator: iterator
                    anchors.rightMargin: scrollbar.hasContent ?
                        blurryOverlay.width : 0
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    Behavior on anchors.rightMargin {
                        NumberAnimation {duration: 100; easing.type: Easing.InQuad}
                    }
                }

                JamiChatView.ChatView {
                    id: chatView
                    width: Math.min(600, timelinePage.width - 50)
                    height: parent.height
                    anchors.horizontalCenter: parent.horizontalCenter
                    forceTime: scrollbar.overlayVisible
                }

                // It needs to be here due to z-index conflicts between
                // chatScrollView and timelinePage
                Item {
                    id: blurryOverlay
                    z: 2
                    opacity: chatView.displayExtraTime && scrollbar.hasContent ? 1 : 0
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.rightMargin: - 15
                    height: chatScrollView.height
                    clip: true
                    width: chatView.displayExtraTime ?
                        scrollbar.fullWidth + 15 : 0
                    visible: opacity > 0

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
                        color: Kirigami.Theme.backgroundColor
                        opacity: 0.75
                    }
                }
            }

            JamiTimelineBase.Scrollbar {
                id: scrollbar
                z: 1000
                bottomUp: true
                Layout.fillHeight: true
                Layout.preferredWidth: 10
                display: chatView.moving || timelinePage.showScrollbar
                model: chatView.model
                view: chatView
                forceOverlay: chatView.displayExtraTime
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
        }

        JamiChatView.ChatBox {
            id: chatBox
            Layout.fillWidth: true
            visible: canSendTexts
            RingQtQuick.MessageBuilder {id: builder}
            requireContactRequest: currentContactMethod &&
                currentContactMethod.confirmationStatus == RingQtQuick.ContactMethod.UNCONFIRMED &&
                currentContactMethod.confirmationStatus != RingQtQuick.ContactMethod.DISABLED
        }
    }

    Connections {
        target: chatBox
        onSendMessage: {
            timelinePage.setContactMethod()
            if (currentContactMethod) {
                if (currentContactMethod.account && currentContactMethod.confirmationStatus == RingQtQuick.ContactMethod.UNCONFIRMED)
                    currentContactMethod.sendContactRequest()

                builder.addPayload("text/plain", message)
                builder.sendWidth(currentContactMethod)
            }
        }
    }
}
