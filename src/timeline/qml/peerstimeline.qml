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
import QtQuick.Controls 1.4
import Ring 1.0
import QtQuick.Layouts 1.0

Rectangle {
    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    SystemPalette {
        id: inactivePalette
        colorGroup: SystemPalette.Disabled
    }

    color: activePalette.base
    id: recentDock
    visible: true

    signal contactMethodSelected(var cm)

    Component {
        id: contactDelegate
        Item {
            id: componentItem
            width: parent.width
            height: 50

            RowLayout {
                anchors.fill: parent
                Item {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.topMargin: 2
                    anchors.leftMargin: 2
                    height: 46
                    width:  46
                    Rectangle {
                        radius: 5
                        color: "white"
                        opacity: 0.05
                        anchors.fill: parent
                    }
                    ModelIconBinder2 {
                        anchors.fill: parent
                        pixmap: decoration
                    }
                }

                ColumnLayout {
                    anchors.verticalCenter: parent.verticalCenter
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Item {
                        height: 1
                        Layout.preferredHeight: 1
                        Layout.minimumHeight: 1
                        Layout.maximumHeight: 1
                    }
                    Text {
                        text: name
                        font.bold : true
                        color: componentItem.ListView.isCurrentItem ?
                            activePalette.highlightedText : activePalette.text
                    }
                    Item {
                        height: 1
                        Layout.preferredHeight: 1
                        Layout.minimumHeight: 1
                        Layout.maximumHeight: 1
                    }
                    Text {
                        text: number.length == 40 ? "RingId" : number
                        color: componentItem.ListView.isCurrentItem ?
                            activePalette.highlightedText : inactivePalette.text
                    }
                    Item {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                    }
                    Item {
                        height: 1
                        Layout.preferredHeight: 1
                        Layout.minimumHeight: 1
                        Layout.maximumHeight: 1
                    }
                }
            }

            // Allow to set/remove bookmarks
            Image {
                id: bookmarkSwitch
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.rightMargin: 1
                anchors.topMargin: 3
                height: 16
                width: 16
                source: isBookmarked ? "icons/bookmarked.svg" : "icons/not_bookmarked.svg"
                z: 100
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        mouse.accepted = true
                        isBookmarked = !isBookmarked
                        bookmarkSwitch.source = isBookmarked ?
                            "icons/bookmarked.svg" : "icons/not_bookmarked.svg"
                    }
                }
            }

            // Add the other indicators in the bottom right
            RowLayout {
                height: 16
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.rightMargin: 3
                anchors.bottomMargin: 3

                Rectangle {
                    id: hasActiveVideoBox
                    height: 16
                    radius: 3
                    color: "#b93030"
                    opacity: 0.33
                    width: 20
                    visible: hasActiveVideo

                    Image {
                        anchors.horizontalCenter: parent.horizontalCenter
                        height: 16
                        width:  16
                        source: "image://icon/camera-on"
                    }
                }

                Rectangle {
                    id: isRecordingBox
                    height: 16
                    radius: 3
                    color: "#b93030"
                    opacity: 0.33
                    width: 20
                    visible: isRecording

                    Image {
                        height: 16
                        width:  16
                        anchors.horizontalCenter: parent.horizontalCenter
                        source: "image://icon/media-record"
                    }
                }

                Rectangle {
                    id: hasActiveCallBox
                    height: 16
                    radius: 3
                    color: "#0b4714"
                    width: 20
                    opacity: 0.33
                    visible: hasActiveCall

                    Image {
                        height: 16
                        width:  16
                        anchors.horizontalCenter: parent.horizontalCenter
                        source: "image://icon/call-start"
                    }
                }

                Item {
                    height: 16
                    width: 40
                    visible: unreadTextMessageCount > 0

                    Rectangle {
                        anchors.fill: parent
                        radius: 3
                        color: "#a6a6b9"
                        opacity: 0.33
                    }

                    Image {
                        height: 16
                        width:  16
                        source: "image://icon/folder-mail"
                    }

                    Text {
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        text: ""+unreadTextMessageCount
                        color: "white"
                    }
                }
            }

            //FIXME temporary polling until all the objects are correctly in sync
            Timer {
                interval: 5000
                running: true
                repeat: true
                onTriggered: {
                    isRecordingBox.visible    = isRecording
                    hasActiveVideoBox.visible = hasActiveVideo
                }
            }

            MouseArea {
                anchors.fill: parent
                z: 99
                onClicked: {
                    recentView.currentIndex = index
                    contactMethodSelected(recentView.currentIndex);
                }
            }
        }
    }

    ScrollView {
        anchors.fill: parent
        flickableItem.interactive: true
        ListView {
            id: recentView
            anchors.fill: parent
            highlightMoveVelocity: Infinity //HACK
            delegate: contactDelegate
            highlight: Rectangle {
                color: activePalette.highlight
            }
            model: PeersTimelineModel
        }
    }

}
