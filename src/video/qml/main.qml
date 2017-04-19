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

Item {
    id: videoDock
    visible: true

    height: 480
    width: 640

    // C++ bindings
    property alias  rendererName   : videoWidget.rendererName
    property bool   displayPreview : false
    property string mode           : "PREVIEW"
    property bool   previewRunning : false
    property alias  peerRunning    : videoWidget.started
    property var    call           : null

    VideoWidget {
        id: videoWidget
        anchors.fill: parent
        z: -100
        started: false
    }

    VideoWidget {
        id: videoPreview
        z: -95
        started: false
        visible: false
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        width: 192
        height: 108
    }

    VideoControlToolbar {
        id: controlToolbar
        y: parent.y
        visible: false
    }

    VideoStateToolbar {
        id: actionToolbar
        y: parent.height - height
        visible: false

    }

    // Hide both toolbars when the mouse isn't moving
    //TODO keep visible if the mouse if over the toolbars
    MouseArea {
        Timer {
            id: activityTimer
            interval: 3000
            running: true
            repeat: false
            onTriggered: {
                actionToolbar.visible  = false
                controlToolbar.visible = false
            }
        }

        function trackActivity() {
            if (mode == "PREVIEW")
                return;

            actionToolbar.visible  = true
            controlToolbar.visible = true
            activityTimer.restart()
        }

        anchors.fill: parent
        hoverEnabled: true
        onMouseXChanged: trackActivity()
        onMouseYChanged: trackActivity()
    }

    onModeChanged: {
        if (mode == "PREVIEW") {
            controlToolbar.visible = false
            actionToolbar.visible  = false
            videoPreview.visible   = false
            videoWidget.rendererName = "preview"
        }
        else if (mode == "CONVERSATION") {
            videoPreview.visible = true
            videoPreview.started = PreviewManager.previewing
            videoWidget.rendererName = "peer"
        }
    }

    onCallChanged: {
        if (call) {
            actionToolbar.userActionModel = call.userActionModel
        }
    }

    Connections {
        target: PreviewManager
        onPreviewingChanged: {
            if (mode == "PREVIEW")
                videoWidget.started = PreviewManager.previewing
            else
                videoPreview.visible = PreviewManager.previewing

            if (videoPreview.visible)
                videoPreview.started = PreviewManager.previewing
        }
    }

    Connections {
        target: call
        onVideoStopped: {
            videoWidget.started = false
        }
    }

    Connections {
        target: call
        onVideoStarted: {
            videoWidget.started = true
        }
    }
}
