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
import QtQuick.Layouts 1.0
import org.kde.kirigami 2.2 as Kirigami

Loader {
    id: loader
    height: 200// item ? item.height : 0
    opacity: 0.7
    active: displayTips.showSearchTip
    sourceComponent: Item {
        height: outline.height + 20
        anchors.left: loader.left
        anchors.right: loader.right

        MouseArea {
            anchors.fill: parent
            onClicked: {
                displayTips.showSearchTip = false
            }
        }

        Rectangle {
            id: outline
            border.width: 1
            radius: 5
            color: "transparent"
            border.color: activePalette.text
            anchors.margins: 15
            anchors.left: parent.left
            anchors.right: parent.right
            height: content.implicitHeight + 10

            ColumnLayout {
                id: content
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 5
                spacing: 6

                Item {
                    Layout.fillWidth: true
                    height: 1
                }

                Item {
                    Layout.fillWidth: true
                    height: button.height
                    Rectangle {
                        id: button
                        height: label.implicitHeight + 10
                        width: label.implicitWidth + 10
                        anchors.right: parent.right
                        border.width: 1
                        radius: 5
                        color: "transparent"
                        border.color: activePalette.text
                        Text {
                            id: label
                            color: activePalette.text
                            text: i18n("Got it")
                            anchors.centerIn: parent
                        }
                    }
                }

                Text {
                    color: activePalette.text
                    Layout.fillWidth: true
                    textFormat: Text.RichText
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignJustify
                    text: i18n("The search results come from 4 different sources. It first look in the <b style=\"color:#be3411;\">history</b>, the <b style=\"color:#cfa02a;\">bookmarks</b> and the <b style=\"color:#14883b;\">contacts</b>. If there is no perfect match it then query the <b style=\"color:#2c53bd;\">registered name database</b>. The source of the result is indicated using a colored circle on the left side of the result. Note that accessing the registered name database requires sending the search string to the server.")
                }
            }
        }
    }
}
