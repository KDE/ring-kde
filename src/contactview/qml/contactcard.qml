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
import QtQuick.Controls 2.0
import QtQml.Models 2.2
import RingQmlWidgets 1.0
import Ring 1.0
import ContactView 1.0

Item {
    width: parent.width
    height: 56 + rowCount*(displayNameLabel.implicitHeight+4) + 10

    property var selectionCallback: undefined
    property var rightControls: undefined

    Component.onCompleted: {
        if (!rightControls)
            return

        var widget = rightControls.createObject(parent, {
            "anchors.right" : parent.right,
            "widget.anchors" : parent.verticalCenter,
            "visible" : true

        })
        widget.anchors.right = parent.right
        widget.anchors.verticalCenter = parent.verticalCenter
    }

    // Support both ContactRequests and Person
    property var personObj: objectType != Ring.Person ? person : object

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
                    person: personObj
                }

//                 // Allow to set/remove bookmarks
//                 Image {
//                     id: bookmarkSwitch
//                     anchors.top: parent.top
//                     anchors.right: parent.right
//                     anchors.rightMargin: 1
//                     anchors.topMargin: 3
//                     height: 16
//                     width: 16
//                     source: isBookmarked ? "icons/bookmarked.svg" : "icons/not_bookmarked.svg"
//                     z: 100
//                     MouseArea {
//                         anchors.fill: parent
//                         onClicked: {
//                             mouse.accepted = true
//                             isBookmarked = !isBookmarked
//                             bookmarkSwitch.source = isBookmarked ?
//                                 "icons/bookmarked.svg" : "icons/not_bookmarked.svg"
//                         }
//                     }
//                 }
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
                    visible: rowCount == 0
                    color: inactivePalette.text
                    text: "TODO"
                }
                Item {
                    Layout.fillHeight: true
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (selectionCallback)
                    selectionCallback(index, object, modelIndex)
                else if (treeView != undefined) { //FIXME move elsewhere
                    treeView.selectItem(modelIndex)
                    if (objectType == 0)
                        contactList.contactMethodSelected(
                            object.lastUsedContactMethod
                        )
                    if (objectType == 1)
                        contactList.contactMethodSelected(object)
                }
            }
        }
    }
}
