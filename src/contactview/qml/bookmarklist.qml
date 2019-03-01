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

import org.kde.kirigami 2.2 as Kirigami
import org.kde.playground.kquickitemviews 1.0 as KQuickItemViews
import org.kde.ringkde.jamicontactview 1.0 as JamiContactView

Kirigami.Page {
    id: bookmarkList
    signal contactMethodSelected(var cm)
    anchors.fill: parent

    leftPadding: 0
    topPadding: 0
    bottomPadding: 0
    rightPadding: 0

    FontMetrics {
        id: fontMetrics
    }

    Rectangle {
        color: Kirigami.Theme.backgroundColor
        anchors.fill: parent

        KQuickItemViews.TreeView {
            id: treeView
            width: bookmarkList.width
            height: bookmarkList.height - sorting.height
            anchors.fill: parent

            function selectItem(index) {
                treeView.currentIndex = index
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
                                color: Kirigami.Theme.textColor
                            }

                            Rectangle {
                                height: 1
                                width: parent.width
                                color: Kirigami.Theme.textColor
                                anchors.top: parent.top
                            }
                        }
                    }

                    Component {
                        id: contactComponent
                        JamiContactView.BookmarkCard {
                            width: treeView.width
                        }
                    }

                    sourceComponent: objectType ? contactComponent : categoryComponent
                }
            }

            model: RingSession.bookmarkModel
            delegate: masterComponent
            sortingEnabled: true

            highlight: Item {
                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 5
                    anchors.leftMargin: 10
                    anchors.rightMargin: 10
                    radius: 5
                    color: Kirigami.Theme.highlightColor
                }
            }
        }

//         // It needs to be here due to z-index conflicts between
//         // chatScrollView and timelinePage
//         Item {
//             id: burryOverlay
//             z: 2
//             visible: false
//             opacity: 0
//             anchors.right: parent.right
//             anchors.top: parent.top
//             anchors.rightMargin: - 15
//             width: scrollbar.fullWidth + 15
//             height: treeView.height
//             clip: true
//
//             Behavior on opacity {
//                 NumberAnimation {duration: 300; easing.type: Easing.InQuad}
//             }
//
//             Repeater {
//                 anchors.fill: parent
//                 model: 5
//                 FastBlur {
//                     anchors.fill: parent
//                     source: effectSource
//                     radius: 30
//                 }
//             }
//
//             Rectangle {
//                 anchors.fill: parent
//                 color: Kirigami.Theme.backgroundColor
//                 opacity: 0.75
//             }
//         }
//
//         // Add a blurry background
//         ShaderEffectSource {
//             id: effectSource
//             visible: false
//
//             sourceItem: treeView
//             anchors.right: bookmarkList.right
//             anchors.top: bookmarkList.top
//             width: scrollbar.fullWidth + 15
//             height: treeView.height
//
//             sourceRect: Qt.rect(
//                 burryOverlay.x,
//                 burryOverlay.y,
//                 burryOverlay.width,
//                 burryOverlay.height
//             )
//         }
//
//         TimelineScrollbar {
//             id: scrollbar
//             z: 1000
//             width: 10
//             height: parent.height
//             anchors.right: parent.right
//             display: treeView.moving
//             model: RingSession.bookmarkModel
//             view: treeView
//
//             onWidthChanged: {
//                 burryOverlay.width = scrollbar.fullWidth + 15
//             }
//
//             onOverlayVisibleChanged: {
//                 burryOverlay.visible = overlayVisible
//                 burryOverlay.opacity = overlayVisible ? 1 : 0
//                 effectSource.visible = overlayVisible
//             }
//         }
    }
}
