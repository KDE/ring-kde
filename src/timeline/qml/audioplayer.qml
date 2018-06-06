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
    height: content.implicitHeight

    RowLayout {
        id: content
        width: parent.width
        state: "disabled"

        Controls2.Button {
            id: reset
            text: "⏮" // "⏸" ⏹ ⏮ ⏭
            Layout.fillHeight: true
            Layout.preferredWidth: 30
            Layout.maximumWidth: 30
            enabled: false
        }
        Controls2.Button {
            id: playPause
            text: "▶"
            Layout.fillHeight: true
            Layout.preferredWidth: 30
            Layout.maximumWidth: 30
            enabled: false

            onClicked: {
                if (!audioRecording.isCurrent) return

                if (stateGroup.state == "enabled")
                    audioRecording.play()
                else if(stateGroup.state == "playing")
                    audioRecording.pause()
            }
        }
        Controls2.Slider {
            id: progress
            from: 0
            value: audioRecording.position
            to: audioRecording.duration
            Layout.fillWidth: true
            Layout.fillHeight: true
            enabled: false
        }

        Controls2.Button {
            id: end
            text: "⏭"
            Layout.preferredWidth: 30
            Layout.maximumWidth: 30
            Layout.fillHeight: true
            enabled: false
        }
    }

    Rectangle {
        id: selectOverlay
        color: inactivePalette.base
        opacity: 0.5
        anchors.fill: parent
        Text {
            anchors.centerIn: parent
            text: i18n("Click to play")
            color: inactivePalette.text
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
