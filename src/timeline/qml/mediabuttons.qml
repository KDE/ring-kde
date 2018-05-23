/***************************************************************************
 *   Copyright (C) 2017-2018 by Bluesystems                                *
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
import Ring 1.0
import ContactView 1.0

import RingQmlWidgets 1.0
import PhotoSelectorPlugin 1.0

Row {
    //TODO Qt5.10 deps: use native icons

    spacing: 5
    leftPadding: 5

    function getContactMethod(callback) {

        if (currentIndividual == null)
            return

        if (currentIndividual.requireUserSelection) {
            var component = Qt.createComponent("CMSelector.qml")
            if (component.status == Component.Ready) {
                var window = component.createObject(applicationWindow().contentItem)
                window.currentIndividual = currentIndividual
                window.callback = callback
                window.open()
            }
            else
                console.log("ERROR", component.status, component.errorString())
                return
        }

        var cm = currentIndividual.mainContactMethod

        if (!cm)
            return

        if (callback)
            callback(cm)

        return cm
    }

    AvailabilityTracker {
        id: availabilityTracker
        individual: currentIndividual
    }

    Button {
        id: button
        implicitWidth: label.implicitWidth + 20
        visible: availabilityTracker.canCall

        checkable: currentIndividual && currentIndividual.firstActiveCall != null
        checked: currentIndividual && currentIndividual.firstActiveCall

        onClicked: {

            getContactMethod(function(cm) {
                if (!cm)
                    return

                if (cm.hasInitCall) {
                    contactHeader.selectVideo()
                    return
                }

                CallModel.dialingCall(cm)
                    .performAction(Call.ACCEPT)
            })
        }

        Row {
            id: label
            anchors.centerIn: parent
            height: parent.height
            Image {
                height: 22
                width: 22
                sourceSize.width: 22
                sourceSize.height: 22
                anchors.verticalCenter: parent.verticalCenter
                source: "image://SymbolicColorizer/:/sharedassets/outline/call.svg"
            }

            Text {
                anchors.margins: 5
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                color: activePalette.text
                text: "   " + i18n("Call")
            }
        }
    }

    Button {
        id: button2
        implicitWidth: label2.implicitWidth + 20

        visible: availabilityTracker.canVideoCall

        checkable: currentIndividual && currentIndividual.firstActiveCall
        checked: currentIndividual && currentIndividual.firstActiveCall
            && currentIndividual.firstActiveCall.videoRenderer

        onClicked: {
            getContactMethod(function(cm) {
                if (!cm)
                    return


                if (cm.hasInitCall) {
                    contactHeader.selectVideo()
                    return
                }

                CallModel.dialingCall(cm)
                    .performAction(Call.ACCEPT)
            })
        }

        Row {
            id: label2
            anchors.centerIn: parent
            height: parent.height
            Image {
                height: 22
                width: 22
                sourceSize.width: 22
                sourceSize.height: 22
                anchors.verticalCenter: parent.verticalCenter
                source: "image://SymbolicColorizer/:/sharedassets/outline/camera.svg"
            }

            Text {
                anchors.margins: 5
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                color: activePalette.text
                text: "   " + i18n("Video")
            }
        }
    }

    Button {
        id: button3
        implicitWidth: label3.implicitWidth + 20
        checkable: currentIndividual && currentIndividual.firstActiveCall
        checked: currentIndividual && currentIndividual.firstActiveCall != null
            && currentIndividual.firstActiveCall.videoRenderer != null

        visible: availabilityTracker.canVideoCall

        onClicked: {
            getContactMethod(function(cm) {
                if (!cm)
                    return

                if (cm.hasInitCall) {
                    contactHeader.selectVideo()
                    return
                }

                CallModel.dialingCall(cm)
                    .performAction(Call.ACCEPT)
            })
        }

        Row {
            id: label3
            anchors.centerIn: parent
            height: parent.height
            Image {
                height: 22
                width: 22
                sourceSize.width: 22
                sourceSize.height: 22
                anchors.verticalCenter: parent.verticalCenter
                source: "image://SymbolicColorizer/:/sharedassets/outline/screen.svg"
            }

            Text {
                anchors.margins: 5
                color: activePalette.text
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                text: "   " + i18n("Screen sharing")
            }
        }
    }

    Button {
        id: button4
        implicitWidth: label4.implicitWidth + 20

        visible: availabilityTracker.canSendTexts

        onClicked: {
            if (currentIndividual == null) return

            contactHeader.selectChat()
        }

        Row {
            id: label4
            anchors.centerIn: parent
            height: parent.height
            Image {
                height: 22
                width: 22
                sourceSize.width: 22
                sourceSize.height: 22
                anchors.verticalCenter: parent.verticalCenter
                source: "image://SymbolicColorizer/:/sharedassets/outline/chat.svg"
            }

            Text {
                anchors.margins: 5
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                color: activePalette.text
                text: "   " + i18n("Chat")
            }
        }
    }
}
