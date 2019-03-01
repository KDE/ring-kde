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

/**
 * At some point the toolbar and sidebar header shared the same module, but
 * eventually the number of "if" to separate the 2 modes outgrew the amount
 * of shared code.
 */
MouseArea {
    property var textColor: Kirigami.Theme.textColor
    property real photoSize: Kirigami.Units.largeSpacing

    // The `currentIndividual` is to force it to be reloaded
    property bool fits: workflow.currentIndividual == workflow.currentIndividual &&
        pageStack.wideMode && grid.implicitWidth < parent.width

    implicitHeight: grid.implicitHeight

    function defaultDisplay() {
        editContact.active = true
    }

    GridLayout {
        id: grid
        rows: 3
        columns: 3
        rowSpacing: 0
        flow: GridLayout.LeftToRight
        columnSpacing: Kirigami.Units.smallSpacing
        anchors.fill: parent

        JamiContactView.ContactPhoto {
            Layout.preferredWidth: photoSize
            Layout.preferredHeight: photoSize
            Layout.maximumWidth: photoSize
            Layout.maximumHeight: photoSize
            Layout.minimumWidth: photoSize
            Layout.minimumHeight: photoSize
            Layout.fillHeight: true
            Layout.rowSpan: 2
            Layout.alignment: Qt.AlignVCenter

            individual: workflow.currentIndividual
            defaultColor: Kirigami.Theme.textColor
            drawEmptyOutline: false
            MouseArea {
                onClicked: defaultDisplay()
                anchors.fill: parent
            }
        }

        Kirigami.Heading {
            id: mainHeading
            level: onlineLabel.visible || (!pageStack.wideMode) ? 3 : 1
            text: workflow.currentIndividual ?
                workflow.currentIndividual.bestName : ""

            color: textColor

            Layout.fillWidth: true
            elide: Text.ElideRight
            //show only when at least half of the string has been painted: use
            //opacity as using visible it won't correctly recalculate the width
            opacity: width > implicitWidth/2
            Layout.columnSpan: 2

            MouseArea {
                onClicked: defaultDisplay()
                anchors.fill: parent
            }
        }

        Controls.Label {
            id: onlineLabel
            text: workflow.currentIndividual && workflow.currentIndividual.isOnline ?
                i18n("Online") : i18n("Offline")

            elide: Text.ElideRight

            Layout.preferredHeight: visible ? undefined : 0

            opacity: workflow.currentIndividual && (
                workflow.currentIndividual.isOnline || workflow.currentIndividual.isOffline
            ) ? 1 : 0

            visible: opacity > 0

            color: workflow.currentIndividual ? Qt.tint(
                textColor,
                workflow.currentIndividual.isOnline ?
                    Kirigami.Theme.positiveTextColor :
                    Kirigami.Theme.negativeTextColor
            ) : "transparent"

            MouseArea {
                onClicked: defaultDisplay()
                anchors.fill: parent
            }
        }

        // Display reasons why the media buttons are not present
        JamiTroubleshooting.MediaAvailability {
            id: content

            Layout.fillWidth: true

            Layout.topMargin:   Kirigami.Units.largeSpacing
            Layout.bottomMargin:Kirigami.Units.largeSpacing
            Layout.rightMargin: Kirigami.Units.largeSpacing
            Layout.leftMargin:  Kirigami.Units.largeSpacing

            Layout.columnSpan: 3

            persistent: true
            defaultSize: parent.height < 48 ? parent.height : 48
            currentIndividual: workflow.currentIndividual

            background: Kirigami.Theme.neutralTextColor
            foreground: Kirigami.Theme.highlightedTextColor
        }
    }
}
