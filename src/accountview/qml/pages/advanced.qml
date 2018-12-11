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
import org.kde.playground.kquickview 1.0 as KQuickItemViews


Page {
    Kirigami.FormLayout {
        anchors.fill: parent

        SpinBox {
            Kirigami.FormData.label: i18n("Registration expire")
            KQuickItemViews.RoleBinder.modelRole: "registrationExpire"
            KQuickItemViews.RoleBinder.objectProperty: "value"
            RingQtQuick.FieldStatus.name: "registrationExpire"
            visible: RingQtQuick.FieldStatus.available
        }

        TextField {
            Kirigami.FormData.label: i18n("Use custom useragent")
            KQuickItemViews.RoleBinder.modelRole: "hasCustomUserAgent"
            KQuickItemViews.RoleBinder.objectProperty: "text"
            RingQtQuick.FieldStatus.name: "hasCustomUserAgent"
            visible: RingQtQuick.FieldStatus.available
            readOnly: RingQtQuick.FieldStatus.readOnly
        }

        TextField {
            Kirigami.FormData.label: i18n("Useragent")
            KQuickItemViews.RoleBinder.modelRole: "userAgent"
            KQuickItemViews.RoleBinder.objectProperty: "text"
            RingQtQuick.FieldStatus.name: "userAgent"
            visible: RingQtQuick.FieldStatus.available
            readOnly: RingQtQuick.FieldStatus.readOnly
        }

        CheckBox {
            Kirigami.FormData.label: i18n("allowIncomingFromUnknown")
            KQuickItemViews.RoleBinder.modelRole: "allowIncomingFromUnknown"
            KQuickItemViews.RoleBinder.objectProperty: "checked"
            RingQtQuick.FieldStatus.name: "allowIncomingFromUnknown"
            visible: RingQtQuick.FieldStatus.available
        }

        CheckBox {
            Kirigami.FormData.label: i18n("allowIncomingFromHistory")
            KQuickItemViews.RoleBinder.modelRole: "allowIncomingFromHistory"
            KQuickItemViews.RoleBinder.objectProperty: "checked"
            RingQtQuick.FieldStatus.name: "allowIncomingFromHistory"
            visible: RingQtQuick.FieldStatus.available
        }

        CheckBox {
            Kirigami.FormData.label: i18n("allowIncomingFromContact")
            KQuickItemViews.RoleBinder.modelRole: "allowIncomingFromContact"
            KQuickItemViews.RoleBinder.objectProperty: "checked"
            RingQtQuick.FieldStatus.name: "allowIncomingFromContact"
            visible: RingQtQuick.FieldStatus.available
        }

        CheckBox {
            Kirigami.FormData.label: i18n("hasProxy")
            KQuickItemViews.RoleBinder.modelRole: "hasProxy"
            KQuickItemViews.RoleBinder.objectProperty: "checked"
            RingQtQuick.FieldStatus.name: "hasProxy"
            visible: RingQtQuick.FieldStatus.available
        }

        TextField {
            Kirigami.FormData.label: i18n("proxy")
            KQuickItemViews.RoleBinder.modelRole: "proxy"
            KQuickItemViews.RoleBinder.objectProperty: "text"
            RingQtQuick.FieldStatus.name: "proxy"
            visible: RingQtQuick.FieldStatus.available
            readOnly: RingQtQuick.FieldStatus.readOnly
        }

        CheckBox {
            Kirigami.FormData.label: i18n("sipStunEnabled")
            KQuickItemViews.RoleBinder.modelRole: "sipStunEnabled"
            KQuickItemViews.RoleBinder.objectProperty: "checked"
            RingQtQuick.FieldStatus.name: "sipStunEnabled"
            visible: RingQtQuick.FieldStatus.available
        }

        TextField {
            Kirigami.FormData.label: i18n("sipStunServer")
            KQuickItemViews.RoleBinder.modelRole: "sipStunServer"
            KQuickItemViews.RoleBinder.objectProperty: "text"
            RingQtQuick.FieldStatus.name: "sipStunServer"
            visible: RingQtQuick.FieldStatus.available
            readOnly: RingQtQuick.FieldStatus.readOnly
        }

        CheckBox {
            Kirigami.FormData.label: i18n("turnEnabled")
            KQuickItemViews.RoleBinder.modelRole: "turnServerEnabled"
            KQuickItemViews.RoleBinder.objectProperty: "checked"
            RingQtQuick.FieldStatus.name: "turnServerEnabled"
            visible: RingQtQuick.FieldStatus.available
        }

        TextField {
            Kirigami.FormData.label: i18n("turnServer")
            KQuickItemViews.RoleBinder.modelRole: "turnServer"
            KQuickItemViews.RoleBinder.objectProperty: "text"
            RingQtQuick.FieldStatus.name: "turnServer"
            visible: RingQtQuick.FieldStatus.available
            readOnly: RingQtQuick.FieldStatus.readOnly
        }

        CheckBox {
            Kirigami.FormData.label: i18n("upnpEnabled")
            KQuickItemViews.RoleBinder.modelRole: "isUpnpEnabled"
            KQuickItemViews.RoleBinder.objectProperty: "checked"
            RingQtQuick.FieldStatus.name: "isUpnpEnabled"
            visible: RingQtQuick.FieldStatus.available
        }

        CheckBox {
            Kirigami.FormData.label: i18n("presenceEnabled")
            KQuickItemViews.RoleBinder.modelRole: "presenceEnabled"
            KQuickItemViews.RoleBinder.objectProperty: "checked"
            RingQtQuick.FieldStatus.name: "presenceEnabled"
            visible: RingQtQuick.FieldStatus.available
        }

        CheckBox {
            Kirigami.FormData.label: i18n("isVideoEnabled")
            KQuickItemViews.RoleBinder.modelRole: "isVideoEnabled"
            KQuickItemViews.RoleBinder.objectProperty: "checked"
            RingQtQuick.FieldStatus.name: "isVideoEnabled"
            visible: RingQtQuick.FieldStatus.available
        }

        CheckBox {
            Kirigami.FormData.label: i18n("hasActiveCallLimit")
            KQuickItemViews.RoleBinder.modelRole: "hasActiveCallLimit"
            KQuickItemViews.RoleBinder.objectProperty: "checked"
            RingQtQuick.FieldStatus.name: "hasActiveCallLimit"
            visible: RingQtQuick.FieldStatus.available
        }

        SpinBox {
            Kirigami.FormData.label: i18n("activeCallLimit")
            KQuickItemViews.RoleBinder.modelRole: "activeCallLimit"
            KQuickItemViews.RoleBinder.objectProperty: "value"
            RingQtQuick.FieldStatus.name: "activeCallLimit"
            visible: RingQtQuick.FieldStatus.available
        }

        ComboBox {
            Kirigami.FormData.label: i18n("dTMFType")
            KQuickItemViews.RoleBinder.modelRole: "dTMFType"
            RingQtQuick.FieldStatus.name: "dTMFType"
            visible: RingQtQuick.FieldStatus.available
        }
    }
}
