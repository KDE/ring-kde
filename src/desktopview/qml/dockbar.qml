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
import Ring 1.0
import QtQuick.Controls 2.0 as Controls
import org.kde.kirigami 2.2 as Kirigami
import QtQuick.Window 2.2

import DesktopView 1.0

Image {
    id: dockBar

    property string selectedItem: "timeline"

    source: "gui/icons/assets/toolbar_bg.png"
    fillMode: Image.Tile
    width: 48
    height: parent.height

    DockModel {
        id: icons
    }

    FontMetrics {
        id: fontMetrics
    }

    Column {
        Repeater {
            model: icons
            Rectangle {
                id: actionIcon
                color: dockBar.selectedItem == identifier ? "#111111" : "transparent"
                height: 58
                width: 48

                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    source: decoration
                    width: 48
                    height: 48
                    fillMode: Image.PreserveAspectFit
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        dockBar.selectedItem = identifier
                    }
                }

                Rectangle {
                    color: "red"
                    radius: 99
                    x: 2
                    y: 2
                    width: fontMetrics.height + 4
                    height: fontMetrics.height + 4
                    visible: activeCount > 0
                    Text {
                        color: "white"
                        anchors.centerIn: parent
                        font.bold: true
                        text: activeCount
                    }
                }
            }
        }
    }
}
