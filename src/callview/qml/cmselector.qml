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
    property var individual: null
    property var callback: undefined

    parent: applicationWindow().contentItem
    x: applicationWindow().contentItem.width / 2 - width/2
    y: applicationWindow().contentItem.height / 2 - height/2
    width: applicationWindow().contentItem.width * 0.5
    height: content.implicitHeight + 100

    standardButtons: Dialog.Cancel | Dialog.Apply
    modal: true

    onAccepted: {
        if (phoneNumbers.callback)
            phoneNumbers.callback(numbers.currentItem.cm)
    }

    property string text: i18n("This contact has multiple phone numbers, please select one below.")

    clip: true

    contentItem: ColumnLayout {
        id: content

        Text {
            id: label
            wrapMode: Text.WordWrap
            text: phoneNumbers.text
            color: Kirigami.Theme.textColor
            Layout.fillWidth: true
        }

        ListView {
            id: numbers
            Layout.margins: 3
            Layout.topMargin: Kirigami.Units.spacing
            model: individual
            interactive: false
            currentIndex: individual ? individual.defaultIndex.row : -1
            Layout.fillWidth: true
            spacing: Kirigami.Units.spacing

            Layout.preferredHeight: contentHeight

            highlight: Rectangle {
                color: Kirigami.Theme.highlightColor
            }

            delegate: MouseArea {
                property var cm: object
                id: delegate
                height: columns.implicitHeight + 2*Kirigami.Units.largeSpacing
                width: parent.width
                implicitHeight: columns.implicitHeight + 2*Kirigami.Units.largeSpacing

                onClicked: {
                    numbers.currentIndex = index
                }

                onDoubleClicked: {
                    if (phoneNumbers.callback)
                        phoneNumbers.callback(object)
                    phoneNumbers.close()
                }

                GridLayout {
                    id: columns
                    rows: 2
                    columns: 3
                    width: parent.width

                    anchors.verticalCenter: parent.verticalCenter

                    Kirigami.Icon {
                        id: icon
                        width: Kirigami.Units.iconSizes.smallMedium
                        height: width
                        source: "call-start"
                        Layout.leftMargin: height/3
                        Layout.alignment: Qt.AlignVCenter
                        Layout.rowSpan: 2
                        color:numbers.currentIndex == index ?
                            Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
                    }

                    Text {
                        text: display
                        color:numbers.currentIndex == index ?
                            Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
                    }

                    Text {
                        text: account ? (i18n(" (Account: ") + account.alias+ ")") : ""
                        color: numbers.currentIndex == index ?
                            Kirigami.Theme.highlightedTextColor : Kirigami.Theme.disabledTextColor
                        Layout.fillWidth: true
                    }

                    Text {
                        Layout.columnSpan: 2
                        text: lastUsed == undefined || lastUsed == "" ? i18n("Never used") :
                            i18n("Used ")+totalCallCount+i18n(" time (Last used on: ") + formattedLastUsed + ")"
                        color: numbers.currentIndex == index ?
                            Kirigami.Theme.highlightedTextColor : "#2980b9"
                        Layout.fillWidth: true
                    }
                }
            }
        }
    }
}
