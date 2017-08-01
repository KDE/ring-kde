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

Item {
    id: contactViewPage
    property var currentContactMethod: null
    property var currentPerson: null

    signal changed()

    property bool showStat: true
    property bool showImage: false
    property bool showSave: true

    property var labelColor: undefined

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
        return  qsTr("<b>Texted:</b> ") + currentContactMethod.textRecording.instantTextMessagingModel.rowCount()
    }

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    state: "desktop"

    onCurrentContactMethodChanged: {
        if (!currentContactMethod)
            return

        currentPerson = currentContactMethod.person

        // Stats
        lastContactedTime.text = getLastContacted()
        totalCall.text         = getTotalCall()
        totalText.text         = getTotalText()
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

    /**
     * When showing the profile or adding a contact, display the image at the top.
     *
     * When showing the main GUI, this image is part of the header and should
     * not be shown.
     */
    Item {
        id: contactPicture

        visible: showImage
        height: showImage ? 90 : 0
        width: parent.width
        anchors.top: parent.top

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

    GroupBox {
        id: advanced
        title: qsTr("Advanced")
        clip: true

        ColumnLayout {
            id: tabbedContactInfo

            clip: true
            anchors.fill: parent

            TabBar {
                Layout.fillWidth: true
                id: tabBar


                // Avoid showing invisible pages by default
                currentIndex: contactViewPage.state != "phone" && sv.currentIndex == 0 ?
                    1 : sv.currentIndex

                TabButton {
                    id: detailsButton
                    text: qsTr("Details")
                    visible: false
                    width: 0
                }
                TabButton {
                    text: qsTr("Phone numbers")
                }
                TabButton {
                    text: qsTr("Addresses")
                }
                TabButton {
                    id: statButton
                    text: qsTr("Statistics")
                    visible: false
                    width: 0
                }
            }

            SwipeView {
                id: sv
                Layout.fillWidth: true
                Layout.fillHeight: true

                // Avoid showing invisible pages by default
                currentIndex: contactViewPage.state != "phone" && tabBar.currentIndex == 0 ?
                    1 : tabBar.currentIndex

                background: Rectangle {
                    color: activePalette.base
                }

                Page {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    GridLayout {
                        id: mainInfo
                        height: implicitHeight

                        columns: 2
                        rowSpacing: 10
                        columnSpacing: 10

                        Label {
                            id: label
                            text: qsTr("Formatted name:")
                            color: labelColor ? labelColor : activePalette.text
                        }
                        TextField {
                            id: formattedName
                            onTextChanged: {
                                contactViewPage.changed()
                            }
                        }

                        Label {
                            text: qsTr("Primary name:")
                            color: labelColor ? labelColor : activePalette.text
                        }
                        TextField {
                            id: firstName
                            onTextChanged: {
                                contactViewPage.changed()
                            }
                        }

                        Label {
                            text: qsTr("Last name:")
                            color: labelColor ? labelColor : activePalette.text
                        }
                        TextField {
                            id: lastName
                            onTextChanged: {
                                contactViewPage.changed()
                            }
                        }

                        Label {
                            text: qsTr("Email:")
                            color: labelColor ? labelColor : activePalette.text
                        }
                        TextField {
                            id: email
                            onTextChanged: {
                                contactViewPage.changed()
                            }
                        }

                        Label {
                            text: qsTr("Organization:")
                            color: labelColor ? labelColor : activePalette.text
                        }
                        TextField {
                            id: organization
                            onTextChanged: {
                                contactViewPage.changed()
                            }
                        }
                    }

                    background: Rectangle { color: activePalette.base }
                }

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

                    background: Rectangle { color: activePalette.base }
                }

                Page {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Addresses {
                        id: addresses
                        anchors.fill: parent
                    }

                    background: Rectangle { color: activePalette.base }
                }

                Page {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    background: Rectangle { color: activePalette.base }
                    ColumnLayout {
                        id: statistics

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

                        Rectangle {
                            color: activePalette.text
                            height: 1
                            Layout.fillWidth: true
                        }
                    }
                }

            }
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

    /**
     * To make this page scale down, reparent everything depending on the
     * resolution.
     */
    states: [

        // In tablet mode, use 3 columns for the details
        State {
            name: "tablet"
            ParentChange {
                target: advanced
                parent: contactViewPage
            }
            ParentChange {
                target: mainInfo
                parent: contactViewPage
            }
            ParentChange {
                target: statistics
                parent: contactViewPage
            }
            AnchorChanges {
                target: advanced
                anchors.right: contactViewPage.right
                anchors.bottom: contactViewPage.bottom
                anchors.top:  contactPicture.bottom
                anchors.left: undefined
            }
            AnchorChanges {
                target: statistics
                anchors.top: contactPicture.bottom
                anchors.left: contactViewPage.left
            }
            AnchorChanges {
                target: mainInfo
                anchors.left: contactViewPage.left
                anchors.top: statistics.bottom
            }
            PropertyChanges {
                target: advanced
                visible: true
                width: contactViewPage.width / 2
            }
            PropertyChanges {
                target: statistics
                width: contactViewPage.width / 2
                height: statistics.implicitHeight
            }
            PropertyChanges {
                target: mainInfo
                width: contactViewPage.width / 2
            }
        },

        // In desktop mode, put everything on top of each other and get rid
        // of the second tabbar
        State {
            name: "desktop"
            extend: "tablet"

            AnchorChanges {
                target: advanced
                anchors.right: undefined
                anchors.bottom: undefined
                anchors.left: contactViewPage.left
                anchors.top: mainInfo.bottom
            }
            PropertyChanges {
                target: statistics
                width: contactViewPage.width
            }
            PropertyChanges {
                target: mainInfo
                width: mainInfo.implicitWidth
                height: mainInfo.implicitHeight
                anchors.topMargin: 10
            }
            PropertyChanges {
                target: advanced
                width: contactViewPage.width * 0.66
                height: 300
                anchors.topMargin: 10
            }
        },

        // On the phone, use 2 tab bars at the top and bottom to fit all pages
        // in the tiny space
        State {
            name: "phone"
            ParentChange {
                target: tabbedContactInfo
                parent: contactViewPage
            }
            PropertyChanges {
                target: mainInfo
                anchors.fill: mainInfo.parent
            }
            PropertyChanges {
                target: statistics
                anchors.fill: statistics.parent
            }
            PropertyChanges {
                target: detailsButton
                visible: true
                width: undefined
            }
            PropertyChanges {
                target: statButton
                visible: true
                width: undefined
            }
            PropertyChanges {
                target: tabbedContactInfo
                anchors.fill: contactViewPage
            }
            PropertyChanges {
                target: advanced
                visible: false
            }
        }
    ]

    // For some reason, the `when` clause of the state never fire
    onHeightChanged: {

        var isPhone = contactViewPage.height <= 400 || (
            contactViewPage.height > contactViewPage.width &&
            contactViewPage.height < 600
        )

        var isTablet = (
            contactViewPage.height <  300 /*advanced default*/
                + statistics.implicitHeight + mainInfo.implicitHeight
        ) && (
            contactViewPage.height > 400
        ) && advanced.implicitWidth + mainInfo.implicitWidth < contactViewPage.width

        if (isPhone)
            state = "phone"
        else if (isTablet)
            state = "tablet"
        else
            state = "desktop"
    }
}
