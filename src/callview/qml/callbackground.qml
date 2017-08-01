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
import RingQmlWidgets 1.0
import Ring 1.0

Rectangle {
    property QtObject call: null;
    property string   mode: "PREVIEW"

    id: placeholderMessage
    color: "black"

    Text {
        id: defaultText
        color: "white"
        text: qsTr("[No video]")
        anchors.centerIn: parent
        visible: false
    }

    NewCall {
        id: newCall
        visible: false
        anchors.centerIn: parent
    }

    Ringing {
        id: ringingIndicator
        anchors.centerIn: parent
        running: false
        visible: false
    }

    Searching {
        id: searchingIndicator
        anchors.centerIn: parent
        visible: false
        running: false
    }

    Progress {
        id: progresIndicator
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.margins: 5
        visible: false
    }

    Rectangle {
        id: callEnded
        radius: 5
        color: "white"
        width: 100
        height: 30
        visible: false
        anchors.centerIn: parent
        Text {
            color: "black"
            text: "Call Ended"
            anchors.centerIn: parent
        }
    }

    onCallChanged: {
        if (call && (call.state == Call.RINGING || call.state == 1 /*Call.INCOMING*/))
            callStateGroup.state = "RINGING"
        else if (call && (call.state == Call.CONNECTED || call.state == 14 /*Call.INITIALIZATION*/))
            callStateGroup.state = "CONNECTED"
        else
            callStateGroup.state = "DEFAULT"

        lifecycleStateGroup.state = "INITIALIZATION"
        progresIndicator.call = call
    }

    Timer {
        id: clearNotifications
        interval: 3000
        running:  false
        repeat:   false
        onTriggered: {
            if (call.lifeCycleState == Call.FINISHED)
                call = null
        }
    }

    StateGroup {
        id: callStateGroup

        states: [
            State {
                name: "DEFAULT"
                when: !call
                PropertyChanges {
                    target: defaultText
                    visible: mode == "PREVIEW"
                }
                PropertyChanges {
                    target: newCall
                    visible:  mode != "PREVIEW"
                }
            },
            State {
                name: "RINGING"
                when: call && (call.state == Call.RINGING || call.state == 1 /*Call.INCOMING*/)
                PropertyChanges {
                    target: ringingIndicator
                    visible:  true
                    running: true
                }
            },
            State {
                name: "CONNECTED"
                when: call && (call.state == Call.CONNECTED || call.state == 14 /*Call.INITIALIZATION*/)
                PropertyChanges {
                    target: searchingIndicator
                    visible:  true
                    running: true
                }
            },
            State {
                name: "ENDED"
                when: call.lifeCycleState == Call.FINISHED
                PropertyChanges {
                    target: callEnded
                    visible:  true
                }
                PropertyChanges {
                    target: clearNotifications
                    running:  true
                }
            }
        ]
    }

    StateGroup {
        id: lifecycleStateGroup

        states: [
            State {
                name: "CREATION"
                when: call.lifeCycleState == Call.CREATION
                PropertyChanges {
                    target: progresIndicator
                    visible:  true
                }
            },
            State {
                name: "INITIALIZATION"
                when: call.lifeCycleState == Call.INITIALIZATION
                PropertyChanges {
                    target: progresIndicator
                    visible:  true
                }
            },
            State {
                name: "PROGRESS"
                when: call.lifeCycleState == Call.PROGRESS
            },
            State {
                name: "FINISHED"
                when: (!call) || call.lifeCycleState == Call.FINISHED
            }
        ]
    }
}
