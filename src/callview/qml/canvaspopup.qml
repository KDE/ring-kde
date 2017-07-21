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
import QtGraphicalEffects 1.0

Loader {
    id: dialog
    property string title: "Titlebar"
    property var contentItem: undefined
    property QtObject container: null


    function show() {
        active = true
    }

    function hide() {
        active = false
    }

    onContainerChanged: {
        if (!container)
            return

        if (contentItem) {
            var wdg = contentItem.createObject(container)
            wdg.anchors.fill = container
        }
    }

    active: false
    asynchronous: true
    clip: true

    sourceComponent: Component {
        Item {

            anchors.margins: 50
            z: 10000

            // Blur
            Item {
                ShaderEffectSource {
                    id: effectSource
                    visible: true
                    sourceItem: dialog.parent
                    sourceRect: Qt.rect(
                        dialog.parent.x,
                        dialog.parent.y,
                        dialog.parent.width,
                        dialog.parent.height
                    )
                }

                id: burryOverlay
                visible: true
                opacity: 0.3
                anchors.fill: dialog
                clip: true

                RecursiveBlur {
                    anchors.fill: parent
                    source: effectSource
                    radius: 16
                    //loops: 30
                }
            }

            // Background
            Rectangle {
                anchors.fill: parent
                color: "#11FFFFFF"
                radius: 10
                border.width: 3
                border.color: "#333333"
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10

                // Titlebar
                MouseArea {
                    Layout.fillWidth: true
                    height: 30
                    drag.target: dialog

                    RowLayout {
                        anchors.fill: parent

                        Text {
                            id: titleText
                            color: "white"
                            text: dialog.title
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter

                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }

                        Image {
                            Layout.fillHeight: true
                            width: 30
                            source: "image://icon/dialog-close"

                            MouseArea {
                                anchors.fill: parent
                                onClicked: dialog.hide()
                            }
                        }
                    }

                    onClicked: {
                        drag.active = true
                    }
                }

                // Popup content
                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Component.onCompleted: {
                        dialog.container = this
                    }
                }
            }
        }
    }
}
