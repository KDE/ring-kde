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
import QtQuick 2.2
import QtQuick.Controls 2.2 as Controls
import QtQuick.Layouts 1.4
import org.kde.kirigami 2.4 as Kirigami
import QtGraphicalEffects 1.0 as Effect
import org.kde.ringkde.jamicontactview 1.0 as JamiContactView

ListView {
    currentIndex: -1
    id: list

    delegate: Kirigami.SwipeListItem {
        onClicked: {
            list.currentIndex = index
            mainPage.currentIndividual = object
        }

        backgroundColor: index == currentIndex ?
            Kirigami.Theme.highlightColor : Kirigami.Theme.backgroundColor

        clip: true

        height: 4 * Kirigami.Units.fontMetrics.height

        contentItem: GridLayout {
            height: 4 * Kirigami.Units.fontMetrics.height

            rows: 2
            columns: 2

            JamiContactView.ContactPhoto {
                id: img
                Layout.margins: 3
                height: 3 * Kirigami.Units.fontMetrics.height
                width: 3 * Kirigami.Units.fontMetrics.height
                Layout.alignment: Qt.AlignVCenter

                individual: object
                defaultColor: index == currentIndex ?
                    Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor

                drawEmptyOutline: false
                Layout.rowSpan: 2
            }

            Kirigami.Heading {
                level: 3
                text: object.bestName
            }

            Kirigami.Heading {
                level: 4
                text: object.formattedLastUsedTime
                color: Kirigami.Theme.disabledTextColor
                Layout.fillHeight: true
                Layout.fillWidth: true
                opacity: 0.5
            }
        }

        actions: [
            actionCollection.callAction,
            actionCollection.mailAction,
        ]
    }
}
