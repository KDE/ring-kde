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
import QtQuick 2.0
import Ring 1.0
import QtQuick.Layouts 1.0
import Ring 1.0
import RingQmlWidgets 1.0

Item {
    property bool tracksCall: false
    property alias count: callList.count

    width: 300

    Component {
        id: callDelegate

        Item {
            width: 300
            height: 30

            Rectangle {
                color: activePalette.text
                opacity: 0.1
                radius: 5
                anchors.fill: parent
            }

            Rectangle {
                color: "transparent"
                opacity: 0.7
                border.color: activePalette.highlight
                border.width: 1
                radius: 5
                anchors.fill: parent
            }

            RowLayout {
                anchors.margins: 4
                anchors.fill: parent
                PixmapWrapper {
                    pixmap: decoration
                    width: parent.height
                    height: parent.height
                    opacity: 0.7
                }
                Text {
                    font.bold: true
                    text: lenght
                    color: activePalette.text
                }
                Text {
                    text: "In call with " + display
                    Layout.fillWidth: true
                    color: activePalette.text
                }
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Hang up"
                    color: activePalette.text
                    z: 23
                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: -4
                        z: 22
                        color: activePalette.text
                        opacity: 0.3
                        width: 70
                        radius: 3

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                object.performAction(Call.REFUSE)
                            }
                        }
                    }
                }
            }
        }
    }

    ListView {
        id: callList
        width: parent.width
        spacing: 3
        delegate: callDelegate
        model: CallModel
    }

}
