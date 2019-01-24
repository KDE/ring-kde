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
import QtQuick.Layouts 1.2

import org.kde.kirigami 2.2 as Kirigami
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import org.kde.ringkde.jamicontactview 1.0 as JamiContactView

Kirigami.ScrollablePage {
    id: contactViewPage
    property string defaultName: ""
    property var individual: null
    property string forcedState: ""
    property bool editable: true
    property bool editing: editable

    signal changed()

    signal selectChat()
    signal selectHistory()

    property bool showStat: true
    property bool showImage: false
    property bool showSave: true

    property bool isChanged: false

    property var labelColor: inactivePalette.text

    property var cachedPhoto: undefined

    state: forcedState

//     verticalScrollBarPolicy: Qt.ScrollBarAlwaysOff
//     horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff

    function save() {
        if (!individual)
            return

        var person = individual.person

        if (!person)
            person = contactBuilder.from(individual, vCardForm.name)


        if (!vCardForm.currentPerson)
            vCardForm.currentPerson = person

        vCardForm.syncDetails(person)

        if (contactViewPage.cachedPhoto != undefined)
            person.photo = contactViewPage.cachedPhoto

        person.save()

        //currentPerson = person
        isChanged = false
    }

    onChanged: {
        isChanged = true
    }

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    SystemPalette {
        id: inactivePalette
        colorGroup: SystemPalette.Inactive
    }

    actions {
        main: Kirigami.Action {
            iconName: editing ? "document-save" : "document-edit"
            visible: contactViewPage.state == "mobile"
            onTriggered: {
                if (editing && isChanged) {
                    console.log("Saving!")
                    contactViewPage.save()
                }
                editing = (!editing) && editable
            }
        }
    }

    onIndividualChanged: {
        vCardForm.currentPerson = individual ? individual.person : null
        vCardForm.individual = individual

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
            spacing: 0

            clip: true
            anchors.fill: parent

            TabBar {
                Layout.fillWidth: true
                id: tabBar

                onCurrentIndexChanged: {
                    sv.currentIndex = currentIndex
                }

                TabButton {
                    text: i18n("Phone numbers")
                }
//                 TabButton {
//                     text: i18n("Addresses")
//                 }
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

                    JamiContactView.VCardForm {
                        id: vCardForm
                        height: preferredHeight
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
                    JamiContactView.PhoneNumbers {
                        id: phoneNumbers
                        width: parent.width
                        model: contactViewPage.individual
                        buttonColor: contactViewPage.labelColor
                        showAdd: contactViewPage.editing
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
                    JamiContactView.Statistics {
                        id: statistics
                        Layout.fillWidth: true
                        visible: showStat
                        individual: contactViewPage.individual
                        labelColor: contactViewPage.labelColor
                    }
                }

            }
        }
    }

    RingQtQuick.ContactBuilder {
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

    ColumnLayout {
        id: phoneLayout
        visible: false
        anchors.left: parent.left
        anchors.top: parent.top
        width: parent.width
        spacing: 10

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
            implicitHeight: showImage ? 90 : 0
            Layout.fillWidth: true

            JamiContactView.ContactPhoto {
                id: photoRect

                tracked: false
                visible: showImage
                anchors.centerIn: parent
                height: 90
                width: 90
                defaultColor: contactViewPage.labelColor

                individual: contactViewPage.individual

                function onNewPhoto(p) {
                    contactViewPage.cachedPhoto = p

                    contactViewPage.changed()
                }

                MouseArea {
                    anchors.fill: parent
                    z: 100
                    onClicked: {
                        if (!contactViewPage.editing)
                            return false

                        var component = Qt.createComponent("PhotoEditor.qml")
                        if (component.status == Component.Ready) {
                            var window    = component.createObject(contactViewPage)
                            window.person = individual ?
                                individual.person : null
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
//             Layout.leftAnchor: parent.left
            height: statistics.implicitHeight
            Layout.fillWidth: true
        }

        // Keep in columns to avoid spacing
        ColumnLayout {
            Layout.fillWidth: true
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
        }

        Kirigami.Heading {
            text: i18n("Contact details")
            color: contactViewPage.labelColor
            level: 2
        }

        Item {
            id: contactHolder
            height: vCardForm.preferredHeight
            Layout.preferredHeight: height
            Layout.maximumHeight: height
            Layout.minimumHeight: height
            Layout.fillWidth: true
        }

        Kirigami.Heading {
            text: i18n("Phone numbers")
            color: contactViewPage.labelColor
            level: 2
        }

        Item {
            id: phoneNumberHolder
            height: phoneNumbers.preferredHeight
            Layout.preferredHeight: height
            Layout.maximumHeight: height
            Layout.minimumHeight: height
            Layout.fillWidth: true
        }

//         Kirigami.Heading {
//             text: i18n("Addresses")
//             color: contactViewPage.labelColor
//             level: 2
//         }

//         Item {
//             id: addressesHolder
//             height: addresses.contentHeight
//             width: parent.width
//         }
    }

    onStateChanged: {
        tabBar.currentIndex = 0
        sv.currentIndex = 1
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
                statistics.implicitHeight + 320) // 320 = advanced.height + 2*spacing
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
                height: phoneNumbersPage.height
                interactive: true
            }
            PropertyChanges {
                target: contactViewPage
                editing: editable
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
                height: preferredHeight
                x: 0
                y: 0
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
                editing: editable
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
