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
import QtQuick.Layouts 1.0
import org.kde.kirigami 2.2 as Kirigami

import RingQmlWidgets 1.0
import net.lvindustries.ringqtquick 1.0 as RingQtQuick

FocusScope {
    id: dialView
    focus: true
    anchors.fill: parent

    signal selectCall(RingQtQuick.Call call)

    SystemPalette {
        id: inactivePalette
        colorGroup: SystemPalette.Disabled
    }

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    FontMetrics {
        id: fontMetrics
    }

    TreeHelper {
        id: completionSelection
        selectionModel: RingSession.numberCompletionModel.selectionModel
    }

    CallList {
        anchors.fill: parent
    }

    DialPad {
        width: parent.width
        visible: RingSession.callModel.supportsDTMF
        height: 200
        anchors.horizontalCenter: dialView.horizontalCenter
        anchors.bottom: actionToolbar.top
        anchors.bottomMargin: 50
    }

    ActionToolbar {
        id: actionToolbar
        width: parent.width
        anchors.bottom: parent.bottom
    }

    function selectPrevious(call) {
        if (call.state == RingQtQuick.Call.DIALING && completionSelection.selectPrevious())
            return

        completionSelection.clearSelection()

        var idx = RingSession.callModel.getIndex(call)

        if (!idx.valid)
            return

        var directPrev = RingSession.callModel.index(idx.row-1, 0, idx.parent)

        var nextCall = RingSession.callModel.getCall(directPrev)

        if (!nextCall)
            return

        RingSession.callModel.selectedCall = nextCall

        dialView.selectCall(nextCall)
    }

    function selectNext(call) {
        if (call.state == RingQtQuick.Call.DIALING && completionSelection.selectNext())
            return

        completionSelection.clearSelection()

        var idx = RingSession.callModel.getIndex(call)

        if (!idx.valid)
            return

        var directPrev = RingSession.callModel.index(idx.row+1, 0, idx.parent)

        var nextCall = RingSession.callModel.getCall(directPrev)

        if (!nextCall)
            return

        RingSession.callModel.selectedCall = nextCall

        dialView.selectCall(nextCall)
    }

    function performCall() {
        var call = RingSession.callModel.selectedCall

        if (!call) {
            call = RingSession.callModel.dialingCall()
            RingSession.callModel.selectedCall = call
        }

        // Apply the auto completion
        if (call.state == RingQtQuick.Call.DIALING && RingSession.numberCompletionModell.selectedContactMethod)
            call.peerContactMethod = RingSession.ompletionModel.selectedContactMethod

        call.performAction(RingQtQuick.Call.ACCEPT)
    }


    function getCall() {
        var call = RingSession.callModel.selectedCall

        if (!call) {
            call = RingSession.callModel.dialingCall()
            RingSession.callModel.selectedCall = call
        }

        return call
    }

    Keys.onPressed: {
        switch (event.key) {
            case Qt.Key_Up:
                selectPrevious(getCall())
                break
            case Qt.Key_Down:
                selectNext(getCall())
                break
            case Qt.Key_Escape:
                getCall().performAction(RingQtQuick.Call.REFUSE)
                break
            case Qt.Key_Backspace:
                getCall().backspaceItemText()
                break;
            case Qt.Key_Return:
            case Qt.Key_Enter:
                performCall()
                break
            default:
                // Prevent "control" from creating a dialing call
                if (event.text == "")
                    return

                var call = getCall()

                call.appendText(event.text)
                call.playDTMF(event.text)
        }
    }

    Connections {
        target: RingSession.callModel
        onCallAttentionRequest: {
            RingSession.callModel.selectedCall = call
        }
    }

    Component.onCompleted: {
        dialView.forceActiveFocus()
    }

    MouseArea {
        z: 9999
        anchors.fill: parent
        propagateComposedEvents: true
        onClicked: {
            mouse.accepted = false
            mouse.refused = true
            dialView.focus = true
            dialView.forceActiveFocus()
        }
    }
}
