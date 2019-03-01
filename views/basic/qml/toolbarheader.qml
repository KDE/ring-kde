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
    property var textColor: Kirigami.Theme.textColor
    property real photoSize: Kirigami.Units.largeSpacing

    // The `currentIndividual` is to force it to be reloaded
    property bool fits: workflow.currentIndividual == workflow.currentIndividual &&
        pageStack.wideMode && grid.implicitWidth < parent.width

    implicitHeight: grid.implicitHeight

    function defaultDisplay() {
        viewContact.active = true
    }

    GridLayout {
        id: grid
        rows: 2
        columns: 4
        rowSpacing: 0
        flow: GridLayout.TopToBottom
        columnSpacing: Kirigami.Units.smallSpacing
        anchors.fill: parent

        JamiContactView.ContactPhoto {
            Layout.preferredWidth: photoSize
            Layout.preferredHeight: photoSize
            Layout.fillHeight: true
            Layout.rowSpan: 2

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
            Layout.preferredWidth: implicitWidth
            Layout.alignment: Qt.AlignVCenter
            elide: Text.ElideRight
            //show only when at least half of the string has been painted: use
            //opacity as using visible it won't correctly recalculate the width
            opacity: width > implicitWidth/2
            Layout.columnSpan: 1
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

            Layout.maximumHeight: visible ? undefined : 0

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

            Layout.columnSpan: 2
            MouseArea {
                onClicked: defaultDisplay()
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
                onClicked: editContact.active = true
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
            Layout.columnSpan: 1
            Layout.fillHeight: true

            Layout.topMargin: 0
            Layout.bottomMargin: 0
            Layout.rightMargin: 0
            Layout.leftMargin: 0

            Layout.minimumHeight: photoSize
            Layout.maximumWidth: parent.parent.width - 2*Kirigami.Units.largeSpacing

            // Display reasons why the media buttons are not present
            JamiTroubleshooting.MediaAvailability {
                id: content
                width: parent.width
                persistent: false
                defaultSize: parent.height < 48 ? parent.height : 48
                currentIndividual: workflow.currentIndividual
                anchors.verticalCenter: parent.verticalCenter
                foreground: Kirigami.Theme.textColor
            }
        }
    }

    hoverEnabled: !Kirigami.Settings.isMobile
    onContainsMouseChanged: edit.opacity = containsMouse
}
