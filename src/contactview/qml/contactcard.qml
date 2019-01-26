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

import org.kde.ringkde.jamicontactview 1.0 as JamiContactView

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
        border.color: Kirigami.Theme.textColor
        color: "transparent"
        anchors.fill: parent
        radius: 5

        RowLayout {
            anchors.topMargin: 5
            anchors.bottomMargin: 5
            anchors.fill: parent
            Item {
                Layout.preferredWidth: 48
                Layout.fillHeight: true
                JamiContactView.ContactPhoto {
                    width:  36
                    height: 36

                    anchors.centerIn: parent
                    anchors.margins: 5
                    person: personObj
                }
            }

            ColumnLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Text {
                    Layout.fillWidth: true
                    id: displayNameLabel
                    text: display
                    color: Kirigami.Theme.textColor
                    font.bold: true
                }
                Text {
                    Layout.fillWidth: true
                    visible: !individual.hasPhoneNumbers
                    color: Kirigami.Theme.textColor
                    text: i18n("There is no way to reach this contact")
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
                    selectionCallback(index, object, rootIndex)
                else if (treeView != undefined) { //FIXME move elsewhere
                    treeView.selectItem(rootIndex)
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
