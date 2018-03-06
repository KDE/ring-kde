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

        ListModel {
            id: sortingModel
            ListElement {
                name: "By name"
            }
            ListElement {
                name: "By date"
            }
        }


        Row {
            id: sorting
            anchors.top: search.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            height: 32

            Repeater {
                model: sortingModel

                delegate: MouseArea {
                    height: 32
                    width: content.implicitWidth + 10
                    Text {
                        id: content
                        text: name
                        color:activePalette.text
                        font.bold: true
                        anchors.centerIn: parent
                    }
                    onClicked: {
                    }
                }

//                 highlightFollowsCurrentItem: true

    //             highlight: Item {
    //                 height: 32
    //                 width: 15
    //                 Rectangle {
    //                     height: 4
    //                     color:activePalette.highlight
    //                     width:15
    //                     y: parent.height/2 + fontMetrics.height/2 + 3
    //                     anchors.horizontalCenter: parent.horizontalCenter
    //                 }
    //             }

//                 onCurrentIndexChanged: {
//                     helper.selectIndex(currentIndex)
//                 }
            }
        }


        QuickTreeView {
            id: treeView
            anchors.top: sorting.bottom
            anchors.bottom: parent.bottom
            width: parent.width

            function selectItem(index) {
                treeView.currentIndex = index
            }

            rawModel: CategorizedBookmarkModel
            delegate: masterComponent
            sortingEnabled: true

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

            Component {
                id: masterComponent
                Loader {
                    Component {
                        id: categoryComponent
                        Item {
                            height: catName.implicitHeight + 3
                            width: bookmarkList.width
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
                        BookmarkCard {
                            width: treeView.width
                        }
                    }

                    sourceComponent: objectType ? contactComponent : categoryComponent
                }
            }
        }
    }
}
