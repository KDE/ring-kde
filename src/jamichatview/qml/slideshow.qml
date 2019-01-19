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
import QtQuick.Layouts 1.0

Loader {
    id: slideshow
    property QtObject model: null
    property string source: ""

    property QtObject current_: null

    Connections {
        target: model

        onViewImage: {
            if ((!slideshow.active) || (!current_))
                return

            current_.source = path
        }
    }

    active: false
    anchors.fill: parent
    asynchronous: true
    z: 10000

    sourceComponent: Component {
        MouseArea {
            anchors.fill: parent
            Rectangle {
                anchors.fill: parent
                color: "black"
                ColumnLayout {
                    anchors.fill: parent
                    Image {
                        id: currentImage
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        fillMode: Image.PreserveAspectFit
                        source: slideshow.source
                        z: 10001
                        Component.onCompleted: {
                            slideshow.current_ = currentImage
                        }
                    }
                    ListView {
                        Layout.fillWidth: true
                        orientation: Qt.Horizontal
                        model: slideshow.model
                        z: 10001
                        height: 96
                    }
                }
            }

            onClicked: {
                slideshow.active = false
            }
        }
    }
}
