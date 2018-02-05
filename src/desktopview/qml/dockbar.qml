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
import Ring 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0 as Controls
import org.kde.kirigami 2.2 as Kirigami
import QtQuick.Window 2.2

import DesktopView 1.0

Item {
    id: topLevel
    width: dockLayout.implicitWidth
    property var newHolder: null

    RowLayout {
        id: dockLayout
        anchors.fill: parent

        Image {
            id: dockBar

            property string selectedItem: "timeline"

            source: "gui/icons/assets/toolbar_bg.png"
            fillMode: Image.Tile
            width: 48
            height: parent.height

            Layout.fillHeight: true
            Layout.maximumWidth: width

            Behavior on width {
                NumberAnimation {duration: 200;  easing.type: Easing.OutQuad }
            }

            DockModel {
                id: icons
            }

            FontMetrics {
                id: fontMetrics
            }

            Column {
                Repeater {
                    model: icons
                    Rectangle {
                        id: actionIcon
                        color: dockBar.selectedItem == identifier ? "#111111" : "transparent"
                        height: dockBar.width + 10
                        width: dockBar.width

                        Image {
                            anchors.verticalCenter: parent.verticalCenter
                            source: decoration
                            width: dockBar.width
                            height: dockBar.width
                            fillMode: Image.PreserveAspectFit
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                dockBar.selectedItem = identifier

                                // Show the sliding page
                                if (topLevel.state == "mobile")
                                    newHolder.show = true
                            }
                        }

                        Rectangle {
                            color: "red"
                            radius: 99
                            x: 2
                            y: 2
                            width: fontMetrics.height + 4
                            height: fontMetrics.height + 4
                            visible: activeCount > 0
                            Text {
                                color: "white"
                                anchors.centerIn: parent
                                font.bold: true
                                text: activeCount
                            }
                        }
                    }
                }
            }
        }

        Item {
            id: dockHolder
            width: Math.min(335, root.width-48)
            Layout.fillHeight: true
            visible: (
                timelineView.active || dialView.active || contactView.active
            )

            Loader {
                id: timelineView
                active: dockBar.selectedItem == "timeline"
                anchors.fill: parent
                sourceComponent: PeersTimeline {
                    anchors.fill: parent
                    state: topLevel.state
                    onContactMethodSelected: {
                        mainPage.setContactMethod(cm)
                        if (topLevel.state == "mobile")
                            newHolder.show = false
                    }
                }
            }
            Loader {
                id: dialView
                active: dockBar.selectedItem == "call"
                anchors.fill: parent
                sourceComponent: DialView {
                    anchors.fill: parent
                    onSelectCall: {
                        mainPage.showVideo(call)
                    }
                }
            }
            Loader {
                id: contactView
                active: dockBar.selectedItem == "contact"
                anchors.fill: parent
                sourceComponent: ContactList {
                    anchors.fill: parent
                    onContactMethodSelected: {
                        mainPage.setContactMethod(cm)
                    }
                }
            }
        }
    }

    // Has both a desktop and mobile state
    states: [
        // In tablet mode, use 3 columns for the details
        State {
            name: ""
        },
        State {
            name: "mobile"
            ParentChange {
                target: timelineView
                parent: newHolder.container
            }
            ParentChange {
                target: dialView
                parent: newHolder.container
            }
            ParentChange {
                target: contactView
                parent: newHolder.container
            }
            PropertyChanges {
                target: timelineView
                width: Math.min(335, root.width-22)
                x: Math.min(335, root.width-22)
            }

            PropertyChanges {
                target: dockBar
                width: 32
                selectedItem: ""
            }

            PropertyChanges {
                target: dockHolder
                width: 0
            }

            PropertyChanges {
                target: topLevel
                width: 32
//                 visible: false
            }
        }
    ]
}
