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
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import net.lvindustries.ringqtquick.media 1.0 as RingQtMedia
import org.kde.ringkde.jamicallview 1.0 as JamiCallView
import org.kde.ringkde.jamicontactview 1.0 as JamiContactView
import org.kde.ringkde.jamitimeline 1.0 as JamiTimeline
import org.kde.ringkde.jamihistoryview 1.0 as JamiHistoryView
import org.kde.ringkde.jamichatview 1.0 as JamiChatView

Item {
    id: viewContact
    property var currentContactMethod: null
    property var timelineModel: null
    property string currentPage: ""
    property var contactHeader: null
    property bool mobile: false
    property var currentIndividual: null
    property var unsortedListView: null

    function getDefaultCm() {
        if (currentContactMethod)
            return currentContactMethod

        if (currentIndividual)
            return currentIndividual.mainContactMethod

        return null
    }

    function showVideo() {
        avView.active = true
        if (state == "mobile")
            stackView.push(avPage)
        else
            tabBar.currentIndex = 1
    }

    function showNewContent() {
        if (timelinePage.item)
            timelinePage.item.showNewContent()
    }

    clip: true

    onCurrentContactMethodChanged: {
        if (contactHeader)
            contactHeader.currentContactMethod = currentContactMethod

        if (currentContactMethod && currentContactMethod.person)
            personConn.target = currentContactMethod.person

        if (currentContactMethod)
            avView.call = currentContactMethod.firstActiveCall
        else if (currentIndividual)
            avView.call = currentIndividual.firstActiveCall
    }

    onCurrentIndividualChanged: {
        if (contactHeader)
            contactHeader.currentIndividual = currentIndividual

        if (currentIndividual && currentIndividual.person)
            personConn.target = currentIndividual.person

        if (currentIndividual != null)
            avView.call = currentIndividual.firstActiveCall
    }

    onCurrentPageChanged: {
        if (currentPage == "")
            return

        switch(currentPage) {
            case "TIMELINE":
                tabBar.currentIndex = 0
                break
            case "MEDIA":
                tabBar.currentIndex = 1
                break
            case "CALL_HISTORY":
                tabBar.currentIndex = 2
                break
            case "INFORMATION":
                tabBar.currentIndex = 3
                break
        }

        currentPage = ""
    }

    RingQtMedia.AvailabilityTracker {
        id: availabilityTracker
        individual: currentIndividual
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

            avView.call = call && call.lifeCycleState != RingQtQuick.Call.FINISHED ? call : null
        }
    }

    Connections {
        id: personConn
        target: currentContactMethod ? currentContactMethod.person : null
        onCallAdded: {
            avView.call = call
        }
    }

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent

        TabBar {
            Layout.fillWidth: true
            id: tabBar
            currentIndex: swipeView.currentIndex
            TabButton {
                text: i18n("Chat/Timeline")
            }
            TabButton {
                text: i18n("Audio/Video")
            }
            TabButton {
                text: i18n("Calls/Recordings")
            }
            TabButton {
                text: i18n("Information")
            }

            onCurrentIndexChanged: {
                //TODO deactivate it after a minute in other tabs
                if (currentIndex == 0) {
                    timelinePage.active = true
                }
                if (currentIndex == 2) {
                    callHistory.active = true
                }
            }
        }

        SwipeView {
            id: swipeView
            Layout.fillHeight: true
            Layout.fillWidth: true

            background: Rectangle {
                color: Kirigami.Theme.backgroundColor
            }

            currentIndex: tabBar.currentIndex

            Page {
                id: chatPage
                background: Rectangle { color: Kirigami.Theme.backgroundColor }
                Loader {
                    anchors.fill: parent
                    asynchronous: true
                    active: true
                    id: timelinePage
                    property bool showScrollbar: true

                    sourceComponent: JamiChatView.ChatPage {
                        showScrollbar: timelinePage.showScrollbar
                        timelineModel: viewContact.timelineModel
                        currentContactMethod: viewContact.currentContactMethod
                        currentIndividual: viewContact.currentIndividual
                        anchors.fill: parent
                    }
                }
            }

            Page {
                id: avPage
                background: Rectangle { color: Kirigami.Theme.backgroundColor }
                Loader {
                    id: avView
                    asynchronous: true
                    active: false
                    anchors.fill: parent

                    property QtObject call: null

                    // QML bug?
                    onCallChanged: {
                        if (active)
                            callViewWidget.call = call
                    }

                    JamiCallView.CallView {
                        id: callViewWidget
                        mode: "CONVERSATION"
                        anchors.fill: parent
                        call: avView.call
                        onCallWithAudio: {
                            var cm = viewContact.getDefaultCm()

                            if (cm && cm.hasInitCall) {
                                contactHeader.selectVideo()
                                return
                            }

                            RingSession.callModel.dialingCall(cm)
                                .performAction(RingQtQuick.Call.ACCEPT)
                        }
                        onCallWithVideo: {
                            var cm = viewContact.getDefaultCm()

                            if (cm && cm.hasInitCall) {
                                contactHeader.selectVideo()
                                return
                            }

                            RingSession.callModel.dialingCall(cm)
                                .performAction(RingQtQuick.Call.ACCEPT)
                        }
                        onCallWithScreen: {
                            var cm = viewContact.getDefaultCm()

                            if (cm && cm.hasInitCall) {
                                contactHeader.selectVideo()
                                return
                            }

                            RingSession.callModel.dialingCall(cm)
                                .performAction(RingQtQuick.Call.ACCEPT)
                        }
                    }
                }
            }

            Page {
                id: historyPage
                background: Rectangle { color: Kirigami.Theme.backgroundColor }
                Loader {
                    anchors.fill: parent
                    id: callHistory
                    asynchronous: true
                    active: false
                    sourceComponent: JamiHistoryView.CallHistory {
                        showScrollbar: timelinePage.showScrollbar
                        timelineModel: viewContact.timelineModel
                        currentContactMethod: viewContact.currentContactMethod
                        currentIndividual: viewContact.currentIndividual
                        anchors.fill: parent
                    }
                }
            }

            Page {
                id: contactInfoPage
                background: Rectangle { color: Kirigami.Theme.backgroundColor }
                JamiContactView.ContactInfo {
                    id: contactInfo
                    individual: viewContact.currentIndividual
                    anchors.fill: parent

                    onSelectChat: {
                        stackView.push(page3)
                    }

                    onSelectHistory: {
                        stackView.push(page2)
                    }
                }
            }
        }
    }

    Kirigami.PageRow {
        id: stackView
        visible: false
        background: Rectangle {
            color: Kirigami.Theme.backgroundColor
        }
        anchors.fill: parent

        Page {
            id: page2
            background: Rectangle {
                color: Kirigami.Theme.backgroundColor
            }
        }
        Page {
            id: page3
            background: Rectangle {
                color: Kirigami.Theme.backgroundColor
            }
        }
        Page {
            id: page4
            background: Rectangle {
                color: Kirigami.Theme.backgroundColor
            }
        }
        Page {
            id: page1
            anchors.margins: 0
            background: Rectangle {
            color: Kirigami.Theme.backgroundColor
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
