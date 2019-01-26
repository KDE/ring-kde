/***************************************************************************
 *   Copyright (C) 2017 by Bluesystems                                     *
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
import org.kde.kirigami 2.2 as Kirigami


Kirigami.ApplicationItem {
    property alias individual: contactInfo.individual
    property alias showStat     : contactInfo.showStat
    property alias showSave     : contactInfo.showSave
    property alias showImage    : contactInfo.showImage
    property alias forcedState  : contactInfo.forcedState
    property alias isChanged    : contactInfo.isChanged

    function save() {
        contactInfo.save()
    }

    signal changed(bool value)

    onIsChangedChanged: {
        changed(isChanged)
    }

    FontMetrics {
        id: fontMetrics
    }

    ContactInfo {
        id: contactInfo
        anchors.fill: parent
    }
}
