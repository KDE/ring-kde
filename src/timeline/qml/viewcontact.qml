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
import org.kde.kirigami 2.2 as Kirigami

Item {
    id: viewContact
    property var currentContactMethod: null
    property var currentPerson: null
    property string currentPage: ""
    property var contactHeader: null
    property bool mobile: false

    function showVideo() {
        avView.active = true
        if (state == "mobile")
            stackView.push(avPage)
        else
            tabBar.currentIndex = 1
    }

    clip: true

    onCurrentContactMethodChanged: {
        contactHeader.currentContactMethod = currentContactMethod
        contactInfo.currentContactMethod   = currentContactMethod
        callHistory.currentContactMethod   = currentContactMethod
        timelinePage.currentContactMethod  = currentContactMethod

        if (currentContactMethod && currentContactMethod.person)
            personConn.target = currentContactMethod.person

        var call = null

        // Check if the CM already has an active call, switch to it
        for (var i=0; i<CallModel.size; i++) {
            call = CallModel.getCall(CallModel.index(i, 0))
            if (call && call.peerContactMethod == currentContactMethod && call.lifeCycleState != Call.FINISHED)
                break
        }

        avView.call = call
    }

    onCurrentPageChanged: {
        if (currentPage == "")
            return

        switch(currentPage) {
            case "INFORMATION":
                tabBar.currentIndex = 0
                break
            case "MEDIA":
                tabBar.currentIndex = 1
                break
            case "TIMELINE":
                tabBar.currentIndex = 2
                break
            case "CALL_HISTORY":
                tabBar.currentIndex = 3
                break
        }

        currentPage = ""
    }

    Connections {
        target: currentContactMethod
        onContactChanged: {
            if (currentContactMethod.person)
                personConn.target = currentContactMethod.person
        }
    }

    Connections {
        target: currentContactMethod
        onCallAdded: {
            if (currentContactMethod.person)
                return

            avView.call = call && call.lifeCycleState != Call.FINISHED ? call : null
        }
    }

    Connections {
        id: personConn
        target: currentContactMethod ? currentContactMethod.person : null
        onCallAdded: {
            avView.call = call
        }
    }

    Timer {
        repeat: false
        running: true
        interval: 500
        onTriggered: {
            if (PeersTimelineModel.rowCount() > 0) {
                firstRun.visible = false
            }
        }
    }

    FirstRun {
        id: firstRun
        anchors.fill: parent
        z: 9999
        color: activePalette.base
        visible: currentContactMethod == null || currentContactMethod.isSelf
    }

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent

        TabBar {
            Layout.fillWidth: true
            id: tabBar
            currentIndex: swipeView.currentIndex
            TabButton {
                text: i18n("Information")
            }
            TabButton {
                text: i18n("Audio/Video")
            }
            TabButton {
                text: i18n("Chat/Timeline")
            }
            TabButton {
                text: i18n("Calls/Recordings")
            }

            onCurrentIndexChanged: {
                //TODO deactivate it after a minute in other tabs
                if (currentIndex == 2) {
                    timelinePage.active = true
                }
                if (currentIndex == 3) {
                    callHistory.active = true
                }
            }
        }

        SwipeView {
            id: swipeView
            Layout.fillHeight: true
            Layout.fillWidth: true

            background: Rectangle {
                color: activePalette.base
            }

            currentIndex: tabBar.currentIndex

            Page {
                id: contactInfoPage
                background: Rectangle { color: activePalette.base }
                ContactInfo {
                    id: contactInfo
                    anchors.fill: parent

                    onSelectChat: {
                        stackView.push(page3)
                    }

                    onSelectHistory: {
                        stackView.push(page2)
                    }
                }
            }

            Page {
                id: avPage
                background: Rectangle { color: activePalette.base }
                Loader {
                    id: avView
                    asynchronous: true
                    active: false
                    anchors.fill: parent

                    property var call: null

                    // QML bug?
                    onCallChanged: {
                        if (active)
                            callViewWidget.call = call
                    }

                    CallView {
                        id: callViewWidget
                        mode: "CONVERSATION"
                        anchors.fill: parent
                        call: avView.call
                        onCallWithAudio: {
                            if (currentContactMethod.hasInitCall) {
                                contactHeader.selectVideo()
                                return
                            }

                            CallModel.dialingCall(currentContactMethod)
                                .performAction(Call.ACCEPT)
                        }
                        onCallWithVideo: {
                            if (currentContactMethod.hasInitCall) {
                                contactHeader.selectVideo()
                                return
                            }

                            CallModel.dialingCall(currentContactMethod)
                                .performAction(Call.ACCEPT)
                        }
                        onCallWithScreen: {
                            if (currentContactMethod.hasInitCall) {
                                contactHeader.selectVideo()
                                return
                            }

                            CallModel.dialingCall(currentContactMethod)
                                .performAction(Call.ACCEPT)
                        }
                    }
                }
            }

            Page {
                id: chatPage
                background: Rectangle { color: activePalette.base }
                Loader {
                    anchors.fill: parent
                    asynchronous: true
                    active: false
                    id: timelinePage
                    property QtObject currentContactMethod: null
                    property var currentInstance: undefined
                    property bool showScrollbar: true

                    sourceComponent: TimelinePage {
                        showScrollbar: timelinePage.showScrollbar
                        anchors.fill: parent
                        Component.onDestruction: {
                            timelinePage.currentInstance = undefined
                        }
                    }

                    onLoaded: {
                        currentInstance = item
                        if (currentContactMethod)
                            item.currentContactMethod = currentContactMethod
                    }

                    onCurrentContactMethodChanged: {
                        if (currentInstance)
                            currentInstance.currentContactMethod = currentContactMethod
                    }
                }
            }

            Page {
                id: historyPage
                background: Rectangle { color: activePalette.base }
                Loader {
                    property QtObject currentContactMethod: null
                    property var currentInstance: undefined
                    anchors.fill: parent
                    id: callHistory
                    asynchronous: true
                    active: false
                    sourceComponent: CallHistory {
                        anchors.fill: parent
                        Component.onDestruction: {
                            timelinePage.currentInstance = undefined
                        }
                    }

                    onLoaded: {
                        currentInstance = item
                        if (currentContactMethod)
                            item.currentContactMethod = currentContactMethod
                    }

                    onCurrentContactMethodChanged: {
                        if (currentInstance)
                            currentInstance.currentContactMethod = currentContactMethod
                    }
                }
            }
        }
    }

    Kirigami.PageRow {
        id: stackView
        visible: false
        background: Rectangle {
            color: activePalette.base
        }
        anchors.fill: parent

        Page {
            id: page1
            anchors.margins: 0
            background: Rectangle {
            color: activePalette.base
            }
        }
        Page {
            id: page3
            background: Rectangle {
                color: activePalette.base
            }
        }
        Page {
            id: page2
            background: Rectangle {
                color: activePalette.base
            }
        }
        Page {
            id: page4
            background: Rectangle {
                color: activePalette.base
            }
        }

    }

    onStateChanged: {
        stackView.clear()

        if (state == "mobile")
            stackView.push(page1)
    }

    states: [
        State {
            name: ""
            when: !viewContact.mobile

            ParentChange {
                target: contactInfo
                parent: contactInfoPage
            }

            ParentChange {
                target: callHistory
                parent: historyPage
            }

            ParentChange {
                target: timelinePage
                parent: chatPage
            }

            PropertyChanges {
                target: columnLayout
                visible: true
            }

            PropertyChanges {
                target: stackView
                visible: false
            }

            PropertyChanges {
                target: contactInfo
                anchors.fill: contactInfoPage
                flickable.interactive: false
            }

            PropertyChanges {
                target: timelinePage
                showScrollbar: true
            }
        },
        State {
            name: "mobile"
            when: viewContact.mobile

            ParentChange {
                target: contactInfo
                parent: page1
            }

            ParentChange {
                target: callHistory
                parent: page2
            }

            ParentChange {
                target: timelinePage
                parent: page3
            }

            PropertyChanges {
                target: columnLayout
                visible: false
            }

            PropertyChanges {
                target: stackView
                visible: true
            }

            PropertyChanges {
                target: contactInfo
                anchors.fill: page1
                flickable.interactive: true
            }
            PropertyChanges {
                target: callHistory
                active: true
            }
            PropertyChanges {
                target: timelinePage
                active: true
            }
            PropertyChanges {
                target: timelinePage
                showScrollbar: false
            }
        }
    ]
}
