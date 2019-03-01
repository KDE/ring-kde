/***************************************************************************
 *   Copyright (C) 2019 by Bluesystems                                     *
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
import QtQuick 2.9
import QtQuick.Controls 2.2 as Controls
import org.kde.kirigami 2.4 as Kirigami
import QtQuick.Layouts 1.2 as Layouts

Layouts.RowLayout {
    property alias icon: icn.source
    property alias text: txt.text
    property alias checkable: swt.visible
    property alias checked: swt.checked

    Layouts.Layout.margins: 0
    Layouts.Layout.leftMargin: Kirigami.Units.smallSpacing
    spacing: Kirigami.Units.smallSpacing

    Layouts.Layout.preferredHeight: Math.max(
        Kirigami.Units.fontMetrics.height,
        Kirigami.Units.iconSizes.smallMedium
    )

    Layouts.Layout.maximumHeight: Math.max(
        Kirigami.Units.fontMetrics.height,
        Kirigami.Units.iconSizes.smallMedium
    )

    Layouts.Layout.fillWidth: true

    Kirigami.Icon {
        id: icn
        source: "favorite"
        height: Kirigami.Units.iconSizes.smallMedium
        width: Kirigami.Units.iconSizes.smallMedium
    }

    Controls.Label {
        id: txt
        text: "Bookmark"
        Layouts.Layout.fillWidth: true
    }

    Controls.Switch {
        id: swt
    }
}
