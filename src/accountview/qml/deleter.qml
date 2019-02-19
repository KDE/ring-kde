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
import QtQuick 2.8
import QtQuick.Controls 2.2 as Controls

Loader {
    id: accountDeleter

    function deleteAccount(a) {
        accountDeleter.name = a.alias
        accountDeleter.account = a
        accountDeleter.active = true
        accountDeleter.item.open()
    }

    property string name: ""
    property var account: ""
    active: false
    sourceComponent: Controls.Dialog {
        height: 150
        parent: applicationWindow().contentItem
        x: applicationWindow().contentItem.width / 2 - width/2
        y: applicationWindow().contentItem.height / 2 - height/2
        standardButtons: Controls.Dialog.Ok | Controls.Dialog.Cancel
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
        modal: true
        title: i18n("Delete an account")

        Controls.Label {
            text: i18n("<center>Are you sure you want to delete the account called ")
                + name + i18n(". <br><br> This cannot be undone and you will lose the account <b>permanently</b></center>")
        }

        onAccepted: {
            accountDeleter.active = false
            RingSession.accountModel.remove(account)
            RingSession.accountModel.save()
            accountDeleter.account = null
        }

        onRejected: {
            accountDeleter.active = false
            accountDeleter.account = null
        }
    }
}
