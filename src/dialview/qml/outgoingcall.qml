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
import Ring 1.0

Column {
    id: outgoingCall
    property QtObject call: null

    spacing: 6

    Text {
        text: call.toHumanStateName
        color: activePalette.text
        wrapMode: Text.WordWrap
        font.bold: true
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Progress {
        call: outgoingCall.call
        displayLabels: false
        width: parent.width*0.66
        outlineColor: activePalette.text
        anchors.horizontalCenter: parent.horizontalCenter
        backgroundColor: selected ? activePalette.highlight : activePalette.base
    }

    Item {
        height: 1
        width: parent.width
    }
}
