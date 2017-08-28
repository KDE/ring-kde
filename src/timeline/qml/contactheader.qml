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

import RingQmlWidgets 1.0
import PhotoSelectorPlugin 1.0

Rectangle {
    id: contactHeader
    property QtObject currentContactMethod: null

    property alias backgroundColor: contactHeader.color
    property var textColor: undefined
    property var cachedPhoto: undefined

    onCurrentContactMethodChanged: {
        primaryName.text = currentContactMethod.primaryName

        photo.pixmap = currentContactMethod.person ?
             currentContactMethod.person.photo : undefined

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
            primaryName.text = currentContactMethod.primaryName
            photo.pixmap     = currentContactMethod.person ?
                currentContactMethod.person.photo : undefined
            bookmarkSwitch.source = (currentContactMethod && currentContactMethod.bookmarked) ?
                "icons/bookmarked.svg" : "icons/not_bookmarked.svg"
        }
    }

    color: "gray"
    height: 100
    Layout.fillWidth: true

    PersistentCallControls {
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 8

        Rectangle {
            id: photoRect
            clip: true
            radius: 5
            height: 90
            width: 90
            color: "white"
            PixmapWrapper {
                id: photo
                anchors.fill: parent
            }

            function onNewPhoto(p) {
                contactHeader.cachedPhoto = p
                photo.pixmap = p
            }

            MouseArea {
                anchors.fill: parent
                z: 100
                onClicked: {
                    var component = Qt.createComponent("PhotoEditor.qml")
                    if (component.status == Component.Ready) {
                        var window    = component.createObject(contactHeader)
                        window.person = currentContactMethod ? currentContactMethod.person :
                            null
                        window.newPhoto.connect(photoRect.onNewPhoto)
                    }
                    else
                        console.log("ERROR", component.status, component.errorString())
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            RowLayout {
                Text {
                    id: primaryName
                    font.bold: true
                    font.pointSize: 16
                    text: "My name"
                    color: textColor
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
                    Layout.fillWidth: true
                }
            }

            RowLayout {
                id: buttonBox

                Button {
                    id: button
                    text: i18n("Call")
                    onClicked: {
                        if (currentContactMethod == null) return
                        CallModel.dialingCall(currentContactMethod)
                            .performAction(Call.ACCEPT)
                    }
                }

                Button {
                    id: button2
                    text: i18n("Video")
                    onClicked: {
                        if (currentContactMethod == null) return
                        CallModel.dialingCall(currentContactMethod)
                            .performAction(Call.ACCEPT)
                    }
                }

                Button {
                    text: i18n("Screen sharing")
                    onClicked: {
                        if (currentContactMethod == null) return
                        CallModel.dialingCall(currentContactMethod)
                            .performAction(Call.ACCEPT)
                    }
                }
            }
        }
    }

    states: [
        State {
            name: "compact"
            PropertyChanges {
                target: contactHeader
                height: 40
                Layout.maximumHeight: 40
            }
            PropertyChanges {
                target: buttonBox
                visible: false
            }
            PropertyChanges {
                target: photoRect
                height: 30
                width: 30
                Layout.maximumHeight: 30
                Layout.maximumWidth: 30
            }
        }
    ]
}
