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
import org.kde.kirigami 2.2 as Kirigami

Column {
    property QtObject call: null

    Item {
        width: 1
        height: 5
    }

    Image {
        height: 22
        fillMode: Image.PreserveAspectFit
        width: parent.width
        sourceSize.width: 32
        sourceSize.height: 32
        horizontalAlignment: Image.AlignHCenter
        source: "image://SymbolicColorizer/:/sharedassets/phone_dark/missed_incoming.svg"
        opacity: 0.5
    }

    Item {
        height: 3
    }

    Text {
        horizontalAlignment: Text.AlignHCenter
        text: i18n("<center><b>Missed call</b></center> <br />from: ")+ display + "<br /><br />"+formattedDate
        color: Kirigami.Theme.textColor
        wrapMode: Text.WordWrap
        width: parent.width
    }

    Item {
        width: 1
        height: 5
    }

}
