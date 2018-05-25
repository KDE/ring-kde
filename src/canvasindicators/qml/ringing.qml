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
import Ring 1.0

import RingQmlWidgets 1.0

Rectangle {
    width: 128
    height: 128
    color: "transparent"
    property alias running: frameTimer.running

    Image {
        id: ringingImage
        width: parent.height
        height: parent.height
        sourceSize.width: parent.height
        sourceSize.height: parent.height
        smooth: true
        source: "image://RingingImageProvider/ringing/0"
    }

    property var counter: 0

    Timer {
        id: frameTimer
        interval: 33
        repeat: true
        onTriggered: {
            counter = counter < 35 ? counter + 1 : 0
            ringingImage.source = "image://RingingImageProvider/ringing/"+counter
        }
    }
}
