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

        SpinBox {
            Kirigami.FormData.label: i18n("localPort")
            KQuickItemViews.RoleBinder.objectProperty: "value"
            KQuickItemViews.RoleBinder.modelRole: "localPort"
            RingQtQuick.FieldStatus.name: "localPort"
            visible: RingQtQuick.FieldStatus.available
        }

        CheckBox {
            Kirigami.FormData.label: i18n("publishedSameAsLocal")
            KQuickItemViews.RoleBinder.objectProperty: "checked"
            KQuickItemViews.RoleBinder.modelRole: "publishedSameAsLocal"
            RingQtQuick.FieldStatus.name: "publishedSameAsLocal"
            visible: RingQtQuick.FieldStatus.available
        }

        SpinBox {
            Kirigami.FormData.label: i18n("publishedPort")
            KQuickItemViews.RoleBinder.objectProperty: "value"
            KQuickItemViews.RoleBinder.modelRole: "publishedPort"
            RingQtQuick.FieldStatus.name: "publishedPort"
            visible: RingQtQuick.FieldStatus.available
        }

        TextField {
            Kirigami.FormData.label: i18n("publishedAddress")
            KQuickItemViews.RoleBinder.objectProperty: "text"
            KQuickItemViews.RoleBinder.modelRole: "publishedAddress"
            RingQtQuick.FieldStatus.name: "publishedAddress"
            visible: RingQtQuick.FieldStatus.available
            readOnly: RingQtQuick.FieldStatus.readOnly
        }

        SpinBox {
            Kirigami.FormData.label: i18n("audioPortMin")
            KQuickItemViews.RoleBinder.objectProperty: "value"
            KQuickItemViews.RoleBinder.modelRole: "audioPortMin"
            RingQtQuick.FieldStatus.name: "audioPortMin"
            visible: RingQtQuick.FieldStatus.available
        }

        SpinBox {
            Kirigami.FormData.label: i18n("audioPortMax")
            KQuickItemViews.RoleBinder.objectProperty: "value"
            KQuickItemViews.RoleBinder.modelRole: "audioPortMax"
            RingQtQuick.FieldStatus.name: "audioPortMax"
            visible: RingQtQuick.FieldStatus.available
        }

        SpinBox {
            Kirigami.FormData.label: i18n("videoPortMin")
            KQuickItemViews.RoleBinder.objectProperty: "value"
            KQuickItemViews.RoleBinder.modelRole: "videoPortMin"
            RingQtQuick.FieldStatus.name: "videoPortMin"
            visible: RingQtQuick.FieldStatus.available
        }

        SpinBox {
            Kirigami.FormData.label: i18n("videoPortMax")
            KQuickItemViews.RoleBinder.objectProperty: "value"
            KQuickItemViews.RoleBinder.modelRole: "videoPortMax"
            RingQtQuick.FieldStatus.name: "videoPortMax"
            visible: RingQtQuick.FieldStatus.available
        }

    }
}
