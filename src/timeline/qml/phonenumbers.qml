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
    property color buttonColor: inactivePalette.text
    property alias model: numbers.model
    property QtObject person: null
    property alias contentHeight: numbers.contentHeight
    property alias interactive: numbers.interactive
    property bool editing: (model && model.editRow) || !person

    clip: true
    signal personCreated(QtObject newPerson)

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    SystemPalette {
        id: inactivePalette
        colorGroup: SystemPalette.Disabled
    }

    Component {
        id: editComponent

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
                text: editUri
            }

            CheckBox {
                id: customAccount
                checked: obj.account && visible
                visible: AccountModel.size > 1
            }

            ComboBox {
                id: numberAccount
                model: AccountModel
                enabled: customAccount.checked
                textRole: "display"
                visible: AccountModel.size > 1
                currentIndex: obj.account ? obj.account.index.row : 2
                onActivated: {
                }
            }

            Button {
                id: button
                text: i18n("Save")
                onClicked: {

                    if (newPhoneNumber.text == "") {
                        console.log("No number added, the field is empty")
                        return
                    }

                    var accIdx = customAccount.checked ? numberAccount.index : -1

                    var p = phoneNumbers.person ? phoneNumbers.person : phoneNumbers.model.person

                    var cm = contactBuilder.updatePhoneNumber(obj,
                        p, newPhoneNumber.text, numbertype.index, accIdx
                    )

                    if (cm && cm.person) {
                        console.log("Setting the person", cm, cm.person)
                        person = cm.person
                        //phoneNumbers.model = cm.person.individual
                    }

                    if (cm.person) {
                        console.log("Saving", cm.person)
                        cm.person.save()
                    }

                    personCreated(cm.person)

                    numbers.currentIndex = idx
                    numbers.currentItem.state = ""
                }
            }
            Button {
                text: i18n("Cancel")
                onClicked: {
                    if (cmType == ContactMethod.TEMPORARY)
                        numbers.model.editRow = false

                    numbers.currentIndex = idx
                    numbers.currentItem.state = ""
                }
            }
        }

    }

    ColumnLayout {
        anchors.fill: parent

        ListView {
            id: numbers
            Layout.fillWidth: true
            Layout.fillHeight: true
            anchors.margins: 3

            footer: OutlineButton {
                id: mainArea
                height: fontMetrics.height * 3.5
                expandedHeight: fontMetrics.height * 3.5
                sideMargin: 2
                width: parent.width
                color: phoneNumbers.buttonColor
                label: i18n("Add a phone number or GNU Ring identity")
                topPadding: 2
                visible: !numbers.model.editRow
                onClicked: {
                    if (phoneNumbers.model) {
                        contactBuilder.addEmptyPhoneNumber(phoneNumbers.person)
                        numbers.model.editRow = true
                        numbers.currentIndex = numbers.count - 1
                        numbers.currentItem.state = "edit"
                    }
                    else
                        console.log("No contact, not implemented")
                }
            }

            delegate: Kirigami.SwipeListItem {
                height: readOnly.height
                implicitHeight: readOnly.height

                states: [
                    State {
                        name: ""
                    },
                    State {
                        name: "edit"
                        PropertyChanges {
                            target: editorLoader
                            editUri: uri

                            active: true
                        }

                        PropertyChanges {
                            target: readOnly
                            visible: false
                        }
                    }
                ]

                actions: [
                    Kirigami.Action {
                        iconName: "edit-delete"
                        text: i18n("Delete")
                        onTriggered: {
                            // Cache the person to avoid the race condition where
                            // the delegate is deleted before the end of the callback
                            var p = phoneNumbers.model.person
                            phoneNumbers.model.removePhoneNumber(object).
                            if (p)
                                p.save()
                        }
                    },
                    Kirigami.Action {
                        iconName: "document-edit"
                        text: i18n("Edit")
                        onTriggered: {
                            state = "edit"
                        }
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

                // Wrap in an extra Item to bypass Kirigami limitations regarding
                // the number of elements
                Item {
                    height: readOnly.height
                    implicitHeight: readOnly.height

                    Loader {
                        id: editorLoader
                        property ContactMethod obj: object
                        property int idx: index
                        property string editUri: object ? object.uri : ""
                        property var cmType: type
                        sourceComponent: editComponent
                        anchors.fill: parent
                        anchors.rightMargin: 50
                        active: false
                    }

                    RowLayout {
                        id: readOnly
                        anchors.leftMargin: 10
                        anchors.fill: parent
                        height: columns.implicitHeight + 30 // 30 == 3*spacing
                        implicitHeight: columns.implicitHeight + 30
                        spacing: 10

                        PixmapWrapper {
                            Layout.preferredHeight: 16
                            Layout.preferredWidth: 16
                            anchors.verticalCenter: parent.verticalCenter
                            pixmap: decoration
                        }

                        ColumnLayout {
                            id: columns
                            Layout.fillWidth: true
                            Row {
                                Layout.fillWidth: true
                                Text {
                                    id: dsfdsf
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
        }

        ContactBuilder {
            id: contactBuilder
        }
    }
}
