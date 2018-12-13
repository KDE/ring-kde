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
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Ring 1.0
import org.kde.playground.kquickitemviews 1.0 as KQuickItemViews

import RingQmlWidgets 1.0

KQuickItemViews.TreeView {
    id: chatView
    clip: true

    property var treeHelper: _treeHelper

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    TreeHelper {
        id: _treeHelper
    }

    // Display something when the chat is empty
    Text {
        color: activePalette.text
        text: i18n("There is nothing yet, enter a message below or place a call using the buttons\nfound in the header")
        anchors.centerIn: parent
        visible: chatView.empty
        horizontalAlignment: Text.AlignHCenter
    }

    Component {
        id: messageDelegate
        Loader {
            id: chatLoader

            property bool showDetailed: false

            // Create a delegate for each type
            Component {
                id: sectionDelegate
                Item {
                    height: content.implicitHeight + 10
                    width: parent.width

                    Rectangle {
                        width: 1
                        color: inactivePalette.text
                        height: parent.height
                        x: 10
                    }

                    Rectangle {
                        radius: 99
                        color: activePalette.base
                        border.width: 1
                        border.color: inactivePalette.text
                        width: 16
                        height: 16
                        y: 10
                        x: 3 // (16 - 10) / 2

                        Rectangle {
                            id: demandsAttention
                            radius: 99
                            color: inactivePalette.text
                            anchors.centerIn: parent
                            height: 8
                            width: 8
                        }
                    }

                    Rectangle {
                        border.color: inactivePalette.text
                        border.width: 1
                        anchors.fill: parent

                        anchors.topMargin: 5
                        anchors.bottomMargin: 5
                        anchors.leftMargin: 30
                        anchors.rightMargin: 40

                        color: "transparent"
                        radius: 10

                        ColumnLayout {
                            id: content
                            anchors.fill: parent

                            Row {
                                Layout.preferredHeight: 2*fontMetrics.height
                                Image {
                                    height: parent.height
                                    width: parent.height
                                    asynchronous: true
                                    anchors.margins: 6
                                    source: "image://icon/dialog-messages"
                                }
                                Text {
                                    height: parent.height
                                    text: formattedDate

                                    leftPadding: 10

                                    verticalAlignment: Text.AlignVCenter
                                    color: ListView.isCurrentItem ?
                                        activePalette.highlightedText : activePalette.text
                                }
                            }

                            Rectangle {
                                color: inactivePalette.text
                                height:1
                                Layout.preferredHeight: 1
                                Layout.fillWidth: true
                            }

                            Text {
                                Layout.fillWidth: true
                                text: incomingEntryCount + i18n(" incoming messages")

                                Layout.preferredHeight: 2*fontMetrics.height
                                leftPadding: 10

                                verticalAlignment: Text.AlignVCenter
                                color: ListView.isCurrentItem ?
                                    activePalette.highlightedText : activePalette.text
                            }

                            Text {
                                Layout.fillWidth: true
                                text: outgoingEntryCount + i18n(" outgoing messages")

                                Layout.preferredHeight: 2*fontMetrics.height
                                leftPadding: 10

                                verticalAlignment: Text.AlignVCenter
                                color: ListView.isCurrentItem ?
                                    activePalette.highlightedText : activePalette.text
                            }

                            Item {
                                Layout.fillHeight: true
                            }
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            treeView.currentIndex = index
                        }
                    }
                }
            }

            Component {
                id: callDelegate
                Item {
                    height: content.implicitHeight + 10
                    width: parent.width

                    Behavior on height {
                        NumberAnimation {duration: 200;  easing.type: Easing.OutQuad}
                    }

                    Rectangle {
                        width: 1
                        color: inactivePalette.text
                        height: parent.height
                        x: 10
                    }

                    Rectangle {
                        radius: 99
                        color: activePalette.base
                        border.width: 1
                        border.color: inactivePalette.text
                        width: 16
                        height: 16
                        y: 10
                        x: 3 // (16 - 10) / 2

                        Rectangle {
                            id: demandsAttention
                            radius: 99
                            color: inactivePalette.text
                            anchors.centerIn: parent
                            height: 8
                            width: 8
                        }
                    }

                    Rectangle {
                        border.color: inactivePalette.text
                        border.width: 1
                        anchors.fill: parent

                        anchors.topMargin: 5
                        anchors.bottomMargin: 5
                        anchors.leftMargin: 30
                        anchors.rightMargin: 40

                        color: "transparent"
                        radius: 10

                        ColumnLayout {
                            id: content
                            anchors.fill: parent

                            Row {
                                Layout.preferredHeight: 2*fontMetrics.height
                                Image {
                                    height: parent.height
                                    width: parent.height
                                    asynchronous: true
                                    anchors.margins: 6
                                    source: "image://icon/call-start"
                                }
                                Text {
                                    height: parent.height
                                    text: formattedDate

                                    leftPadding: 10

                                    verticalAlignment: Text.AlignVCenter
                                    color: ListView.isCurrentItem ?
                                        activePalette.highlightedText : activePalette.text
                                }
                            }

                            Rectangle {
                                color: inactivePalette.text
                                height:1
                                Layout.preferredHeight: 1
                                Layout.fillWidth: true
                            }

                            MultiCall {
                                width: chatView.width - 90
                                modelIndex: rootIndex
                                count: callCount

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        chatLoader.showDetailed = true
                                        chatView.reloadChildren(rootIndex)
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Component {
                id: categoryDelegate
                Item {
                    height: rect.height
                    PeersTimelineCategories {
                        id: rect
                        property var section: display
                        property var recentDate: formattedDate
                    }
                }
            }

            Component {
                id: singleCallDelegate

                RowLayout {
                    width: parent.width

                    Text {
                        text: formattedDate
                        color: activePalette.text
                        Layout.fillWidth: true
                    }

                    Item {
                        height: 1
                        Layout.fillWidth: true
                    }

                    Text {
                        text: length
                        color: inactivePalette.text
                    }
                }
            }

            Component {
                id: recordingDelegate
                Item {
                    height: content.implicitHeight + 10
                    width: parent.width

                    Rectangle {
                        width: 1
                        color: inactivePalette.text
                        height: parent.height
                        x: 10
                    }

                    Rectangle {
                        radius: 99
                        color: activePalette.base
                        border.width: 1
                        border.color: inactivePalette.text
                        width: 16
                        height: 16
                        y: 10
                        x: 3 // (16 - 10) / 2

                        Rectangle {
                            id: demandsAttention
                            radius: 99
                            color: inactivePalette.text
                            anchors.centerIn: parent
                            height: 8
                            width: 8
                        }
                    }

                    Rectangle {
                        id: box
                        border.color: inactivePalette.text
                        border.width: 1
                        anchors.fill: parent

                        anchors.topMargin: 5
                        anchors.bottomMargin: 5
                        anchors.leftMargin: 30
                        anchors.rightMargin: 40

                        color: "transparent"
                        radius: 10

                        ColumnLayout {
                            id: content
                            anchors.fill: parent

                            Row {
                                Layout.preferredHeight: 2*fontMetrics.height
                                Image {
                                    height: parent.height
                                    width: parent.height
                                    asynchronous: true
                                    anchors.margins: 6
                                    source: "image://icon/media-record"
                                }
                                Text {
                                    height: parent.height
                                    text: formattedDate

                                    leftPadding: 10

                                    verticalAlignment: Text.AlignVCenter
                                    color: ListView.isCurrentItem ?
                                        activePalette.highlightedText : activePalette.text
                                }
                            }

                            Rectangle {
                                color: inactivePalette.text
                                height:1
                                Layout.preferredHeight: 1
                                Layout.fillWidth: true
                            }

                            AudioPlayer {
                                id: rect
                                Layout.minimumWidth: box.width - 10
                            }

                            Item {
                                Layout.fillHeight: true
                            }
                        }
                    }
                }
            }

            // Some elements don't have delegates because they are handled
            // by their parent delegates
            function selectDelegate() {
                if (nodeType == IndividualTimelineModel.TIME_CATEGORY)
                    return categoryDelegate

                if (nodeType == IndividualTimelineModel.SECTION_DELIMITER)
                    return sectionDelegate

                if (nodeType == IndividualTimelineModel.CALL_GROUP)
                    return callDelegate

                if (nodeType == IndividualTimelineModel.RECORDINGS)
                    return recordingDelegate

                if (nodeType == IndividualTimelineModel.CALL_GROUP)
                    return callDelegate

                if (nodeType == IndividualTimelineModel.CALL
                  && chatView.parentTreeItem(rootIndex).showDetailed)
                    return singleCallDelegate
            }

            sourceComponent: selectDelegate()
        }
    }

    delegate: messageDelegate
}
