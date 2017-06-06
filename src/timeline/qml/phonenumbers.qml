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

Rectangle {
    id: phoneNumbers
    property alias model : numbers.model
    property QtObject person: null

    signal personCreated(QtObject newPerson)

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    SystemPalette {
        id: inactivePalette
        colorGroup: SystemPalette.Disabled
    }

    color: activePalette.base

    ColumnLayout {
        anchors.fill: parent

        ListView {
            id: numbers
            Layout.fillWidth: true
            Layout.fillHeight: true
            anchors.margins: 3
            delegate: Item {
                height: 30
                width: phoneNumbers.width
                RowLayout {
                    anchors.fill: parent

                    PixmapWrapper {
                        Layout.preferredHeight: parent.height
                        Layout.preferredWidth: parent.height
                        pixmap: decoration
                    }
                    Text {
                        text: display
                        Layout.fillWidth: true
                        color: activePalette.text
                    }
                    Text {
                        text: categoryName
                        color: inactivePalette.text
                    }
                    Rectangle {
                        id: videoButton
                        visible: isReachable
                        Layout.preferredWidth: parent.height
                        Layout.preferredHeight: parent.height
                        color: "transparent"
                        radius: 3
                        Image {
                            anchors.fill: parent
                            smooth : false
                            source: "sharedassets/phone_dark/accept_video.svg"
                            sourceSize.width: parent.height
                            sourceSize.height: parent.height
                        }

                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: videoButton.color = activePalette.highlight
                            onExited: videoButton.color = "transparent"
                            onClicked: CallModel.dialingCall(object).performAction(Call.ACCEPT)
                        }
                    }
                    Rectangle {
                        id: callButton
                        visible: isReachable
                        Layout.preferredWidth: parent.height
                        Layout.preferredHeight: parent.height
                        color: "transparent"
                        radius: 3
                        Image {
                            anchors.fill: parent
                            smooth : false
                            source: "sharedassets/phone_dark/accept.svg"
                            sourceSize.width: parent.height
                            sourceSize.height: parent.height
                        }

                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: callButton.color = activePalette.highlight
                            onExited: callButton.color = "transparent"
                            onClicked: CallModel.dialingCall(object).performAction(Call.ACCEPT)
                        }
                    }
                    Rectangle {
                        id: remove
                        visible: isReachable
                        Layout.preferredWidth: parent.height
                        Layout.preferredHeight: parent.height
                        color: "transparent"
                        radius: 3
                        Image {
                            anchors.fill: parent
                            smooth : false
                            source: "image://icon/list-remove"
                            sourceSize.width: parent.height
                            sourceSize.height: parent.height
                        }

                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: remove.color = activePalette.highlight
                            onExited: remove.color = "transparent"
    //                         onClicked: CallModel.dialingCall(object).performAction(Call.ACCEPT)
                        }
                    }
                    Item {
                        Layout.preferredWidth: 5
                    }
                }
            }
        }

        ContactBuilder {
            id: contactBuilder
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            ComboBox {
                id: numbertype
                model: NumberCategoryModel
                textRole: "display"
                onActivated: {
                }
            }

            TextField {
                Layout.fillWidth: true
                id: newPhoneNumber
            }

            Button {
                id: button
                text: qsTr("Add")
                onClicked: {

                    if (newPhoneNumber.text == "") {
                        console.log("No number added, the field is empty")
                        return
                    }

                    var cm = contactBuilder.addPhoneNumber(
                        person, newPhoneNumber.text, numbertype.index
                    )

                    console.log("adding", newPhoneNumber.text, cm)

                    if (cm && cm.person) {
                        console.log("Setting the person")
                        person = cm.person
                        phoneNumbers.model = cm.person.phoneNumbersModel
                        personCreated(cm.person)
                    }
                }
            }
        }
    }
}
