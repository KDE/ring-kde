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
import org.kde.ringkde.genericutils 1.0 as GenericUtils

/**
 * Add a visual queue when the user has to accept / reject / neglect contact
 * requests.
 *
 * It's orange because if it's not annoying enough, the user will ignore it and
 * the peer will think Ring is broken when he fails to call / text. But Ring-KDE
 * also warn about that (but not all clients are created equally).
 */
Rectangle {
    radius: 5
    color: Kirigami.Theme.neutralTextColor
    height: content.implicitHeight + 40
    opacity: 0.8

    ColumnLayout {
        id: content
        y: 10
        x: 10
        width: parent.width - 20

        Image {
            height: 32
            Layout.fillWidth: true
            fillMode: Image.PreserveAspectFit
            width: parent.width
            sourceSize.width: 32
            sourceSize.height: 32
            horizontalAlignment: Image.AlignHCenter
            source: "image://SymbolicColorizer/:/sharedassets/outline/warning.svg"
        }

        Text {
            Layout.fillWidth: true
            text: i18n("You have pending contact requests:")
            color: activePalette.text
            wrapMode: Text.WordWrap
            width: parent.width
        }

        Rectangle {
            color: "transparent"
            border.width: 1
            border.color: activePalette.text
            height: buttonText.implicitHeight + 15
            radius: 5

            Layout.fillWidth: true
            anchors.margins: 10

            Text {
                id: buttonText
                anchors.centerIn: parent
                color: activePalette.text
                text: i18n("View all contact requests")
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    applicationWindow().showContactRequests()
                }
            }
        }
    }
}
