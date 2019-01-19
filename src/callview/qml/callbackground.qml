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

import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import org.kde.ringkde.jamicanvasindicator 1.0 as JamiCanvasIndicator
import org.kde.ringkde.jamicallview 1.0 as JamiCallView
import org.kde.ringkde.jamitroubleshooting 1.0 as JamiTroubleshooting

Rectangle {
    property QtObject call: null;
    property string   mode: "PREVIEW"
    property alias toubleshooting: troubleshootDispatcher
    property real bottomMargin: 0

    id: placeholderMessage
    color: "black"

    Text {
        id: defaultText
        color: "white"
        text: i18n("[No video]")
        anchors.centerIn: parent
        visible: false
    }

    JamiTroubleshooting.Troubleshoot {
        id: troubleshootDispatcher
        call: placeholderMessage.call
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 5
        z: 100000
    }

    JamiCanvasIndicator.NewCall {
        id: newCall
        visible: false
        anchors.centerIn: parent
    }

    JamiCanvasIndicator.Ringing {
        id: ringingIndicator
        anchors.centerIn: parent
        running: false
        visible: false
    }

    JamiCanvasIndicator.Searching {
        id: searchingIndicator
        anchors.centerIn: parent
        visible: false
        running: false
    }

    JamiCanvasIndicator.Progress {
        id: progresIndicator
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.margins: 5
        visible: false
        anchors.bottomMargin: bottomMargin > 0 ? bottomMargin : undefined
        transitions: Transition {
            AnchorAnimation { duration: 300 }
        }
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
            text: i18n("Call Ended")
            anchors.centerIn: parent
        }
    }

    onCallChanged: {
        if (call && (call.state == RingQtQuick.Call.RINGING || call.state == 1 /*Call.INCOMING*/))
            callStateGroup.state = "RINGING"
        else if (call && (call.state == RingQtQuick.Call.CONNECTED || call.state == 14 /*Call.INITIALIZATION*/))
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
            if (call.lifeCycleState == RingQtQuick.Call.FINISHED)
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
                    visible:  mode != "PREVIEW" && !troubleshootDispatcher.isActive
                }
            },
            State {
                name: "RINGING"
                when: call && (call.state == RingQtQuick.Call.RINGING || call.state == 1 /*Call.INCOMING*/)
                PropertyChanges {
                    target: ringingIndicator
                    visible:  true
                    running: true
                }
            },
            State {
                name: "CONNECTED"
                when: call && (call.state == RingQtQuick.Call.CONNECTED || call.state == 14 /*Call.INITIALIZATION*/)
                PropertyChanges {
                    target: searchingIndicator
                    visible:  true
                    running: true
                }
            },
            State {
                name: "ENDED"
                when: call.lifeCycleState == RingQtQuick.Call.FINISHED
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
                when: call.lifeCycleState == RingQtQuick.Call.CREATION
                PropertyChanges {
                    target: progresIndicator
                    visible:  true
                }
            },
            State {
                name: "INITIALIZATION"
                when: call.lifeCycleState == RingQtQuick.Call.INITIALIZATION
                PropertyChanges {
                    target: progresIndicator
                    visible:  true
                }
            },
            State {
                name: "PROGRESS"
                when: call.lifeCycleState == RingQtQuick.Call.PROGRESS
            },
            State {
                name: "FINISHED"
                when: (!call) || call.lifeCycleState == RingQtQuick.Call.FINISHED
            }
        ]
    }
}
