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
import QtQuick.Layouts 1.4
import org.kde.kirigami 2.4 as Kirigami
import org.kde.ringkde.basicview 1.0 as BasicView
import org.kde.ringkde.jamicontactview 1.0 as JamiContactView

Kirigami.OverlaySheet {
    Kirigami.Theme.colorSet: Kirigami.Theme.View

    JamiContactView.ContactInfo {
        id: contactInfo

        Layout.preferredWidth: applicationWindow().width * (
            Kirigami.Settings.isMobile ? 0.8 : 0.5
        )

        height: applicationWindow().height  * (
            Kirigami.Settings.isMobile ? 0.8 : 0.5
        )

        individual: mainPage.currentIndividual
        showStat: false
        showImage: true
        showSettings: true
        showSave: true
        forcedState: "profile"
        defaultName: ""
    }
}
