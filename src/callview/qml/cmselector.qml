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

/**
 * A flexible way to handle the corder case when the ContactMethod cannot be
 * selected automatically.
 */
Dialog {
    id: phoneNumbers
    property var currentIndividual: null
    property var callback: undefined

    parent: applicationWindow().contentItem
    x: applicationWindow().contentItem.width / 2 - width/2
    y: applicationWindow().contentItem.height / 2 - height/2
    width: applicationWindow().contentItem.width * 0.5
    height: applicationWindow().contentItem.height * 0.5

    standardButtons: Dialog.Cancel | Dialog.Apply
    modal: true

    property string text: i18n("This contact has multiple phone numbers, please select one below.")

    clip: true

    Text {
        id: label
        wrapMode: Text.WordWrap
        text: phoneNumbers.text
        color: Kirigami.Theme.textColor
    }

    ListView {
        id: numbers
        anchors.fill: parent
        anchors.margins: 3
        anchors.topMargin: label.implicitHeight + 10
        model: currentIndividual
        currentIndex: currentIndividual.defaultIndex.row

        delegate: Rectangle {
            id: delegate
            radius: 3
            color: "transparent"
            border.color: "transparent"
            border.width: 1
            height: readOnly.height
            width: parent.width
            implicitHeight: readOnly.height

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    numbers.currentIndex = index
                }

                onDoubleClicked: {
                    if (phoneNumbers.callback)
                        phoneNumbers.callback(object)
                    phoneNumbers.close()
                }
            }

            states: [
                State {
                    name: ""
                    PropertyChanges {
                        target: delegate
                        border.color: "transparent"
                        color: "transparent"
                    }
                },
                State {
                    name: "selected"
                    when: numbers.currentItem == delegate
                    PropertyChanges {
                        target: delegate
                        border.color: Kirigami.Theme.highlightedTextColor
                        color: Kirigami.Theme.highlightColor
                    }
                },
                State {
                    name: "hover"
                    when: mouseArea.containsMouse
                    PropertyChanges {
                        target: delegate
                        border.color: Kirigami.Theme.highlightedTextColor
                    }
                }
            ]

            RowLayout {
                id: readOnly
                anchors.leftMargin: 10
                anchors.fill: parent
                height: columns.implicitHeight + 30 // 30 == 3*spacing
                implicitHeight: columns.implicitHeight + 30
                spacing: 10

                ColumnLayout {
                    id: columns
                    Layout.fillWidth: true
                    Text {
                        text: display
                        color: Kirigami.Theme.textColor
                        Layout.fillWidth: true
                    }

                    Text {
                        text: lastUsed == undefined || lastUsed == "" ? i18n("Never used") :
                            i18n("Used ")+totalCallCount+i18n(" time (Last used on: ") + formattedLastUsed + ")"
                        color: Kirigami.Theme.textColor
                        Layout.fillWidth: true
                    }
                }
            }
        }
    }
}
