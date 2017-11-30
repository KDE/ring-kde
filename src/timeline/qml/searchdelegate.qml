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
    height: getHeight()

    function getHeight() {
        return 4*labelHeight + 16 + (temporary ? buttonHeight : 0)
    }

    property QtObject contactMethod: object
    property double buttonHeight: 30
    property double labelHeight: 30

    TextMetrics {
        id: accTextMetrics
        text: accountAlias
    }

    RowLayout {
        anchors.margins: 3
        anchors.fill: parent
        Item {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 2
            anchors.leftMargin: 2
            height: Math.min(46, 4*componentItem.labelHeight + 12)
            width:  Math.min(46, 4*componentItem.labelHeight + 12)

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
                    color: ListView.isCurrentItem ?
                        activePalette.highlightedText : activePalette.text
                }
                Text {
                    anchors.rightMargin: 5
                    text: formattedLastUsed
                    color: ListView.isCurrentItem ?
                        activePalette.highlightedText : inactivePalette.text
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
                    color: ListView.isCurrentItem ?
                        activePalette.highlightedText : inactivePalette.text
                }
                Text {
                    Layout.fillWidth: true
                    text: uri
                    color: ListView.isCurrentItem ?
                        activePalette.highlightedText : activePalette.text
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
                    height: componentItem.labelHeight + 4
                    visible: accountAlias != ""
                    width: accTextMetrics.width + 32
                    Text {
                        id: accountAliasText
                        anchors.centerIn: parent
                        anchors.leftMargin: 16
                        anchors.rightMargin: 16
                        text: accountAlias
                        color: activePalette.highlightedText
                    }
                }
            }
            RowLayout {
                visible: temporary
                height: componentItem.buttonHeight
                Layout.preferredHeight: componentItem.buttonHeight
                Layout.fillWidth: true
                Rectangle {
                    id: contactRequestButton
                    anchors.margins: 3
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "transparent"
                    radius: 5
                    border.width: 1
                    border.color: ListView.isCurrentItem ?
                        activePalette.highlightedText : activePalette.text
                    opacity: 0.8
                    Behavior on color {
                        ColorAnimation {duration:100}
                    }
                    Text {
                        anchors.centerIn: parent
                        text: "Send request"
                        color: ListView.isCurrentItem ?
                        activePalette.highlightedText : activePalette.text
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
                    border.color: ListView.isCurrentItem ?
                        activePalette.highlightedText : activePalette.text
                    opacity: 0.8
                    Behavior on color {
                        ColorAnimation {duration:100}
                    }
                    Text {
                        anchors.centerIn: parent
                        text: "Call"
                        color: ListView.isCurrentItem ?
                        activePalette.highlightedText : activePalette.text
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
        color: ListView.isCurrentItem ?
                        activePalette.highlightedText : inactivePalette.text
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
