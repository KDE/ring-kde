import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Ring 1.0

Item {
    id: item1

    property alias registerUserName: registerUserName

    property bool nextAvailable: false

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

        var account = AccountModel.add(userName.text, Account.RING);
        account.displayName     = userName.text
        account.archivePassword = password.text
        account.upnpEnabled     = true;

        account.performAction(Account.SAVE)
        account.performAction(Account.RELOAD)

        if (registerUserName.checked) {
            var hasStarted = account.registerName(account.archivePassword, account.displayName)
            console.log("START",hasStarted)
        }
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
        onNameRegistrationEnded: {}
    }
}
