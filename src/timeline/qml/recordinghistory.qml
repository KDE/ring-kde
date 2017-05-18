import QtQuick 2.7
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.4 as Controls1
import QtQml.Models 2.2
import Ring 1.0

Item {
    id: recordingHistory
    property var currentContactMethod: null
    property var selectedElement : Undefined

//     onCurrentContactMethodChanged: currentContactMethod ?
//         recordingList.model = currentContactMethod.callsModel : null


    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    SystemPalette {
        id: inactivePalette
        colorGroup: SystemPalette.Disabled
    }

    Component {
        id: recordingListDelegate
        Item {
            height: 30
            width: parent.width
            anchors.leftMargin: 5
            Text {
                text: display
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
                                    }
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            // Update the visual selection
                                            color = "red"
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
