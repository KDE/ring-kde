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
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import org.kde.kirigami 2.2 as Kirigami
import net.lvindustries.ringqtquick 1.0 as RingQtQuick

Item {
    id: searchBox

    property bool empty: search.text == ""
    property var searchView: null
    property real xPadding: 2*Kirigami.Units.largeSpacing +2 /*border*/
    property alias searchFocus: search.focus
    property alias labelWidth: findLabel.implicitWidth

    Kirigami.Theme.colorSet: Kirigami.Theme.View

    function forceFocus() {
        search.forceActiveFocus(Qt.OtherFocusReason)
    }

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

        // Auto complete an auto completion == fail
        inputMethodHints: Qt.ImhNoPredictiveText

        width: focus ? searchView.width - xPadding : parent.width
        x: focus ? -(searchView.width-searchBox.width ) + xPadding: 0
        y: (focus ? searchBox.height : 0) + Kirigami.Units.largeSpacing
        leftPadding: Kirigami.Units.largeSpacing + 1 /*border*/
        topPadding: Kirigami.Units.largeSpacing

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
                border.color: Kirigami.Theme.textColor
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
                    y: search.text == "" && !search.focus ? 0 : -search.height
                    x: search.focus ? (searchView.width-searchBox.width)/2 : 0
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
                        Layout.alignment: Qt.AlignVCenter
                        sourceSize.height: parent.height - 6
                        sourceSize.width: parent.height - 6
                        source: "image://SymbolicColorizer/image://icon/file-search-symbolic"
                    }

                    Text {
                        id: findLabel
                        Layout.fillWidth: true
                        color: Kirigami.Theme.textColor
                        text: i18n("Find someone")
                        Layout.alignment: Qt.AlignVCenter
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
            var cm = searchView.currentItem.contactMethod

            // Display an error message when the selected element doesn't exist
            if (!searchView.currentItem.isSelectable) {
                searchView.displayNotFoundMessage()
                return
            }

            if (!cm)
                return

            cm = RingSession.individualDirectory.fromTemporary(cm)

            mainPage.currentContactMethod = cm

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
