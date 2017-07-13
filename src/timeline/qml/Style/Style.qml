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
pragma Singleton
import QtQuick 2.7

QtObject {
    id: style

    property SystemPalette activePalette: new SystemPalette()

    property FontMetrics chatBubbleMetrics: new FontMetrics("Noto Color Emoji")


    function bubbleRect(text, maximum) {
//         if (!this.chatBubbleMetrics) {
//             this.chatBubbleMetrics = new FontMetrics
//         }

        var rect = style.chatBubbleMetrics.boundingRect(text)

        if (rect.width > maximum*0.66) {
            var row = Math.ceil(rect.width/(maximum*0.66))
            rect.width   = maximum*0.66
            rect.height += row*chatBubbleMetrics.height
        }

        rect.width  = Math.max(style.dateWidth, rect.width)
        rect.height = Math.max(rect.height, 50)

        return rect
    }

    property var  bubbleBackground: blendColor()
    property var  bubbleForeground: ""
    property var  unreadBackground: ""
    property var  unreadForeground: ""
    property int  dateWidth: 0
    property date currentDate: new Date()

    function blendColor() {
        var base2 = activePalette.highlight
        base2     = Qt.rgba(base2.r, base2.g, base2.b, 0.3)
        var base1 = Qt.tint(activePalette.base, base2)

        chatView.bubbleBackground = base1
        chatView.unreadBackground = Qt.tint(activePalette.base, "#33BB0000")
        chatView.bubbleForeground = activePalette.text
        chatView.unreadForeground = activePalette.text

        return base1
    }

    Component.onCompleted: {
//         if (!this.chatBubbleMetrics) {
//             this.chatBubbleMetrics = new FontMetrics
//         }
        style.dateWidth = style.chatBubbleMetrics.boundingRect(currentDate.toLocaleDateString()) + 100
        chatBubbleMetrics.font.family = "Noto Color Emoji"
        activePalette.colorGroup = SystemPalette.Active
    }
}
