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


Page {
    Kirigami.FormLayout {
        anchors.fill: parent

        Kirigami.Separator {
            Kirigami.FormData.label: i18n("Registration")
            Kirigami.FormData.isSection: true
        }

        SpinBox {
            Kirigami.FormData.label: i18n("Registration expire")
            KQuickItemViews.RoleBinder.modelRole: "registrationExpire"
            KQuickItemViews.RoleBinder.objectProperty: "value"
            RingQtQuick.FieldStatus.name: "registrationExpire"
            visible: RingQtQuick.FieldStatus.available
        }

        TextField {
            Kirigami.FormData.label: i18n("Useragent")
            KQuickItemViews.RoleBinder.modelRole: "userAgent"
            KQuickItemViews.RoleBinder.objectProperty: "text"
            RingQtQuick.FieldStatus.name: "userAgent"
            visible: RingQtQuick.FieldStatus.available
            readOnly: RingQtQuick.FieldStatus.readOnly
            enabled: hasCustomUserAgent
            Kirigami.FormData.checkable: true
            Kirigami.FormData.checked: hasCustomUserAgent
            Kirigami.FormData.onCheckedChanged: {hasCustomUserAgent = checked}
        }

        Kirigami.Separator {
            Kirigami.FormData.label: i18n("Incoming calls")
            Kirigami.FormData.isSection: true
        }

        CheckBox {
            Kirigami.FormData.label: i18n("Allow calls from unknown peers")
            KQuickItemViews.RoleBinder.modelRole: "allowIncomingFromUnknown"
            KQuickItemViews.RoleBinder.objectProperty: "checked"
            RingQtQuick.FieldStatus.name: "allowIncomingFromUnknown"
            visible: RingQtQuick.FieldStatus.available
        }

        CheckBox {
            Kirigami.FormData.label: i18n("Allow calls from previously called peers")
            KQuickItemViews.RoleBinder.modelRole: "allowIncomingFromHistory"
            KQuickItemViews.RoleBinder.objectProperty: "checked"
            RingQtQuick.FieldStatus.name: "allowIncomingFromHistory"
            visible: RingQtQuick.FieldStatus.available
        }

        CheckBox {
            Kirigami.FormData.label: i18n("Allow calls from your contacts")
            KQuickItemViews.RoleBinder.modelRole: "allowIncomingFromContact"
            KQuickItemViews.RoleBinder.objectProperty: "checked"
            RingQtQuick.FieldStatus.name: "allowIncomingFromContact"
            visible: RingQtQuick.FieldStatus.available
        }

        Kirigami.Separator {
            Kirigami.FormData.label: i18n("Negotiation")
            Kirigami.FormData.isSection: true
        }

        TextField {
            Kirigami.FormData.label: i18n("Proxy server URL")
            KQuickItemViews.RoleBinder.modelRole: "proxy"
            KQuickItemViews.RoleBinder.objectProperty: "text"
            RingQtQuick.FieldStatus.name: "proxy"
            visible: RingQtQuick.FieldStatus.available
            readOnly: RingQtQuick.FieldStatus.readOnly
            Kirigami.FormData.checkable: true
            Kirigami.FormData.checked: hasProxy
            enabled: hasProxy
            Kirigami.FormData.onCheckedChanged: {hasProxy = checked}
        }

        TextField {
            Kirigami.FormData.label: i18n("STUN server URL")
            KQuickItemViews.RoleBinder.modelRole: "sipStunServer"
            KQuickItemViews.RoleBinder.objectProperty: "text"
            RingQtQuick.FieldStatus.name: "sipStunServer"
            visible: RingQtQuick.FieldStatus.available
            readOnly: RingQtQuick.FieldStatus.readOnly
            Kirigami.FormData.checkable: true
            Kirigami.FormData.checked: sipStunEnabled
            enabled: sipStunEnabled
            Kirigami.FormData.onCheckedChanged: {sipStunEnabled = checked}
        }

        TextField {
            Kirigami.FormData.label: i18n("TURN server URL")
            KQuickItemViews.RoleBinder.modelRole: "turnServer"
            KQuickItemViews.RoleBinder.objectProperty: "text"
            RingQtQuick.FieldStatus.name: "turnServer"
            visible: RingQtQuick.FieldStatus.available
            readOnly: RingQtQuick.FieldStatus.readOnly
            Kirigami.FormData.checkable: true
            Kirigami.FormData.checked: turnServerEnabled
            enabled: turnServerEnabled
            Kirigami.FormData.onCheckedChanged: {turnServerEnabled = checked}
        }

        Kirigami.Separator {
            Kirigami.FormData.label: i18n("Features")
            Kirigami.FormData.isSection: true
        }

        CheckBox {
            Kirigami.FormData.label: i18n("Enable Plug and Play (UPnP)")
            KQuickItemViews.RoleBinder.modelRole: "isUpnpEnabled"
            KQuickItemViews.RoleBinder.objectProperty: "checked"
            RingQtQuick.FieldStatus.name: "isUpnpEnabled"
            visible: RingQtQuick.FieldStatus.available
        }

        CheckBox {
            Kirigami.FormData.label: i18n("Enable presence broadcasting")
            KQuickItemViews.RoleBinder.modelRole: "presenceEnabled"
            KQuickItemViews.RoleBinder.objectProperty: "checked"
            RingQtQuick.FieldStatus.name: "presenceEnabled"
            visible: RingQtQuick.FieldStatus.available
        }

        CheckBox {
            Kirigami.FormData.label: i18n("Enable video calls")
            KQuickItemViews.RoleBinder.modelRole: "isVideoEnabled"
            KQuickItemViews.RoleBinder.objectProperty: "checked"
            RingQtQuick.FieldStatus.name: "isVideoEnabled"
            visible: RingQtQuick.FieldStatus.available
        }

        SpinBox {
            Kirigami.FormData.label: i18n("Concurrent call limit")
            KQuickItemViews.RoleBinder.modelRole: "activeCallLimit"
            KQuickItemViews.RoleBinder.objectProperty: "value"
            RingQtQuick.FieldStatus.name: "activeCallLimit"
            visible: RingQtQuick.FieldStatus.available
            Kirigami.FormData.checkable: true
            Kirigami.FormData.checked: hasActiveCallLimit
            Kirigami.FormData.onCheckedChanged: {hasActiveCallLimit = checked}
        }

        Row {
            Kirigami.FormData.label: i18n("DTMF type")
                RingQtQuick.FieldStatus.name: "dTMFType"
                visible: RingQtQuick.FieldStatus.available
            RadioButton {
                text: i18n("DTMF over SIP")
                KQuickItemViews.RoleBinder.modelRole: "dTMFType"
                KQuickItemViews.RoleBinder.objectProperty: "checked"
            }
            RadioButton {
                text: i18n("DTMF over RTP")
            }
        }
    }
}
