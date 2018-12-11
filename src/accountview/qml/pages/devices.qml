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

Page {
    id: deviceList
    property var account: object

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    SystemPalette {
        id: inactivePalette
        colorGroup: SystemPalette.Inactive
    }

    Component {
        id: deviceDelegate

        Item {
            id: deviceDelegateRect
            clip: true

            width: deviceList.width
            height: confirmPassword.height + confirmLabel.height + 9
            anchors.margins: 3

            RowLayout {
                id: infoView
                width: parent.width
                height: parent.height

                ColumnLayout {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Label {
                        text: name
                        color: activePalette.text
                        font.bold: true
                        Layout.fillWidth: true
                    }

                    Label {
                        text: id
                        color: activePalette.text
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                    }
                }

                Image {
                    visible: isSelf
                    fillMode: Image.PreserveAspectFit
                    Layout.fillHeight: true
                    Layout.preferredWidth: 30
                    width: 30
                    source: "image://icon/document-edit"

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            deviceDelegateRect.state = "beginEdit"
                        }
                    }
                }

                Image {
                    visible: !isSelf
                    fillMode: Image.PreserveAspectFit
                    Layout.fillHeight: true
                    Layout.preferredWidth: 30
                    width: 30
                    source: "image://icon/edit-delete"

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            deviceDelegateRect.state = "beginRevoke"
                        }
                    }
                }

                Behavior on x {
                    NumberAnimation {duration: 200}
                }
            }

            RowLayout {
                id: revokeForm
                x: deviceList.width
                width: parent.width
                height: parent.height

                ColumnLayout {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Label {
                        id: confirmLabel
                        color: activePalette.text
                        text: i18n("Enter your account password to confirm:")
                        Layout.fillWidth: true
                    }

                    TextField {
                        id: confirmPassword
                        Layout.fillWidth: true
                        placeholderText: i18n("Enter your Ring account password")
                        onTextChanged: {
//                             contactViewPage.changed()
                        }
                    }
                }

                Image {
                    fillMode: Image.PreserveAspectFit
                    Layout.fillHeight: true
                    Layout.preferredWidth: 30
                    width: 30
                    source: "image://icon/dialog-ok"

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            object.revoke(confirmPassword.text)
                            deviceDelegateRect.state = "revokingProgress"
                        }
                    }
                }

                Image {
                    fillMode: Image.PreserveAspectFit
                    Layout.fillHeight: true
                    Layout.preferredWidth: 30
                    width: 30
                    source: "image://icon/dialog-cancel"

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            deviceDelegateRect.state = "*"
                        }
                    }
                }

                Behavior on x {
                    NumberAnimation {duration: 200}
                }

                Behavior on y {
                    NumberAnimation {duration: 200}
                }
            }

            RowLayout {
                id: renameForm
                x: deviceList.width
                width: parent.width
                height: parent.height

                TextField {
                    anchors.verticalCenter: parent.verticalCenter
                    id: deviceName
                    text: name
                    Layout.fillWidth: true

                    placeholderText: i18n("Enter a name for this device")

                    onTextChanged: {
//                             contactViewPage.changed()
                    }
                }

                Image {
                    fillMode: Image.PreserveAspectFit
                    Layout.fillHeight: true
                    Layout.preferredWidth: 30
                    width: 30
                    source: "image://icon/dialog-ok"

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            name = deviceName.text
                            deviceDelegateRect.state = "*"
                        }
                    }
                }

                Behavior on x {
                    NumberAnimation {duration: 200}
                }
            }

            RowLayout {
                id: revoking
                y: height
                width: parent.width
                height: parent.height

                BusyIndicator {
                    id: registrationIndicator
                    anchors.margins: 9
                    Layout.fillHeight: false
                }

                Label {
                    text: i18n("Revokation in progress")
                    color: activePalette.text
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                Behavior on y {
                    NumberAnimation {duration: 200}
                }
            }

            Rectangle {
                height: 1
                opacity: 0.2
                width: parent.width
                color: inactivePalette.text
                anchors.bottom: parent.bottom
            }

            states: [
                State {
                    name: "beginRevoke"
                    PropertyChanges {
                        target: infoView
                        x: -deviceList.width
                    }
                    PropertyChanges {
                        target: revokeForm
                        x: 0
                    }
                },
                State {
                    name: "revokingProgress"
                    extend: "beginRevoke"
                    PropertyChanges {
                        target: revoking
                        y: 0
                    }
                    PropertyChanges {
                        target: revokeForm
                        y: -revokeForm.height
                    }
                },
                State {
                    name: "beginEdit"
                    PropertyChanges {
                        target: infoView
                        x: -deviceList.width
                    }
                    PropertyChanges {
                        target: renameForm
                        x: 0
                    }
                }
            ]
        }
    }

    ColumnLayout {
        anchors.fill: parent

        ListView {
            id: deviceListView
            delegate: deviceDelegate
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        Rectangle {
            id: addDevicePanel
            height: 30
            Layout.fillWidth: true
            color: "green"
            clip: true

            ColumnLayout {
                anchors.fill: parent

                Text {
                    id: exportButton
                    height: 30
                    Layout.preferredHeight: 30
                    Layout.fillWidth: true
                    text: i18n("Link another device to the account")
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: "red"

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            console.log("click click")
                        }
                    }
                }

                Text {
                    text: i18n("Your pin is:")
                }

//                 Text {
//                     text: "<b>2C33721B</b>"
//                     Layout.fillWidth: true
//                     font.pointSize: 16
//                     horizontalAlignment: Text.AlignHCenter
//
//                 }

                Text {
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                    text: i18n("To complete the process, you need to open Ring on the new device. Create a new account with \"Link this device to an account\". Your pin is valid for 10 minutes.")
                }

                Text {
                    text: i18n("Close")
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter

                }

            }

            Behavior on height {
                NumberAnimation {duration: 200}
            }

            states: [
                State {
                    name: "enterPassword"
                    PropertyChanges {
                        target: exportButton
                        y: -exportButton.height
                    }
                    PropertyChanges {
                        target: addDevicePanel
                        height: 90
                    }
                }
            ]
        }
    }

    onAccountChanged: {
        if (!account)
            return
        deviceListView.model = account.ringDeviceModel
    }
}
