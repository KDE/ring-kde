/***************************************************************************
 *   Copyright (C) 2017-2019 by Bluesystems                                *
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
import QtQuick.Controls 2.0 as Controls
import QtQuick.Layouts 1.0 as Layouts
import org.kde.kirigami 2.2 as Kirigami

Layouts.ColumnLayout {
    id: statistics
    property var individual: null
    property var labelColor: Kirigami.Theme.textColor

    Layouts.Layout.topMargin: 0
    Layouts.Layout.bottomMargin: 0
    Layouts.Layout.leftMargin: 0
    Layouts.Layout.rightMargin: 0

    Row {
        Item {width: Kirigami.Units.smallSpacing; height: 1}
        Kirigami.Icon {
            source: "appointment-new"
            height: Kirigami.Units.iconSizes.smallMedium
            width: Kirigami.Units.iconSizes.smallMedium
        }
        Item {width: Kirigami.Units.smallSpacing; height: 1}
        Controls.Label {
            color:labelColor
            text:  mainPage.currentIndividual?
                mainPage.currentIndividual.formattedLastUsedTime : ""
        }
        Layouts.Layout.fillWidth: true
    }

    Row {
        Item {width: Kirigami.Units.smallSpacing; height: 1}
        Kirigami.Icon {
            source: "dialog-messages"
            height: Kirigami.Units.iconSizes.smallMedium
            width: Kirigami.Units.iconSizes.smallMedium
        }
        Item {width: Kirigami.Units.smallSpacing; height: 1}
        Controls.Label {
            color:labelColor
            text: "Texted "+ (mainPage.currentIndividual?
                mainPage.currentIndividual.textMessageCount : 0) + " time"
        }
        Layouts.Layout.fillWidth: true
    }

    Row {
        Item {width: Kirigami.Units.smallSpacing; height: 1}
        Kirigami.Icon {
            source: "dialog-messages"
            height: Kirigami.Units.iconSizes.smallMedium
            width: Kirigami.Units.iconSizes.smallMedium
        }
        Item {width: Kirigami.Units.smallSpacing; height: 1}
        Controls.Label {
            color:labelColor
            text: (mainPage.currentIndividual?
                mainPage.currentIndividual.unreadTextMessageCount : 0) + " unread messages"
        }
        Layouts.Layout.fillWidth: true
    }

    Row {
        Item {width: Kirigami.Units.smallSpacing; height: 1}
        Kirigami.Icon {
            source: "call-start"
            height: Kirigami.Units.iconSizes.smallMedium
            width: Kirigami.Units.iconSizes.smallMedium
        }
        Item {width: Kirigami.Units.smallSpacing; height: 1}
        Controls.Label {
            color:labelColor
            text: "Called "+ (mainPage.currentIndividual?
                mainPage.currentIndividual.callCount : 0) + " time"
        }
        Layouts.Layout.fillWidth: true
    }

    Row {
        Item {width: Kirigami.Units.smallSpacing; height: 1}
        Kirigami.Icon {
            source: "call-start"
            height: Kirigami.Units.iconSizes.smallMedium
            width: Kirigami.Units.iconSizes.smallMedium
        }
        Item {width: Kirigami.Units.smallSpacing; height: 1}
        Controls.Label {
            color:labelColor
            text: "Spoken "+ Math.ceil((mainPage.currentIndividual?
                mainPage.currentIndividual.totalSpentTime : 0)/60) + " minutes"
        }
        Layouts.Layout.fillWidth: true
    }
}
