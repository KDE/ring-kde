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
import org.kde.kirigami 2.2 as Kirigami
import org.kde.ringkde.genericutils 1.0 as GenericUtils
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import net.lvindustries.ringqtquick.media 1.0 as RingQtMedia

Grid {
    id: container
    //TODO Qt5.10 deps: use native icons

    spacing: 5
    leftPadding: 5
    rows: 1

    property real button1IW: button.visible  ? button.implicitWidth  + 5 : 0
    property real button2IW: button2.visible ? button2.implicitWidth + 5 : 0
    property real button3IW: button3.visible ? button3.implicitWidth + 5 : 0
    property real button4IW: button4.visible ? button4.implicitWidth + 5 : 0

    // Favor being on one row, but allow 2 rows too
    property real preferredWidth: Math.max(preferredWidth,
        button1IW + button2IW + button3IW + button4IW
    )

    property real minimumWidth: (button3.visible ? maximumButtonWidth * 2 : 0) + 15

    property real iconifiedWidth: 4*button.implicitHeight + 20

    property real maximumButtonWidth: Math.max(maximumButtonWidth, button3.implicitWidth)


    states: [
        State {
            extend: ""
            name: "2rows"
            when: width > iconifiedWidth && width <= minimumWidth

            PropertyChanges {
                target: container
                rows: 2
            }

            PropertyChanges {
                target: button
                width: maximumButtonWidth
            }

            PropertyChanges {
                target: button2
                width: maximumButtonWidth
            }

            PropertyChanges {
                target: button3
                width: maximumButtonWidth
            }

            PropertyChanges {
                target: button4
                width: maximumButtonWidth
            }
        },
        State {
            name: ""
            when: width > minimumWidth

            PropertyChanges {
                target: container
                rows: 1
            }

            PropertyChanges {
                target: textLabel1
                visible: true
            }

            PropertyChanges {
                target: textLabel2
                visible: true
            }

            PropertyChanges {
                target: textLabel3
                visible: true
            }

            PropertyChanges {
                target: textLabel4
                visible: true
            }

            PropertyChanges {
                target: button
                width: undefined
            }

            PropertyChanges {
                target: button2
                width: undefined
            }

            PropertyChanges {
                target: button3
                width: undefined
            }

            PropertyChanges {
                target: button4
                width: undefined
            }
        },
        State {
            name: "iconified"
            extend: ""
            when: width <= iconifiedWidth

            PropertyChanges {
                target: container
                rows: 1
            }

            PropertyChanges {
                target: textLabel1
                visible: false
            }

            PropertyChanges {
                target: textLabel2
                visible: false
            }

            PropertyChanges {
                target: textLabel3
                visible: false
            }

            PropertyChanges {
                target: textLabel4
                visible: false
            }
        }
    ]

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

    RingQtMedia.AvailabilityTracker {
        id: availabilityTracker
        individual: currentIndividual
    }

    Button {
        id: button
        implicitWidth: label.implicitWidth + 20
        visible: availabilityTracker.canCall

        checkable: checked
        checked: availabilityTracker.audioCallControlState == RingQtMedia.AvailabilityTracker.CHECKED

        onClicked: {
            focus = false

            if (checked)
                return

            // Do not create a new call, just switch the media
            if (currentIndividual && currentIndividual.firstActiveCall) {
                contactHeader.selectVideo()
                currentIndividual.firstActiveCall.sourceModel.switchTo(0)
                return
            }

            getContactMethod(function(cm) {
                if (!cm)
                    return

                if (cm.hasInitCall) {
                    contactHeader.selectVideo()
                    return
                }

                var call = RingSession.callModel.dialingCall(cm)

                call.removeMedia(RingQtMedia.Media.VIDEO)

                call.performAction(RingQtQuick.Call.ACCEPT)
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
                id: textLabel1
                anchors.margins: 5
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                color: Kirigami.Theme.textColor
                text: "   " + i18n("Call")
            }
        }
    }

    Button {
        id: button2
        implicitWidth: label2.implicitWidth + 20

        visible: availabilityTracker.canVideoCall

        checkable: checked
        checked: availabilityTracker.videoCallControlState == RingQtMedia.AvailabilityTracker.CHECKED

        onClicked: {
            focus = false

            if (checked)
                return

            // Do not create a new call, just switch the media
            if (currentIndividual && currentIndividual.firstActiveCall) {
                contactHeader.selectVideo()
                currentIndividual.firstActiveCall.sourceModel.switchTo(3)
                return
            }

            getContactMethod(function(cm) {
                if (!cm)
                    return


                if (cm.hasInitCall) {
                    contactHeader.selectVideo()
                    return
                }

                RingSession.callModel.dialingCall(cm)
                    .performAction(RingQtQuick.Call.ACCEPT)
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
                id: textLabel2
                anchors.margins: 5
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                color: Kirigami.Theme.textColor
                text: "   " + i18n("Video")
            }
        }
    }

    Button {
        id: button3
        implicitWidth: label3.implicitWidth + 20
        checkable: checked
        checked: availabilityTracker.screenSharingControlState == RingQtMedia.AvailabilityTracker.CHECKED

        visible: availabilityTracker.canVideoCall

        onClicked: {
            focus = false

            if (checked)
                return

            // Do not create a new call, just switch the media
            if (currentIndividual && currentIndividual.firstActiveCall) {
                contactHeader.selectVideo()
                currentIndividual.firstActiveCall.sourceModel.switchTo(1)
                return
            }

            getContactMethod(function(cm) {
                if (!cm)
                    return

                if (cm.hasInitCall) {
                    contactHeader.selectVideo()
                    return
                }

                RingSession.callModel.dialingCall(cm)
                    .performAction(RingQtQuick.Call.ACCEPT)
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
                id: textLabel3
                anchors.margins: 5
                color: Kirigami.Theme.textColor
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
                id: textLabel4
                anchors.margins: 5
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                color: Kirigami.Theme.textColor
                text: "   " + i18n("Chat")
            }
        }

    Button {
        id: button5
        implicitWidth: label4.implicitWidth + 20
        text: i18n("Hang up")

        visible: availabilityTracker.hangUpControlState == RingQtMedia.AvailabilityTracker.NORMAL

        onClicked: {
            if (currentIndividual == null) return

            var c = currentIndividual.firstActiveCall

            if (!c)
                return

            c.performAction(RingQtQuick.Call.REFUSE)
        }
    }
    }
}
