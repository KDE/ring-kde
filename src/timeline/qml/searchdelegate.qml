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

    property bool showAccount: AccountModel.hasAmbiguousAccounts
    property bool showPhoto: true
    property bool showControls: true
    property bool showSeparator: true
    property var  textColor: ListView.isCurrentItem ?
                        activePalette.highlightedText : activePalette.text
    property var  altTextColor: ListView.isCurrentItem ?
                        activePalette.highlightedText : inactivePalette.text

    width: parent.width
    height: getHeight()

    function getHeight() {
        var rowCount = 2 + (showAccount ? 2 : 0)
        return rowCount*labelHeight + 16 + ((showControls && temporary) ? buttonHeight : 0)
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
        PixmapWrapper {
            visible: componentItem.showPhoto
            height:  Math.min(46, 4*componentItem.labelHeight + 12)
            width:  Math.min(46, 4*componentItem.labelHeight + 12)
            pixmap: decoration
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            RowLayout {
                Text {
                    Layout.fillWidth: true
                    text: display
                    font.bold: true
                    color: textColor
                }
                Text {
                    visible: componentItem.showPhoto
                    anchors.rightMargin: 5
                    text: formattedLastUsed
                    color: altTextColor
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
                    color: altTextColor
                }
                Text {
                    Layout.fillWidth: true
                    text: uri
                    color: textColor
                }
            }
            RowLayout {
                Layout.fillWidth: true
                Text {
                    color: nameStatus == NumberCompletionModel.SUCCESS ?
                        "green"  : (nameStatus == NumberCompletionModel.IN_PROGRESS ?
                        "yellow" :
                        "red")
                    visible: supportsRegistry
                    text: nameStatusString
                }
                Item {
                    Layout.fillWidth: true
                }
                Rectangle {
                    color: activePalette.highlight
                    radius: 99
                    height: componentItem.labelHeight + 4
                    visible: componentItem.showAccount && accountAlias != ""
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
            Loader {
                active:  componentItem.showControls && temporary
                visible: componentItem.showControls && temporary
                height:  componentItem.buttonHeight
                Layout.preferredHeight: componentItem.buttonHeight
                Layout.fillWidth: true

                sourceComponent: RowLayout {
                    anchors.fill: parent
                    Rectangle {
                        id: contactRequestButton
                        anchors.margins: 3
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: "transparent"
                        radius: 5
                        border.width: 1
                        border.color: textColor
                        opacity: 0.8
                        Behavior on color {
                            ColorAnimation {duration:100}
                        }
                        Text {
                            anchors.centerIn: parent
                            text: "Send request"
                            color: textColor
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
                        border.color: textColor
                        opacity: 0.8
                        Behavior on color {
                            ColorAnimation {duration:100}
                        }
                        Text {
                            anchors.centerIn: parent
                            text: "Call"
                            color: textColor
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
            }
            Item {
                Layout.fillHeight: true
            }
        }
    }


    Rectangle {
        visible: componentItem.showSeparator
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
