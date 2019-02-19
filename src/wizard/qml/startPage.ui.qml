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
import QtQuick.Controls 2.0 as Controls
import org.kde.ringkde.jamiwizard 1.0 as JamiWizard

Controls.Page {
    id: frontPage

    property real bottomMargin: 0

    Rectangle {
        anchors.fill: parent
        x: -1
        color: "#004d61"
    }

    property alias importRingAccount: importRingAccount
    property alias createRingAccount: createRingAccount
    property alias createAnonRingAccount: createAnonRingAccount
    property alias importSIPAccount: importSIPAccount
    property alias createIp2IPAccount: createIp2IPAccount
    property alias createRing: createRing
    property alias importRing: importRing
    property alias profilePage: profilePage
    property alias welcomeMessage: text1
    property alias logo: image

    Text {
        id: text1
        anchors.horizontalCenter: parent.horizontalCenter
        y: 149
        width: Math.min(423, frontPage.width - 114)
        height: 86
        text: i18n("Welcome to Ring-KDE. Before you can contact your friend, you have to have an account. Don't worry, creating one is easy and doesn't require sharing any personal information. If you are in an office or your phone service provider offers a SIP account, you can also configure Ring-KDE to take your \"real\" phone calls.")
        wrapMode: Text.WordWrap
        font.pixelSize: 12
        color: "white"
    }

    Controls.Button {
        id: createRingAccount
        anchors.horizontalCenter: parent.horizontalCenter
        y: 246
        text: i18n("Create a new GNU Ring account")
    }

    Controls.Button {
        id: importRingAccount
        anchors.horizontalCenter: parent.horizontalCenter
        y: 300
        text: i18n("Import an existing account")
    }

    Controls.Button {
        id: createAnonRingAccount
        anchors.horizontalCenter: parent.horizontalCenter
        y: 354
        text: i18n("Use an anonymous account")
        visible: false
    }

    Controls.Button {
        id: importSIPAccount
        anchors.horizontalCenter: parent.horizontalCenter
        y: 408
        text: i18n("Import a SIP account")
        visible: false
    }

    Controls.Button {
        id: createIp2IPAccount
        anchors.horizontalCenter: parent.horizontalCenter
        y: 462
        text: i18n("Use on local network")
        visible: false
    }

    Image {
        id: image
        anchors.horizontalCenter: parent.horizontalCenter
        y: 43
        width: 100
        height: 100
        source: "qrc:/wizard/ring-kde.svg"
        sourceSize.width: 100
        sourceSize.height: 100
    }

    JamiWizard.CreateRing {
        id: createRing
        anchors.top: parent.top
        anchors.topMargin: 120 //the logo height is 100
        anchors.bottomMargin: frontPage.bottomMargin + 10
        anchors.horizontalCenter: parent.horizontalCenter
        opacity: 0
        visible: false
    }

    JamiWizard.ImportRing {
        id: importRing
        opacity: 0
        visible: false
        width: logo.width + text1.width
        x: frontPage.width/2 - width/2 - 20
        y: frontPage.height/2 - height/2
    }

    JamiWizard.ProfilePage {
        id: profilePage
        visible: false
        y: text1.height + 8
        height: parent.height - text1.height - 8 - frontPage.bottomMargin
        width: parent.width
        anchors.bottomMargin: frontPage.bottomMargin
        anchors.topMargin: 12
    }
}
