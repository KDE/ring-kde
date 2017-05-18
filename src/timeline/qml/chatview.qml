import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Ring 1.0

import RingQmlWidgets 1.0

ListView {

    delegate: Item {
        width: parent.width

        height: 80

        RowLayout {
            anchors.fill: parent

            Item {
                visible: direction == 0
                Layout.preferredWidth: parent.width*0.3
                Layout.minimumWidth: parent.width*0.3
                Layout.maximumWidth: parent.width*0.3
            }

            Rectangle {
                width: 50
                height: 50
                color: "gray"
                radius: 5
                visible: direction == 1
            }

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true

                Bubble {
                    anchors.fill: parent
                    anchors.margins: 5

                    alignment: direction == 0 ? Text.AlignRight : Text.AlignLeft
                    color: isRead ? "green" : "red"

                    Text {
                        anchors.fill: parent
                        anchors.leftMargin: 30
                        anchors.rightMargin: 30
                        anchors.topMargin: 5
                        anchors.bottomMargin: 5
                        horizontalAlignment: direction == 0 ? Text.AlignRight : Text.AlignLeft
                        font.family: "Noto Color Emoji"
                        text: display
                    }

                    Text {
                        anchors.bottom: parent.bottom
                        anchors.left: direction == 0 ? parent.left : undefined
                        anchors.right: direction == 1 ? parent.right : undefined
                        anchors.bottomMargin: 4
                        anchors.leftMargin: direction == 0 ? 4 : undefined
                        anchors.rightMargin: direction == 1 ? 4 : undefined
                        text: formattedDate
                        color: "gray"
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            console.log("SET isRead")
                            isRead = true
                        }
                    }
                }
            }

            Item {
                visible: direction == 1
                Layout.preferredWidth: parent.width*0.3
                Layout.minimumWidth: parent.width*0.3
                Layout.maximumWidth: parent.width*0.3
            }

            Rectangle {
                width: 50
                height: 50
                color: "gray"
                radius: 5
                visible: direction == 0
            }
        }
    }
}
