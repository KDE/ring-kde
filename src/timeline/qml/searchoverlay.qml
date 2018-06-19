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
    property bool delayed: false

    signal contactMethodSelected(var cm)
    signal displayNotFoundMessage()

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

    Timer {
        id: buggyTimer
        repeat: false
        running: true
        interval: 0
        onTriggered: {
            delayed = true
        }
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

    FirstRun {
        id: firstRun
        visible: active
        active: searchStateGroup.state == "firstSearch"
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height* 0.4
    }

    SearchBox {
        id: searchBox
        searchView: searchView
        width: topLevel.width
        xPadding: dockBar.width
    }

    // The close and QR CODE buttons

    Loader {
        height: 30
        width: parent.width - searchBox.labelWidth
        anchors.right: parent.right
        clip: true
        active: seachOverlay.active && searchStateGroup.state != "firstSearch"
        sourceComponent: RowLayout {

            Behavior on x {
                NumberAnimation {duration: 350; easing.type: Easing.OutQuad}
            }

            Item {
                Layout.fillWidth: true
            }

            OutlineButton {
                label: "  "+i18n("Scan a QR Code")
                visible: false //Not implemented
                height: 24
                alignment: Qt.AlignRight
                Layout.maximumWidth: width
                icon: "image://SymbolicColorizer/:/sharedassets/outline/qrcode.svg"
            }

            OutlineButton {
                label: "  "+i18n("Close")
                height: 24
                alignment: Qt.AlignRight
                icon: "image://SymbolicColorizer/:/sharedassets/outline/close.svg"
                onClicked: {
                    hide()
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
    SearchTip {
        active: seachOverlay.active
            && (!filterList.active)
            && searchStateGroup.state != "firstSearch"
            && displayTips.showSearchTip
        anchors.top: searchBox.bottom
        anchors.topMargin: 15
        width: parent.width
    }

    FindPeers {
        id: searchView
        visible: false
        anchors.fill: parent
        anchors.topMargin: filterList.y + filterList.height
        z: 99999999
    }

    Rectangle {
        id: notFoundMessage
        z: 999999999
        visible: false
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 20
        radius: 5
        width: parent.width * 0.80
        height: errorLabel.implicitHeight + 20
        color: inactivePalette.text
        opacity: 0

        Behavior on opacity {
            NumberAnimation {duration: 150; easing.type: Easing.OutQuad}
        }

        Timer {
            id: errorMessageTimer
            running: false
            interval: 3000
            repeat: false

            onTriggered: {
                notFoundMessage.visible = false
                notFoundMessage.opacity = 0
            }
        }

        Text {
            id: errorLabel
            width: parent.width
            y: 10
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            color: inactivePalette.base
            text: i18n("Cannot select this person because it was not found")
        }
    }

    onDisplayNotFoundMessage: {
        errorMessageTimer.running = true
        notFoundMessage.visible = true
        notFoundMessage.opacity = 1
    }

    CheckBox {
        z: 199999999
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 10
        text: i18n("Hide and do not show again")
        checked: false
        checkable: true
        visible: displayTips.showFirstTip && searchStateGroup.state == "firstSearch"
        onCheckStateChanged: {
            searchBox.searchFocus    = false
            displayTips.showFirstTip = false
        }
    }

    // Search
    StateGroup {
        id: searchStateGroup

        states: [
            State {
                name: ""
                when: ((!wizardVisible) && (!seachOverlay.active) && (
                    (!PeersTimelineModel.empty) || (!displayTips.showFirstTip)
                )) || (!delayed)

                ParentChange {
                    target: seachOverlay
                    parent: topLevel
                }

                PropertyChanges {
                    target:  seachOverlay
                    anchors.fill: parent
                }

                PropertyChanges {
                    target:  searchView
                    visible: false
                }

                PropertyChanges {
                    target:  burryOverlay
                    visible: false
                    opacity: 0
                }

                PropertyChanges {
                    target: desktopOverlay
                    visible: false
                }
            },
            State {
                name: "searchActive"
                when: delayed && (!wizardVisible) && seachOverlay.active
                    && (
                        (!PeersTimelineModel.empty) || (!displayTips.showFirstTip)
                    )

                PropertyChanges {
                    target:  seachOverlay
                    anchors.fill: parent
                    width: undefined
                    height: undefined
                }

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

                PropertyChanges {
                    target: desktopOverlay
                    visible: false
                }
            },
            State {
                name: "firstSearch"
                extend: "searchActive"
                when: delayed && (!wizardVisible) && (
                    PeersTimelineModel.empty || !searchBox.empty
                ) && displayTips.showFirstTip
                PropertyChanges {
                    target:  seachOverlay
                    anchors.fill: undefined
                    width: applicationWindow().contentItem.width
                    height: applicationWindow().contentItem.height
                }

                ParentChange {
                    target: seachOverlay
                    parent: desktopOverlay
                }

                PropertyChanges {
                    target: searchBox
                    y: firstRun.height + 10
                    width: seachOverlay.width*0.9
                    x: seachOverlay.width*0.05
                    searchFocus: true
                }

                PropertyChanges {
                    target: desktopOverlay
                    visible: true
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
