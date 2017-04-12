import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Item {
    id: buttonBar

    property alias backButton: backButton
    property alias nextButton: nextButton
    property alias skipButton: skipButton

    Rectangle {
        id: rectangle
        color: "#0886a0"
        anchors.fill: parent

        // Left buttons
        RowLayout {
            anchors.fill: parent

            // Align left
            Button {
                id: backButton
                Layout.fillHeight: true
                text: qsTr("Back")
                //visible: false //FIXME
            }

            Item {
                Layout.fillWidth: true
            }

            // Align right

            Label {
                id: missingFields
                text: qsTr("Please fill the required fields")
                verticalAlignment: Text.AlignVCenter
                color: "red"
                visible: true
                Layout.fillHeight: true
            }

            Button {
                id: nextButton
                Layout.fillHeight: true
                text: qsTr("Next")
                visible: false
            }

            Button {
                id: skipButton
                Layout.fillHeight: true
                text: qsTr("Skip")
            }
        }
    }
    states: [
        State {
            name: "shown"

            PropertyChanges {
                target: buttonBar
                visible: true
                y: buttonBar.parent.height - buttonBar.height
                state: "nextAvailable"
            }
        },

        State {
            name: "locked"

            PropertyChanges {
                target: buttonBar
                anchors.right: buttonBar.parent.right
                anchors.left: buttonBar.parent.left
                anchors.bottom: buttonBar.parent.bottom
                anchors.leftMargin: 0
                visible: true
            }
        },
        State {
            name: "firstStep"

            PropertyChanges {
                target: backButton
                visible: false
            }
        },
        State {
            name: "nextAvailable"
            extend: "locked"
            PropertyChanges {
                target: nextButton
                visible: true
            }
            PropertyChanges {
                target: missingFields
                visible: false
            }
        },

        State {
            name: "finish"
            extend: "nextAvailable"
            PropertyChanges {
                target: skipButton
                visible: false
            }

            PropertyChanges {
                target: nextButton
                text: "Finish"
            }
        }
    ]

    /*transitions: [
        Transition {
            from: "*"; to: "shown"

            // Slide-in the new account form
            NumberAnimation {
                target: buttonBar
                easing.type: Easing.OutQuad
                properties: "y";
                duration: 700
            }
        }
    ]*/
}
