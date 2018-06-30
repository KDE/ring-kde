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
import QtGraphicalEffects 1.0
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

        QuickTreeView {
            id: treeView
            anchors.fill: parent

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

        // It needs to be here due to z-index conflicts between
        // chatScrollView and timelinePage
        Item {
            id: burryOverlay
            z: 2
            visible: false
            opacity: 0
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.rightMargin: - 15
            width: scrollbar.fullWidth + 15
            height: chatView.height
            clip: true

            Behavior on opacity {
                NumberAnimation {duration: 300; easing.type: Easing.InQuad}
            }

            Repeater {
                anchors.fill: parent
                model: 5
                FastBlur {
                    anchors.fill: parent
                    source: effectSource
                    radius: 30
                }
            }

            Rectangle {
                anchors.fill: parent
                color: activePalette.base
                opacity: 0.75
            }
        }

        // Add a blurry background
        ShaderEffectSource {
            id: effectSource
            visible: false

            sourceItem: chatView
            anchors.right: timelinePage.right
            anchors.top: timelinePage.top
            width: scrollbar.fullWidth + 15
            height: chatView.height

            sourceRect: Qt.rect(
                burryOverlay.x,
                burryOverlay.y,
                burryOverlay.width,
                burryOverlay.height
            )
        }

        TimelineScrollbar {
            id: scrollbar
            z: 1000
            width: 10
            height: parent.height
            anchors.right: parent.right
            display: treeView.moving
            model: CategorizedBookmarkModel
            view: treeView

            onWidthChanged: {
                burryOverlay.width = scrollbar.fullWidth + 15
            }

            onOverlayVisibleChanged: {
                burryOverlay.visible = overlayVisible
                burryOverlay.opacity = overlayVisible ? 1 : 0
                effectSource.visible = overlayVisible
            }
        }
    }
}
