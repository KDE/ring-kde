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

Item {
    property QtObject recording: audioRecording

    id: audioPlayer
    height: 30
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
                if (!recording.isCurrent) return

                if (stateGroup.state == "enabled")
                    recording.play()
                else if(stateGroup.state == "playing")
                    recording.pause()
            }
        }
        Controls2.Slider {
            id: progress
            from: 0
            value: recording.position
            to: recording.duration
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

    Rectangle {
        id: selectOverlay
        color: "black"
        opacity: 0.5
        anchors.fill: parent
        Text {
            anchors.centerIn: parent
            text: "Click to play"
            color: "white"
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                RecordingModel.currentRecording = recording
            }
        }
    }

    StateGroup {
        id: stateGroup
        states: [
            // Hide all buttons
            State {
                name: "disabled"
                when: !recording == RecordingModel.currentRecording
            },
            State {
                name: "enabled"
                when: recording == RecordingModel.currentRecording

                PropertyChanges {
                    target: reset
                    enabled: true
                }
                PropertyChanges {
                    target: selectOverlay
                    visible: false
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
                when: recording.isPlaying

                PropertyChanges {
                    target: playPause
                    text: "⏸"
                }
            }
        ]
    }
}
