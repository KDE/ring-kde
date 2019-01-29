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
import QtQuick 2.0
import org.kde.kirigami 2.4 as Kirigami
import QtQuick.Controls.Material 2.3

QtObject {
    readonly property Kirigami.Action mailAction: Kirigami.Action {
        iconName: "mail-message"
        text: i18n("Write mail")
    }

    readonly property Kirigami.Action callAction: Kirigami.Action {
        iconName: "call-start"
        text: i18n("Make call")
    }

    readonly property Kirigami.Action chatAction: Kirigami.Action {
        iconName: "kmouth-phrase-new"
        text: i18n("Write SMS")
    }

    readonly property Kirigami.Action bookmarkAction: Kirigami.Action {
        iconName: "favorite"
        text: i18n("Select as favorite")
    }

    readonly property Kirigami.Action shareAction: Kirigami.Action {
        iconName: "document-share"
        text: i18n("Share")
    }

    readonly property Kirigami.Action editAction: Kirigami.Action {
        iconName: "document-edit"
        text: i18n("Edit")
    }

    readonly property Kirigami.Action photoAction: Kirigami.Action {
        iconName: "edit-image-face-add"
        text: i18n("Choose photo")
    }

    readonly property Kirigami.Action banAction: Kirigami.Action {
        iconName: "im-kick-user"
        text: i18n("Block number")
    }

    readonly property Kirigami.Action deleteAction: Kirigami.Action {
        iconName: "delete"
        text: i18n("Delete contact")
    }

    readonly property Kirigami.Action clearAction: Kirigami.Action {
        iconName: "edit-clear-history"
        text: i18n("Delete history")
    }

    readonly property Kirigami.Action newContactAction: Kirigami.Action {
        iconName: "contact-new"
        text: "Create contact"
    }

    readonly property Kirigami.Action darkMode: Kirigami.Action {
        iconName: "mail-message"
        text: i18n("Dark mode")
        onTriggered: {
            console.log("Enable dark mode", Material.Dark, Material.Blue)
            root.Material.theme = Material.Dark
            root.Material.accent = Material.Blue
        }
    }

    readonly property Kirigami.Action lightMode: Kirigami.Action {
        iconName: "mail-message"
        text: i18n("Light mode")
        onTriggered: {
            console.log("Enable light mode", Material.Light, Material.Purple)
            root.Material.theme = Material.Light
            root.Material.accent = Material.Purple
        }
    }
}
