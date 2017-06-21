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
import QtQuick.Controls 2.0
import Ring 1.0
import RingQmlWidgets 1.0

Item {
    id: componentItem
    width: parent.width
    height: 70 + (temporary ? 30 : 0)

    property QtObject contactMethod: object

    RowLayout {
        anchors.margins: 3
        anchors.fill: parent
        Item {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 2
            anchors.leftMargin: 2
            height: 46
            width:  46

            Rectangle {
                radius: 5
                color: "white"
                opacity: 0.05
                anchors.fill: parent
            }

            PixmapWrapper {
                anchors.fill: parent
                pixmap: decoration
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            RowLayout {
                Text {
                    Layout.fillWidth: true
                    text: display
                    font.bold: true
                    color: "white"
                }
                Text {
                    anchors.rightMargin: 5
                    text: formattedLastUsed
                    color: "gray"
                }
                Item {
                    width: 2
                }
            }
            RowLayout {
                Layout.fillWidth: true
                PixmapWrapper {
                    height: 16
                    width:  16
                    pixmap: categoryIcon
                }
                Text {
                    text: categoryName+"  "
                    color: "gray"
                }
                Text {
                    Layout.fillWidth: true
                    text: uri
                    color: "white"
                }
            }
            RowLayout {
                Layout.fillWidth: true
                Text {
                    color: nameStatus == NumberCompletionModel.SUCCESS ?
                        "green"  : (nameStatus == NumberCompletionModel.IN_PROGRESS ?
                        "yellow" :
                        "red")
                    visible: temporary
                    text: nameStatusString
                }
                Item {
                    Layout.fillWidth: true
                }
                Rectangle {
                    color: activePalette.highlight
                    radius: 99
                    height: 16
                    visible: accountAlias != ""
                    width: 100 //TODO use font metric
                    Text {
                        id: accountAliasText
                        anchors.fill:parent
                        anchors.leftMargin: 16
                        anchors.rightMargin: 16
                        text: accountAlias
                        color: activePalette.highlightedText
                    }
                }
            }
            RowLayout {
                visible: temporary
                height: 30
                Layout.preferredHeight: 30
                Layout.fillWidth: true
                Rectangle {
                    id: contactRequestButton
                    anchors.margins: 3
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "transparent"
                    radius: 5
                    border.width: 1
                    border.color: "white"
                    opacity: 0.8
                    Behavior on color {
                        ColorAnimation {duration:100}
                    }
                    Text {
                        anchors.centerIn: parent
                        text: "Send request"
                        color: "white"
                    }
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onContainsMouseChanged: {
                            contactRequestButton.color = containsMouse ? "#55ffffff" : "transparent"
                        }
                    }
                }
                Rectangle {
                    id: callButton
                    anchors.margins: 3
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "transparent"
                    radius: 5
                    border.width: 1
                    border.color: "white"
                    opacity: 0.8
                    Behavior on color {
                        ColorAnimation {duration:100}
                    }
                    Text {
                        anchors.centerIn: parent
                        text: "Call"
                        color: "white"
                    }
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onContainsMouseChanged: {
                            callButton.color = containsMouse ? "#55ffffff" : "transparent"
                        }
                    }
                }
            }
            Item {
                Layout.fillHeight: true
            }
        }
    }


    Rectangle {
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 3
        width: parent.width
        height: 1
        color: "gray"
        opacity: 0.7
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            searchView.currentIndex = index
            contactMethodSelected(object)
        }
    }
}
