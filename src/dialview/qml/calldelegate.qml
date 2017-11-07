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
import Ring 1.0

Item {
    id: item
    height: confDelegateLayout.height + 40 + 12
    width: parent.width - 20

    //If it is not a conference, use this delegate
    CallDelegateItem {
        id: callDelegateItem

        //Geometry
        width: parent.width - 20
        y: 5
        x: 10
        anchors.margins: 2
    }

    Rectangle {
        id: confDelegateItem

        //Geometry
        anchors.topMargin: 10
        anchors.margins: 2
        anchors.fill: parent
        visible: false
        x: 10
        y: 10

        //Display
        color: "transparent"
        radius: 5
        border.color: inactivePalette.text
        border.width: 2

        Component {
            id:confItemDelegate
            CallDelegateItem {
                width: isConference ? item.width - 40 : item.width - 20
            }
        }

        Row {
            y:parent.y
            x:parent.x
            spacing: 4
            PixmapWrapper {
                pixmap: decoration
                height: 20
                width: 20
            }

            Text {
                text: "Conference"
                font.bold: true
                font.underline: true
                color: activePalette.text
            }
        }

        Column {
            id: confDelegateLayout
            y:30
            width: parent.width - 20
            x: 10
            //anchors.fill: parent
            anchors.margins: 2
            spacing: 4
            Repeater {
                id:confDelegateRepeater
                model:VisualDataModel {
                    id:confDelegateVisualDataModel
                }
            }
        }
    } //Conference delegate

    Component.onCompleted: {
        if (isConference == true) {
            callDelegateItem.visible = false
            confDelegateItem.visible = true
            confDelegateVisualDataModel.model = CallModel
            confDelegateVisualDataModel.delegate = confItemDelegate
            confDelegateRepeater.model.rootIndex = confDelegateRepeater.model.modelIndex(index)
        }
    }

    MouseArea {
        anchors.fill: parent
        visible: !isConference
        onClicked: {
            CallModel.selectedCall = object
        } //onClicked
    } //MouseArea

}
