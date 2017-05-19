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

import RingQmlWidgets 1.0

ListView {

    property var textColor: "blue"
    property var bubbleBackground: "red"

    delegate: Item {
        width: parent.width

        height: 80

        RowLayout {
            anchors.fill: parent

            Item {
                visible: direction == 0
                Layout.preferredWidth: parent.width*0.3
                Layout.minimumWidth: parent.width*0.3
                Layout.maximumWidth: parent.width*0.3
            }

            Rectangle {
                width: 50
                height: 50
                color: "gray"
                radius: 5
                visible: direction == 1
                PixmapWrapper {
                    anchors.fill: parent
                    pixmap: decoration
                }
            }

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true

                Bubble {
                    anchors.fill: parent
                    anchors.margins: 5

                    alignment: direction == 0 ? Text.AlignRight : Text.AlignLeft
                    color: bubbleBackground //isRead ? "green" : "red"

                    Text {
                        anchors.fill: parent
                        anchors.leftMargin: 30
                        anchors.rightMargin: 30
                        anchors.topMargin: 5
                        anchors.bottomMargin: 5
                        horizontalAlignment: direction == 0 ? Text.AlignRight : Text.AlignLeft
                        font.family: "Noto Color Emoji"
                        text: display
                        color: textColor
                    }

                    Text {
                        anchors.bottom: parent.bottom
                        anchors.left: direction == 0 ? parent.left : undefined
                        anchors.right: direction == 1 ? parent.right : undefined
                        anchors.bottomMargin: 4
                        anchors.leftMargin: direction == 0 ? 4 : undefined
                        anchors.rightMargin: direction == 1 ? 4 : undefined
                        text: formattedDate
                        color: "gray"
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            console.log("SET isRead")
                            isRead = true
                        }
                    }
                }
            }

            Item {
                visible: direction == 1
                Layout.preferredWidth: parent.width*0.3
                Layout.minimumWidth: parent.width*0.3
                Layout.maximumWidth: parent.width*0.3
            }

            Rectangle {
                width: 50
                height: 50
                color: "gray"
                radius: 5
                visible: direction == 0
                PixmapWrapper {
                    anchors.fill: parent
                    pixmap: decoration
                }
            }
        }
    }
}
