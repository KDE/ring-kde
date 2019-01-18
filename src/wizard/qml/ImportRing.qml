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
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import org.kde.ringkde.jamiwizard 1.0 as JamiWizard
import Ring 1.0

Item {
    id: importRing

    property bool nextAvailable: false
    property var account: null

    signal registrationCompleted()

    width: Math.min(createForm.implicitWidth, parent.width - 20)
    height: createForm.implicitHeight

    JamiWizard.WelcomeDialog {
        id: welcomeDialog
    }

    function isNextAvailable() {
        nextAvailable = pinField.text.length > 0 && password.text.length > 0
    }

    function createAccount() {
        if (!nextAvailable) {
            console.log("Account creation failed: missing fields")
            return;
        }

        registrationPopup.visible   = true
        registrationTimeout.running = true
        registrationPopup.color     = "white"

        account = RingSession.accountModel.add(welcomeDialog.defaultUserName, RingQtQuick.Account.RING)
        account.archivePassword = password.text
        account.archivePin      = pinField.text
        account.upnpEnabled     = true;

        account.performAction(RingQtQuick.Account.SAVE)
        account.performAction(RingQtQuick.Account.RELOAD)
    }

    ColumnLayout {
        anchors.fill: parent

        Label {
            id: label2
            clip: false
            text: i18n("Link this device")
            verticalAlignment: Text.AlignVCenter
            color: "white"
            padding: 0
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
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

        Label {
            id: label1
            text: i18n("Enter your main Ring account password")
            color: "white"
            Layout.fillWidth: true
            anchors.leftMargin: 8
            anchors.left: parent.left
        }

        TextField {
            id: password
            echoMode: "Password"
            Layout.fillWidth: true
            onTextChanged: isNextAvailable()
        }

        Label {
            id: label
            wrapMode: "WordWrap"
            color: "white"
            text: i18n("Enter the PIN number from another configured Ring account. Use the \"Export account on Ring\" feature to obtain a PIN.")
            Layout.fillWidth: true
            anchors.leftMargin: 8
        }

        TextField {
            id: pinField
            echoMode: "Password"
            Layout.fillWidth: true
            onTextChanged: isNextAvailable()
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
                text: i18n("Importing account")
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
            registerFoundLabel.text = i18n("Timeout")
            hidePopup.running       = true
        }
    }

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

    Connections {
        target: account
        onStateChanged: {
            switch(state) {
                case RingQtQuick.Account.ERROR:
                    registrationPopup.color = "red"
                    registrationStatus.text = i18n("Importing failed")
                    hidePopup.running = true
                    registrationTimeout.stop()
                    break
                case RingQtQuick.Account.READY:
                    registrationPopup.visible = false
                    registrationTimeout.stop()
                    importRing.registrationCompleted()
                    account = null
                    break
            }
        }
    }
}
