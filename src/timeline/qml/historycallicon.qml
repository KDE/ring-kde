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
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

import net.lvindustries.ringqtquick 1.0 as RingQtQuick

Item {
    width:  30
    height: 30
    Image {
        function selectIcon(isMissed, direction) {
            if (isMissed && direction == RingQtQuick.Call.INCOMING)
                return "sharedassets/phone_dark/missed_incoming.svg"
            else if (isMissed && direction == RingQtQuick.Call.OUTGOING)
                return "sharedassets/phone_dark/missed_outgoing.svg"
            else if (direction == RingQtQuick.Call.INCOMING)
                return "sharedassets/phone_dark/incoming.svg"
            else
                return "sharedassets/phone_dark/outgoing.svg"
        }

        source: selectIcon(object.missed, object.direction)
        asynchronous: true
        Layout.fillHeight: true
        width:  30
        height: 30
    }
}
