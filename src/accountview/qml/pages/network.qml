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
            /*KQuickItemViews.RoleBinder.modelRole: "alias"
            KQuickItemViews.RoleBinder.objectProperty: "text"
            Kirigami.FormData.label: i18n("Alias")*/

        Kirigami.Separator {
            Kirigami.FormData.label: i18n("Network interface")
            Kirigami.FormData.isSection: true
        }

        SpinBox {
            Kirigami.FormData.label: i18n("localPort")
            KQuickItemViews.RoleBinder.objectProperty: "value"
            KQuickItemViews.RoleBinder.modelRole: "localPort"
            RingQtQuick.FieldStatus.name: "localPort"
            visible: RingQtQuick.FieldStatus.available
            from: 1024
            to: 65535
        }

        Kirigami.Separator {
            Kirigami.FormData.label: i18n("Published address")
            Kirigami.FormData.isSection: true
        }

        RadioButton {
            Kirigami.FormData.label: i18n("Same as the local parameters")
            KQuickItemViews.RoleBinder.objectProperty: "checked"
            KQuickItemViews.RoleBinder.modelRole: "publishedSameAsLocal"
            RingQtQuick.FieldStatus.name: "publishedSameAsLocal"
            visible: RingQtQuick.FieldStatus.available
        }

        RadioButton {
            id: customPublished
            Kirigami.FormData.label: i18n("Use custom values")
        }

        SpinBox {
            Kirigami.FormData.label: i18n("Published port")
            KQuickItemViews.RoleBinder.objectProperty: "value"
            KQuickItemViews.RoleBinder.modelRole: "publishedPort"
            RingQtQuick.FieldStatus.name: "publishedPort"
            visible: RingQtQuick.FieldStatus.available
            enabled: customPublished.checked
            from: 1024
            to: 65535
        }

        TextField {
            Kirigami.FormData.label: i18n("Published address")
            KQuickItemViews.RoleBinder.objectProperty: "text"
            KQuickItemViews.RoleBinder.modelRole: "publishedAddress"
            RingQtQuick.FieldStatus.name: "publishedAddress"
            visible: RingQtQuick.FieldStatus.available
            readOnly: RingQtQuick.FieldStatus.readOnly
            enabled: customPublished.checked
        }

        Kirigami.Separator {
            Kirigami.FormData.label: i18n("RTP configuration (not recommended)")
            Kirigami.FormData.isSection: true
        }

        SpinBox {
            Kirigami.FormData.label: i18n("Minimum audio port")
            KQuickItemViews.RoleBinder.objectProperty: "value"
            KQuickItemViews.RoleBinder.modelRole: "audioPortMin"
            RingQtQuick.FieldStatus.name: "audioPortMin"
            visible: RingQtQuick.FieldStatus.available
            from: 1024
            to: 65535
        }

        SpinBox {
            Kirigami.FormData.label: i18n("Maximum audio port")
            KQuickItemViews.RoleBinder.objectProperty: "value"
            KQuickItemViews.RoleBinder.modelRole: "audioPortMax"
            RingQtQuick.FieldStatus.name: "audioPortMax"
            visible: RingQtQuick.FieldStatus.available
            from: 1024
            to: 65535
        }

        SpinBox {
            Kirigami.FormData.label: i18n("Minimum video port")
            KQuickItemViews.RoleBinder.objectProperty: "value"
            KQuickItemViews.RoleBinder.modelRole: "videoPortMin"
            RingQtQuick.FieldStatus.name: "videoPortMin"
            visible: RingQtQuick.FieldStatus.available
            from: 1024
            to: 65535
        }

        SpinBox {
            Kirigami.FormData.label: i18n("Maximum video port")
            KQuickItemViews.RoleBinder.objectProperty: "value"
            KQuickItemViews.RoleBinder.modelRole: "videoPortMax"
            RingQtQuick.FieldStatus.name: "videoPortMax"
            visible: RingQtQuick.FieldStatus.available
            from: 1024
            to: 65535
        }

    }
}
