/***************************************************************************
 *   Copyright (C) 2018 by Bluesystems                                     *
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

Item {
    property var recentView: null
    property real xPadding: 0
    property alias searchFocus: search.focus
    property alias labelWidth: findLabel.implicitWidth
    height: search.focus ? 60 : 30

    signal contactMethodSelected(var cm)

    function hide() {
        search.text = ""
        search.focus = false
    }

    TextField {
        id: search
        width: parent.width - (focus ? 0 : parent.xPadding)
        x: focus ? 0 : parent.xPadding
        y: focus ? 30 : 0

        font.pixelSize: height * 0.55
        text: CallModel.hasDialingCall ?
            CallModel.dialingCall().dialNumber : ""

        Behavior on x {
            NumberAnimation {duration: 300}
        }

        Behavior on y {
            NumberAnimation {duration: 150}
        }

        Behavior on width {
            NumberAnimation {duration: 300}
        }

        background : Item {
            height: 30
            Rectangle {
                height: 28 // 1px border
                width: parent.width
                color: "transparent"
                border.width: 1
                border.color: activePalette.text
                opacity: search.focus ? 0.8 : 0.5
                radius: search.text == "" && !search.focus ? 99 : 5

                Behavior on opacity {
                    NumberAnimation {duration: 100}
                }

                Behavior on radius {
                    NumberAnimation {duration: 200;  easing.type: Easing.OutQuad}
                }

                RowLayout {
                    height: parent.height
//                     opacity: search.text == "" && !search.focus ? 1 : 0
                    y: search.text == "" && !search.focus ? 0 : -search.height
                    spacing: 5

                    Behavior on opacity {
                        NumberAnimation {duration: 100}
                    }

                    Behavior on y {
                        NumberAnimation {duration: 200}
                    }

                    Item {
                        width: 2
                    }

                    Image {
                        width: parent.height - 6
                        height: parent.height - 6
                        anchors.verticalCenter: parent.verticalCenter
                        sourceSize.height: parent.height - 6
                        sourceSize.width: parent.height - 6
                        source: "image://icon/file-search-symbolic"
                    }
                    Text {
                        id: findLabel
                        Layout.fillWidth: true
                        color: activePalette.text
                        text: i18n("Find someone")
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: parent.height * 0.55
                    }
                }
            }
        }

        onTextChanged: {
            // Cache the text to avoid a binding loop when the dialing call
            // is created for the first time
            var text = search.text
            var call = CallModel.dialingCall()
            call.dialNumber = text
        }
        Keys.onDownPressed: {
            recentView.currentIndex = (recentView.currentIndex == recentView.count - 1) ?
                0 : recentView.currentIndex + 1
        }
        Keys.onUpPressed: {
            recentView.currentIndex = (recentView.currentIndex == 0) ?
                recentView.count - 1 : recentView.currentIndex - 1
        }
        Keys.onReturnPressed: {
            if (searchStateGroup.state != "searchActive")
                return

            var cm = recentView.currentItem.contactMethod

            if (!cm)
                return

            contactMethodSelected(cm)
        }
        Keys.onEscapePressed: {
            search.text = ""
            search.focus = false
        }

        Keys.onPressed: {
            if (event.key == Qt.Key_Backspace && search.text == "")
                search.focus = false
        }
    }
}
