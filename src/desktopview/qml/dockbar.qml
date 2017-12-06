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
import QtQuick.Controls 2.0 as Controls
import org.kde.kirigami 2.2 as Kirigami
import QtQuick.Window 2.2

Image {
    id: dockBar

    property string selectedItem: "timeline"

    source: "gui/icons/assets/toolbar_bg.png"
    fillMode: Image.Tile
    width: 48
    height: parent.height

    ListModel {
        id: icons
        ListElement {
            name: "timeline"
            icon: "/toolbar/timeline.svg"
            enabled2: true
            actionName: "showTimelineDockAction"
        }
        ListElement {
            name: "call"
            icon: "/toolbar/call.svg"
            enabled2: true
            actionName: "showDialDockAction"
        }
        ListElement {
            name: "contact"
            icon: "/toolbar/contact.svg"
            enabled2: true
            actionName: "showContactDockAction"
        }
        ListElement {
            name: "bookmark"
            icon: "/toolbar/bookmark.svg"
            enabled2: false
            actionName: "showBookmarkDockAction"
        }
        ListElement {
            name: "history"
            icon: "/toolbar/history.svg"
            enabled2: false
            actionName: "showHistoryDockAction"
        }
    }

    Column {
        Repeater {
            model: icons
            Rectangle {
                id: action
                color: dockBar.selectedItem == name ? "#111111" : "transparent"
                height: 58
                width: 48
                visible: getAction().checked

                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    source: icon
                    width: 48
                    height: 48
                    fillMode: Image.PreserveAspectFit
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        dockBar.selectedItem = name
                    }
                }

                function getAction() {
                    return ActionCollection[actionName]
                }

                Connections {
                    target: getAction()
                    onToggled: action.enabled = ActionCollection[actionName].checked
                }
            }
        }
    }
}
