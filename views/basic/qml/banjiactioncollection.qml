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

/**
 * This module contains the actions specific to the Banji application.
 *
 * This is different from the MainActionCollection because those are Kirigami
 * toolbar (main) actions. They are not the JamiKDEIntegration.ActionCollection
 * either. Those are business logic and (C++) platform integration.
 */
QtObject {

    readonly property Kirigami.Action mailAction: Kirigami.Action {
        iconName: "mail-message"
        text: i18n("Write mail")
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

    readonly property Kirigami.Action quitAction: Kirigami.Action {
        iconName: "application-exit"
        text: i18n("Exit")
        onTriggered: Qt.quit()
    }

    readonly property Kirigami.Action settings: Kirigami.Action {
        text: i18n("Settings")
        iconName: "configure"

        Kirigami.Action {
            text: i18n("Export account")
            iconName: "document-export"
        }

        Kirigami.Action {
            text: i18n("Add a Jami account")
            iconName: "tools-wizard"
            onTriggered: events.requestsWizard()
        }

        Kirigami.Action {
            text: i18n("Configure accounts")
            iconName: "configure"
            onTriggered: events.requestsConfigureAccounts()
        }

        Kirigami.Action {
            text: i18n("Video settings")
            iconName: "camera-web"
            onTriggered: events.configureVideo()
        }
    }
}
