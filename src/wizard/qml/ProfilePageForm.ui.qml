import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Item {
    id: item1

    ColumnLayout {
        anchors.fill: parent


        Label {
            id: label2
            clip: true
            x: -7
            y: 170
            width: 284
            height: 14
            text: qsTr("Fill the form to share informations with your peers. Those details are not public.
They are trasnmissted everytime you and a peer talk.")
            wrapMode: Text.WordWrap
            anchors.leftMargin: 8
            anchors.left: parent.left
            Layout.fillWidth: true
        }

        Rectangle {
            width: 100
            height: 100
            Text {
                text: "PHOTO\nPLACEHOLDER"
                anchors.fill: parent
            }
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            id: label3
            clip: true
            x: -7
            y: 170
            width: 284
            height: 14
            text: qsTr("The name displayed during conversations")
            anchors.leftMargin: 8
            anchors.left: parent.left
            Layout.fillWidth: true
            anchors.horizontalCenter: parent.horizontalCenter
        }

        TextField {
            id: userName
            clip: true
            x: 8
            y: 74
            width: 284
            height: 40
            Layout.fillWidth: true
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            id: label1
            x: -1
            y: 233
            width: 284
            height: 14
            text: qsTr("Phone number")
            Layout.fillWidth: true
            anchors.leftMargin: 8
            anchors.left: parent.left
            anchors.horizontalCenter: parent.horizontalCenter
        }

        TextField {
            id: password
            x: 8
            y: 187
            width: 284
            height: 40
            Layout.fillWidth: true
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            id: label
            x: -1
            y: 54
            width: 284
            height: 14
            text: qsTr("Full name")
            Layout.fillWidth: true
            anchors.leftMargin: 8
            anchors.horizontalCenter: parent.horizontalCenter
        }

        TextField {
            id: repeatPassword
            x: 8
            y: 253
            width: 284
            height: 40
            Layout.fillWidth: true
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
