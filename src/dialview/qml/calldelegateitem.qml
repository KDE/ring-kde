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

Rectangle {
    id: callDelegateItem
    anchors.margins: 2
    radius: 5
    color: selected ? activePalette.highlight: "transparent"
    height: content.implicitHeight + 20
    property bool selected: object == CallModel.selectedCall

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
        model: CallModel
    }

    Drag.onDragFinished: {
        if (dropAction == Qt.MoveAction) {
            item.display = "hello"
        }
    }

    RowLayout {
        id: content
        spacing: 10
        width: parent.width - 4

        PixmapWrapper {
            pixmap: decoration
            height:40
            width:40
            anchors.verticalCenter: parent.verticalCenter
        }

        Column {
            Layout.fillWidth: true

            Text {
                text: display
                width: parent.width
                wrapMode: Text.WrapAnywhere
                color: callDelegateItem.selected ?
                    activePalette.highlightedText : activePalette.text
                font.bold: true
            }
            Text {
                text: model.number
                width: parent.width
                wrapMode: Text.WrapAnywhere
                color: callDelegateItem.selected ?
                    activePalette.highlightedText : activePalette.text
            }
        }
    }

    Text {
        text: length
        color: callDelegateItem.selected ?
            inactivePalette.highlightedText : inactivePalette.text
        anchors.right: parent.right
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
            CallModel.selectedCall = object
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
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10
                model: CompletionModel
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
                    SearchDelegate {
                        id: searchDelegate
                        textColor: parent.selected ?
                            activePalette.highlight : activePalette.highlightedText
                        altTextColor: parent.selected ?
                            activePalette.highlight : activePalette.highlightedText
                        showPhoto: false
                        showControls: false
                        showSeparator: false
                        height: 3*fontMetrics.height
                        labelHeight: fontMetrics.height
                    }
                }

                onCountChanged: {
                    completionLoader.height = Math.min(4, count)*(3*fontMetrics.height+10)
                    callDelegateItem.height = content.implicitHeight +
                        Math.min(4, count)*(3*fontMetrics.height+10) + 10
                }
            }
        }
    }

    StateGroup {
        id: stateGroup
        states: [
            State {
                name: "dialing"
                when: selected && object.state == Call.DIALING
                PropertyChanges {
                    target: callDelegateItem
                    height: content.implicitHeight +
                        Math.min(4, count)*(3*fontMetrics.height+10) + 10
                }
                PropertyChanges {
                    target: completionLoader
                    active: true
                    opacity: 1
                    height: Math.min(4, count)*(3*fontMetrics.height+10) + 10
                }
            }
        ]
    }

} //Call delegate

