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

FocusScope {
    id: dialView
    focus: true
    anchors.fill: parent

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
        selectionModel: CompletionModel.selectionModel
    }

    Kirigami.ScrollablePage {
        id: page
        anchors.fill: parent
        title: "Calls"

//         ColumnLayout {
//             anchors.fill: parent
//
//             Rectangle {
//                 Layout.fillWidth: true
//                 height: 10
//                 color: "blue"
//             }

            CallList {
            anchors.fill: parent
//                 Layout.fillWidth: true
//                 Layout.fillHeight: true
            }

//         }
    }

    DialPad {
        width: parent.width
        visible: CallModel.supportsDTMF
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
        if (call.state == Call.DIALING && completionSelection.selectPrevious())
            return

        var idx = CallModel.getIndex(call)

        if (!idx.valid)
            return

        var directPrev = CallModel.index(idx.row-1, 0, idx.parent)

        var nextCall = CallModel.getCall(directPrev)

        CallModel.selectedCall = nextCall
    }

    function selectNext(call) {
        if (call.state == Call.DIALING && completionSelection.selectNext())
            return

        var idx = CallModel.getIndex(call)

        if (!idx.valid)
            return

        var directPrev = CallModel.index(idx.row+1, 0, idx.parent)

        var nextCall = CallModel.getCall(directPrev)
        CallModel.selectedCall = nextCall
    }

    Keys.onPressed: {
        var call = CallModel.selectedCall

        if (!call) {
            call = CallModel.dialingCall()
            CallModel.selectedCall = call
        }

        switch (event.key) {
            case Qt.Key_Up:
                selectPrevious(call)
                break
            case Qt.Key_Down:
                selectNext(call)
                break
            case Qt.Key_Escape:
                call.performAction(Call.REFUSE)
                break
            case Qt.Key_Backspace:
                call.backspaceItemText()
                break;
            case Qt.Key_Return:
            case Qt.Key_Enter:
                call.performAction(Call.ACCEPT)
                break
            default:
                call.appendText(event.text)
                call.playDTMF(event.text)
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
