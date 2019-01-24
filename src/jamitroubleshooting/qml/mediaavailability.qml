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
import QtQuick.Controls 2.0 as Controls
import org.kde.ringkde.genericutils 1.0 as GenericUtils

import net.lvindustries.ringqtquick.media 1.0 as RingQtMedia

Rectangle {
    id: mediaAvailability
    property QtObject currentIndividual: null
    property real defaultSize: 48

    property bool accountState: true

    RingQtMedia.AvailabilityTracker {
        id: availabilityTracker
        individual: currentIndividual
    }

    border.width: 1
    border.color: activePalette.text
    color: "transparent"
    radius: 99
    width: defaultSize
    height: defaultSize
    visible: availabilityTracker.hasWarning
    opacity: 0.5

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    Image {
        height: defaultSize
        width: defaultSize
        sourceSize.width: defaultSize
        sourceSize.height: defaultSize
        anchors.verticalCenter: parent.verticalCenter
        source: "image://SymbolicColorizer/:/sharedassets/outline/warning.svg"
    }

    Controls.Label {
        id: errorMessage
        width: parent.width - 52
        height: parent.height
        anchors.right: parent.right
        visible: false
        color: activePalette.text
        wrapMode: Text.WordWrap
    }

    MouseArea {
        id: mouseArea
        hoverEnabled: true
        anchors.fill: parent
    }

    Behavior on width {
        NumberAnimation {duration: 200;  easing.type: Easing.OutQuad }
    }

    Behavior on height {
        NumberAnimation {duration: 200;  easing.type: Easing.OutQuad }
    }

    Behavior on radius {
        NumberAnimation {duration: 100;  easing.type: Easing.OutQuad }
    }

    Behavior on opacity {
        NumberAnimation {duration: 200;  easing.type: Easing.OutQuad }
    }

    StateGroup {
        id: stateGroup
        states: [
            State {
                name: "hover"
                when: mouseArea.containsMouse
                extend: "active"
                PropertyChanges {
                    target: mediaAvailability
                    opacity: 0.8
                    radius: 5
                    height: mediaAvailability.parent.height
                    width: mediaAvailability.parent.width
                }
                PropertyChanges {
                    target: errorMessage
                    visible: true
                    text: availabilityTracker.warningMessage
                }
            },
            State {
                name: "active"
                when: availabilityTracker.hasWarning
                PropertyChanges {
                    target: errorMessage
                    visible: false
                }
                PropertyChanges {
                    target: mediaAvailability
                    visible: true
                    opacity: 0.5
                    radius: 99
                    width: defaultSize
                }
            }
        ]
    }
}
