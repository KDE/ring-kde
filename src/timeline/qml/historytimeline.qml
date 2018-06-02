/***************************************************************************
 *   Copyright (C) 2018 by Bluesystems                                     *
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
import QtQml.Models 2.2
import RingQmlWidgets 1.0
import Ring 1.0
import org.kde.kirigami 2.2 as Kirigami

Kirigami.Page {
    id: bookmarkList
    signal contactMethodSelected(var cm)

    leftPadding: 0
    topPadding: 0
    bottomPadding: 0
    rightPadding: 0

    SystemPalette {
        id: inactivePalette
        colorGroup: SystemPalette.Disabled
    }

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    FontMetrics {
        id: fontMetrics
    }

    Rectangle {
        color: activePalette.base
        anchors.fill: parent

        ListView {
            id: treeView
            anchors.fill: parent
            width: parent.width

            function selectItem(index) {
                treeView.currentIndex = index
            }

            model: EventModel
            delegate: Text {
                text: display
            }
//             sortingEnabled: true

            highlight: Item {
                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 5
                    anchors.leftMargin: 10
                    anchors.rightMargin: 10
                    radius: 5
                    color: activePalette.highlight
                }
            }

//             Component {
//                 id: masterComponent
//                 Loader {
//                     Component {
//                         id: categoryComponent
//                         Item {
//                             height: catName.implicitHeight + 3
//                             width: bookmarkList.width
//                             Text {
//                                 anchors.topMargin: 3
//                                 id: catName
//                                 text: display
//                                 color: inactivePalette.text
//                             }
//
//                             Rectangle {
//                                 height: 1
//                                 width: parent.width
//                                 color: inactivePalette.text
//                                 anchors.top: parent.top
//                             }
//                         }
//                     }
//
//                     Component {
//                         id: contactComponent
//                         BookmarkCard {
//                             width: treeView.width
//                         }
//                     }
//
//                     sourceComponent: objectType ? contactComponent : categoryComponent
//                 }
//             }
        }
    }
}
