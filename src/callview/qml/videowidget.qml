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
    property bool     stretch: false
    property string   rendererName: "preview"
    property alias    started: frameTimer.running
    property bool     hasFailed: false
    property QtObject call: null

    property int _delay: 0

    color: "black"

    Image {
        id: videoBackground
        fillMode: Image.PreserveAspectFit
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

            // There is many race conditions with the different types of renderers. This
            // code helps cleanup the messes in case a new source of issues is introduced.
            // It was the case enough time I don't think ever removing this code is a good
            // idea.
            if (rendererName == "peer" && ((!call) || (!call.lifeCycleState == Call.Finished)))
                _delay = _delay + 1

            if (_delay >= 10) {
                running = false
                _delay  = 0
            }
        }
    }

    onStretchChanged: {
        videoBackground.fillMode = stretch ? Image.Stretch : Image.PreserveAspectFit
    }
}
