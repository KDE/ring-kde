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
import ContactView 1.0

import RingQmlWidgets 1.0
import PhotoSelectorPlugin 1.0

Rectangle {
    id: contactHeader
    property QtObject currentContactMethod: null
    color: "gray"
    height: 70
    Layout.fillWidth: true

    property alias backgroundColor: contactHeader.color
    property var textColor: undefined
    property var cachedPhoto: undefined

    signal selectChat()
    signal selectVideo()

    onCurrentContactMethodChanged: {
        primaryName.text = currentContactMethod.bestName

        photo.contactMethod = currentContactMethod

        bookmarkSwitch.source = (currentContactMethod && currentContactMethod.bookmarked) ?
            "icons/bookmarked.svg" : "icons/not_bookmarked.svg"
    }

    Connections {
        target: currentContactMethod
        onBookmarkedChanged: {
            bookmarkSwitch.source = (currentContactMethod && currentContactMethod.bookmarked) ?
                "icons/bookmarked.svg" : "icons/not_bookmarked.svg"
        }
    }

    Connections {
        target: currentContactMethod
        onChanged: {
            primaryName.text = currentContactMethod.bestName
            photo.contactMethod = currentContactMethod

            bookmarkSwitch.source = (currentContactMethod && currentContactMethod.bookmarked) ?
                "icons/bookmarked.svg" : "icons/not_bookmarked.svg"
        }
    }

    PersistentCallControls {
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
    }

    RowLayout {
        id: layout
//         height: contactHeader.state == "compact"?  40 : 70
        width: parent.width
        anchors.margins: contactHeader.state == "compact"? 2 : 8

        ContactPhoto {
            id: photo
            Layout.preferredHeight: (contactHeader.state == "compact"?  40 : 70)-10
            Layout.preferredWidth: (contactHeader.state == "compact" ?  40 : 70)-10
            Layout.maximumHeight: (contactHeader.state == "compact" ?  40 : 70)-10
            Layout.maximumWidth: (contactHeader.state == "compact" ?  40 : 70)-10
            displayEmpty: false
            MouseArea {
                id: mouseArea
                anchors.fill: parent
                z: 100
                hoverEnabled: true
                onClicked: {
                    var component = Qt.createComponent("PhotoEditor.qml")
                    if (component.status == Component.Ready) {
                        var window = component.createObject(contactHeader)
                        window.contactMethod = currentContactMethod
                    }
                    else
                        console.log("ERROR", component.status, component.errorString())
                }
            }

            Rectangle {
                anchors.fill: parent
                border.width: 1
                border.color: activePalette.text
                radius: 5
                color: "transparent"
                visible: mouseArea.containsMouse || (!photo.hasPhoto)

                Text {
                    text: i18n("Add\nPhoto")
                    font.bold: true
                    color: activePalette.text
                    anchors.centerIn: parent
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }

        Text {
            id: primaryName
            font.bold: true
//             font.pointSize: 14
            text: "My name"
            color: textColor
            Layout.fillHeight: true
            verticalAlignment: Text.AlignVCenter
        }
        Image {
            id: bookmarkSwitch
            anchors.rightMargin: 1
            anchors.topMargin: 3
            height: 16
            width: 16
            source: (currentContactMethod && currentContactMethod.bookmarked) ?
                "icons/bookmarked.svg" : "icons/not_bookmarked.svg"
            z: 100
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    mouse.accepted = true
                    currentContactMethod.bookmarked = !currentContactMethod.bookmarked
                    bookmarkSwitch.source = currentContactMethod.bookmarked ?
                        "icons/bookmarked.svg" : "icons/not_bookmarked.svg"
                }
            }
        }

        Item {
            id: separator
            Layout.fillHeight: true
            width: 2
            Rectangle {
                height: parent.height -10
                y: 5
                width: 1
                color: inactivePalette.text
                opacity: 0.2
            }
        }

        //TODO Qt5.10 deps: use native icons
        Button {
            id: button
            implicitWidth: label.implicitWidth + 20
            visible: currentContactMethod &&
                ((!currentContactMethod) ||
                    currentContactMethod.canCall == ContactMethod.AVAILABLE)

            checkable: currentContactMethod && currentContactMethod.hasActiveCall
            checked: currentContactMethod && currentContactMethod.firstOutgoingCall

            onClicked: {
                if (currentContactMethod == null) return

                if (currentContactMethod.hasInitCall) {
                    contactHeader.selectVideo()
                    return
                }

                CallModel.dialingCall(currentContactMethod)
                    .performAction(Call.ACCEPT)
            }

            Row {
                id: label
                anchors.centerIn: parent
                height: parent.height
                Image {
                    height: 22
                    width: 22
                    sourceSize.width: 22
                    sourceSize.height: 22
                    anchors.verticalCenter: parent.verticalCenter
                    source: "image://SymbolicColorizer/:/sharedassets/outline/call.svg"
                }

                Text {
                    anchors.margins: 5
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    color: activePalette.text
                    text: "   " + i18n("Call")
                }
            }
        }

        Button {
            id: button2
            implicitWidth: label2.implicitWidth + 20

            visible: currentContactMethod &&
                ((!currentContactMethod) ||
                    currentContactMethod.canVideoCall == ContactMethod.AVAILABLE)

            checkable: currentContactMethod && currentContactMethod.hasActiveCall
            checked: currentContactMethod && currentContactMethod.firstOutgoingCall
                && currentContactMethod.firstOutgoingCall.videoRenderer

            onClicked: {
                if (currentContactMethod == null) return

                if (currentContactMethod.hasInitCall) {
                    contactHeader.selectVideo()
                    return
                }

                CallModel.dialingCall(currentContactMethod)
                    .performAction(Call.ACCEPT)
            }

            Row {
                id: label2
                anchors.centerIn: parent
                height: parent.height
                Image {
                    height: 22
                    width: 22
                    sourceSize.width: 22
                    sourceSize.height: 22
                    anchors.verticalCenter: parent.verticalCenter
                    source: "image://SymbolicColorizer/:/sharedassets/outline/camera.svg"
                }

                Text {
                    anchors.margins: 5
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    color: activePalette.text
                    text: "   " + i18n("Video")
                }
            }
        }

        Button {
            id: button3
            implicitWidth: label3.implicitWidth + 20
            checkable: currentContactMethod && currentContactMethod.hasActiveCall
            checked: currentContactMethod && currentContactMethod.firstOutgoingCall
                && currentContactMethod.firstOutgoingCall.videoRenderer

            visible: currentContactMethod &&
                ((!currentContactMethod) ||
                    currentContactMethod.canVideoCall == ContactMethod.AVAILABLE)

            onClicked: {
                if (currentContactMethod == null) return

                if (currentContactMethod.hasInitCall) {
                    contactHeader.selectVideo()
                    return
                }

                CallModel.dialingCall(currentContactMethod)
                    .performAction(Call.ACCEPT)
            }

            Row {
                id: label3
                anchors.centerIn: parent
                height: parent.height
                Image {
                    height: 22
                    width: 22
                    sourceSize.width: 22
                    sourceSize.height: 22
                    anchors.verticalCenter: parent.verticalCenter
                    source: "image://SymbolicColorizer/:/sharedassets/outline/screen.svg"
                }

                Text {
                    anchors.margins: 5
                    color: activePalette.text
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    text: "   " + i18n("Screen sharing")
                }
            }
        }

        Button {
            id: button4
            implicitWidth: label4.implicitWidth + 20

            visible: currentContactMethod &&
                currentContactMethod.canSendTexts == ContactMethod.AVAILABLE

            onClicked: {
                if (currentContactMethod == null) return

                contactHeader.selectChat()
            }

            Row {
                id: label4
                anchors.centerIn: parent
                height: parent.height
                Image {
                    height: 22
                    width: 22
                    sourceSize.width: 22
                    sourceSize.height: 22
                    anchors.verticalCenter: parent.verticalCenter
                    source: "image://SymbolicColorizer/:/sharedassets/outline/chat.svg"
                }

                Text {
                    anchors.margins: 5
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    color: activePalette.text
                    text: "   " + i18n("Chat")
                }
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            // Display reasons why the media buttons are not present
            MediaAvailability {
                defaultSize: parent.height < 48 ? parent.height : 48
                currentContactMethod: contactHeader.currentContactMethod
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    onStateChanged: {
        layout.height = state == "compact" ? 40 : 70
        console.log("the fucking state changed", state)
    }

    states: [
        State {
            name: ""
            PropertyChanges {
                target: contactHeader
                height: 70
            }
        },
        State {
            name: "compact"
            PropertyChanges {
                target: contactHeader
                height: 40
            }
            PropertyChanges {
                target: button
                visible: false
            }
            PropertyChanges {
                target: button2
                visible: false
            }
            PropertyChanges {
                target: button3
                visible: false
            }
            PropertyChanges {
                target: button4
                visible: false
            }
            PropertyChanges {
                target: photo
//                 height: 30
//                 width: 30
            }
        }
    ]
}
