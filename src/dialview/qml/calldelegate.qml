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

import org.kde.playground.kquickitemviews 1.0 as KQuickItemViews
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import org.kde.ringkde.jamidialview 1.0 as JamiDialView
import org.kde.kirigami 2.2 as Kirigami

Item {
    id: item
    height: callDelegateItem.visible ? callDelegateItem.height :
        confDelegateLayout.height + 40 + 12
    width: parent.width - 10
    x: 5
    property bool selected: object == RingSession.callModel.selectedCall

    //If it is not a conference, use this delegate
    JamiDialView.CallDelegateItem {
        id: callDelegateItem

        //Geometry
        width: parent.width
        anchors.margins: 2
    }

    Rectangle {
        id: confDelegateItem

        //Geometry
        anchors.topMargin: 10
        anchors.margins: 2
        anchors.fill: parent
        visible: false
        y: 10

        //Display
        color: item.selected ? activePalette.highlight: "transparent"
        radius: 5
        border.color: inactivePalette.text
        border.width: 2

        Component {
            id:confItemDelegate
            JamiDialView.CallDelegateItem {
                width: isConference ? item.width - 40 : item.width - 20
            }
        }

        Row {
            y:parent.y
            x:parent.x
            spacing: 4
            height: 20
            width: parent.width

            KQuickItemViews.DecorationAdapter {
                pixmap: decoration
                height: 20
                width: 20
            }

            Text {
                text: i18n("Conference")
                font.bold: true
                font.underline: true
                color: activePalette.text
            }

            MouseArea {
                anchors.fill: parent
                propagateComposedEvents: true
                onClicked: {
                    RingSession.callModel.selectedCall = object
                }
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
            confDelegateVisualDataModel.model = RingSession.callModel
            confDelegateVisualDataModel.delegate = confItemDelegate
            confDelegateRepeater.model.rootIndex = confDelegateRepeater.model.modelIndex(index)
        }
    }
}
