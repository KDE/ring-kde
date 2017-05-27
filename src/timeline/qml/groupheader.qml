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

RowLayout {
    property string type: "text"

    Item {
        Layout.preferredWidth: 5
    }

    function getIcon() {
        if (type == "text")
            return "image://icon/dialog-messages"
        else
            return "image://icon/call-start"
    }

    Rectangle {
        height: 30
        width: 30
        radius: 99
        border.width: 3
        border.color: "white"
        color: "transparent"

        Image {
            anchors.margins: 6
            anchors.fill: parent
            source: getIcon()
        }
    }

    Item {
        Layout.preferredWidth: 10
    }

    Text {
        text: display
        color: activePalette.text
    }
}
