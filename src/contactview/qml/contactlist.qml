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
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.2 as Kirigami
import org.kde.playground.kquickitemviews 1.0 as KQuickItemViews

Kirigami.Page {
    id: contactList
    signal contactMethodSelected(var cm)
    signal individualSelected(var ind)

    leftPadding: 0
    topPadding: 0
    bottomPadding: 0
    rightPadding: 0

    anchors.fill: parent

    function selectContact(index, object, modelIndex) {
        treeView.selectItem(modelIndex)
        contactList.individualSelected(
            object.individual
        )
    }

    mainAction: Kirigami.Action {
        iconName: "list-add"
        onTriggered: {
            addNewContact()
        }
    }

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
            id: sorting
            height: 32
            width: parent.width
            model: RingSession.sontactCategoryModel
            orientation: ListView.Horizontal
            delegate: MouseArea {
                height: 32
                width: content.implicitWidth + 10
                Text {
                    id: content
                    text: display
                    color:activePalette.text
                    font.bold: true
                    anchors.centerIn: parent
                }
                onClicked: {
                    sorting.currentIndex = index
                }
            }

            highlightFollowsCurrentItem: true

            TreeHelper {
                id: helper
                model: RingSession.sontactCategoryModel
                selectionModel: RingSession.contactCategorySelectionModel
            }

            highlight: Item {
                height: 32
                width: 15
                Rectangle {
                    height: 4
                    color:activePalette.highlight
                    width:15
                    y: parent.height/2 + fontMetrics.height/2 + 3
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }

            onCurrentIndexChanged: {
                helper.selectIndex(currentIndex)
            }
        }

        KQuickItemViews.TreeView {
            id: treeView
            anchors.top: sorting.bottom
            anchors.bottom: parent.bottom
            width: contactList.width
            height: contactList.height - sorting.height

            function selectItem(index) {
                treeView.currentIndex = index
            }

            model: RingSession.sortedContactModel
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
                    width: contactList.width
                    Component {
                        id: categoryComponent
                        Item {
                            height: catName.implicitHeight + 3
                            width: contactList.width
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
                        ContactCard {
                            selectionCallback: selectContact
                        }
                    }

                    Component {
                        id: phoneNumberComponent
                        Item {
                            height: 0
                            Rectangle {
                                color: "blue"
                                y: -48 + index*(numberField.implicitHeight+4)
                                x: 64
                                height: 30
                                Text {
                                    id: numberField
                                    anchors.fill: parent
                                    text: display
                                    color: inactivePalette.text
                                }
                            }
                        }
                    }

                    function selectDelegate() {
                        if (objectType == 0)
                            return contactComponent
                        else if (objectType == 1)
                            return phoneNumberComponent

                        return categoryComponent
                    }

                    sourceComponent: selectDelegate()
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
