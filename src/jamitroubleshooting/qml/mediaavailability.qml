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
import org.kde.kirigami 2.2 as Kirigami
import net.lvindustries.ringqtquick.media 1.0 as RingQtMedia

Rectangle {
    id: mediaAvailability
    property QtObject currentIndividual: null
    property real defaultSize: 48
    property bool persistent: false
    property color foreground: Kirigami.Theme.textColor
    property color background: "transparent"

    property bool accountState: true

    RingQtMedia.AvailabilityTracker {
        id: availabilityTracker
        individual: currentIndividual
    }

    border.width: 1
    border.color: foreground
    color: background
    radius: 99
    width: defaultSize
    height: defaultSize
    visible: availabilityTracker.hasWarning
    opacity: persistent ? 1 : 0.5

    implicitHeight: icon.height + errorMessage.implicitHeight
        + 3*Kirigami.Units.largeSpacing

    Image {
        id: icon
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
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        visible: false
        color: foreground
        wrapMode: Text.WordWrap
    }

    MouseArea {
        id: mouseArea
        hoverEnabled: true
        anchors.fill: parent
    }

    Behavior on width {
        enabled: !persistent
        NumberAnimation {duration: 200;  easing.type: Easing.OutQuad }
    }

    Behavior on height {
        enabled: !persistent
        NumberAnimation {duration: 200;  easing.type: Easing.OutQuad }
    }

    Behavior on radius {
        enabled: !persistent
        NumberAnimation {duration: 100;  easing.type: Easing.OutQuad }
    }

    Behavior on opacity {
        enabled: !persistent
        NumberAnimation {duration: 200;  easing.type: Easing.OutQuad }
    }

    StateGroup {
        id: stateGroup

        states: [
            State {
                name: "hover"
                when: mouseArea.containsMouse && !mediaAvailability.persistent
                extend: "active"
                PropertyChanges {
                    target: mediaAvailability
                    opacity: persistent ? 1 : 0.8
                    radius: 5
                    height: mediaAvailability.parent.height
                    width: mediaAvailability.parent.width
                }
                PropertyChanges {
                    target: errorMessage
                    visible: true
                    text: availabilityTracker.warningMessage
                }
                AnchorChanges {
                    target: icon
                    anchors.top: undefined
                    //anchors.left: mediaAvailability.left
                }
            },
            State {
                name: "active"
                when: availabilityTracker.hasWarning && !mediaAvailability.persistent
                PropertyChanges {
                    target: errorMessage
                    visible: false
                }
                AnchorChanges {
                    target: icon
                    anchors.top: undefined
                    //anchors.left: mediaAvailability.left
                }
                PropertyChanges {
                    target: mediaAvailability
                    visible: true
                    opacity: persistent ? 1 : 0.5
                    radius: 99
                    width: defaultSize
                }
            },
            State {
                name: "persistent"
                extend: "hover"
                when: availabilityTracker.hasWarning && mediaAvailability.persistent
                PropertyChanges {
                    target: icon
                    anchors.verticalCenter: undefined
                    anchors.horizontalCenter: mediaAvailability.horizontalCenter
                    anchors.topMargin: Kirigami.Units.largeSpacing
                }
                PropertyChanges {
                    target: mediaAvailability
                    height: icon.implicitHeight
                        + errorMessage.implicitHeight
                        + 2*Kirigami.Units.largeSpacing
                    width: mediaAvailability.parent.width
                }
                AnchorChanges {
                    target: icon
                    anchors.top: mediaAvailability.top
                    //anchors.left: undefined
                }
                AnchorChanges {
                    target: errorMessage
                    anchors.top: icon.bottom
                    anchors.left: mediaAvailability.left
                    anchors.verticalCenter: undefined
                }
            }
        ]
    }
}
