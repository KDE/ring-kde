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
import QtQuick.Controls 2.0
import org.kde.kirigami 2.2 as Kirigami

/**
 * When chatting with a Ring contact, display a "you need to send a friend
 * request" button. It's a checkbox and is checked by default so remove some
 * burden from the user.
 */
Rectangle {
    radius: 5
    color: Kirigami.Theme.neutralTextColor
    height: content.implicitHeight + 6
    implicitHeight: content.implicitHeight + 6
    opacity: 0.8
    property alias sendRequests: checkbox.checked

    signal disableContactRequests(bool send)

    RowLayout {
        id: content
        y: 3
        x: 10
        width: parent.width - 20

        CheckBox {
            id: checkbox
            Layout.fillWidth: true
            text: i18n("Send a friend request first")
            checked: true
        }

        Rectangle {
            color: "transparent"
            border.width: 1
            border.color: activePalette.text
            height: buttonText.implicitHeight + 15
            width: buttonText.implicitWidth + 20
            radius: 5

            Layout.fillWidth: true

            Text {
                id: buttonText
                anchors.centerIn: parent
                color: activePalette.text
                text: i18n("Dismiss")
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    disableContactRequests(checkbox.checked)
                }
            }
        }
    }
}
