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

Item {

    property var currentContactMethod: null

    clip: true

    onCurrentContactMethodChanged: {
        contactHeader.currentContactMethod = currentContactMethod
        contactInfo.currentContactMethod   = currentContactMethod
        callHistory.currentContactMethod   = currentContactMethod
        timelinePage.currentContactMethod  = currentContactMethod
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
        }

        TabBar {
            Layout.fillWidth: true
            id: tabBar
            currentIndex: swipeView.currentIndex
            TabButton {
                text: qsTr("Information")
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

            currentIndex: tabBar.currentIndex
            Page {
                ContactInfo {
                    anchors.fill: parent
                    id: contactInfo
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
