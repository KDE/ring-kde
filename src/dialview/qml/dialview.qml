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
import QtQuick.Controls 2.0 as Controls
import org.kde.kirigami 2.2 as Kirigami

Item {
    SystemPalette {
        id: inactivePalette
        colorGroup: SystemPalette.Disabled
    }

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    Kirigami.ScrollablePage {
        id: page
        anchors.fill: parent
        title: "Calls"

        CallList {
            anchors.fill: parent
        }

        actions {
            main: Kirigami.Action {
                iconName: CallModel.hasDialingCall ? "dialog-cancel" : "document-edit"
                text: "Main Action Text"
                checkable: true
                onCheckedChanged: {
                    //sheet.sheetOpen = checked;
                    if (checked)
                        CallModel.selectDialingCall()
                    else if (CallModel.hasDialingCall)
                        CallModel.dialingCall().performAction(Call.REFUSE)

                }
            }
        }
    }

    DialPad {
        width: parent.width
        visible: CallModel.supportsDTMF
//             anchors.bottom: page.actions.main.top
        height: 200
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
//             anchors.bottomMargin: page.actions.main.height + 20
        anchors.bottomMargin: 50
    }
}
