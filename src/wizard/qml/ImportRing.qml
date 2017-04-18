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
    id: importRing

    property bool nextAvailable: false

    function isNextAvailable() {
        nextAvailable = pinField.text.length > 0 && password.text.length > 0
    }

    function createAccount() {
        if (!nextAvailable) {
            console.log("Account creation failed: missing fields")
            return;
        }

        var account = AccountModel.add(WelcomeDialog.defaultUserName, Account.RING)
        account.archivePassword = password.text
        account.archivePin      = pinField.text
        account.upnpEnabled     = true;

        account.performAction(Account.SAVE)
        account.performAction(Account.RELOAD)
    }

    ColumnLayout {
        anchors.fill: parent

        Label {
            id: label2
            clip: false
            text: qsTr("Link this device")
            verticalAlignment: Text.AlignVCenter
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
            text: qsTr("Enter your main Ring account password")
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
            text: qsTr("Enter the PIN number form another configured Ring account. Use the \"Export account on Ring\" feature to obtain a PIN.")
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
}
