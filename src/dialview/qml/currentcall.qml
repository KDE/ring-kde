/******************************************************************************
 *   Copyright (C) 2018 by Bluesystems                                        *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                      *
 *                                                                            *
 *   This library is free software; you can redistribute it and/or            *
 *   modify it under the terms of the GNU Lesser General Public               *
 *   License as published by the Free Software Foundation; either             *
 *   version 2.1 of the License, or (at your option) any later version.       *
 *                                                                            *
 *   This library is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU        *
 *   Lesser General Public License for more details.                          *
 *                                                                            *
 *   You should have received a copy of the Lesser GNU General Public License *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *****************************************************************************/
import QtQuick 2.0


/*
 * This module display a 5 seconds message before the call is removed.
 *
 * It is intended to prevent the case where the call "just disappear" because
 * it cut or the peer hung up unexpectedly. This way, the user isn't confused
 * as what just happened.
 */
Column {
    property QtObject call: null

    spacing: 6

    Text {
        text: call.length
        color: selected ? activePalette.highlightedText : activePalette.text
        anchors.horizontalCenter: parent.horizontalCenter
        font.bold: true
    }

    Item {
        height: 1
        width: parent.width
    }

    //TODO, recording and hold
}
