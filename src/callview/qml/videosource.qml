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
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Ring 1.0

Item {
    property QtObject call: null
    property int current: -1
    id: videoSource
    width: 50
    height: elements.implicitHeight

    // Camera popup
    CanvasPopup {
        id: sourcePopup
        parent: videoSource.parent
        width: 400
        height: 400
        x: videoSource.parent.width - width - 50
        y: 50
        title: "Camera settings"

        contentItem: Component {
            ColumnLayout {
                anchors.fill: parent
                DeviceSetting {
                    id: deviceSettings
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
                Item {
                    Layout.fillWidth: true
                    height: applyButton.height
                    Button {
                        id: applyButton
                        text: i18n("Apply")
                        anchors.right: parent.right
                        onClicked: {
                            if (!videoSource.call)
                                return

                            videoSource.call.sourceModel.switchTo(
                                videoSource.current
                            )
                        }
                    }
                }
            }
        }
    }

    // Screen sharing popup
    CanvasPopup {
        id: screenPopup
        parent: videoSource.parent
        width: 400
        height: 400
        x: videoSource.parent.width - width - 50
        y: 50
        title: "Screen sharing"

        contentItem: Component {
            ScreenSharing {
                id: deviceSettings
                call: videoSource.call
                anchors.fill: parent
            }
        }
    }

    // Streaming popup
    CanvasPopup {
        id: filePopup
        parent: videoSource.parent
        width: 400
        height: 400
        x: videoSource.parent.width - width - 50
        y: 50
        title: "Camera settings"

        contentItem: Component {
            Streaming {
                id: deviceSettings
                call: videoSource.call
                anchors.fill: parent
            }
        }
    }

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
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.left
        anchors.margins: 5

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

    onCallChanged: {
        devices.model = call ? call.sourceModel : null
    }

    Connections {
        target: call
        onMediaAdded: {
            devices.model = call.sourceModel
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        ColumnLayout {
            id: elements
            anchors.fill: parent

            Component {
                id: deviceDelegate
                Rectangle {
                    id: backgroundRect
                    width: 50
                    height: 50
                    radius: 99
                    color:  "#CC222222"
                    border.width:  mouseArea.containsMouse ? 3 : 0
                    border.color: "#dd5555"
                    PixmapWrapper {
                        anchors.fill: parent
                        anchors.margins: 10
                        pixmap: decoration
                    }

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onContainsMouseChanged: {
                            if (containsMouse) {
                                currentTextText.text = display
                            }
                            backgroundRect.color = containsMouse ? "#CC333333" : "#CC222222"
                        }
                        onClicked: {
                            videoSource.current = index

                            switch(index) {
                                case SourceModel.NONE:
                                    videoSource.call.sourceModel.switchTo(0)
                                    return
                                case SourceModel.SCREEN:
                                    screenPopup.show()
                                    return
                                case SourceModel.FILE:
                                    filePopup.show()
                                    return
                            }

                            sourcePopup.show()
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

            Repeater {
                id: devices
                delegate: deviceDelegate
                model: videoSource.call ? videoSource.call.sourceModel : null
            }
        }
        onContainsMouseChanged: {
            currentText.visible = containsMouse
        }
    }

}
