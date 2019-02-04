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
import QtQuick.Layouts 1.0

import QtQuick.Controls 2.2
import org.kde.ringkde.jamicontactview 1.0 as JamiContactView
import org.kde.kirigami 2.2 as Kirigami

Dialog {
    id: dialog
    parent: applicationWindow().contentItem
    x: applicationWindow().contentItem.width / 2 - width/2
    y: applicationWindow().contentItem.height / 2 - height/2

    width: applicationWindow().contentItem.width * (
        Kirigami.Settings.isMobile ? 0.9 : 0.66
    )

    height: applicationWindow().contentItem.height * (
        Kirigami.Settings.isMobile ? 0.9 : 0.75
    )

    property QtObject individual: null

    JamiContactView.ContactInfo {
        id: contactInfo
        anchors.fill: parent
        showStat: false
        showImage: true
        showSave: false
        forcedState: "profile"
        individual: dialog.individual
    }

    onAccepted: {
        if (individual)
            contactInfo.save()
    }

    standardButtons: Dialog.Close | Dialog.Save
}
