/******************************************************************************
 *   Copyright (C) 2012 by Savoir-Faire Linux                                 *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>   *
 *                                                                            *
 *   This library is free software; you can redistribute it and/or            *
 *   modify it under the terms of the GNU Lesser General Public               *
 *   License as published by the Free Software Foundation; either             *
 *   version 2.1 of the License, or (at your option) any later version.       *
 *                                                                            *
 *   This library is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU        *
 *   Lesser General Public License for more details.                          *
 *                                                                            *
 *   You should have received a copy of the Lesser GNU General Public License *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *****************************************************************************/
import QtQuick 2.0
import QtQuick.Layouts 1.0
import Ring 1.0
import RingQmlWidgets 1.0
import org.kde.kirigami 2.2 as Kirigami
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import org.kde.ringkde.jamicontactview 1.0 as JamiContactView
import org.kde.ringkde.jamicontactview 1.0 as JamiContactView

Rectangle {
    id: callDelegateItem
    anchors.margins: 2
    radius: 5
    border.width: 0
    color: selected ? activePalette.highlight: "transparent"

    height: content.implicitHeight + 20 + errorMessage.height
    property bool selected: object == RingSession.callModel.selectedCall

    property bool skipSelect: errorMessage.active || missedMessage.active

    property var labelColor: callDelegateItem.selected && !skipSelect ?
        activePalette.highlightedText : activePalette.text

    Drag.active: mouseArea.drag.active
    Drag.dragType: Drag.Automatic
    Drag.onDragStarted: {
        var ret = treeHelper.mimeData(model.rootIndex, index)
        Drag.mimeData = ret
    }

    Behavior on height {
        NumberAnimation {duration: 200;  easing.type: Easing.OutQuad }
    }

    TreeHelper {
        id: treeHelper
        model: RingSession.callModel
    }

    Drag.onDragFinished: {
        if (dropAction == Qt.MoveAction) {
            item.display = "hello"
        }
    }

    OutlineButton {
        id: closeButton
        label: "  "+i18n("Close")
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 3
        height: 24
        visible: false
        z: 100
        alignment: Qt.AlignRight

        property string colString: selected && !skipSelect ?
            activePalette.highlightedText.toString(16) :
                activePalette.text.toString(16)

        color: colString

        icon: "image://SymbolicColorizer/?color="+colString+";:/sharedassets/outline/close.svg"
        onClicked: {
            object.performAction(RingQtQuick.Call.REFUSE)
        }
    }

    RowLayout {
        id: content
        spacing: 10
        width: parent.width - 4

        JamiContactView.ContactPhoto {
            contactMethod: object.peerContactMethod
            height:40
            width:40
            drawEmptyOutline: false
            defaultColor: labelColor
            anchors.verticalCenter: parent.verticalCenter
        }

        Column {
            Layout.fillWidth: true

            Text {
                text: display
                width: parent.width
                wrapMode: Text.WrapAnywhere
                color: labelColor
                font.bold: true
            }

            Text {
                text: model.number
                width: parent.width
                wrapMode: Text.WrapAnywhere
                color: labelColor
            }
        }
    }

    DropArea {
        anchors.fill: parent
        keys: ["text/ring.call.id", "text/plain"]
        onEntered: {
            callDelegateItem.color = "red"
        }
        onExited: {
            callDelegateItem.color = "blue"
        }
        onDropped: {
            var formats = drop.formats
            var ret = {}

            ret["x-ring/dropaction"] = "join"

            // stupid lack of official APIs...
            for(var i=0; i<  formats.length; i++) {
                ret[formats[i]] = drop.getDataAsArrayBuffer(formats[i])
            }

            treeHelper.dropMimeData2(ret, model.rootIndex, index)
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        propagateComposedEvents: true
        onClicked: {
            mouse.accepted = true
            RingSession.callModel.selectedCall = object
            dialView.selectCall(object)
        }
        drag.target: callDelegateItem
    }

    Loader {
        id: completionLoader
        active: false
        opacity: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        width: callDelegateItem.width
        height: 0

        Behavior on height {
            NumberAnimation {duration: 200;  easing.type: Easing.OutQuad }
        }

        Behavior on opacity {
            NumberAnimation {duration: 200;  easing.type: Easing.OutQuad }
        }

        sourceComponent: Component {
            ListView {
                clip: true
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10
                model: RingSession.numberCompletionModel
                currentIndex: completionSelection.currentIndex
                delegate: Rectangle {
                    property bool selected: ListView.isCurrentItem
                    height: searchDelegate.height
                    width: callDelegateItem.width - 20
                    color: selected ? activePalette.highlightedText : "transparent"
                    border.width: 1
                    border.color: activePalette.highlightedText
                    radius: 5
                    clip: true
                    CompletionDelegate {
                        id: searchDelegate
                        showPhoto: false
                        showControls: false
                        showSeparator: false
                        labelHeight: fontMetrics.height
                    }
                }

                onCountChanged: {
                    completionLoader.height = Math.min(4, count)*(3*fontMetrics.height+12)
                    callDelegateItem.height = content.implicitHeight +
                        Math.min(4, count)*(3*fontMetrics.height+10) + 10
                }
            }
        }
    }

    Loader {
        id: errorMessage
        anchors.top: content.bottom
        active: false
        width: parent.width
        height: active ? item.implicitHeight : 0
        sourceComponent: Component {
            CallError {
                call: object
                width: errorMessage.width
            }
        }
    }

    Loader {
        id: outgoingMessage
        anchors.top: content.bottom
        active: false
        width: parent.width
        height: active ? item.implicitHeight : 0
        sourceComponent: Component {
            OutgoingCall {
                call: object
                width: outgoingMessage.width
            }
        }
    }

    Loader {
        id: finishedMessage
        anchors.top: content.bottom
        active: false
        width: parent.width
        height: active ? item.implicitHeight : 0
        sourceComponent: Component {
            FinishedCall {
                call: object
                width: finishedMessage.width
            }
        }
    }

    Loader {
        id: currentMessage
        anchors.top: content.bottom
        active: false
        width: parent.width
        height: active ? item.implicitHeight : 0
        sourceComponent: Component {
            CurrentCall {
                call: object
                width: currentMessage.width
            }
        }
    }

    Loader {
        id: missedMessage
        active: false
        width: parent.width
        height: active ? item.implicitHeight : 0
        sourceComponent: Component {
            MissedCall {
                call: object
                width: missedMessage.width
            }
        }
    }

    Loader {
        id: rigningAnimation
        active: false
        width: parent.width
        height: active ? 32 : 0
        anchors.top: content.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        sourceComponent: Component {
            IncomingCall {
                //call: object
                width: rigningAnimation.width
            }
        }
    }

    StateGroup {
        id: stateGroup
        states: [
            State {
                name: ""

                PropertyChanges {
                    target: callDelegateItem
                    border.width: 0
                }

            },
            State {
                name: "dialing"
                when: selected && object.state == RingQtQuick.Call.DIALING
                PropertyChanges {
                    target: callDelegateItem
                    border.width: 0
                    height: content.implicitHeight +
                        Math.min(4, count)*(3*fontMetrics.height+10) + 10
                }
                PropertyChanges {
                    target: completionLoader
                    active: true
                    opacity: 1
                    height: Math.min(4, count)*(3*fontMetrics.height+10) + 10
                }
                PropertyChanges {
                    target: closeButton
                    visible: false
                }
            },
            State {
                name: "error"
                when: lifeCycleState == RingQtQuick.Call.FINISHED && object.state != RingQtQuick.Call.OVER
                    && object.state != RingQtQuick.Call.ABORTED

                PropertyChanges {
                    target: errorMessage
                    active: true
                }

                PropertyChanges {
                    target: callDelegateItem
                    color: "#33ff0000"
                    border.width: 1
                    border.color: "#55ff0000"
                }

                PropertyChanges {
                    target: closeButton
                    visible: true
                }
            },
            State {
                name: "missed"
                when: object.state == RingQtQuick.Call.OVER && object.missed

                PropertyChanges {
                    target: missedMessage
                    active: true
                }
                PropertyChanges {
                    target: content
                    visible: false
                }

                PropertyChanges {
                    target: callDelegateItem
                    height: missedMessage.height
                    color: "#33ff0000"
                    border.width: 1
                    border.color: "#55ff0000"
                }

                PropertyChanges {
                    target: closeButton
                    visible: true
                }
            },
            State {
                name: "incoming"
                when: object.state == 1/*iNCOMING*/

                PropertyChanges {
                    target: callDelegateItem
                    border.width: 0
                    height: rigningAnimation.height + content.implicitHeight + 3
                }

                PropertyChanges {
                    target: rigningAnimation
                    active: true
                }
                PropertyChanges {
                    target: closeButton
                    visible: false
                }
            },
            State {
                name: "outgoing"
                when: object.lifeCycleState == RingQtQuick.Call.INITIALIZATION && object.direction == 1/*OUTGOING*/

                PropertyChanges {
                    target: callDelegateItem
                    border.width: 0
                    height: outgoingMessage.height + content.implicitHeight
                }

                PropertyChanges {
                    target: outgoingMessage
                    active: true
                }
                PropertyChanges {
                    target: closeButton
                    visible: false
                }
            },
            State {
                name: "finished"
                when: object.state == RingQtQuick.Call.OVER

                PropertyChanges {
                    target: callDelegateItem
                    border.width: 0
                    height: finishedMessage.height + content.implicitHeight
                }

                PropertyChanges {
                    target: finishedMessage
                    active: true
                }
                PropertyChanges {
                    target: closeButton
                    visible: true
                }
            },
            State {
                name: "current"
                when: object.lifeCycleState == RingQtQuick.Call.PROGRESS

                PropertyChanges {
                    target: callDelegateItem
                    border.width: 0
                    height: currentMessage.height + content.implicitHeight
                }

                PropertyChanges {
                    target: currentMessage
                    active: true
                }
                PropertyChanges {
                    target: closeButton
                    visible: false
                }
            }
        ]
    }

} //Call delegate

