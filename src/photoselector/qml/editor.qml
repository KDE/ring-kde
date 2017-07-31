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
import QtQuick.Dialogs 1.2

import PhotoSelectorPlugin 1.0

import Ring 1.0

Dialog {
    property QtObject person: null
    signal newPhoto(var photo)
    signal done()

    id: root
    visible: true
    width: 500
    height: 400
    standardButtons: StandardButton.Save | StandardButton.Cancel

    PhotoSelector {
        id: selector
    }

    ColumnLayout {
        anchors.fill: parent

        // The photo fills the item, but is a child of the selection rectangle
        Item {
            id: selectorContainer
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            Rectangle {
                id: overlay
                z: 100

                VideoWidget {
                    id: videoWidget
                    visible: false
                    z: -1
                }

                Image {
                    id: fromFile
                    z: -1
                    visible: true
                    fillMode: Image.PreserveAspectFit
                }

                color: "transparent"
                border.width: 3
                border.color: "red"
            }

            MouseArea {
                anchors.fill: parent
                property bool track: false
                property int rootX: 0
                property int rootY: 0
                onPressed: {
                    track = true
                    rootX = mouse.x
                    rootY = mouse.y
                    overlay.x = mouse.x
                    overlay.y = mouse.y
                    overlay.width = 0
                    overlay.height = 0
                    fromFile.x = -overlay.x
                    fromFile.y = -overlay.y
                    fromFile.width = selectorContainer.width
                    fromFile.height = selectorContainer.height
                    videoWidget.x = -overlay.x
                    videoWidget.y = -overlay.y
                    videoWidget.width = selectorContainer.width
                    videoWidget.height = selectorContainer.height
                }
                onPositionChanged: {

                    if (track) {
                        var w = 0
                        var h = 0
                        if (mouse.x >= rootX) {
                            overlay.x = rootX
                            w = mouse.x - rootX
                        }
                        else {
                            overlay.x = mouse.x
                            w = rootX - mouse.x
                        }

                        if (mouse.y >= rootY) {
                            overlay.y = rootY
                            h = mouse.y - rootY
                        }
                        else {
                            overlay.y = mouse.y
                            h = rootY - mouse.y
                        }

                        var size = w > h ? h : w

                        overlay.width  = size
                        overlay.height = size
                        fromFile.x = -overlay.x
                        fromFile.y = -overlay.y
                        fromFile.width = selectorContainer.width
                        fromFile.height = selectorContainer.height
                        videoWidget.x = -overlay.x
                        videoWidget.y = -overlay.y
                        videoWidget.width = selectorContainer.width
                        videoWidget.height = selectorContainer.height
                        //FIXME bottom-right -> top-left [x,y]
                    }
                }
                onReleased: {
                    track = false
                    overlay.border.color = "transparent"
                    overlay.grabToImage(function(result) {
                        selector.image = result.image;
                        newPhoto(result.image)
                    });
                    overlay.border.color = "red"
                }
            }

            Component.onCompleted: {
                fromFile.x = -overlay.x
                fromFile.y = -overlay.y
                fromFile.width = selectorContainer.width
                fromFile.height = selectorContainer.height
                videoWidget.x = -overlay.x
                videoWidget.y = -overlay.y
                videoWidget.width = selectorContainer.width
                videoWidget.height = selectorContainer.height
            }
        }

        RowLayout {
            Button {
                text: "From file"
                Layout.fillWidth: true
                onClicked: {
                    videoWidget.visible = false
                    fromFile.visible = true
                    fromFile.source = selector.selectFile("")
                }
            }
            Button {
                text: "From camera"
                Layout.fillWidth: true
                onClicked: {
                    videoWidget.visible = true
                    fromFile.visible = false

                    if (!PreviewManager.previewing) {
                        PreviewManager.startPreview()
                        videoWidget.started = true
                    }
                    else {
                        PreviewManager.stopPreview()
                        videoWidget.started = false
                        videoWidget.started = false
                    }
                }
            }
        }
    }

    onAccepted: {
        selector.setToPerson(person)
        videoWidget.started = false
        if (PreviewManager.previewing)
            PreviewManager.stopPreview()

        done()
    }

    onRejected: {
        videoWidget.started = false
        if (PreviewManager.previewing)
            PreviewManager.stopPreview()

        done()
    }
}
