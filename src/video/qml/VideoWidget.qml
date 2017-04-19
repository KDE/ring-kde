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

Rectangle {
    property bool   stretch: false
    property string rendererName: "preview"
    property alias  started: frameTimer.running

    color: "black"

    Image {
        id: videoBackground
        fillMode: Image.PreserveAspectFit
        source: "image://VideoFrame/Image.png"
        anchors.fill: parent
        cache: false
    }

    // HACK This is a workaround until the OpenGL underlay based framebuffer
    // starts working. It Eats CPU and is otherwise ugly, but it works.
    property var counter: 1
    Timer {
        id: frameTimer
        interval: 33
        running: false
        repeat: true
        onTriggered: {
            videoBackground.source = "image://VideoFrame/"+rendererName+"/"+counter+".png"
            counter = counter + 1
        }
    }

    onStretchChanged: {
        videoBackground.fillMode = stretch ? Image.Stretch : Image.PreserveAspectFit
    }
}
