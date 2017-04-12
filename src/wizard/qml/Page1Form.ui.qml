import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Page {
    id: frontPage
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
        width: 423
        height: 86
        text: qsTr("Welcome to Ring-KDE. Before you can contact your friend,
you have to have an account. Don't worry, creating one is easy and
doesn't require sharing any personal information. If you are in an
office or your phone service provider offers a SIP account, you
can also configure Ring-KDE to take your \"real\" phone calls")
        wrapMode: Text.WordWrap
        font.pixelSize: 12
        color: "white"
    }

    Button {
        id: createRingAccount
        anchors.horizontalCenter: parent.horizontalCenter
        y: 246
        text: qsTr("Create a new GNU Ring account")
    }

    Button {
        id: importRingAccount
        anchors.horizontalCenter: parent.horizontalCenter
        y: 300
        text: qsTr("Import an existing account")
    }

    Button {
        id: createAnonRingAccount
        anchors.horizontalCenter: parent.horizontalCenter
        y: 354
        text: qsTr("Use an anonymous account")
        visible: false
    }

    Button {
        id: importSIPAccount
        anchors.horizontalCenter: parent.horizontalCenter
        y: 408
        text: qsTr("Import a SIP account")
        visible: false
    }

    Button {
        id: createIp2IPAccount
        anchors.horizontalCenter: parent.horizontalCenter
        y: 462
        text: qsTr("Use on local network")
        visible: false
    }

    Image {
        id: image
        anchors.horizontalCenter: parent.horizontalCenter
        y: 43
        width: 100
        height: 100
        source: "ring-kde.svg"
    }

    CreateRing {
        id: createRing
        x: -20
        y: text1.height + 8
        opacity: 0
        visible: false
        width: logo.width + text1.width
    }

    ImportRing {
        id: importRing
        x: -20
        y: text1.height + 8
        opacity: 0
        visible: false
        width: logo.width + text1.width
    }

    ProfilePage {
        id: profilePage
        visible: false
        y: text1.height + 8
        height: parent.height - text1.height - 8 -45/*footer.height*/
        width: parent.width
        anchors.bottomMargin: 45/*footer.height*/
        anchors.topMargin: 12
    }
}
