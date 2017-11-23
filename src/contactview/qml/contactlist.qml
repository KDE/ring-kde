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
import QtQml.Models 2.2
import RingQmlWidgets 1.0
import Ring 1.0

Item {
    id: contactList

    SystemPalette {
        id: inactivePalette
        colorGroup: SystemPalette.Disabled
    }

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        color: activePalette.base
        anchors.fill: parent
        QuickTreeView {
            id: treeView

            function selectItem(index) {
                treeView.currentIndex = index

                console.log("SELECT!", index, treeView.currentIndex)
            }

            anchors.fill: parent
            rawModel: SortedContactModel
            delegate: masterComponent

            highlight: Rectangle {
                color: activePalette.highlight
            }

            Component {
                id: masterComponent
                Loader {
                    Component {
                        id: categoryComponent
                        Item {
                            height: catName.implicitHeight + 3
                            width: contactList.width
                            Text {
                                anchors.topMargin: 3
                                id: catName
                                text: display
                                color: inactivePalette.text
                            }

                            Rectangle {
                                height: 1
                                width: parent.width
                                color: inactivePalette.text
                                anchors.top: parent.top
                            }
                        }
                    }

                    Component {
                        id: contactComponent
                        Item {
                            width: contactList.width
                            height: 56 + rowCount*(displayNameLabel.implicitHeight+4)
                            RowLayout {
                                anchors.topMargin: 5
                                anchors.bottomMargin: 5
                                anchors.fill: parent
                                Item {
                                    width:  56
                                    height: 56
                                    anchors.verticalCenter: parent.verticalCenter
                                    PixmapWrapper {
                                        anchors.fill: parent
                                        anchors.margins: 5
                                        pixmap: decoration
                                    }
                                }
                                ColumnLayout {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Text {
                                        Layout.fillWidth: true
                                        id: displayNameLabel
                                        text: display
                                        color: "white"
                                        font.bold: true
                                    }
                                    Text {
                                        Layout.fillWidth: true
                                        visible: rowCount == 0
                                        color: inactivePalette.text
                                        text: "TODO"
                                    }
                                    Item {
                                        Layout.fillHeight: true
                                    }
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    console.log("PA0")
                                    treeView.selectItem(modelIndex)
                                }
                            }
                        }
                    }

                    Component {
                        id: phoneNumberComponent
                        Item {
                            height: 0
                            Rectangle {
                                color: "blue"
                                y: -48 + index*(numberField.implicitHeight+4)
                                x: 64
                                height: 30
                                Text {
                                    id: numberField
                                    anchors.fill: parent
                                    text: display
                                    color: inactivePalette.text
                                }
                            }
                        }
                    }

                    function selectDelegate() {
                        if (objectType == 0)
                            return contactComponent
                        else if (objectType == 1)
                            return phoneNumberComponent

                        return categoryComponent
                    }

                    sourceComponent: selectDelegate()
                }
            }
        }
    }
}
