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
import org.kde.kirigami 2.2 as Kirigami
import ContactView 1.0

Kirigami.ScrollablePage {
    id: contactViewPage
    property string defaultName: ""
    property var currentContactMethod: null
    property var currentPerson: null
    property var individual: null
    property string forcedState: ""
    property bool editing: true

    signal changed()

    signal selectChat()
    signal selectHistory()

    property bool showStat: true
    property bool showImage: false
    property bool showSave: true

    property bool isChanged: false

    property var labelColor: undefined

    property var cachedPhoto: undefined

    state: forcedState

//     verticalScrollBarPolicy: Qt.ScrollBarAlwaysOff
//     horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff

    function save() {
        if ((!currentContactMethod) && (!currentPerson))
            return

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

        vCardForm.syncDetails(person)

        if (contactViewPage.cachedPhoto != undefined)
            person.photo = contactViewPage.cachedPhoto

        person.save()

        currentPerson = person
        isChanged = false
    }

    onChanged: {
        isChanged = true
    }

    function getLastContacted() {
        if (!individual)
            return

        var d = individual.lastUsedTime == 0 ? i18n("Never") :
            new Date(individual.lastUsedTime * 1000).toLocaleString()

        return i18n("<b>Last contacted on:</b> ") + individual.formattedLastUsedTime
    }

    function getTotalCall() {
        if (!individual)
            return

        return i18n("<b>Called:</b> ") + individual.callCount +
            " (" + (Math.floor(individual.totalSpentTime/60)) + " minutes) [?]"
    }

    function getTotalText() {
        if (!individual)
            return

        return  i18n("<b>Texted:</b> ") + individual.textMessageCount
    }

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    actions {
        main: Kirigami.Action {
            iconName: "document-edit"
            visible: contactViewPage.state == "mobile"
            onTriggered: {
                editing = !editing
            }
        }
    }

    onCurrentContactMethodChanged: {
        if (!currentContactMethod)
            return

        currentPerson = currentContactMethod.person
        isChanged = false
    }

    onCurrentPersonChanged: {
        // Sub-models
//         phoneNumbers.model = currentPerson ?
//             currentPerson.phoneNumbers : null
//         addresses.model = currentPerson ?
//             currentPerson.addressesModel : null

        phoneNumbers.person = currentPerson

        vCardForm.currentPerson = currentPerson

        isChanged = false
    }

    GroupBox {
        id: advanced
        title: i18n("Advanced")
        clip: true
        height: 300

        Behavior on height {
            NumberAnimation { duration: 200 }
        }

        ColumnLayout {
            id: tabbedContactInfo
            anchors.topMargin: contactViewPage.showImage ? 95 : 0

            clip: true
            anchors.fill: parent

            TabBar {
                Layout.fillWidth: true
                id: tabBar

                onCurrentIndexChanged: {
                    sv.currentIndex = currentIndex
                }

                TabButton {
                    id: detailsButton
                    text: i18n("Details")
                }
                TabButton {
                    text: i18n("Phone numbers")
                }
//                 TabButton {
//                     text: i18n("Addresses")
//                 }
                TabButton {
                    id: statButton
                    text: i18n("Statistics")
                }
            }

            SwipeView {
                id: sv
                Layout.fillWidth: true
                Layout.fillHeight: true

                onCurrentIndexChanged: {
                    tabBar.currentIndex = currentIndex
                }

                background: Rectangle {
                    color: activePalette.base
                }

                Page {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    id: tabbedContactInfoPage1

                    VCardForm {
                        id: vCardForm
                        height: implicitHeight
                        editing: contactViewPage.editing

                        onChanged: {
                            isChanged = true
                        }
                    }

                    background: Rectangle { color: activePalette.base }
                }

                Page {
                    id: phoneNumbersPage
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    PhoneNumbers {
                        id: phoneNumbers
                        anchors.fill: parent
                        editing: contactViewPage.editing
                        model: contactViewPage.individual
                        buttonColor: contactViewPage.labelColor
                        onPersonCreated: {
//                             if (!currentPerson) {
//                                 console.log("Setting the person from a phone number")
//                                 currentPerson = phoneNumbers.person
//                             }
                        }
                    }

                    background: Rectangle { color: activePalette.base }
                }

//                 Page {
//                     id: addressesPage
//                     Layout.fillWidth: true
//                     Layout.fillHeight: true
//                     Addresses {
//                         id: addresses
//                         anchors.fill: parent
//                     }
//
//                     background: Rectangle { color: activePalette.base }
//                 }

                Page {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    background: Rectangle { color: activePalette.base }
                    id: tabbedContactInfoPage4
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
                            color: contactViewPage.labelColor ? contactViewPage.labelColor : activePalette.text
                            text: individual ? getLastContacted() : ""
                        }

                        Label {
                            id: totalCall
                            color: contactViewPage.labelColor ? contactViewPage.labelColor : activePalette.text
                            text: individual ? getTotalCall() : ""
                        }

                        Label {
                            id: totalText
                            color: contactViewPage.labelColor ? contactViewPage.labelColor : activePalette.text
                            text: individual ? getTotalText() : ""
                        }

                        Rectangle {
                            color: contactViewPage.state == "mobile" ? "transparent" : activePalette.text
                            height: 1
                            Layout.fillWidth: true
                            Layout.fillHeight: contactViewPage.state == "mobile"
                        }
                    }
                }

            }
        }
    }

    ContactBuilder {
        id: contactBuilder
    }

    Rectangle {
        id: saveButton
        z: 10
        radius: 999
        color: activePalette.highlight
        visible: showSave && isChanged

        anchors.margins: 10
        width: 56
        height: 56

        Image {
            source: "image://icon/edit-save"
            height: 32
            width: 32
            anchors.centerIn: parent
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                contactViewPage.save()
            }
        }
    }

    Column {
        id: phoneLayout
        visible: false
        anchors.left: parent.left
        anchors.top: parent.top
        width: parent.width

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

            ContactPhoto {
                id: photoRect

                tracked: false
                visible: showImage
                anchors.centerIn: parent
                height: 90
                width: 90
                defaultColor: contactViewPage.labelColor

                contactMethod: currentContactMethod
                person: currentPerson

                function onNewPhoto(p) {
                    contactViewPage.cachedPhoto = p

                    contactViewPage.changed()
                }

                MouseArea {
                    anchors.fill: parent
                    z: 100
                    onClicked: {
                        var component = Qt.createComponent("PhotoEditor.qml")
                        if (component.status == Component.Ready) {
                            var window    = component.createObject(contactViewPage)
                            window.person = currentContactMethod ?
                                currentContactMethod.person : currentPerson
                            window.newPhoto.connect(photoRect.onNewPhoto)
                        }
                        else
                            console.log("ERROR", component.status, component.errorString())
                    }
                }
            }
        }

        Kirigami.Heading {
            id: statisticHeader
            text: i18n("Statistics")
            color: contactViewPage.labelColor
            level: 2
        }

        Item {
            id: statisticHolder
            anchors.left: parent.left
            height: statistics.implicitHeight
            width: parent.width
        }

        Kirigami.BasicListItem {
            id: viewHistory
            label: i18n("View history")
            icon: "view-history"
            separatorVisible: true
            onClicked: {
                contactViewPage.selectHistory()
            }
        }

        Kirigami.BasicListItem {
            id: openChat
            label: i18n("Open chat")
            icon: "dialog-messages"
            onClicked: {
                contactViewPage.selectChat()
            }
        }

        Kirigami.Heading {
            text: i18n("Contact details")
            color: contactViewPage.labelColor
            level: 2
        }

        Item {
            id: contactHolder
            height: vCardForm.implicitHeight
            width: parent.width
        }

        Kirigami.Heading {
            text: i18n("Phone numbers")
            color: contactViewPage.labelColor
            level: 2
        }

        Item {
            id: phoneNumberHolder
            height: phoneNumbers.contentHeight+ 48 // 48 == (+)
            width: parent.width
        }

        Kirigami.Heading {
            text: i18n("Addresses")
            color: contactViewPage.labelColor
            level: 2
        }

        Item {
            id: addressesHolder
            height: addresses.contentHeight
            width: parent.width
        }
    }

    onStateChanged: {
        //FIXME this is zombie code, but needs work to remove
        tabBar.currentIndex = 1
        sv.currentIndex = 1
        detailsButton.visible = state == "phone"
        statButton.visible = state == "phone"
        detailsButton.width = state == "phone" ? detailsButton.implicitWidth : 0
        statButton.width = state == "phone" ? statButton.implicitWidth : 0
    }

    /**
     * To make this page scale down, reparent everything depending on the
     * resolution.
     */
    states: [
        // In tablet mode, use 3 columns for the details
        State {
            name: "tablet"
            when: (forcedState == "" ) && (contactViewPage.width >= 600 && contactViewPage.height <= (
                statistics.implicitHeight + vCardForm.implicitHeight + 320) // 320 = advanced.height + 2*spacing
            )
            ParentChange {
                target: advanced
                parent: contactViewPage
            }
            ParentChange {
                target: vCardForm
                parent: contactViewPage
            }
            ParentChange {
                target: statistics
                parent: contactViewPage
            }
            ParentChange {
                target: phoneNumbers
                parent: phoneNumbersPage
            }
//             ParentChange {
//                 target: addresses
//                 parent: addressesPage
//             }
            AnchorChanges {
                target: advanced
                anchors.right: advanced.parent.right
                anchors.bottom: advanced.parent.bottom
                anchors.top:  contactPicture.bottom
                anchors.left: undefined
            }
            AnchorChanges {
                target: statistics
                anchors.top: contactPicture.visible ?
                    contactPicture.bottom : contactViewPage.top
                anchors.left: statistics.parent.left
            }
            AnchorChanges {
                target: vCardForm
                anchors.left: vCardForm.parent.left
                anchors.top: statistics.bottom
            }
            AnchorChanges {
                target: phoneNumbers
                anchors.left: undefined
                anchors.top: undefined
            }

//             AnchorChanges {
//                 target: addresses
//                 anchors.left: undefined
//             }

            PropertyChanges {
                target: advanced
                visible: true
                width: contactViewPage.width / 2
                height: contactViewPage.height
            }
            PropertyChanges {
                target: statistics
                width: contactViewPage.width / 2
                height: statistics.implicitHeight
            }
            PropertyChanges {
                target: vCardForm
                width: contactViewPage.width / 2
            }
            PropertyChanges {
                target: phoneNumbers
                anchors.fill: phoneNumbersPage
                width: undefined
                interactive: true
            }
            PropertyChanges {
                target: contactViewPage
                editing: true
            }

            AnchorChanges {
                target: saveButton
                anchors.bottom: saveButton.parent.bottom
                anchors.top: undefined
                anchors.left: saveButton.parent.left
                anchors.right: undefined
                anchors.horizontalCenter: undefined
            }
            PropertyChanges {
                target: saveButton
                width: 64
                height: 64
            }
        },

        // In desktop mode, put everything on top of each other and get rid
        // of the second tabbar
        State {
            name: ""
            extend: "tablet"

            AnchorChanges {
                target: saveButton
                anchors.top: saveButton.parent.top
                anchors.right: saveButton.parent.right
                anchors.bottom: undefined
                anchors.left: undefined
                anchors.horizontalCenter: undefined
            }
            PropertyChanges {
                target: saveButton
                width: 64
                height: 64
            }

            AnchorChanges {
                target: advanced
                anchors.right: advanced.parent.right
                anchors.bottom: advanced.parent.bottom
                anchors.top: undefined
                anchors.left: advanced.parent.left
            }
            PropertyChanges {
                target: statistics
                width: contactViewPage.width
            }
            PropertyChanges {
                target: vCardForm
                width: vCardForm.implicitWidth
                height: vCardForm.implicitHeight
                anchors.topMargin: 10
                anchors.horizontalCenter: undefined
            }

            PropertyChanges {
                target: advanced
                height: contactViewPage.height ? 300 : 299 //BUG prevent a race condition in QML
                visible: true
                anchors.topMargin: 10
            }
        },

        // The first phone mode was not very usable, lets try again
        State {
            name: "mobile"
            when: ((forcedState == "" && (contactViewPage.width < 600 || contactViewPage.height < (
                statistics.implicitHeight + vCardForm.implicitHeight + 320) // 320 = advanced.height + 2*spacing
            ))) || forcedState == "mobile"

            // Mute the QML warning about having elements with anchors in `Column`
            AnchorChanges {
                target: saveButton
                anchors.right: undefined
                anchors.bottom: undefined
                anchors.top: undefined
                anchors.left: undefined
                anchors.horizontalCenter: undefined
            }

            AnchorChanges {
                target: advanced
                anchors.right: undefined
                anchors.bottom: undefined
                anchors.top: undefined
                anchors.left: undefined
            }

            ParentChange {
                target: vCardForm
                parent: contactHolder
            }
            AnchorChanges {
                target: vCardForm
                anchors.left: contactHolder.left
                anchors.top: contactHolder.top
            }

            PropertyChanges {
                target: vCardForm
                anchors.horizontalCenter: undefined
            }

            ParentChange {
                target: phoneNumbers
                parent: phoneNumberHolder
            }
            AnchorChanges {
                target: phoneNumbers
                anchors.left: phoneNumberHolder.left
                anchors.top: phoneNumberHolder.top
            }

//             ParentChange {
//                 target: addresses
//                 parent: addressesHolder
//             }
//             AnchorChanges {
//                 target: addresses
//                 anchors.left: contactViewPage.left
//             }

            ParentChange {
                target: statistics
                parent: statisticHolder
            }
            AnchorChanges {
                target: statistics
                anchors.left: statisticHolder.left
                anchors.top: statisticHolder.top
            }

            PropertyChanges {
                target: phoneNumbers
                anchors.fill: undefined
                width: contactViewPage.width
                interactive: false
            }

//             PropertyChanges {
//                 target: addresses
//                 anchors.fill: undefined
//             }


            PropertyChanges {
                target: phoneLayout
                visible: true
            }

            // Hide the tabs and groupbox in favor of standard rows
            PropertyChanges {
                target: advanced
                visible: false
            }
            PropertyChanges {
                target: tabBar
                visible: false
            }
            PropertyChanges {
                target: sv
                visible: false
            }

            PropertyChanges {
                target: contactViewPage
                bottomPadding: undefined
                topPadding: undefined
                leftPadding: undefined
                rightPadding: undefined
                editing: false
            }
        },

        State {
            name: "profile"
            extend: "mobile"

            PropertyChanges {
                target: openChat
                visible: false
            }

            PropertyChanges {
                target: vCardForm
                anchors.horizontalCenter: contactHolder.horizontalCenter
            }

            PropertyChanges {
                target: statisticHolder
                visible: false
            }

            PropertyChanges {
                target: viewHistory
                visible: false
            }

            PropertyChanges {
                target: statisticHeader
                visible: false
            }

            PropertyChanges {
                target: contactViewPage
                bottomPadding: 0
                topPadding: 0
                leftPadding: 0
                rightPadding: 0
                editing: true
            }

            AnchorChanges {
                target: phoneLayout
                anchors.top: contactPicture.bottom
            }
        }
    ]

    transitions: Transition {
        AnchorAnimation { duration: 300 }
    }

}
