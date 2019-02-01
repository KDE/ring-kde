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
import QtQuick.Layouts 1.0 as Layouts
import QtGraphicalEffects 1.0

import org.kde.kirigami 2.0 as Kirigami
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import org.kde.ringkde.jamisearch 1.0 as JamiSearch

Item {
    id: seachOverlay
    visible: searchState.activeState || searchState.firstSearchState

    // Properties
    property var source: null
    property bool active: searchState.display
    property QtObject searchBox: null
    property bool displayWelcome: searchState.firstSearchState
    property alias currentItem: searchView.currentItem // used be the search box


    // Signals
    signal contactMethodSelected(var cm)
    signal displayNotFoundMessage()

    // Functions
    function hide() {
        searchBox.hide()
    }

    SystemPalette {
        id: inactivePalette
        colorGroup: SystemPalette.Inactive
    }

    // The algorithm used to define what's visible. It is in an class because
    // the expression size was no longer maintainable.
    JamiSearch.State {
        id: searchState
        searchEmpty: (!searchBox) || searchBox.empty
        focussed: searchBox && searchBox.searchFocus
        onDisplayChanged: {
            if (display) {
                searchBox.searchFocus = true
                searchBox.forceFocus()
            }
        }
    }

    // Track if the user asked to never see some information ever again
    JamiSearch.TipModel {
        id: displayTips
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
            seachOverlay.width,
            seachOverlay.height
        )
    }

    // Background
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

    // Header buttons
    Loader {
        height: active ? searchBox.height : 0
        width: parent.width
        anchors.right: parent.parent.right
        y: -searchBox.height
        clip: true
        active: searchState.activeState
        sourceComponent: JamiSearch.ToolBar {}
    }

    // Content
    Layouts.ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layouts.Layout.fillWidth: true
            Layouts.Layout.preferredHeight: 2*searchBox.height
        }

        // Display a welcome message when the user uses the app for the first time
        JamiSearch.FirstRun {
            id: firstRun
            visible: active
            active: searchState.displayWelcome
            Layouts.Layout.fillWidth: true
            Layouts.Layout.preferredHeight: active ? parent.height* 0.4 : 0
            Layouts.Layout.maximumHeight: active ? parent.height* 0.4 : 0
        }

        // Display an icon for each search categories and match the colors with
        // colored symbols in each search entry
        JamiSearch.MatchCategories {
            id: filterList
            active: searchState.displaySearchCategories
            opacity: searchView.count > 0 ? 1 : 0
            Layouts.Layout.fillWidth: true
            Layouts.Layout.preferredHeight: active ?
                30 + Kirigami.Units.fontMetrics.height*1.5 : 0
        }

        // Display some tips and help to new users
        JamiSearch.SearchTip {
            active: searchState.displaySearchHelp
            Layouts.Layout.fillWidth: true
            Layouts.Layout.preferredHeight: active ? height : 0
            Layouts.Layout.maximumHeight: active ? height : 0
        }

        // Display the results for the current query
        JamiSearch.Results {
            id: searchView
            Layouts.Layout.fillWidth: true
            Layouts.Layout.fillHeight: true
            z: 99999999
        }
    }

    // When the user presses Return with nothing selected, print a message
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
        color: Kirigami.Theme.textColor
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
                when: searchState.inactiveState

                PropertyChanges {
                    target:  burryOverlay
                    visible: false
                    opacity: 0
                }
            },
            State {
                name: "searchActive"
                when: searchState.activeState

                PropertyChanges {
                    target:  burryOverlay
                    visible: true
                    opacity: 1
                }
            },
            State {
                name: "firstSearch"
                extend: "searchActive"
                when: searchState.firstSearchState
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
