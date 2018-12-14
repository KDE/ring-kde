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
import QtQuick 2.8
import org.kde.kirigami 2.5 as Kirigami
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import org.kde.playground.kquickitemviews 1.0 as KQuickItemViews

/**
 * This page contains the most basic elements necessary to create a SIP or
 * Ring/Jami account.
 */
Page {
    Kirigami.FormLayout {
        anchors.fill: parent

        TextField {
            id: alias
            KQuickItemViews.RoleBinder.modelRole: "alias"
            KQuickItemViews.RoleBinder.objectProperty: "text"
            RingQtQuick.FieldStatus.name: "alias"
            visible: RingQtQuick.FieldStatus.available
            Kirigami.FormData.label: i18n("Alias")
            readOnly: RingQtQuick.FieldStatus.readOnly
        }

        KQuickItemViews.ComboBoxView {
//             KQuickItemViews.RoleBinder.modelRole: "profile"
//             KQuickItemViews.RoleBinder.objectProperty: "text"
            id: profile
//             model: ProfileModel
            Kirigami.FormData.label: i18n("Profile")
//             textRole: "display"
            selectionModel: ProfileModel.getAccountSelectionModel(object)
        }

        KQuickItemViews.ComboBoxView {
            id: protocol
//             KQuickItemViews.RoleBinder.modelRole: "protocol"
//             KQuickItemViews.RoleBinder.objectProperty: "text"
//             model: object.protocolModel
            Kirigami.FormData.label: i18n("Protocol")
//             editable: RingQtQuick.FieldStatus.readOnly
            selectionModel: protocolModel.selectionModel
        }

        TextField {
            id: registeredname
            KQuickItemViews.RoleBinder.modelRole: "registeredName"
            KQuickItemViews.RoleBinder.objectProperty: "text"
            RingQtQuick.FieldStatus.name: "registeredName"
            visible: RingQtQuick.FieldStatus.available
            Kirigami.FormData.label: i18n("Registered name")
            readOnly: RingQtQuick.FieldStatus.readOnly
        }

        TextField {
            id: displayname
            KQuickItemViews.RoleBinder.modelRole: "displayName"
            KQuickItemViews.RoleBinder.objectProperty: "text"
            RingQtQuick.FieldStatus.name: "displayName"
            visible: RingQtQuick.FieldStatus.available
            Kirigami.FormData.label: i18n("Display name")
            readOnly: RingQtQuick.FieldStatus.readOnly
        }

        TextField {
            id: name
            Kirigami.FormData.label: i18n("Username")
            RingQtQuick.FieldStatus.name: "username"
            visible: RingQtQuick.FieldStatus.available
            KQuickItemViews.RoleBinder.modelRole: "username"
            KQuickItemViews.RoleBinder.objectProperty: "text"
            readOnly: RingQtQuick.FieldStatus.readOnly
        }

        TextField {
            Kirigami.FormData.label: i18n("Password")
            id: password
            RingQtQuick.FieldStatus.name: "password"
            visible: RingQtQuick.FieldStatus.available
            KQuickItemViews.RoleBinder.modelRole: "password"
            KQuickItemViews.RoleBinder.objectProperty: "text"
            readOnly: RingQtQuick.FieldStatus.readOnly
        }

        TextField {
            Kirigami.FormData.label: i18n("Hostname")
            id: server
            RingQtQuick.FieldStatus.name: "hostname"
            visible: RingQtQuick.FieldStatus.available
            KQuickItemViews.RoleBinder.modelRole: "hostname"
            KQuickItemViews.RoleBinder.objectProperty: "text"
            readOnly: RingQtQuick.FieldStatus.readOnly
        }
//         }

//         server
//         name
//         password
//         autoAnswer


    }
}
