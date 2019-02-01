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
import QtQuick 2.7
import org.kde.kirigami 2.0 as Kirigami
import org.kde.ringkde.genericutils 1.0 as GenericUtils
import QtQuick.Layouts 1.0 as Layouts

Layouts.RowLayout {
    anchors.right: parent.right
    anchors.top: parent.top

    Behavior on x {
        NumberAnimation {duration: 350; easing.type: Easing.OutQuad}
    }

    Item {
        Layouts.Layout.fillWidth: true
    }

    GenericUtils.OutlineButton {
        label: "  "+i18n("Scan a QR Code")
        visible: false //Not implemented
        height: 24
        alignment: Qt.AlignRight
        Layouts.Layout.maximumWidth: width
        icon: "image://SymbolicColorizer/:/sharedassets/outline/qrcode.svg"
    }

    GenericUtils.OutlineButton {
        label: "  "+i18n("Close")
        height: 24
        alignment: Qt.AlignRight
        icon: "image://SymbolicColorizer/:/sharedassets/outline/close.svg"
        onClicked: {
            hide()
        }
    }

    Component.onCompleted: x = 0
}
