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
import RingQmlWidgets 1.0
import ContactView 1.0
// import org.kde.kirigami 2.0 as Kirigami

Item {
    id: componentItem
    width: parent != undefined ? parent.width : undefined
    height: computeHeight()

    property bool hasMessage: hasActiveCall || unreadTextMessageCount > 0 || isRecording || hasActiveVideo

    function computeHeight() {
        if (hasMessage)
            return 7*fontMetrics.height + 14

        return 5*fontMetrics.height + 13
    }

    function thirdRowMessage() {
        if (hasActiveVideo)
            return "Video in progress"
        else if (hasActiveCall)
            return "Call in progress"
        else if (unreadTextMessageCount > 0)
            return unreadTextMessageCount + " new messages"

        return ""
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

        Rectangle {
            id: highlight
            anchors.fill: parent
            color: "red"
            visible: false
            opacity: 0
        }

        ColumnLayout {
            anchors.fill: parent
            RowLayout {
                Layout.fillWidth: true
                height: 3*fontMetrics.height

                Item {
                    anchors.margins: 4

                    height: parent.height
                    width:  parent.height

                    ContactPhoto {
                        anchors.margins: 3
                        anchors.fill: parent
                        contactMethod: object
                        drawEmptyOutline: false
                    }
                }

                Text {
                    text: object.primaryName.length == 40 ? "Unknown" : object.primaryName
                    clip: true
                    font.bold : true
                    Layout.fillWidth: true
                    anchors.verticalCenter: parent.verticalCenter
                    color: isCurrentItem ?
                        activePalette.highlightedText : activePalette.text
                }
            }

            Rectangle {
                color: inactivePalette.text
                height:1
                Layout.fillWidth: true
            }

            Text {
                Layout.fillWidth: true
                text: number.length == 40 ? "RingId" : number

                height: 2*fontMetrics.height
                leftPadding: 10

                verticalAlignment: Text.AlignVCenter
                color: isCurrentItem ?
                    activePalette.highlightedText : inactivePalette.text
            }

            Rectangle {
                color: inactivePalette.text
                height:1
                Layout.fillWidth: true
                visible: componentItem.hasMessage
            }

            Text {
                Layout.fillWidth: true
                visible: componentItem.hasMessage
                text: thirdRowMessage()

                height: 2*fontMetrics.height
                leftPadding: 10

                verticalAlignment: Text.AlignVCenter
                color: isCurrentItem ?
                    activePalette.highlightedText : inactivePalette.text
            }

            Item {
                Layout.fillHeight: true
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
        }

        //FIXME temporary polling until all the objects are correctly in sync
        Timer {
            interval: 5000
            running: true
            repeat: true
            onTriggered: {
                isRecordingBox.visible = isRecording != undefined ? isRecording : false //FIXME
            }
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            z: 99
            onClicked: {
                if (mouse.button == Qt.LeftButton) {
                    recentView.currentIndex = modelIndex.row
                    contactMethodSelected(object)
                }
                else if (mouse.button == Qt.RightButton)
                    contextMenuRequested(object, modelIndex.row)
            }
        }

        StateGroup {
            states: [
                State {
                    name: "demandsAttention"
                    when: unreadTextMessageCount > 0
                    PropertyChanges {
                        target: highlight
                        opacity: 0.1
                        visible: true
                    }

                    PropertyChanges {
                        target: demandsAttention
                        color: "red"
                        opacity: 0.1
                    }
                }
            ]

            transitions: [
                Transition {
                    to: "demandsAttention"
                    NumberAnimation {
                        properties: "opacity"
                        easing.type: Easing.InQuad
                        duration: 1500
                        loops: Animation.Infinite
                    }
                }
            ]
        }
    }
}
