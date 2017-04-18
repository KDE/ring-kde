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
import QtQuick 2.0
import Ring 1.0

Rectangle {
    id:     toolbar
    color:  "#55000000"
    height: 26
    width:  parent.width
    z:      100

    ListModel {
        id: actions

        ListElement {
            action: "action_video_rotate_left"
            icon: "image://icon/object-rotate-left"
            label: qsTr( "Rotate left"       )
        }

        ListElement {
            action: "action_video_rotate_right"
            icon: "image://icon/object-rotate-right"
            label: qsTr( "Rotate right"      )
        }

        ListElement {
            action: "action_video_flip_horizontal"
            icon: "image://icon/object-flip-horizontal"
            label: qsTr( "Flip"              )
        }

        ListElement {
            action: "action_video_flip_vertical"
            icon: "image://icon/object-flip-vertical"
            label: qsTr( "Flip"              )
        }

        ListElement {
            action: "action_video_mute"
            icon: "image://icon/camera-web"
            label: qsTr( "Mute"              )
        }

        ListElement {
            action: "action_video_preview"
            icon: "image://icon/view-preview"
            label: qsTr( "Preview"           )
        }

        ListElement {
            action: "action_video_scale"
            icon: "image://icon/transform-scale"
            label: qsTr( "Keep aspect ratio" )
        }

        ListElement {
            action: "action_video_fullscreen"
            icon: "image://icon/view-fullscreen"
            label: qsTr( "Fullscreen"        )
        }
    }

    // Use a separate label. This allows to use only icons in the buttons,
    // this reducing the footprint and avoiding a second row.
    Rectangle {
        id:      currentText
        color:   "#333333"
        height:  20
        width:   200
        radius:  99 // circle
        visible: false
        anchors.horizontalCenter: parent.horizontalCenter
        y: 30
        Text {
            id:           currentTextText
            anchors.fill: parent
            color:        "white"
            font.bold:     true

            horizontalAlignment:    Text.AlignHCenter
            verticalAlignment:      Text.AlignVCenter
            onContentWidthChanged:  parent.width = contentWidth + 20
            onContentHeightChanged: parent.height = contentHeight + 10
        }
    }

    Component {
        id: actionDelegate
        Item {
            width:  30
            height: 22
            Rectangle {
                color:  mouseArea.containsMouse ? "#222222" : "transparent"
                radius: 3
                width:  22
                height: 22
                anchors.centerIn:parent

                Image {
                    anchors.fill: parent
                    source: icon
                }

                MouseArea {
                    id: mouseArea
                    z: 101
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {/*TODO*/}
                    onContainsMouseChanged: {
                        if (containsMouse) {
                            currentText.visible = true
                            currentTextText.text = label
                        }
                    }
                }
            }
        }
    }

    GridView  {
        id: actionGrid
        anchors.fill: parent
        model: actions
        delegate: actionDelegate
        cellWidth: 30; cellHeight: 26
    }

    // Hide the label when the mouse is out
    MouseArea {
        z: -100
        anchors.fill: parent
        hoverEnabled: true
        onContainsMouseChanged: {
            if (!containsMouse)
                currentText.visible = false
        }
    }

    onVisibleChanged: {
        if (!visible)
            currentText.visible = false
    }
}
