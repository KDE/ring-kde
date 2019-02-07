/***************************************************************************
 *   Copyright (C) 2019 by Bluesystems                                     *
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
import QtQuick 2.9
import org.kde.kirigami 2.2 as Kirigami

Column {
    property QtObject timelineIterator: null
    property int buttonCount: 1
    spacing: Kirigami.Units.largeSpacing
    property real buttonSize: Kirigami.Units.iconSizes.large
    width: buttonCount > 0 ? buttonSize : 0
    height: buttonCount * buttonSize + buttonCount*spacing
    visible: opacity > 0
    opacity: buttonCount > 0 ? 1 : 0
    z: 200

    Behavior on opacity {
        NumberAnimation {duration: 200; easing.type: Easing.InQuad}
    }

    Rectangle {
        color: Kirigami.Theme.highlightColor
        radius: 99
        height: buttonSize
        width: buttonSize
        opacity: chatView.contentHeight - (chatView.contentY + chatView.height) < 10 ? 0 : 1
        visible: opacity > 0
        border.width: 2
        border.color: Kirigami.Theme.highlightedTextColor
        Kirigami.Icon {
            source: "go-down"
            color: Kirigami.Theme.highlightedTextColor
            width: Kirigami.Units.iconSizes.smallMedium
            height: Kirigami.Units.iconSizes.smallMedium
            anchors.centerIn: parent
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                timelineIterator.proposeNewest()
            }
        }
    }
}
