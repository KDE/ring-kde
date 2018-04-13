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
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0
import Ring 1.0
import RingQmlWidgets 1.0
import org.kde.kirigami 2.0 as Kirigami

import Ring 1.0

Item {
    id: seachOverlay
    property var source: null
    property bool active: searchBox.searchFocus
    property alias currentIndex: searchView.currentIndex

    signal contactMethodSelected(var cm)

    function hide() {
        searchBox.hide()
    }

    // Add a blurry background
    ShaderEffectSource {
        id: effectSource
        visible: false

        sourceItem: seachOverlay.source
        anchors.fill: seachOverlay

        sourceRect: Qt.rect(
            0,
            0,
            parent.width,
            parent.height
        )
    }

    Item {
        id: burryOverlay
        visible: false
        opacity: 0
        anchors.fill: parent
        clip: true

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
            color: inactivePalette.highlight
            opacity: 0.75
        }
    }

    SearchBox {
        id: searchBox
        recentView: searchView
        width: topLevel.width
        xPadding: dockBar.width
        onContactMethodSelected: {
            searchBox.hide()
            seachOverlay.contactMethodSelected(cm)
        }
    }

    // The close and QR CODE buttons

    Loader {
        height: 30
        width: parent.width - searchBox.labelWidth
        anchors.right: parent.right
        clip: true
        active: seachOverlay.active
        sourceComponent: RowLayout {

            Behavior on x {
                NumberAnimation {duration: 350; easing.type: Easing.OutQuad}
            }

            Item {
                Layout.fillWidth: true
            }

            OutlineButton {
                label: i18n("Scan a QR Code")
                height: 24
                alignment: Qt.AlignRight
                Layout.maximumWidth: width
                icon: "image://SymbolicColorizer/:/sharedassets/outline/qrcode.svg"
            }

            OutlineButton {
                label: i18n("Close")
                height: 24
                alignment: Qt.AlignRight
                onClicked: {
                    searchBox.hide()
                }
            }

            Component.onCompleted: x = 0
        }
    }

    ListModel {
        id: searchCategoryModel

        ListElement {
            name: "Web"
            elemColor: "#2c53bd"
            src: "image://SymbolicColorizer/?color=#2c53bd;:/sharedassets/outline/web.svg"
        }
        ListElement {
            name: "Bookmarks"
            elemColor: "#cfa02a"
            src: "image://SymbolicColorizer/?color=#cfa02a;:/toolbar/bookmark.svg"
        }
        ListElement {
            name: "Contacts"
            elemColor: "#14883b"
            src: "image://SymbolicColorizer/?color=#14883b;:/toolbar/contact.svg"
        }
        ListElement {
            name: "History"
            elemColor: "#be3411"
            src: "image://SymbolicColorizer/?color=#be3411;:/toolbar/history.svg"
        }
    }

    Loader {
        id: filterList
        active: searchView.count > 0 && searchStateGroup.state == "searchActive"
        opacity: searchView.count > 0 ? 1 : 0
        height: 30 + Kirigami.Units.fontMetrics.height*1.5
        y: searchBox.y+searchBox.height+5
        width: parent.width

        Behavior on opacity {
            NumberAnimation {duration: 150;  easing.type: Easing.InQuad}
        }

        sourceComponent: Row {
            anchors.fill: parent
            Repeater {
                model: searchCategoryModel
                Item {
                    width: parent.width / 4
                    height: sourceName.implicitHeight + 34
                    Rectangle {
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: 30
                        height: 30
                        radius: 99
                        border.width: 2
                        border.color: elemColor
                        color: "transparent"
                        Image {
                            width: parent.width*0.8
                            height: width
                            anchors.centerIn:parent
                            sourceSize.width: width
                            sourceSize.height: width
                            source: src
                        }
                    }
                    Text {
                        id: sourceName
                        anchors.bottom: parent.bottom
                        anchors.topMargin: 4
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: activePalette.text
                        text: name
                    }
                }
            }
        }
    }

    // Display some tips and help to new users
    Loader {
        active: seachOverlay.active && !filterList.active //TODO add a variable to never show again
        y: 100
        sourceComponent: Text {
            color: "red"
            text: "explain the 'card' and 'sources'"
        }
    }

    FindPeers {
        id: searchView
        visible: false
        anchors.fill: parent
        anchors.topMargin: filterList.y + filterList.height
        z: 99999999
    }

    // Search
    StateGroup {
        id: searchStateGroup
        states: [
            State {
                name: "searchActive"
                when: seachOverlay.active

                PropertyChanges {
                    target:  searchView
                    visible: true
                }

                PropertyChanges {
                    target:  burryOverlay
                    visible: true
                    opacity: 1
                }
                PropertyChanges {
                    target:  effectSource
                    sourceRect: Qt.rect(0, 0, parent.width, parent.height)
                }
            }
        ]

        transitions: [
            Transition {
                to: "searchActive"
                NumberAnimation {
                    properties: "opacity"
                    easing.type: Easing.InQuad
                    duration: 200
                    loops: 1
                }
                NumberAnimation {
                    properties: "width,height"
                    easing.type: Easing.InQuad
                    duration: 0
                    loops: 1
                }
            }
        ]
    }
}
