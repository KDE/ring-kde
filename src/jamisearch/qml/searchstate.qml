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

/**
 * Avoids a spaghetti mess of multi line, multi state expressions to define
 * how to display the search popup.
 */
QtObject {
    // States
    property bool inactiveState: false
    property bool firstSearchState: false
    property bool activeState: false
    property string state: "inactive"
    property bool display: _delayed && (firstSearchState || activeState)

    // Extra widgets
    property bool displaySearchHelp: false
    property bool displaySearchCategories: false
    property bool displayWelcome: searchEmpty && firstSearchState

    // External status
    property bool searchEmpty: false
    property bool focussed: false

    // Geometry
    property var normalSearchArea: undefined
    property var firstSearchArea: undefined

    // Internal
    property bool _delayed: false
    property bool _hasSearched: false

    property var _c1: Connections {
        target: displayTips
        onShowFirstTipChanged: _evalState()
    }

    property var _c2: Connections {
        target: RingSession.peersTimelineModel
        onEmptyChanged: _evalState()
    }

    function _evalState() {
        // Do not evaluate during initialization as it wont have the
        // whole picture of what is and isn't set.
        if (!_delayed)
            return

        var isFirst = displayTips.showFirstTip
            && RingSession.peersTimelineModel.empty

        // State
        if (isFirst) {
            firstSearchState = true
            activeState = false
            inactiveState = false
        }
        else if (focussed) {
            firstSearchState = false
            activeState = true
            inactiveState = false
        }
        else {
            firstSearchState = false
            activeState = false
            inactiveState = true
        }

        // Extra widgets
        displaySearchHelp       = activeState && searchEmpty
        displaySearchCategories = activeState && (!searchEmpty)

        var newState = ""

        if (firstSearchState)
            newState = "firstSearch"
        else if (activeState)
            newState = "active"
        else
            newState = "inactive"

        // Prevent too many signals
        if (newState != state)
            state = newState
    }

    onFocussedChanged: _evalState()
    onSearchEmptyChanged: {
        _hasSearched = _hasSearched || !searchEmpty
        _evalState()
    }

    /**
     * QML properties are evaluated in a random order, this exists to stop
     * trying to handle everything.
     *
     * Keep in mind that some other components already have hacks to handle
     * this. So this has to be done /after/ those hacks
     * (which cannot be detected), hence the timer.
     */
    property var _t: Timer {
        repeat: false
        running: true
        interval: 100
        onTriggered: {
            _delayed = true
            _evalState()
        }
    }
}
