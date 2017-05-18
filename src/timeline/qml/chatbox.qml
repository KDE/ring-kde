import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Ring 1.0

Rectangle {
    property QtObject call: null
    id: chatBox

    signal sendMessage(string message)

    color: "blue"

    ListModel {
        id: emoji
        ListElement { symbol: "😀" } ListElement { symbol: "😁" } ListElement { symbol: "😂" }
        ListElement { symbol: "😃" } ListElement { symbol: "😄" } ListElement { symbol: "😅" }
        ListElement { symbol: "😆" } ListElement { symbol: "😇" } ListElement { symbol: "😈" }
        ListElement { symbol: "😉" } ListElement { symbol: "😊" } ListElement { symbol: "😋" }
        ListElement { symbol: "😌" } ListElement { symbol: "😍" } ListElement { symbol: "😎" }
        ListElement { symbol: "😏" } ListElement { symbol: "😐" } ListElement { symbol: "😑" }
        ListElement { symbol: "😒" } ListElement { symbol: "😓" } ListElement { symbol: "😔" }
        ListElement { symbol: "😕" } ListElement { symbol: "😖" } ListElement { symbol: "😗" }
        ListElement { symbol: "😘" } ListElement { symbol: "😙" } ListElement { symbol: "😚" }
        ListElement { symbol: "😛" } ListElement { symbol: "😜" } ListElement { symbol: "😝" }
        ListElement { symbol: "😞" } ListElement { symbol: "😟" } ListElement { symbol: "😠" }
        ListElement { symbol: "😡" } ListElement { symbol: "😢" } ListElement { symbol: "😣" }
        ListElement { symbol: "😤" } ListElement { symbol: "😥" } ListElement { symbol: "😦" }
        ListElement { symbol: "😧" } ListElement { symbol: "😨" } ListElement { symbol: "😩" }
        ListElement { symbol: "😪" } ListElement { symbol: "😫" } ListElement { symbol: "😬" }
        ListElement { symbol: "😭" } ListElement { symbol: "😮" } ListElement { symbol: "😯" }
        ListElement { symbol: "😰" } ListElement { symbol: "😱" } ListElement { symbol: "😲" }
        ListElement { symbol: "😳" } ListElement { symbol: "😴" } ListElement { symbol: "😵" }
        ListElement { symbol: "😶" } ListElement { symbol: "😷" } ListElement { symbol: "😸" }
        ListElement { symbol: "😹" } ListElement { symbol: "😺" } ListElement { symbol: "😻" }
        ListElement { symbol: "😼" } ListElement { symbol: "😽" } ListElement { symbol: "😾" }
        ListElement { symbol: "😿" } ListElement { symbol: "🙀" } ListElement { symbol: "🙁" }
        ListElement { symbol: "🙂" } ListElement { symbol: "🙃" } ListElement { symbol: "🙄" }
        ListElement { symbol: "🙅" } ListElement { symbol: "🙆" } ListElement { symbol: "🙇" }
        ListElement { symbol: "🙈" } ListElement { symbol: "🙉" } ListElement { symbol: "🙊" }
        ListElement { symbol: "🙋" } ListElement { symbol: "🙌" } ListElement { symbol: "🙍" }
        ListElement { symbol: "🙎" } ListElement { symbol: "🙏" }
    }

    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            Button {
                text: "Bold"
            }
            Button {
                text: "Italic"
            }
            Button {
                text: "Underline"
            }
            Item {
                Layout.fillWidth: true
            }
            Button {
                id: emojiButton
                text: "😀"
                font.family: "Noto Color Emoji"
                font.pixelSize : 18
                checkable: true
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Grid {
                id: emojis
                visible: false
                anchors.fill: parent
                spacing: 2
                rows: 2

                Repeater {
                    model: emoji
                    Rectangle {
                        width:  30
                        height: 30
                        color:  "green"
                        radius: 2

                        Text {
                            anchors.centerIn: parent
                            font.family: "Noto Color Emoji"
                            font.pixelSize : 18
                            text: symbol
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: messageTextArea.text += symbol
                        }
                    }
                }
            }

            RowLayout {
                id: textMessagePanel
                anchors.fill: parent

                TextArea {
                    id: messageTextArea

                    Layout.fillHeight: true
                    Layout.fillWidth:  true

                    font.family: "Noto Color Emoji"
                    font.pixelSize : 18

                    placeholderText: qsTr("Write a message and press enter...")

                    Keys.onReturnPressed: sendMessage(text)
                }
                Button {
                    text: "Send"
                    Layout.fillHeight: true
                    onClicked: sendMessage(messageTextArea.text)
                }
            }
        }
    }

    StateGroup {
        id: chatStateGroup

        states: [
            State {
                name: "text"
                when: !emojiButton.checked
                PropertyChanges {
                    target: messageTextArea
                    focus:  true
                }
            },

            State {
                name: "emoji"
                when: emojiButton.checked
                PropertyChanges {
                    target:  textMessagePanel
                    visible: false
                }
                PropertyChanges {
                    target:  emojis
                    visible: true
                }
            }
        ]
    }

    Connections {
        target: chatBox
        onSendMessage: {
            console.log(message)
            messageTextArea.text = ""
        }
    }
}
