/*
 *   Copyright 2018 Fabian Riethmayer
 *   Copyright 2019 Emmanuel Lepage <emmanuel.lepage@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 3, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
import QtQuick 2.6
import org.kde.kirigami 2.4 as Kirigami
import QtQuick.Layouts 1.2 as Layouts
import org.kde.ringkde.basicview 1.0 as BasicView

Kirigami.OverlaySheet {
    property var model;
    id: page
    Kirigami.Theme.colorSet: Kirigami.Theme.View

    leftPadding: 0
    topPadding: 0
    rightPadding: 0
    bottomPadding: 0

    header: Kirigami.Heading {
        level: 2
        text: "Details"
    }

    BasicView.Detail {
        model: page.model
        Layouts.Layout.preferredWidth: 300
    }
}
