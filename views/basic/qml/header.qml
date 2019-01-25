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
import org.kde.kirigami 2.4 as Kirigami
import QtGraphicalEffects 1.0 as Effects

Rectangle {
    color: Kirigami.Theme.backgroundColor
    property alias source: img.source

    id: root

    Image {
        id: img
        source: root.source
        height: parent.height
        width: parent.height
        fillMode: Image.PreserveAspectCrop
    }

    Kirigami.Heading {
        text: model.firstname + " " + model.lastname
        color: "#fcfcfc"
        level: 1
        anchors.left: img.right
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        MouseArea {
            onClicked: mainPage.showContactDetails = true
            anchors.fill: parent
        }
    }

    Kirigami.Icon {
        source: "favorite"
        width: Kirigami.Units.iconSizes.smallMedium
        height: width
        anchors.top: parent.top
        anchors.right: parent.right
    }

    Kirigami.Icon {
        source: "document-edit"
        width: Kirigami.Units.iconSizes.smallMedium
        height: width

        MouseArea {
            onClicked: mainPage.editContact = true
            anchors.fill: parent
        }
        anchors.bottom: parent.bottom
        anchors.right: parent.right
    }
}
