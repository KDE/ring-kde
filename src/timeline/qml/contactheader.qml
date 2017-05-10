import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Ring 1.0

Rectangle {
    property var currentContactMethod: null

    onCurrentContactMethodChanged: {
        primaryName.text = currentContactMethod.primaryName
    }

    color: "gray"
    height: 100
    Layout.fillWidth: true

    RowLayout {
        anchors.fill: parent
        anchors.margins: 8

        Rectangle {
            radius: 5
            height: 90
            width: 90
            color: "white"
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Text {
                id: primaryName
                font.bold: true
                font.pointSize: 16
                text: "My name"
                Layout.fillWidth: true
            }

            RowLayout {
                Button {
                    id: button
                    text: qsTr("Call")
                    onClicked: {
                        if (currentContactMethod == null) return
                        CallModel.dialingCall(currentContactMethod)
                            .performAction(Call.ACCEPT)
                    }
                }

                Button {
                    id: button2
                    text: qsTr("Video")
                    onClicked: {
                        if (currentContactMethod == null) return
                        CallModel.dialingCall(currentContactMethod)
                            .performAction(Call.ACCEPT)
                    }
                }

                Button {
                    text: qsTr("Screen sharing")
                    onClicked: {
                        if (currentContactMethod == null) return
                        CallModel.dialingCall(currentContactMethod)
                            .performAction(Call.ACCEPT)
                    }
                }
            }
        }
    }
}
