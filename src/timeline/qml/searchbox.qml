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
import org.kde.kirigami 2.2 as Kirigami
import net.lvindustries.ringqtquick 1.0 as RingQtQuick

import Ring 1.0

Item {
    id: searchBox

    property bool empty: search.text == ""
    property var searchView: null
    property real xPadding: 0
    property alias searchFocus: search.focus
    property alias labelWidth: findLabel.implicitWidth
    height: (search.focus ? textMetric.height*3 : textMetric.height*1.5) + 6

    function hide() {
        search.text = ""
        search.focus = false
        if (RingSession.callModel.hasDialingCall)
            RingSession.callModel.dialingCall().performAction(RingQtQuick.Call.REFUSE)
    }

    FontMetrics {
        id: textMetric
        font: search.font
    }

    TextField {
        id: search
        width: parent.width - (focus ? 0 : parent.xPadding)
        x: focus ? 0 : parent.xPadding
        y: focus ? textMetric.height*1.5 + 3 : 3


        font.pointSize: Kirigami.Theme.defaultFont.pointSize*1.4
        text: RingSession.callModel.hasDialingCall ?
            RingSession.callModel.dialingCall().dialNumber : ""

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
            height: textMetric.height*1.5
            Rectangle {
                height: parent.height - 2 // 1px border
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
                        source: "image://SymbolicColorizer/image://icon/file-search-symbolic"
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

            if (RingSession.callModel.hasDialingCall || text != "") {
                var call = RingSession.callModel.dialingCall()
                call.dialNumber = text
            }
        }
        Keys.onDownPressed: {
            searchView.currentIndex = (searchView.currentIndex == searchView.count - 1) ?
                0 : searchView.currentIndex + 1
        }
        Keys.onUpPressed: {
            searchView.currentIndex = (searchView.currentIndex == 0) ?
                searchView.count - 1 : searchView.currentIndex - 1
        }
        Keys.onReturnPressed: {
            if (searchStateGroup.state == "")
                return

            var cm = searchView.currentItem.contactMethod

            // Display an error message when the selected element doesn't exist
            if (!searchView.currentItem.isSelectable) {
                displayNotFoundMessage()
                return
            }

            if (!cm)
                return

            cm = RingSession.individualDirectory.fromTemporary(cm)

            contactMethodSelected(cm)
            searchBox.hide()
        }
        Keys.onEscapePressed: {
            searchBox.hide()
        }
        Keys.onPressed: {
            if (event.key == Qt.Key_Backspace && search.text == "")
                searchBox.hide()
        }
    }
}
