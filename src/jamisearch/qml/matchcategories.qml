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
import org.kde.kirigami 2.0 as Kirigami

Loader {
    clip: true
    height: 30 + Kirigami.Units.fontMetrics.height*1.5

    Behavior on opacity {
        NumberAnimation {duration: 150;  easing.type: Easing.InQuad}
    }

    sourceComponent: Row {
        anchors.fill: parent
        Repeater {
            model: ListModel {
                ListElement {
                    name: "Web"
                    elemColor: "#2c53bd"
                    src: "image://SymbolicColorizer/?color=#2c53bd;:/sharedassets/outline/web.svg"
                }
                ListElement {
                    name: "Bookmarks"
                    elemColor: "#cfa02a"
                    src: "image://SymbolicColorizer/?color=#cfa02a;:/searchassets/bookmark.svg"
                }
                ListElement {
                    name: "Contacts"
                    elemColor: "#14883b"
                    src: "image://SymbolicColorizer/?color=#14883b;:/searchassets/contact.svg"
                }
                ListElement {
                    name: "History"
                    elemColor: "#be3411"
                    src: "image://SymbolicColorizer/?color=#be3411;:/searchassets/history.svg"
                }
            }

            Item {
                width: parent.width / 4
                height: sourceName.implicitHeight + 34
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 30
                    height: 30
                    radius: 99
                    border.width: 2
                    border.color: elemColor
                    color: "transparent"
                    Image {
                        width: parent.width*0.8
                        height: width
                        anchors.centerIn:parent
                        sourceSize.width: width
                        sourceSize.height: width
                        source: src
                    }
                }
                Text {
                    id: sourceName
                    anchors.bottom: parent.bottom
                    anchors.topMargin: 4
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: Kirigami.Theme.highlightedTextColor
                    text: name
                }
            }
        }
    }
}
