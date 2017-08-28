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

Rectangle {
    id: viewContact
    property var currentContactMethod: null
    property var currentPerson: null
    property string currentPage: ""

    clip: true

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    color: activePalette.base

    onCurrentContactMethodChanged: {
        contactHeader.currentContactMethod = currentContactMethod
        contactInfo.currentContactMethod   = currentContactMethod
        callHistory.currentContactMethod   = currentContactMethod
        timelinePage.currentContactMethod  = currentContactMethod

        if (currentContactMethod && currentContactMethod.person)
            personConn.target = currentContactMethod.person

        // Check if the CM already has an active call, switch to it
        for (var i=0; i<CallModel.size; i++) {
            var call = CallModel.getCall(CallModel.index(i, 0))
            if (call && call.peerContactMethod == currentContactMethod && call.lifeCycleState != Call.FINISHED) {
                avView.call = call
                break
            }
        }
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

//     RowLayout {
//         anchors.topMargin: 5
//         anchors.rightMargin: 5
//         anchors.top: parent.top
//         anchors.right: parent.right
//         z: 100
//
//         TextField {
//             id: textField1
//             placeholderText: i18n("search box")
//         }
//
//         Button {
//             id: button1
//             text: i18n("Search")
//         }
//     }

    FirstRun {
        anchors.fill: parent
        z: 99999
        color: activePalette.base
        visible: currentContactMethod == null || currentContactMethod.isSelf
    }

    ColumnLayout {
        anchors.fill: parent

        ContactHeader {
            id: contactHeader
            backgroundColor: activePalette.alternateBase
            textColor: activePalette.text
            onCachedPhotoChanged: {
                contactInfo.cachedPhoto = contactHeader.cachedPhoto
            }
        }

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
                background: Rectangle { color: activePalette.base }
                ContactInfo {
                    anchors.fill: parent
                    id: contactInfo
                }
            }

            Page {
                background: Rectangle { color: activePalette.base }
                CallView {
                    id: avView
                    mode: "CONVERSATION"
                    anchors.fill: parent
                    onCallWithAudio: {
                        CallModel.dialingCall(currentContactMethod)
                            .performAction(Call.ACCEPT)
                    }
                    onCallWithVideo: {
                        CallModel.dialingCall(currentContactMethod)
                            .performAction(Call.ACCEPT)
                    }
                    onCallWithScreen: {
                        CallModel.dialingCall(currentContactMethod)
                            .performAction(Call.ACCEPT)
                    }
                }
            }

            Page {
                background: Rectangle { color: activePalette.base }
                Loader {
                    anchors.fill: parent
                    asynchronous: true
                    active: false
                    id: timelinePage
                    property QtObject currentContactMethod: null
                    property var currentInstance: undefined

                    sourceComponent: TimelinePage {
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

    states: [
        State {
            name: "compact"
            when: height < 700
            PropertyChanges {
                target: contactHeader
                state: "compact"
                height: 40
            }
        }
    ]
}
