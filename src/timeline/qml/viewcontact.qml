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

        if (currentContactMethod.person)
            personConn.target = currentContactMethod.person
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
            case "RECORDINGS":
                tabBar.currentIndex = 4
                break
            case "SEARCH":
                tabBar.currentIndex = 5
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

    RowLayout {
        anchors.topMargin: 5
        anchors.rightMargin: 5
        anchors.top: parent.top
        anchors.right: parent.right
        z: 100

        TextField {
            id: textField1
            placeholderText: qsTr("search box")
        }

        Button {
            id: button1
            text: qsTr("Search")
        }
    }

    ColumnLayout {
        anchors.fill: parent

        ContactHeader {
            id: contactHeader
            backgroundColor: activePalette.alternateBase
            textColor: activePalette.text
        }

        TabBar {
            Layout.fillWidth: true
            id: tabBar
            currentIndex: swipeView.currentIndex
            TabButton {
                text: qsTr("Information")
            }
            TabButton {
                text: qsTr("Audio/Video")
            }
            TabButton {
                text: qsTr("Timeline")
            }
            TabButton {
                text: qsTr("Calls")
            }
            TabButton {
                text: qsTr("Recordings")
            }
            TabButton {
                text: qsTr("Search")
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
                ContactInfo {
                    anchors.fill: parent
                    id: contactInfo
                }
            }

            Page {
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
                TimelinePage {
                    anchors.fill: parent
                    id: timelinePage
                }
            }

            Page {
                CallHistory {
                    anchors.fill: parent
                    id: callHistory
                }
            }

            Page {
                RecordingHistory {
                    anchors.fill: parent
                    id: recordings
                }
            }

            Page {
            }
        }
    }
}
