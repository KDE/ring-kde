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
import QtQuick.Layouts 1.0
import RingQmlWidgets 1.0
import Ring 1.0
import ContactView 1.0

Item {
    height: rows.implicitHeight + 10 //10 == 2*margins
    Rectangle {
        anchors.margins: 5
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        border.width: 1
        border.color: inactivePalette.text
        color: "transparent"
        anchors.fill: parent
        radius: 5

        RowLayout {
            id: rows
            anchors.topMargin: 5
            anchors.bottomMargin: 5
            anchors.fill: parent
            Item {
                width:  48
                height: 48
                anchors.verticalCenter: parent.verticalCenter
                ContactPhoto {
                    anchors.fill: parent
                    anchors.margins: 5
                    contactMethod: object
                }
            }

            ColumnLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Text {
                    Layout.fillWidth: true
                    id: displayNameLabel
                    text: display
                    color: activePalette.text
                    font.bold: true
                }

                Text {
                    Layout.fillWidth: true
                    color: inactivePalette.text
                    text: uri
                }

                Item {
                    Layout.fillHeight: true
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                treeView.selectItem(modelIndex)
                bookmarkList.contactMethodSelected(object)
            }
        }
    }
}
