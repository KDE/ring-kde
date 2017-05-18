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
import Ring 1.0

Item {
    property var currentContactMethod: null

    onCurrentContactMethodChanged: currentContactMethod ?
        callsModels.model = currentContactMethod.callsModel : null

    function selectIcon(isMissed, direction) {
        if (isMissed && direction == Call.INCOMING)
            return "sharedassets/phone_dark/missed_incoming.svg"
        else if (isMissed && direction == Call.OUTGOING)
            return "sharedassets/phone_dark/missed_outgoing.svg"
        else if (direction == Call.INCOMING)
            return "sharedassets/phone_dark/incoming.svg"
        else
            return "sharedassets/phone_dark/outgoing.svg"
    }

    Component {
        id: historyDelegate
        Item {
            height: 30
            width: parent.width

            RowLayout {
                Image {
                    source: selectIcon(missed, direction)
                    Layout.fillHeight: true
                    width: 30
                }
                Column {
                    Text { text: formattedDate }
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Text {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 5
                text: length
            }
        }
    }

    ListView {
        id: callsModels
        anchors.fill: parent
        delegate: historyDelegate
    }
}
