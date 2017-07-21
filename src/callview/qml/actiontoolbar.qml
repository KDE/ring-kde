/***************************************************************************
 *   Copyright (C) 2015 by Emmanuel Lepage Vallee                          *
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

import RingQmlWidgets 1.0

Rectangle {
    id: toolbar
    color: "#55000000"
    height: 60
    width:parent.width
    y:parent.height-toolbar.height -10
    z: 100

    property var userActionModel: null

    // Use a separate label. This allows to use only icons in the buttons,
    // this reducing the footprint and avoiding a second row.
    Rectangle {
        id: currentText
        clip: true
        color: "#333333"
        height: 20
        width: 200
        radius: 99 // circle
        visible: false
        anchors.horizontalCenter: parent.horizontalCenter
        y: - 30
        Text {
            id: currentTextText
            anchors.fill: parent
            color: "white"
            font.bold : true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            onContentWidthChanged: parent.width = contentWidth + 20
            onContentHeightChanged: parent.height = contentHeight + 10
        }

        Behavior on width {
            NumberAnimation {duration: 50}
        }
    }

    // Show the accept and hangup buttons in green and red
    function selectColor(action) {
        if (action == UserActionModel.HANGUP)
            return "#550000";
        else if(action == UserActionModel.ACCEPT)
            return "#005500"

        // Default
        return "#CC222222"
    }

    Component {
        id: actionDelegate


        Item {
            width:  actionGrid.cellWidth
            height: actionGrid.cellHeight
            Rectangle {
                color:  mouseArea.containsMouse ? "#CC333333" : selectColor(action)
                radius: 99 // circle
                anchors.leftMargin: 5
                anchors.rightMargin: 5
                anchors.fill: parent
                border.width:  mouseArea.containsMouse ? 3 : 0
                border.color: "#dd5555"

                Column {
                    width: parent.width
                    anchors.verticalCenter: parent.verticalCenter
                    PixmapWrapper {
                        anchors.horizontalCenter: parent.horizontalCenter
                        pixmap: decoration
                        height: 30
                        width:  30
                    }
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    z: 101
                    onClicked: {
                        userActionModel.execute(action)
                    }
                    onContainsMouseChanged: {
                        if (containsMouse) {
                            currentText.visible = true
                            currentTextText.text = display
                        }
                    }
                }

                Behavior on color {
                    ColorAnimation {duration: 300}
                }

                Behavior on border.width {
                    NumberAnimation {duration: 200}
                }
            }
        }
    }

    GridView  {
        id: actionGrid
        anchors.fill: parent
        model: CallModel.userActionModel.activeActionModel
        delegate: actionDelegate
        cellWidth: 70; cellHeight: 60
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

    onUserActionModelChanged: {
        if (!userActionModel) {
            userActionModel = CallModel.userActionModel
            return
        }

        actionGrid.model = (userActionModel && userActionModel.activeActionModel) ?
            userActionModel.activeActionModel : CallModel.userActionModel.activeActionModel
    }
}
