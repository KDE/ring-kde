/***************************************************************************
 *   Copyright (C) 2018 by Bluesystems                                     *
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
import org.kde.ringkde.jamitroubleshooting 1.0 as JamiTroubleShooting
import net.lvindustries.ringqtquick.troubleshoot 1.0 as RingQtTroubleshoot

Loader {
    property bool displayNoAccount: RingSession.accountModel.size == 0
    property bool displayDisabled: !RingSession.accountModel.hasEnabledAccounts
    property bool displayNoRegAccounts: !RingSession.accountModel.hasAvailableAccounts
    //property bool displayContactRequests: RingSession.accountModel.incomingContactRequestModel.size > 0
    property bool displayActionHeader: displayNoAccount || displayNoRegAccounts || displayDisabled

    id: accountError

    sourceComponent: displayNoAccount ? noAccounts : (
        displayDisabled ? noEnabledAccounts : (
            displayNoRegAccounts ? noRegisteredAccounts :
                null
    ))

    Component {
        id: noRegisteredAccounts
        JamiTroubleShooting.AccountError {
            Component.onCompleted: {
                implicitHeight = height + 20
                accountError.height = height + 20
            }
        }
    }

    Component {
        id: noEnabledAccounts
        JamiTroubleShooting.AccountDisabled {
            Component.onCompleted: {
                implicitHeight = height
                accountError.height = height
            }
        }
    }

    Component {
        id: noAccounts
        JamiTroubleShooting.NoAccount {
            Component.onCompleted: {
                implicitHeight = height
                accountError.height = height
            }
        }
    }

    /*Component {
        id: pendingContactRequests
        JamiContactView.ViewContactRequests {
            Component.onCompleted: {
                implicitHeight = height
                accountError.height = height
            }
        }
    }*/
}
