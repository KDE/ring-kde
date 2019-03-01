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
import net.lvindustries.ringqtquick 1.0 as RingQtQuick

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
        text: i18n("Create contact")
    }

    readonly property Kirigami.Action contactRequests: Kirigami.Action {
        iconName: "contact-new"
        text: i18n("Pending contact requests")
        enabled: RingSession.accountModel.incomingContactRequestModel.size > 0
        onTriggered: events.viewContactRequests()
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

    /*
     * This can be done per account, but Banji doesn't support that.
     */
    readonly property Kirigami.Action autoanswer: Kirigami.Action {
        text: i18n("Do not disturb")
        id: dndChoice

        function setMode(m) {
            RingSession.accountModel.globalAutoAnswerStatus = m
            dndDisabled.checked   = RingSession.accountModel.globalAutoAnswerStatus == RingQtQuick.Account.MANUAL
            dndEnabled.checked    = RingSession.accountModel.globalAutoAnswerStatus == RingQtQuick.Account.DO_NOT_DISTURB
            dndSilent.checked     = RingSession.accountModel.globalAutoAnswerStatus == RingQtQuick.Account.SILENT
            dndAutoAnswer.checked = RingSession.accountModel.globalAutoAnswerStatus == RingQtQuick.Account.AUTO_ANSWER
        }

        Kirigami.Action {
            id: dndDisabled
            text: i18n("Disabled")
            checkable: true
            checked: RingSession.accountModel.globalAutoAnswerStatus == RingQtQuick.Account.MANUAL
            onTriggered: dndChoice.setMode(RingQtQuick.Account.MANUAL)
        }

        Kirigami.Action {
            id: dndEnabled
            text: i18n("Enabled")
            checkable: true
            checked: RingSession.accountModel.globalAutoAnswerStatus == RingQtQuick.Account.DO_NOT_DISTURB
            onTriggered: dndChoice.setMode(RingQtQuick.Account.DO_NOT_DISTURB)
        }

        Kirigami.Action {
            id: dndSilent
            text: i18n("Silent")
            checkable: true
            checked: RingSession.accountModel.globalAutoAnswerStatus == RingQtQuick.Account.SILENT
            onTriggered: dndChoice.setMode(RingQtQuick.Account.SILENT)
        }

        Kirigami.Action {
            id: dndAutoAnswer
            text: i18n("Auto answer")
            checkable: true
            checked: RingSession.accountModel.globalAutoAnswerStatus == RingQtQuick.Account.AUTO_ANSWER
            onTriggered: dndChoice.setMode(RingQtQuick.Account.AUTO_ANSWER)
        }
    }
}
