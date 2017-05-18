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
import QtQuick.Controls 2.0 as Controls2
import QtQuick.Layouts 1.0
import Ring 1.0

Rectangle {
    property QtObject call: null

    id: audioPlayer
    height: 30
    color: "blue"
    state: "disabled"

    RowLayout {
        anchors.fill: parent

        Controls2.Button {
            id: reset
            text: "⏮" // "⏸" ⏹ ⏮ ⏭
            width:30
            Layout.fillHeight: true
            Layout.preferredWidth: 30
            enabled: false
        }
        Controls2.Button {
            id: playPause
            text: "▶"
            width:30
            Layout.fillHeight: true
            Layout.preferredWidth: 30
            enabled: false
            onClicked: {
                if (!RecordingModel.currentRecording) return

                if (stateGroup.state == "enabled")
                    RecordingModel.currentRecording.play()
                else if(stateGroup.state == "playing")
                    RecordingModel.currentRecording.pause()
            }
        }
        Controls2.Slider {
            id: progress
            from: 0
            value: 0
            to: 5000
            Layout.fillWidth: true
            Layout.fillHeight: true
            enabled: false
        }

        Controls2.Button {
            id: end
            text: "⏭"
            width:30
            Layout.preferredWidth: 30
            Layout.fillHeight: true
            enabled: false
        }
    }

    Connections {
        target: RecordingModel.selectionModel
        onCurrentChanged: {
            if (call) return
            var cur = RecordingModel.currentRecording

            stateGroup.state = (cur != null && cur.type == Recording.AUDIO_VIDEO) ?
                "enabled" : "disabled"
        }
    }

    Connections {
        target: RecordingModel.currentRecording
        onPlaybackPositionChanged: progress.value = pos*5000
    }

    Connections {
        target: RecordingModel.currentRecording
        onStarted: stateGroup.state = "playing"
    }

    Connections {
        target: RecordingModel.currentRecording
        onStopped: stateGroup.state = "enabled"
    }

    StateGroup {
        id: stateGroup
        states: [
            // Hide all buttons
            State {
                name: "disabled"
            },
            State {
                name: "enabled"

                PropertyChanges {
                    target: reset
                    enabled: true
                }
                PropertyChanges {
                    target: playPause
                    enabled: true
                }
                PropertyChanges {
                    target: progress
                    enabled: true
                    value: RecordingModel.currentRecording.position
                }
                PropertyChanges {
                    target: end
                    enabled: true
                }
            },
            State {
                name: "playing"
                extend: "enabled"

                PropertyChanges {
                    target: playPause
                    text: "⏸"
                }
            }
        ]
    }
}
