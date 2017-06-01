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
    property var currentContactMethod: null

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
        // Stats
        lastContactedTime.text = getLastContacted()
        totalCall.text         = getTotalCall()
        totalText.text         = getTotalText()
        totalRecording.text    = getTotalRecording()
        totalScreenshot.text   = getTotalScreenshot()

        // Sub-models
        phoneNumbersModel.model = currentContactMethod.person ?
            currentContactMethod.person.phoneNumbersModel : null

        // Contact info
        formattedName.text = currentContactMethod.person ?
            currentContactMethod.person.formattedName : ""
        firstName.text = currentContactMethod.person ?
            currentContactMethod.person.firstName : ""
        lastName.text = currentContactMethod.person ?
            currentContactMethod.person.secondName : ""
        email.text = currentContactMethod.person ?
            currentContactMethod.person.preferredEmail : ""
        organization.text = currentContactMethod.person ?
            currentContactMethod.person.organization : ""
    }

    ColumnLayout {
        anchors.fill: parent

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
            }

            Label {
                text: qsTr("Primary name:")
                color: activePalette.text
            }
            TextField {
                id: firstName
            }

            Label {
                text: qsTr("Last name:")
                color: activePalette.text
            }
            TextField {
                id: lastName
            }

            Label {
                text: qsTr("Email:")
                color: activePalette.text
            }
            TextField {
                id: email
            }

            Label {
                text: qsTr("Organization:")
                color: activePalette.text
            }
            TextField {
                id: organization
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
                        }
                    }

                    Page {
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
        onClicked: {
            if ((!currentContactMethod))
                return

            if (!currentContactMethod.person) {
                var ret = contactBuilder.from(currentContactMethod)
            }

            currentContactMethod.person.formattedName  = formattedName.text
            currentContactMethod.person.firstName      = firstName.text
            currentContactMethod.person.secondName     = lastName.text
            currentContactMethod.person.preferredEmail = email.text
            currentContactMethod.person.organization   = organization.text

            currentContactMethod.person.save()
        }
    }
}
