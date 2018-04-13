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

Rectangle {
    property var unsortedListView: null

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    SystemPalette {
        id: inactivePalette
        colorGroup: SystemPalette.Disabled
    }

    function selectIcon(isMissed, direction) {
        if (isMissed && direction == Event.INCOMING)
            return "sharedassets/phone_dark/missed_incoming.svg"
        else if (isMissed && direction == Event.OUTGOING)
            return "sharedassets/phone_dark/missed_outgoing.svg"
        else if (direction == Event.INCOMING)
            return "sharedassets/phone_dark/incoming.svg"
        else
            return "sharedassets/phone_dark/outgoing.svg"
    }

    color: activePalette.base

    Component {
        id: historyDelegate
        Item {
            height: hasAVRecording ? 60 : 30
            width: parent.width

            RowLayout {
                height: 30
                anchors.top: parent.top
                anchors.left: parent.left
                Image {
                    source: selectIcon(status == Event.X_MISSED, direction)
                    Layout.fillHeight: true
                    width: 30
                }
                Column {
                    Text {
                        text: formattedLastUsed
                        color: activePalette.text
                    }
                    anchors.verticalCenter: parent.verticalCenter
                }
                Item {
                    Layout.fillWidth: true
                }
                Text {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.margins: 5
                    text: length
                    color: inactivePalette.text
                }
            }

            Loader {
                property QtObject call: object

                width: parent.width
                height: hasAVRecording ? 30 : 0
                anchors.bottom: parent.bottom
                source: (hasAVRecording) ? "AudioPlayer.qml" : ""
            }
        }
    }

    ListView {
        id: callsModels
        anchors.fill: parent
        model: unsortedListView
        delegate: historyDelegate
    }
}
