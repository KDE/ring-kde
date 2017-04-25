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
import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Ring 1.0

Item {
    id: item1

    property alias registerUserName: registerUserName

    property bool nextAvailable: false
    property bool busy: false

    property var account: null

    signal registrationCompleted()

    function isNextAvailable() {
        nextAvailable = (userName.text.length > 0 || !registerUserName.checked)
            && (password.text.length > 0)
            && (password.text == repeatPassword.text)
    }

    function createAccount() {
        if (!nextAvailable) {
            console.log("Account creation failed: missing fields")
            return;
        }

        // Prevent trigger happy people from DDOSing the daemon with entropy
        // starvation
        nextAvailable = false
        busy = true

        // Display the progress popup
        state = "registrationResult"
        registrationTimeout.running = true

        var name = userName.text == "" ?
            WelcomeDialog.defaultUserName : userName.text

        // Make sure they are unique
        name = name + AccountModel.getSimilarAliasIndex(name)

        account = AccountModel.add(name, Account.RING);
        account.displayName     = name
        account.archivePassword = password.text
        account.upnpEnabled     = true;

        account.performAction(Account.SAVE)
        account.performAction(Account.RELOAD)
    }

    ColumnLayout {
        anchors.fill: parent

        Switch {
            id: registerUserName
            height: 40
            text: qsTr("Register public username (experimental)")
            checked: true
            opacity: 1
            Layout.fillWidth: true
            onCheckedChanged: isNextAvailable()
        }

        Label {
            id: label2
            clip: true
            x: -7
            y: 170
            height: 14
            text: qsTr("Enter an username")
            anchors.leftMargin: 8
            anchors.left: parent.left
            Layout.fillWidth: true

            /*Behavior on Layout.maximumHeight {
                NumberAnimation {
                    easing.type: Easing.OutQuad
                    duration: 500
                }
            }*/
        }

        TextField {
            id: userName
            clip: true
            x: 8
            y: 74
            text: WelcomeDialog.defaultUserName
            height: 40
            Layout.fillWidth: true

            /*Behavior on Layout.maximumHeight {
                NumberAnimation {
                    easing.type: Easing.OutQuad
                    duration: 500
                }
            }*/

            onTextChanged: {
                isNextAvailable()
                busyIndicator.visible = true
                NameDirectory.lookupName(undefined, "", userName.text)
            }
        }

        RowLayout {
            id: rowLayout
            clip: true
            x: 44
            y: 127
            height: 37
            Layout.fillHeight: false
            Layout.maximumHeight: 37
            Layout.fillWidth: true
            spacing: 6

            Item {
                width: 37
                height: 37
                Layout.fillHeight: true
                Layout.preferredWidth: 37

                BusyIndicator {
                    id: busyIndicator
                    anchors.fill: parent
                    visible: false
                }

                Image {
                    id: image
                    anchors.fill: parent
                }
            }

            Label {
                id: registerFoundLabel
                text: qsTr("Please enter an username")
                verticalAlignment: Text.AlignVCenter
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            /*Behavior on Layout.maximumHeight {
                NumberAnimation {
                    easing.type: Easing.OutQuad
                    duration: 500
                }
            }*/
        }

        Label {
            id: label1
            x: -1
            y: 233
            height: 14
            text: qsTr("Enter an archive password")
            Layout.fillWidth: true
            anchors.leftMargin: 8
            anchors.left: parent.left
        }

        TextField {
            id: password
            x: 8
            y: 187
            height: 40
            echoMode: "Password"
            Layout.fillWidth: true
            onTextChanged: isNextAvailable()
        }

        Label {
            id: label
            x: -1
            y: 54
            height: 14
            text: qsTr("Repeat the new password")
            Layout.fillWidth: true
            anchors.leftMargin: 8
        }

        TextField {
            id: repeatPassword
            echoMode: "Password"
            x: 8
            y: 253
            height: 40
            Layout.fillWidth: true
            onTextChanged: isNextAvailable()
        }

        Label {
            id: label4
            color: "red"
            text: qsTr("Passwords don't match")
            verticalAlignment: Text.AlignVCenter
            Layout.fillHeight: true
            Layout.fillWidth: true
            opacity: 0

            // Only show when the passwords mismatch
            states: [
                State
                {
                    name: "invisible"
                    when: repeatPassword.text != password.text
                    PropertyChanges {
                        target: label4
                        opacity: 1
                    }
                }
            ]

            /*Behavior on opacity {
                NumberAnimation {
                    easing.type: Easing.OutQuad
                    duration: 200
                }
            }*/
        }

        Rectangle {
            Layout.fillHeight: true
        }
    }

    Rectangle {
        id: registrationPopup
        width: 200
        height: 75
        color: "#eeeeee"
        visible: false
        z: 200
        anchors.centerIn: item1
        RowLayout {
            anchors.verticalCenter: parent.verticalCenter

            BusyIndicator {
                id: registrationIndicator
                Layout.fillHeight: false
            }

            Label {
                id: registrationStatus
                text: qsTr("Creating account")
                Layout.fillHeight: false
                Layout.fillWidth: true
                color: "black"
            }
        }
    }

    // Hide the error message after a second
    Timer {
        id: hidePopup
        repeat: false
        running: false
        interval: 1000
        onTriggered: {
            registrationPopup.visible = false
        }
    }

    // Remove the popup after 30 seconds if it didn't finish by then
    Timer {
        id: registrationTimeout
        repeat: false
        interval: 30000
        running: false
        onTriggered: {
            registrationPopup.color = "red"
            registerFoundLabel.text = qsTr("Timeout")
            hidePopup.running       = true
            nextAvailable = true
            busy = false
        }
    }

    states: [
        State {
            name: "noRegister"
            when: !registerUserName.checked

            PropertyChanges {
                target: label2
                Layout.maximumHeight: 0
                opacity: 0
            }

            PropertyChanges {
                target: userName
                Layout.maximumHeight: 0
                opacity: 0
            }

            PropertyChanges {
                target: rowLayout
                Layout.maximumHeight: 0
                opacity: 0
            }

            PropertyChanges {
                target: label
                padding: 0
            }
        },
        State {
            name: "registrationResult"

            PropertyChanges {
                target: registrationPopup
                visible: true
            }
        }
    ]

    /*transitions: Transition {
        NumberAnimation {
            properties: "opacity"
            easing.type: Easing.OutQuad
            duration: 500
            onStopped: {
                label2.visible = false
                userName.visible = false
                rowLayout.visible = false
            }
        }
    }*/

    // After each username field change, check availability
    Connections {
        target: NameDirectory
        onRegisteredNameFound: {
            //  Race conditions
            if (userName.text != name)
                return;

            busyIndicator.visible = false
            if (status == 2) { //NameDirectory.NOT_FOUND
                registerFoundLabel.text = qsTr("The username is available")
                registerFoundLabel.color = "green"
            }
            else if (name == "") {
                registerFoundLabel.text = qsTr("Please enter an username")
                registerFoundLabel.color = ""
                nextAvailable = false
            }
            else {
                registerFoundLabel.text = qsTr("The username is not available")
                registerFoundLabel.color = "red"
                nextAvailable = false
            }
        }
    }

    Connections {
        target: NameDirectory
        onNameRegistrationEnded: {
            registrationTimeout.stop()

            if (status == 0)
                registrationPopup.color = "green"
            else {
                registrationPopup.color = "red"
                hidePopup.running = true
            }

            registrationIndicator.visible = false

            switch(status) {
                case 0: //SUCCESS
                    registrationStatus.text = qsTr("Success")
                    busy = false
                    item1.registrationCompleted()
                    break
                case 1: //WRONG_PASSWORD
                    registrationStatus.text = qsTr("Password mismatch")
                    break
                case 2: //INVALID_NAME
                    registrationStatus.text = qsTr("Invalid name")
                    break
                case 3: //ALREADY_TAKEN
                    registrationStatus.text = qsTr("Already taken")
                    break
                case 4: //NETWORK_ERROR
                    registrationStatus.text = qsTr("Network error")
                    break
            }
        }
    }

    Connections {
        target: account
        onStateChanged: {
            if (state == Account.READY && registerUserName.checked) {
                if (account.registerName(password.text, account.displayName)) {
                    registrationStatus.text = qsTr("Registration")
                    registrationTimeout.stop()
                }
                else {
                    registrationPopup.color = "red"
                    registrationStatus.text = qsTr("Can't register")
                    registrationTimeout.stop()
                }
                account = null
            }
        }
    }
}
