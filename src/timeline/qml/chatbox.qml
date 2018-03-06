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

    function focusEdit() {
        messageTextArea.forceActiveFocus()
    }

    id: chatBox

    signal sendMessage(string message, string richMessage)

    color: "blue"

    SystemPalette {
        id: inactivePalette
        colorGroup: SystemPalette.Disabled
    }

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

    Rectangle {
        id: emojiButton

        property bool checked: false

        opacity: 0
        radius: 999
        width: 30
        height: 30

        anchors.bottomMargin: -15
        anchors.bottom: parent.top
        anchors.horizontalCenter: parent.horizontalCenter

        color: "transparent"
        border.width: 2
        border.color: inactivePalette.text

        Text {
            anchors.centerIn: parent
            text: "😀"
            font.family: "Noto Color Emoji"
            font.pixelSize : 18
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                emojiButton.checked = !emojiButton.checked
            }
        }

        Behavior on opacity {
            NumberAnimation {duration: 100; easing.type: Easing.InQuad}
        }

        Behavior on anchors.bottomMargin {
            NumberAnimation {duration: 100; easing.type: Easing.InQuad}
        }

        states: [
            State {
                name: "checked"
                when: emojiButton.checked
                PropertyChanges {
                    target: emojiButton
                    color:  "#00AA00"
                }
            }
        ]
    }

    Loader {
        id: emojis
        visible: false
        anchors.fill: parent
        active: visible
        sourceComponent: Grid {
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
                        onClicked: {
                            messageTextArea.insert(messageTextArea.length, symbol)
                            emojiButton.checked = false
                        }
                    }
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
            textFormat: TextEdit.RichText

            font.family: "Noto Color Emoji"
            font.pixelSize : 18

            placeholderText: i18n("Write a message and press enter...")

            Keys.onReturnPressed: {
                var rawText  = getText(0, length)
                var richText = getFormattedText(0, length)

                sendMessage(rawText, richText)
            }

            persistentSelection: true

            states: [
                State {
                    name: "focus"
                    when: messageTextArea.cursorVisible
                        || chatBox.state == "emoji"
                        || emojis.visible == true
                    PropertyChanges {
                        target: emojiButton
                        opacity: 1
                        anchors.bottomMargin: 0
                    }
                }
            ]
        }
        Button {
            text: "Send"
            Layout.fillHeight: true
            onClicked: {
                var rawText  = messageTextArea.getText(0, messageTextArea.length)
                var richText = messageTextArea.getFormattedText(0, messageTextArea.length)

                sendMessage(rawText, richText)
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
