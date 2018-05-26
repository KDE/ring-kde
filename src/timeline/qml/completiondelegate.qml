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

    height: content.implicitHeight

    property bool showAccount: AccountModel.hasAmbiguousAccounts
    property bool showPhoto: true
    property bool showControls: true
    property bool showSeparator: true

    property var  textColor: selected ?
        activePalette.highlight : activePalette.highlightedText

    property var  altTextColor: selected ?
        activePalette.highlight : activePalette.highlightedText

    property var baseColor: selected ?
        activePalette.highlightedText : activePalette.highlight

    width: parent.width

    property QtObject contactMethod: object
    property double buttonHeight: 30
    property double labelHeight: fontMetrics.height*2

    TextMetrics {
        id: accTextMetrics
        text: accountAlias
    }

    RowLayout {
        id: content
        anchors.margins: 3
        anchors.fill: parent
        spacing: 0
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
                spacing: 2
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

            Loader {
                active: componentItem.showAccount && accountAlias != ""
                Layout.fillWidth: true
                Layout.preferredHeight: active ? componentItem.labelHeight + 14 : 0
                Layout.minimumHeight: active ? componentItem.labelHeight +  14 : 0
                sourceComponent: Item {
                    anchors.fill: parent
                    Rectangle {
                        color: textColor
                        radius: 99
                        implicitHeight: componentItem.labelHeight + 4
                        height: implicitHeight
                        width: accTextMetrics.width + height

                        anchors.top: parent.top
                        anchors.topMargin: 3
                        anchors.right: parent.right
                        anchors.margins: 3

                        Text {
                            id: accountAliasText
                            anchors.centerIn: parent
                            anchors.leftMargin: 16
                            anchors.rightMargin: 16
                            text: accountAlias
                            color: baseColor
                        }
                    }
                }
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            searchView.currentIndex = index
            contactMethodSelected(object)
        }
    }
}
