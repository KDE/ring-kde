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
        id: listItem
        onClicked: {
            list.currentIndex = index
            mainPage.currentIndividual = object
        }

        activeBackgroundColor: Kirigami.Theme.highlightColor

        // DefaultListItemBackground copy paste
        background: Rectangle {
            id: background
            color: listItem.checked || listItem.highlighted || (listItem.supportsMouseEvents && listItem.pressed && !listItem.checked && !listItem.sectionDelegate) ? listItem.activeBackgroundColor : listItem.backgroundColor

            visible: listItem.ListView.view ? listItem.ListView.view.highlight === null : true
            Rectangle {
                id: internal
                property bool indicateActiveFocus: listItem.pressed || Kirigami.Settings.tabletMode || listItem.activeFocus || (listItem.ListView.view ? listItem.ListView.view.activeFocus : false)
                anchors.fill: parent
                visible: !Kirigami.Settings.tabletMode && listItem.supportsMouseEvents
                color: listItem.activeBackgroundColor
                opacity: (listItem.hovered || listItem.highlighted || listItem.activeFocus) && !listItem.pressed ? 0.5 : 0
                Behavior on opacity { NumberAnimation { duration: Kirigami.Units.longDuration } }
            }

            readonly property bool __separatorVisible: listItem.separatorVisible

            on__SeparatorVisibleChanged: {
                if (__separatorVisible) {
                    var newObject = Qt.createQmlObject('import QtQuick 2.0; import org.kde.kirigami 2.4; Separator {anchors {left: parent.left; right: parent.right; bottom: parent.top} visible: listItem.separatorVisible}',
                                        background);
                    newObject = Qt.createQmlObject('import QtQuick 2.0; import org.kde.kirigami 2.4; Separator {anchors {left: parent.left; right: parent.right; bottom: parent.bottom} visible: listItem.separatorVisible}',
                                        background);
                }
            }
        }


        highlighted: index == currentIndex

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
                defaultColor: highlighted ?
                    Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor

                drawEmptyOutline: false
                Layout.rowSpan: 2
            }

            Kirigami.Heading {
                level: 3
                text: object.bestName
                color: highlighted ?
                    Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
            }

            Kirigami.Heading {
                level: 4
                text: object.formattedLastUsedTime
                Layout.fillHeight: true
                Layout.fillWidth: true
                opacity: 0.5
                color: highlighted ?
                    Kirigami.Theme.highlightedTextColor : Kirigami.Theme.disabledTextColor
            }
        }

        actions: [
            actionCollection.callAction,
            actionCollection.mailAction,
        ]
    }
}
