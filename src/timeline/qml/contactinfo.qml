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
import PhotoSelectorPlugin 1.0
import RingQmlWidgets 1.0

Rectangle {
    id: contactViewPage
    property var currentContactMethod: null
    property var currentPerson: null

    signal changed()

    property bool showStat: true
    property bool showImage: false
    property bool showSave: true

    property var cachedPhoto: undefined

    function save() {
        if ((!currentContactMethod) && (!currentPerson))
            return

        // Changing the CM will flush the content, preserve it
        var old_formattedName  = formattedName.text
        var old_firstName      = firstName.text
        var old_secondName     = lastName.text
        var old_preferredEmail = email.text
        var old_organization   = organization.text

        // Create a real contact method in case this is a temporary one
        if (currentContactMethod && currentContactMethod.type == ContactMethod.TEMPORARY)
            currentContactMethod = PhoneDirectoryModel.getNumber(
                currentContactMethod.uri,
                null,
                currentContactMethod.account
            )

        var person = currentPerson
        if (!currentPerson)
            person = contactBuilder.from(currentContactMethod)

        person.formattedName  = old_formattedName
        person.firstName      = old_firstName
        person.secondName     = old_secondName
        person.preferredEmail = old_preferredEmail
        person.organization   = old_organization

        if (contactViewPage.cachedPhoto != undefined)
            person.photo = contactViewPage.cachedPhoto

        person.save()

        currentPerson = person
    }

    function getLastContacted() {
        var d = currentContactMethod.lastUsed == 0 ? qsTr("Never") :
            new Date(currentContactMethod.lastUsed * 1000).toLocaleString()

        return qsTr("<b>Last contacted on:</b> ") + d + "[?]"
    }

    function getTotalCall() {
        return qsTr("<b>Called:</b> ") + currentContactMethod.callCount +
            " (" + (currentContactMethod.totalSpentTime/60) + " minutes) [?]"
    }

    function getTotalText() {
        return  qsTr("<b>Texted:</b> ") + "N/A time [?]"
    }

    function getTotalRecording() {
        return  qsTr("<b>Recordings:</b> ") + "N/A [?]"
    }

    function getTotalScreenshot() {
        return  qsTr("<b>Screenshots:</b> ") + "N/A [?]"
    }

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    color: activePalette.base

    onCurrentContactMethodChanged: {
        if (!currentContactMethod)
            return

        currentPerson = currentContactMethod.person

        // Stats
        lastContactedTime.text = getLastContacted()
        totalCall.text         = getTotalCall()
        totalText.text         = getTotalText()
        totalRecording.text    = getTotalRecording()
        totalScreenshot.text   = getTotalScreenshot()
    }

    onCurrentPersonChanged: {
        // Sub-models
        phoneNumbersModel.model = currentPerson ?
            currentPerson.phoneNumbersModel : null
        addresses.model = currentPerson ?
            currentPerson.addressesModel : null

        // Contact info
        formattedName.text = currentPerson ?
            currentPerson.formattedName : ""
        firstName.text = currentPerson ?
            currentPerson.firstName : ""
        lastName.text = currentPerson ?
            currentPerson.secondName : ""
        email.text = currentPerson ?
            currentPerson.preferredEmail : ""
        organization.text = currentPerson ?
            currentPerson.organization : ""
        photo.pixmap = currentPerson ?
            currentPerson.photo : undefined
        phoneNumbersModel.person = currentPerson
    }

    ColumnLayout {
        anchors.fill: parent

        Item {
            Layout.fillWidth: true
            height: 90

            Rectangle {
                id: photoRect
                visible: showImage
                anchors.centerIn: parent
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
                    contactViewPage.cachedPhoto = p
                    photo.pixmap = p
                    contactViewPage.changed()
                }

                MouseArea {
                    anchors.fill: parent
                    z: 100
                    onClicked: {
                        var component = Qt.createComponent("PhotoEditor.qml")
                        if (component.status == Component.Ready) {
                            var window    = component.createObject(contactViewPage)
                            window.person = currentContactMethod ? currentContactMethod.person : null
                            window.newPhoto.connect(photoRect.onNewPhoto)
                        }
                        else
                            console.log("ERROR", component.status, component.errorString())
                    }
                }
            }
        }

        ColumnLayout {
            visible: showStat
            Layout.fillWidth: true
            Rectangle {
                color: activePalette.text
                height: 1
                Layout.fillWidth: true
            }

            Label {
                id: lastContactedTime
                color: activePalette.text
            }

            Label {
                id: totalCall
                color: activePalette.text
            }

            Label {
                id: totalText
                color: activePalette.text
            }

            Label {
                id: totalRecording
                color: activePalette.text
            }

            Label {
                id: totalScreenshot
                color: activePalette.text
            }

            Rectangle {
                color: activePalette.text
                height: 1
                Layout.fillWidth: true
            }
        }

        GridLayout {
            columns: 2
            rowSpacing: 10
            columnSpacing: 10
            Layout.fillWidth: true

            Label {
                id: label
                text: qsTr("Formatted name:")
                color: activePalette.text
            }
            TextField {
                id: formattedName
                onTextChanged: {
                    contactViewPage.changed()
                }
            }

            Label {
                text: qsTr("Primary name:")
                color: activePalette.text
            }
            TextField {
                id: firstName
                onTextChanged: {
                    contactViewPage.changed()
                }
            }

            Label {
                text: qsTr("Last name:")
                color: activePalette.text
            }
            TextField {
                id: lastName
                onTextChanged: {
                    contactViewPage.changed()
                }
            }

            Label {
                text: qsTr("Email:")
                color: activePalette.text
            }
            TextField {
                id: email
                onTextChanged: {
                    contactViewPage.changed()
                }
            }

            Label {
                text: qsTr("Organization:")
                color: activePalette.text
            }
            TextField {
                id: organization
                onTextChanged: {
                    contactViewPage.changed()
                }
            }
        }

        Item {
            Layout.preferredHeight: 10
        }

        GroupBox {
            title: qsTr("Advanced")

            clip: true
            Layout.preferredHeight: 300
            Layout.preferredWidth: 600

            ColumnLayout {
                clip: true
                anchors.fill: parent

                TabBar {
                    Layout.fillWidth: true
                    id: tabBar
                    currentIndex: sv.currentIndex
                    TabButton {
                        text: qsTr("Phone numbers")
                    }
                    TabButton {
                        text: qsTr("Addresses")
                    }
                }

                SwipeView {
                    id: sv
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    background: Rectangle {
                        color: activePalette.base
                    }

                    currentIndex: tabBar.currentIndex

                    Page {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        PhoneNumbers {
                            id: phoneNumbersModel
                            anchors.fill: parent
                            onPersonCreated: {
                                if (!currentPerson) {
                                    console.log("Setting the person from a phone number")
                                    currentPerson = phoneNumbersModel.person
                                }
                            }
                        }
                    }

                    Page {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Addresses {
                            id: addresses
                            anchors.fill: parent
                        }
                    }

                }
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }

    ContactBuilder {
        id: contactBuilder
    }

    Button {
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 5
        text: qsTr("Save")
        visible: showSave
        onClicked: {
            contactViewPage.save()
        }
    }
}
