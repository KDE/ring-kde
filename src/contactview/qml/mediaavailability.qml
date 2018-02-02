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
import ContactView 1.0

Rectangle {
    id: mediaAvailability
    property QtObject currentContactMethod: null
    property real defaultSize: 48
    property bool hasIssue: currentContactMethod && (
        // If there's a contact, take the cumulative of all ways to reach it
        (currentContactMethod.contact && (
            (!currentContactMethod.contact.canCall) ||
            (!currentContactMethod.contact.canVideoCall) ||
            (!currentContactMethod.contact.canSendTexts)
        ))
        // Otherwise use the CM directly
        || (
            currentContactMethod.canCall      != ContactMethod.AVAILABLE ||
            currentContactMethod.canVideoCall != ContactMethod.AVAILABLE ||
            currentContactMethod.canSendTexts != ContactMethod.AVAILABLE
        )
    )

    function getFaultyCM() {
        if (currentContactMethod && (
          currentContactMethod.canCall      != ContactMethod.AVAILABLE ||
          currentContactMethod.canVideoCall != ContactMethod.AVAILABLE ||
          currentContactMethod.canSendTexts != ContactMethod.AVAILABLE ))
            return currentContactMethod

        //TODO
        return null
    }

    function getFirstIssue(cm) {
        var textsIssue = cm.canSendTexts
        var videoIssue = cm.canVideoCall
        var audioIssue = cm.canCall

        return Math.max(textsIssue, videoIssue, audioIssue)
    }

    function getErrorMessage() {
        var cm = getFaultyCM()

        if (cm == null) {
            return i18n("This contact has no known phone number or GNU Ring account")
        }

        switch(getFirstIssue(cm)) {
            case ContactMethod.AVAILABLE   :
                return "";
            case ContactMethod.NO_CALL     :
                return i18n("Sending text messages can only happen during an audio call in SIP accounts")
            case ContactMethod.UNSUPPORTED :
                return i18n("This account doesn't support all media")
            case ContactMethod.SETTINGS    :
                return i18n("Video isn't available because it's disabled for this account")
            case ContactMethod.NO_ACCOUNT  :
                return i18n("There is no account capable of reaching this person")
            case ContactMethod.CODECS      :
                return i18n("All video codecs are disabled, video call isn't possible")
            case ContactMethod.ACCOUNT_DOWN:
                return i18n("All accounts capable of reaching this person are currently unavailable")
            case ContactMethod.NETWORK     :
                return i18n("Ring-KDE is experiencing a network issue, please try later")
        }

        return ""
    }

    border.width: 1
    border.color: activePalette.text
    color: "transparent"
    radius: 99
    width: defaultSize
    height: defaultSize
    visible: hasIssue
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

    Label {
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
                    text: getErrorMessage()
                }
            },
            State {
                name: "active"
                when: mediaAvailability.hasIssue
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
