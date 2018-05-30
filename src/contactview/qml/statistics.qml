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
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

ColumnLayout {
    id: statistics
    property var individual: null
    property var labelColor: inactivePalette.text

    function getLastContacted() {
        if (!individual)
            return

        if (individual.lastUsedTime == 0)
            return i18n("<b>Never contacted</b>")

        return i18n("<b>Last contacted on:</b> ") + individual.formattedLastUsedTime
    }

    function getTotalCall() {
        if (!individual)
            return

        var label = i18n("<b>Called:</b> ")

        if (individual.totalSpentTime == 0)
            return label + i18n("Never")

        return label + individual.callCount +
            " time (" + (Math.floor(individual.totalSpentTime/60)) + " minutes)"
    }

    function getTotalText() {
        if (!individual)
            return

        var label = i18n("<b>Texted:</b> ")

        if (individual.textMessageCount == 0)
            return label + i18n("Never")

        return label + individual.textMessageCount
    }

    Rectangle {
        color: activePalette.text
        height: 1
        opacity: 0.3
        Layout.fillWidth: true
    }

    Label {
        id: lastContactedTime
        color: contactViewPage.labelColor ? contactViewPage.labelColor : activePalette.text
        text: individual ? getLastContacted() : ""
    }

    Label {
        id: totalCall
        color: contactViewPage.labelColor ? contactViewPage.labelColor : activePalette.text
        text: individual ? getTotalCall() : ""
    }

    Label {
        id: totalText
        color: contactViewPage.labelColor ? contactViewPage.labelColor : activePalette.text
        text: individual ? getTotalText() : ""
    }

    Rectangle {
        color: contactViewPage.state == "mobile" ? "transparent" : activePalette.text
        opacity: 0.3
        height: 1
        Layout.fillWidth: true
        Layout.fillHeight: contactViewPage.state == "mobile"
    }
}
