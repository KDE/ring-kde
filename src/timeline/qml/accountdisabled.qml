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
import RingQmlWidgets 1.0
import QtQuick.Controls 2.2
import net.lvindustries.ringqtquick 1.0 as RingQtQuick

/**
 * Add a very obvious warning and reload button to make the "/!\ No accounts"
 * error messages in the header less confusing.
 */
Rectangle {
    radius: 5
    color: Kirigami.Theme.negativeTextColor
    height: content.implicitHeight + 20
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
            text: i18n("All accounts are currently disabled. Please click on an account below to enable it:")
            color: activePalette.text
            wrapMode: Text.WordWrap
            width: parent.width
        }

        ListView {
            model: RingSession.accountModel
            interactive: false
            height: contentHeight
            implicitHeight: contentHeight
            width: parent.width
            x: 10
            delegate: Rectangle {
                color: "transparent"
                border.width: 1
                border.color: activePalette.text
                height: buttonText.implicitHeight + 15
                width: content.width
                radius: 5

                Layout.fillWidth: true
                anchors.margins: 10

                Text {
                    id: buttonText
                    anchors.centerIn: parent
                    color: activePalette.text
                    text: object.alias
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        object.enabled = true
                        object.performAction(RingQtQuick.Account.SAVE)
                    }
                }
            }
        }
    }
}
