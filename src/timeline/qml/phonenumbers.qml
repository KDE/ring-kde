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
import org.kde.kirigami 2.2 as Kirigami

Item {
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

    ColumnLayout {
        anchors.fill: parent

        ListView {
            id: numbers
            Layout.fillWidth: true
            Layout.fillHeight: true
            anchors.margins: 3
            delegate: Kirigami.SwipeListItem {
//                 height: 30
//                 width: phoneNumbers.width

                actions: [
                    Kirigami.Action {
                        iconName: "edit-delete"
                        text: i18n("Delete")
                        onTriggered: print("Action 1 clicked")
                    },
                    Kirigami.Action {
                        iconSource: "image://SymbolicColorizer/:/sharedassets/outline/call.svg"
                        text: i18n("Call")
                        visible: canCall
                        onTriggered: CallModel.dialingCall(object).performAction(Call.ACCEPT)
                    },
                    Kirigami.Action {
                        iconSource: "image://SymbolicColorizer/:/sharedassets/outline/camera.svg"
                        text: i18n("Video call")
                        visible: canVideoCall
                        onTriggered: CallModel.dialingCall(object).performAction(Call.ACCEPT)
                    },
                    Kirigami.Action {
                        iconSource: "image://SymbolicColorizer/:/sharedassets/outline/screen.svg"
                        text: i18n("Share screen")
                        visible: canVideoCall
                        onTriggered: CallModel.dialingCall(object).performAction(Call.ACCEPT)
                    }
                ]

                RowLayout {
                    anchors.leftMargin: 10
                    anchors.fill: parent
                    spacing: 10

                    PixmapWrapper {
                        Layout.preferredHeight: 16
                        Layout.preferredWidth: 16
                        anchors.verticalCenter: parent.verticalCenter
                        pixmap: decoration
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        Row {
                            Layout.fillWidth: true
                            Text {
                                text: display
                                color: activePalette.text
                            }
                            Text {
                                text: "  ("+categoryName+")"
                                color: inactivePalette.text
                            }
                        }

                        Text {
                            text: lastUsed == undefined || lastUsed == "" ? i18n("Never used") :
                                i18n("Used ")+totalCallCount+i18n(" time (Last used on: ") + formattedLastUsed + ")"
                            color: inactivePalette.text
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
                text: i18n("Add")
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
