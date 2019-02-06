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
import QtQuick 2.9
import QtQuick.Controls 2.2 as Controls
import QtQuick.Layouts 1.0
import org.kde.kirigami 2.2 as Kirigami

import net.lvindustries.ringqtquick 1.0 as RingQtQuick

Item {
    id: createRing
    property alias registerUserName: registerUserName
    property alias account: ringAccountBuilder.account
    property alias nextAvailable: ringAccountBuilder.canCreate

    property bool busy: false
    property var lookupState: 2

    signal registrationCompleted(QtObject account)

    width: Math.min(createForm.implicitWidth, parent.width - 20)
    height: createForm.implicitHeight

    function createAccount() {
        ringAccountBuilder.create()
    }

    function commitAccount() {
        ringAccountBuilder.commit()
    }

    /**
     * Business logic and validation logic
     */
    RingQtQuick.RingAccountBuilder {
        id: ringAccountBuilder
        registerRingName  : registerUserName.checked
        registeredRingName: userName.text
        repeatPassword    : repeatPasswordTxt.text
        password          : passwordTxt.text
        usePassword       : usePassword.checked
        onFinished: {
            registrationCompleted(account)
        console.log("\n\nCOMMIT")
        }
    }

    ColumnLayout {
        id: createForm
        width: parent.width

        Controls.Switch {
            id: registerUserName
            height: 40
            text: i18n("Register a public username*")
            opacity: 1
            Layout.fillWidth: true
            checked: true//ringAccountBuilder.registerRingName //FIXME race
            contentItem: Text {
                text: registerUserName.text
                font: registerUserName.font
                color: "white"
                verticalAlignment: Text.AlignVCenter
                leftPadding: registerUserName.indicator.width + registerUserName.spacing
            }
        }

        Controls.Label {
            id: label2
            opacity: ringAccountBuilder.registerRingName ? 1 : 0
            text: i18n("Enter an username")
            color: "white"
            anchors.leftMargin: 8
            Layout.fillWidth: true
            Behavior on opacity {
                NumberAnimation {
                    easing.type: Easing.OutQuad
                    duration: 200
                }
            }
        }

        Controls.TextField {
            id: userName
            color: "white"
            Layout.fillWidth: true
            opacity: ringAccountBuilder.registerRingName ? 1 : 0
            enabled: ringAccountBuilder.registerRingName
            Behavior on opacity {
                NumberAnimation {
                    easing.type: Easing.OutQuad
                    duration: 200
                }
            }
        }

        RowLayout {
            id: rowLayout
            clip: true
            Layout.fillHeight: false
            Layout.maximumHeight: 37
            Layout.fillWidth: true
            opacity: ringAccountBuilder.registerRingName ? 1 : 0
            spacing: 6

            Item {
                width: 37
                height: 37
                Layout.fillHeight: true
                Layout.preferredWidth: 37

                Controls.BusyIndicator {
                    id: busyIndicator
                    anchors.fill: parent
                    visible: ringAccountBuilder.nameLookup
                }
            }

            Controls.Label {
                id: registerFoundLabel
                text: ringAccountBuilder.nameStatusMessage
                verticalAlignment: Text.AlignVCenter
                color: ringAccountBuilder.nameLookupError ? "#ff1111" : (
                    ringAccountBuilder.nameStatus == RingQtQuick.NameDirectory.NOT_FOUND ?
                        "#66ff66" : "white"
                )
                font.bold: ringAccountBuilder.nameLookupError || (
                    ringAccountBuilder.nameStatus == RingQtQuick.NameDirectory.NOT_FOUND
                )
                Layout.fillHeight: true
                Layout.fillWidth: true
                Behavior on color {
                    ColorAnimation {duration: 100}
                }
            }
        }

        Controls.Switch {
            id: usePassword
            text: i18n("Protect your crypto keys with a password")
            opacity: 1
            enabled: !ringAccountBuilder.registerRingName
            Layout.fillWidth: true
            checked: ringAccountBuilder.usePassword
            contentItem: Text {
                text: usePassword.text
                font: usePassword.font
                color: "white"
                verticalAlignment: Text.AlignVCenter
                leftPadding: usePassword.indicator.width + usePassword.spacing
            }
        }

        Controls.Label {
            id: label1
            text: i18n("Enter an archive password")
            opacity: ringAccountBuilder.usePassword ? 1 : 0
            enabled: ringAccountBuilder.usePassword
            color: "white"
            Layout.fillWidth: true
            anchors.leftMargin: 8
            Behavior on opacity {
                NumberAnimation {
                    easing.type: Easing.OutQuad
                    duration: 200
                }
            }
        }

        Controls.TextField {
            id: passwordTxt
            echoMode: "Password"
            opacity: ringAccountBuilder.usePassword ? 1 : 0
            enabled: ringAccountBuilder.usePassword
            text: ringAccountBuilder.password
            Layout.fillWidth: true
            color: "white"
            Behavior on opacity {
                NumberAnimation {
                    easing.type: Easing.OutQuad
                    duration: 200
                }
            }
        }

        Controls.Label {
            id: label
            color: "white"
            opacity: ringAccountBuilder.usePassword ? 1 : 0
            enabled: ringAccountBuilder.usePassword
            text: i18n("Repeat the new password")
            Layout.fillWidth: true
            anchors.leftMargin: 8
            Behavior on opacity {
                NumberAnimation {
                    easing.type: Easing.OutQuad
                    duration: 200
                }
            }
        }

        Controls.TextField {
            id: repeatPasswordTxt
            echoMode: "Password"
            opacity: ringAccountBuilder.usePassword ? 1 : 0
            enabled: ringAccountBuilder.usePassword
            Layout.fillWidth: true
            text: ringAccountBuilder.repeatPassword
            color: "white"
            Behavior on opacity {
                NumberAnimation {
                    easing.type: Easing.OutQuad
                    duration: 200
                }
            }
        }

        Controls.Label {
            id: label4
            color: "#ff1111"
            font.bold: true
            text: i18n("Passwords don't match")
            verticalAlignment: Text.AlignVCenter
            Layout.fillHeight: true
            Layout.fillWidth: true
            opacity: ringAccountBuilder.passwordMatch ? 0 : 1

            Behavior on opacity {
                NumberAnimation {
                    easing.type: Easing.OutQuad
                    duration: 200
                }
            }
        }

        Item {
            height: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
        }

        Controls.Label {
            id: labelInfo
            color: "white"
            text: i18n("(*) registered usernames can be reached by their username-string instead of their generated ring-id number only.")
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        Rectangle {
            Layout.fillHeight: true
        }
    }

    Rectangle{
        id: registrationPopup

        width: Math.min(applicationWindow.width*0.8, implicitWidth + 50)
        height: popupLayout.implicitHeight + 10
        anchors.centerIn: createRing
        color: "#0886a0"

        visible: ringAccountBuilder.creating
        z: 200

        RowLayout {
            id: popupLayout
            anchors.verticalCenter: parent.verticalCenter

            Controls.BusyIndicator {
                id: registrationIndicator
                Layout.fillHeight: false
                visible: ringAccountBuilder.creating
            }

            Controls.Label {
                id: registrationStatus
                text: ringAccountBuilder.creationStatusMessage
                wrapMode: Text.WordWrap
                 verticalAlignment: Text.AlignVCenter
                Layout.fillHeight: true
                Layout.fillWidth: true
                color: "black"
            }
        }
    }
}
