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
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Ring 1.0

Rectangle {
    property QtObject call: null

    property alias textColor: messageTextArea.color
    property alias backgroundColor: chatBox.color
    property var emojiColor: undefined

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
                        color:  emojiColor
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
