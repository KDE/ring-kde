/******************************************************************************
 *   Copyright (C) 2012 by Savoir-Faire Linux                                 *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>   *
 *                                                                            *
 *   This library is free software; you can redistribute it and/or            *
 *   modify it under the terms of the GNU Lesser General Public               *
 *   License as published by the Free Software Foundation; either             *
 *   version 2.1 of the License, or (at your option) any later version.       *
 *                                                                            *
 *   This library is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU        *
 *   Lesser General Public License for more details.                          *
 *                                                                            *
 *   You should have received a copy of the Lesser GNU General Public License *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *****************************************************************************/
import QtQuick 2.0
import QtQuick.Layouts 1.0
import Ring 1.0

Rectangle {
    id: callDelegateItem
    anchors.margins: 2
    radius: 5
    color: selected ? activePalette.highlight: "transparent"
    height: content.implicitHeight + 20
    property bool selected: object == CallModel.selectedCall

    RowLayout {
        id: content
        spacing: 10
        width: parent.width - 4

        PixmapWrapper {
            pixmap: decoration
            height:40
            width:40
            anchors.verticalCenter: callDelegateItem.verticalCenter
        }

        Column {
            Layout.fillWidth: true

            Text {
                text: display
                width: parent.width
                wrapMode: Text.WrapAnywhere
                color: callDelegateItem.selected ?
                    activePalette.highlightedText : activePalette.text
                font.bold: true
            }
            Text {
                text: model.number
                width: parent.width
                wrapMode: Text.WrapAnywhere
                color: callDelegateItem.selected ?
                    activePalette.highlightedText : activePalette.text
            }
        }
    }

    Text {
        text: length
        color: callDelegateItem.selected ?
            inactivePalette.highlightedText : inactivePalette.text
        anchors.right: parent.right
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            CallModel.selectedCall = object
        }
    }
} //Call delegate

