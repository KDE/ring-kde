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
import QtQuick.Controls 1.4 as Controls1
import QtQml.Models 2.2
import Ring 1.0

Rectangle {
    id: recordingHistory
    property var currentContactMethod: null
    property var selectedElement : Undefined

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    SystemPalette {
        id: inactivePalette
        colorGroup: SystemPalette.Disabled
    }

    color: activePalette.base

//     onCurrentContactMethodChanged: currentContactMethod ?
//         recordingList.model = currentContactMethod.callsModel : null


    Component {
        id: recordingListDelegate
        Item {
            height: 30
            width: parent.width
            anchors.leftMargin: 5
            Text {
                text: display
                color: activePalette.text
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    selectItem(index)
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent

        AudioPlayer {
            Layout.fillWidth: true
        }

        Controls1.ScrollView {
            id: scrollView
            Layout.fillWidth: true
            Layout.fillHeight: true
            flickableItem.interactive: true
            clip: true

            Column {
                width: parent.width

                Repeater {
                    model: RecordingModel

                    ColumnLayout {
                        ColumnLayout {
                            Rectangle {
                                width: scrollView.width;
                                height: 1;
                                color: inactivePalette.text
                            }
                            Text {
                                text: display
                                color: inactivePalette.text
                            }
                        }
                        Repeater {
                            id: childrenView

                            model: VisualDataModel {
                                id: childrenVisualDataModel
                                model: RecordingModel
                                Component.onCompleted: {
                                    childrenView.model.rootIndex = childrenView.model.modelIndex(index)
                                }
                                delegate: Rectangle {
                                    id: recordingItem
                                    height: 30
                                    width: scrollView.width;
                                    anchors.leftMargin: 5
                                    color: "transparent"
                                    Text {
                                        text: display
                                        color: activePalette.text
                                    }
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            // Update the visual selection
                                            color = activePalette.highlight
                                            if (recordingHistory.selectedElement && recordingHistory.selectedElement != recordingItem) {
                                                recordingHistory.selectedElement.color = "transparent"
                                            }
                                            recordingHistory.selectedElement = recordingItem

                                            // Update the model selection
                                            RecordingModel.selectionModel.setCurrentIndex(
                                                childrenView.model.modelIndex(index),
                                                ItemSelectionModel.ClearAndSelect
                                            )
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
