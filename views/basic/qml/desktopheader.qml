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
import QtQuick.Layouts 1.4
import QtQuick.Controls 2.2 as Controls
import org.kde.kirigami 2.6 as Kirigami
import org.kde.ringkde.jamicontactview 1.0 as JamiContactView
import org.kde.ringkde.jamitroubleshooting 1.0 as JamiTroubleshooting

MouseArea {
    property var textColor: Kirigami.Theme.highlightedTextColor
    property real photoSize: Kirigami.Units.largeSpacing

    // The `currentIndividual` is to force it to be reloaded
    property bool fits: mainPage.currentIndividual == mainPage.currentIndividual &&
        pageStack.wideMode && grid.implicitWidth < parent.width

    implicitHeight: parent.parent.height - 2*photoSize

    GridLayout {
        id: grid
        rows: 2
        columns: 5
        rowSpacing: 0
        flow: GridLayout.TopToBottom
        columnSpacing: Kirigami.Units.smallSpacing
        anchors.fill: parent
        JamiContactView.ContactPhoto {
            Layout.preferredWidth: parent.parent.height
            Layout.preferredHeight: parent.parent.height
            Layout.rowSpan: 2

            individual: mainPage.currentIndividual
            defaultColor: Kirigami.Theme.highlightedTextColor
            drawEmptyOutline: false
            MouseArea {
                onClicked: chatPage.showContactDetails = true
                anchors.fill: parent
            }
        }

        Kirigami.Heading {
            id: mainHeading
            level: 3
            text: mainPage.currentIndividual ?
                mainPage.currentIndividual.bestName : ""

            color: textColor
            Layout.preferredWidth: implicitWidth
            elide: Text.ElideRight
            //show only when at least half of the string has been painted: use
            //opacity as using visible it won't correctly recalculate the width
            opacity: width > implicitWidth/2
            Layout.columnSpan: 1
            MouseArea {
                onClicked: chatPage.showContactDetails = true
                anchors.fill: parent
            }
        }

        Controls.Label {
            text: "Online"
            elide: Text.ElideRight
            color: textColor

            opacity: width > implicitWidth/2
//             color: Qt.Tint(
//                 mainHeading.color,
//                 Kirigami.Theme.positiveTextColor
//             )
            Layout.columnSpan: 2
            MouseArea {
                onClicked: chatPage.showContactDetails = true
                anchors.fill: parent
            }
        }

        Kirigami.Icon {
            id: edit
            opacity: Kirigami.Settings.isMobile
            color: textColor
            source: "document-edit"
            Layout.preferredWidth: Kirigami.Units.iconSizes.smallMedium
            Layout.preferredHeight: Kirigami.Units.iconSizes.smallMedium

            MouseArea {
                onClicked: chatPage.editContact = true
                anchors.fill: parent
            }

            Behavior on opacity {
                NumberAnimation {duration: 200}
            }

            Layout.rowSpan: 2
        }

        Item {
            Layout.fillWidth: true
            Layout.rowSpan: 2
            Layout.fillHeight: true

            // Display reasons why the media buttons are not present
            JamiTroubleshooting.MediaAvailability {
                width: parent.width
                defaultSize: parent.height < 48 ? parent.height : 48
                currentIndividual: mainPage.currentIndividual
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    hoverEnabled: !Kirigami.Settings.isMobile
    onContainsMouseChanged: edit.opacity = containsMouse
}
