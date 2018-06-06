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

Item {
    id: videoDock

    signal callWithVideo ()
    signal callWithAudio ()
    signal callWithScreen()

    height: 480
    width: 640

    // C++ bindings
    property alias    rendererName   : videoWidget.rendererName
    property bool     displayPreview : false
    property string   mode           : "PREVIEW"
    property bool     previewRunning : false
    property alias    peerRunning    : videoWidget.started
    property QtObject call           : null
    property QtObject renderer       : call ? call.renderer : null

    property bool previewVisible: mode != "PREVIEW" &&
        call && PreviewManager.previewing

    Connections {
        target: renderer
        onDestroyed: {
            videoWidget.started = false
        }
    }

    // Let the animations finish before
    Timer {
        id: toolbarTimer
        running: false
        interval: 150
        repeat: false

        onTriggered: {
            actionToolbar.visible  = false
            videoSource.visible    = false
            controlToolbar.visible = false
        }
    }

    function showToolbars() {
        actionToolbar.visible  = true
        videoSource.visible    = true

        // This toolbar is only useful when there is video
        if (videoWidget.started)
            controlToolbar.visible = true

        actionToolbar.opacity  = 1
        videoSource.opacity    = 1
        controlToolbar.opacity = 1
        videoPreview.opacity   = 0.8
        actionToolbar.anchors.bottomMargin = 0
        videoSource.anchors.rightMargin    = 0
        controlToolbar.anchors.topMargin   = 0
    }

    function hideToolbars() {
        actionToolbar.opacity  = 0
        videoSource.opacity    = 0
        controlToolbar.opacity = 0
        videoPreview.opacity   = 1
        videoSource.anchors.rightMargin    = -20
        actionToolbar.anchors.bottomMargin = -20
        controlToolbar.anchors.topMargin   = -20
        toolbarTimer.running = true
    }

    VideoWidget {
        id: videoWidget
        anchors.fill: parent
        z: -100
        started: false
        visible: started && !hasFailed
    }

    VideoWidget {
        id: videoPreview
        z: -95
        started: false
        visible: previewVisible
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        width: 192
        height: 108
    }

    VideoControlToolbar {
        id: controlToolbar
        anchors.top: parent.top
        visible: false
        Behavior on opacity {
            NumberAnimation {duration: 100}
        }
        Behavior on anchors.topMargin {
            NumberAnimation {duration: 150}
        }
    }

    ActionToolbar {
        id: actionToolbar
        anchors.bottom: parent.bottom
        visible: false
        Behavior on opacity {
            NumberAnimation {duration: 100}
        }
        Behavior on anchors.bottomMargin {
            NumberAnimation {duration: 150}
        }
    }

    RecordingIcon {
        anchors.right: videoDock.right
        anchors.top: controlToolbar.bottom
    }

    DeviceSetting {
        id: deviceSettings
        visible: mode == "PREVIEW"
        width: parent.width
        z: 100
        Rectangle {
            anchors.fill: parent
            color: "black"
            opacity: 0.5
            z: -1
        }
    }

    VideoSource {
        id: videoSource
        z: 101
        visible: false
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        Behavior on opacity {
            NumberAnimation {duration: 100}
        }
        Behavior on anchors.rightMargin {
            NumberAnimation {duration: 150}
        }
    }

    // The background
    CallBackground {
        id: placeholderMessage
        z: -99
        anchors.fill: parent
        bottomMargin: actionToolbar.visible ? actionToolbar.height : 0
    }

    // Hide both toolbars when the mouse isn't moving
    //TODO keep visible if the mouse if over the toolbars
    MouseArea {
        id: mainMouseArea
        Timer {
            id: activityTimer
            interval: 3000
            running: true
            repeat: false
            onTriggered: {
                hideToolbars()
            }
        }

        function trackActivity() {
            if (mode == "PREVIEW") {
                deviceSettings.visible = true
            }
            else if (call) {
                showToolbars()
            }

            activityTimer.restart()
        }

        anchors.fill: parent
        hoverEnabled: true
        propagateComposedEvents: true
        onMouseXChanged: trackActivity()
        onMouseYChanged: trackActivity()
    }

    onModeChanged: {
        if (mode == "PREVIEW") {
            hideToolbars()
            videoWidget.rendererName = "preview"
        }
        else if (mode == "CONVERSATION") {
            videoPreview.started = PreviewManager.previewing
            videoWidget.rendererName = "peer"
        }
        placeholderMessage.mode = mode
    }

    onCallChanged: {
        if (call) {
            actionToolbar.userActionModel = call.userActionModel
            placeholderMessage.call = call
            videoSource.call        = call
            controlToolbar.call     = call
        }
        videoWidget.hasFailed = false
        mainMouseArea.visible = call != null || mode == "PREVIEW"
    }

    Connections {
        target: PreviewManager
        onPreviewingChanged: {
            if (mode == "PREVIEW")
                videoWidget.started = PreviewManager.previewing

            videoPreview.started = PreviewManager.previewing
        }
    }

    Connections {
        target: call
    }

    Connections {
        target: call
        onVideoStarted: {
            videoWidget.started = true
        }
        onVideoStopped: {
            videoWidget.started = false
        }
        onLiveMediaIssuesChanaged: {
            // This isn't using properties because the renderer live in their
            // own thread and QML doesn't support this yet
            videoWidget.hasFailed = call.hasIssue(Call.VIDEO_ACQUISITION_FAILED)
        }
    }

    Connections {
        target: videoWidget
        onStartedChanged: {
            placeholderMessage.visible = (!videoWidget.started) || (videoWidget.hasFailed)
        }
        onHasFailedChanged: {
            placeholderMessage.visible = (!videoWidget.started) || (videoWidget.hasFailed)
        }
    }

    Connections {
        target: call
        onStateChanged: {
            if (call == null || call.lifeCycleState == Call.FINISHED) {
                call = null
                hideToolbars()
            }
        }
    }
}
