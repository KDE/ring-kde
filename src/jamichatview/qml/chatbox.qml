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
import org.kde.kirigami 2.2 as Kirigami
import QtQuick.Layouts 1.0

Item {
    property bool requireContactRequest: false

    height: Math.max(messageTextArea.implicitHeight, emojis.optimalHeight)
    implicitHeight: height

    Behavior on height {
        NumberAnimation {duration: 200;  easing.type: Easing.OutQuad}
    }

    function focusEdit() {
        // Forcing the focus shows the keyboard and hide the toolbar. This
        // isn't usable. It would be fine if the controls were usable, but it
        // isn't the case for for no this is disabled.
        if (!Kirigami.Settings.isMobile)
            messageTextArea.forceActiveFocus()
    }

    id: chatBox

    signal sendMessage(string message, string richMessage)

    Rectangle {
        id: emojiButton

        property bool checked: false

        opacity: 0
        radius: 999
        width:  Kirigami.Settings.isMobile ? 50 : 30
        height: Kirigami.Settings.isMobile ? 50 : 30
        visible: opacity > 0

        anchors.bottomMargin: -15
        anchors.bottom: parent.top
        anchors.horizontalCenter: parent.horizontalCenter

        color: Kirigami.Theme.backgroundColor
        border.width: 2
        border.color: Kirigami.Theme.disabledTextColor

        Text {
            anchors.fill: parent
            text: "😀"
            color: Kirigami.Theme.textColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.family: "Noto Color Emoji"
            font.pixelSize : Kirigami.Settings.isMobile ? 24 : 18
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
                    color: Kirigami.Theme.highlightColor
                }
            }
        ]
    }

    Loader {
        id: emojis
        visible: false
        anchors.fill: parent
        property real optimalHeight: item && visible ? item.implicitHeight : 0

        /**
         * Only load once, then keep alive because otherwise it takes like 2
         * seconds each time on mobile.
         */
        active: (Kirigami.Settings.isMobile && active) || visible

        sourceComponent: Grid {
            id: grid
            height: parent.height
            anchors.centerIn: emojis
            spacing: 0
            width: Math.ceil(emoji.count/rows) * maxWidth*1.2
            rows: Math.ceil((emoji.count*maxWidth*1.2)/width)

            property real maxWidth: 0

            Repeater {
                model: ListModel {
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

                MouseArea {
                    width:  1.3*maxWidth
                    height: 2*emojiTxt.contentHeight

                    Text {
                        id: emojiTxt
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.family: "Noto Color Emoji"
                        color: Kirigami.Theme.textColor
                        font.pixelSize : Kirigami.Settings.isMobile ? 24 : 18
                        text: symbol
                        Component.onCompleted: maxWidth = Math.max(maxWidth, emojiTxt.contentWidth)
                    }

                    onClicked: {
                        messageTextArea.insert(messageTextArea.length, symbol)
                        emojiButton.checked = false
                    }
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            id: textMessagePanel

            Layout.fillHeight: true
            Layout.fillWidth : true
            spacing: 0

            TextArea {
                id: messageTextArea

                Layout.fillHeight: true
                Layout.fillWidth:  true
                textFormat: TextEdit.RichText
                wrapMode: TextEdit.WordWrap

                font.family: "Noto Color Emoji"
                font.pixelSize : 18

                placeholderText: " "+i18n("Write a message and press enter...")

                Keys.onReturnPressed: {
                    var rawText  = getText(0, length)
                    var richText = getFormattedText(0, length)

                    sendMessage(rawText, richText)
                }

                Keys.onEscapePressed: {
                    console.log("escape")
                    focus = false
                }

                background: Rectangle {
                    color: Kirigami.Theme.backgroundColor
                    anchors.fill: parent
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
            Kirigami.Separator {
                Layout.fillHeight: true
            }
            Button {
                text: i18n("Send")
                Layout.fillHeight: true
                onClicked: {
                    var rawText  = messageTextArea.getText(0, messageTextArea.length)
                    var richText = messageTextArea.getFormattedText(0, messageTextArea.length)

                    sendMessage(rawText, richText)
                }
                background: Rectangle {
                    color: Kirigami.Theme.buttonBackgroundColor
                    anchors.fill: parent
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
