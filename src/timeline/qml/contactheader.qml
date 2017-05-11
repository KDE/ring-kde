import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Ring 1.0

Rectangle {
    property var currentContactMethod: null

    onCurrentContactMethodChanged: {
        primaryName.text = currentContactMethod.primaryName
        bookmarkSwitch.source = currentContactMethod.bookmarked ?
            "icons/bookmarked.svg" : "icons/not_bookmarked.svg"
    }

    /*Connections {
        target: currentContactMethod
        onBookmarked: {
            bookmarkSwitch.source = currentContactMethod.bookmarked ?
                "icons/bookmarked.svg" : "icons/not_bookmarked.svg"
        }
    }*/

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

            RowLayout {
                Text {
                    id: primaryName
                    font.bold: true
                    font.pointSize: 16
                    text: "My name"
                }
                Image {
                    id: bookmarkSwitch
                    anchors.rightMargin: 1
                    anchors.topMargin: 3
                    height: 16
                    width: 16
                    source: currentContactMethod.bookmarked ? "icons/bookmarked.svg" : "icons/not_bookmarked.svg"
                    z: 100
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            mouse.accepted = true
                            currentContactMethod.bookmarked = !currentContactMethod.bookmarked
                            bookmarkSwitch.source = currentContactMethod.bookmarked ?
                                "icons/bookmarked.svg" : "icons/not_bookmarked.svg"
                        }
                    }
                }
                Item {
                    Layout.fillWidth: true
                }
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
