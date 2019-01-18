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
import org.kde.ringkde.jamicontactview 1.0 as JamiContactView
// import org.kde.kirigami 2.0 as Kirigami

MouseArea {
    id: componentItem
    width: parent != undefined ? parent.width : undefined
    height: computeHeight()
    acceptedButtons: Qt.LeftButton | Qt.RightButton
    hoverEnabled: true

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

    onClicked: {
        if (mouse.button == Qt.LeftButton) {
            recentView.currentIndex = rootIndex.row
            individualSelected(object)
        }
        else if (mouse.button == Qt.RightButton)
            contextMenuRequested(object.lastUsedContactMethod, rootIndex.row)
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
        id: outline
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
                    height: parent.height
                    width:  parent.height

                    JamiContactView.ContactPhoto {
                        anchors.margins: 3
                        anchors.fill: parent
                        individual: object
                        defaultColor: isCurrentItem ?
                            activePalette.highlightedText : activePalette.text
                        drawEmptyOutline: false
                    }
                }

                Text {
                    text: object.bestName
                    clip: true
                    font.bold : true
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter
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
                text: object.lastUsedUri.length == 40 ? "RingId" : object.lastUsedUri

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

            Loader {
                id: messageLoader
                active: true
                Layout.fillWidth: true
                Layout.minimumHeight: contentHeight2 * 2
                Layout.maximumHeight: contentHeight2 * 2
                property real contentHeight2: 10

                sourceComponent: MouseArea {
                    id: markAsRead
                    z: 101
                    anchors.fill: messageLoader

                    Component.onCompleted: {
                        messageLoader.contentHeight2 = unreadLabel.implicitHeight
                    }

                    Text {
                        id: unreadLabel
                        Layout.fillWidth: true
                        visible: componentItem.hasMessage
                        text: thirdRowMessage()
                        anchors.verticalCenter: parent.verticalCenter

                        height: 2*fontMetrics.height
                        leftPadding: 10

                        verticalAlignment: Text.AlignVCenter
                        color: isCurrentItem ?
                            activePalette.highlightedText : inactivePalette.text

                        onImplicitHeightChanged: {
                            messageLoader.contentHeight2 = implicitHeight
                        }
                    }

                    onClicked: {
                        if (object)
                            object.markAsRead()

                        mouse.accepted = true
                    }

                    Rectangle {
                        id: button
                        anchors.leftMargin: 7
                        anchors.rightMargin: 7
                        anchors.bottomMargin: 7
                        color: "transparent"
                        visible: false
                        radius: 5
                        anchors.fill: parent
                        border.width: 1
                        border.color: outline.border.color

                        Text {
                            text: i18n("Mark as read")
                            color: isCurrentItem ?
                                activePalette.highlightedText : activePalette.text
                            font.bold: true
                            anchors.centerIn: parent
                        }
                    }

                    states: [
                        State {
                            name: ""
                            PropertyChanges {
                                target: button
                                visible: false
                            }
                        },
                        State {
                            name: "hover"
                            when: messageLoader.state == "hover"
                            PropertyChanges {
                                target: button
                                visible: true
                                color: isCurrentItem ?
                                    activePalette.highlight : activePalette.base
                            }
                        }
                    ]
                }

                states: [
                    State {
                        name: ""
                    },
                    State {
                        name: "show"
                        when: componentItem.hasMessage && !componentItem.containsMouse
                        PropertyChanges {
                            target: messageLoader
                            active: true
                        }
                    },
                    State {
                        name: "hover"
                        extend: "show"
                        when:  componentItem.hasMessage && componentItem.containsMouse
                    }
                ]
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
            source: object.hasBookmarks ? "icons/bookmarked.svg" : "icons/not_bookmarked.svg"
            z: 100
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    mouse.accepted = true
                    var cm = object.firstBookmark

                    if (!cm)
                        cm = object.lastUsedContactMethod

                    if (cm)
                        cm.bookmarked = !cm.bookmarked
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
