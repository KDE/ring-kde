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
import org.kde.kirigami 2.2 as Kirigami

RowLayout {
    property string type: "text"

    // This information has no value 99% of the time, so fade it away
    opacity: chatView.displayExtraTime ? 1 : 0.1

    Behavior on opacity {
        NumberAnimation {duration: 500}
    }

    Item {
        Layout.preferredWidth: 5
    }

    function getIcon() {
        if (type == "text")
            return "dialog-messages"
        else
            return "call-start"
    }

    Rectangle {
        height: 30
        width: 30
        radius: 99
        border.width: 1
        border.color: Kirigami.Theme.disabledTextColor
        color: "transparent"

        Kirigami.Icon {
            anchors.margins: 6
            anchors.fill: parent
            color: Kirigami.Theme.disabledTextColor
            source: getIcon()
        }
    }

    Item {
        Layout.preferredWidth: 10
    }

    Text {
        text: display
        color: Kirigami.Theme.textColor
    }
}
