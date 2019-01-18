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


Column {
    id: outgoingCall
    property QtObject call: null

    spacing: 6

    Ringing {
        height: 32
        width: 32
        anchors.horizontalCenter: parent.horizontalCenter
        running: true
        visible: true
    }

    Item {
        height: 4
        width: parent.width
    }
}
