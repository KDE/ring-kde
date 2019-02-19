/***************************************************************************
 *   Copyright (C) 2017 by Bluesystems                                     *
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
import org.kde.kirigami 2.2 as Kirigami
import QtQuick.Controls 2.2 as Controls
import QtQuick.Layouts 1.0

import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import org.kde.playground.kquickitemviews 1.0 as KQuickItemViews
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import org.kde.ringkde.jamiaccountview 1.0 as JamiAccountView
import org.kde.ringkde.jamiaccountview.pages 1.0 as JamiAccountPages

Controls.Dialog {
    id: accountDialog
    standardButtons: Controls.Dialog.Save | Controls.Dialog.Cancel

    property var profilePage: undefined

    parent: applicationWindow().contentItem
    x: applicationWindow().contentItem.width / 2 - width/2
    y: applicationWindow().contentItem.height / 2 - height/2

    width:  applicationWindow().contentItem.width  * 0.85
    height: applicationWindow().contentItem.height * 0.85

    onAccepted: {
        if (accountTree.selectedAccount) {
            if (profilePage)
                profilePage.save()
            accountTree.selectedAccount.performAction(RingQtQuick.Account.SAVE)
        }
    }

    onRejected: {
        if (accountTree.selectedAccount) {
            accountTree.selectedAccount.performAction(RingQtQuick.Account.CANCEL)
        }
    }

    RowLayout {
        anchors.fill: parent

        JamiAccountView.Accounts {
            id: accountTree
            Layout.fillHeight: true
            Layout.preferredWidth: 300
            Layout.maximumWidth: 300
        }

        KQuickItemViews.IndexView {
            Layout.fillHeight: true
            Layout.fillWidth: true
            modelIndex: accountTree.selectedAccount ?
                accountTree.selectedAccount.index : undefined
            clip: true

            delegate: RingQtQuick.AccountFields {
                anchors.fill: parent
                account: object

                ColumnLayout {
                    anchors.fill: parent
                    Controls.TabBar {
                        id: pageTabs
                        Layout.fillWidth: true
                        Controls.TabButton {
                            text: i18n("Basic")
                        }
                        Controls.TabButton {
                            text: i18n("Profile")
                        }
                        Controls.TabButton {
                            text: i18n("Advanced")
                        }
                        Controls.TabButton {
                            text: i18n("Network")
                        }
                        Controls.TabButton {
                            text: i18n("Devices")
                        }
                        Controls.TabButton {
                            text: i18n("Codecs")
                        }
                        Controls.TabButton {
                            text: i18n("Credentials")
                        }
                        Controls.TabButton {
                            text: i18n("Security")
                        }
                        Controls.TabButton {
                            text: i18n("Ringtones")
                        }
                    }

                    Controls.SwipeView {
                        currentIndex: pageTabs.currentIndex
                        interactive : false
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        JamiAccountPages.Basic {}
                        JamiAccountPages.Profiles {
                            id: prof
                            Component.onCompleted: {
                                accountDialog.profilePage = prof
                            }
                        }
                        JamiAccountPages.Advanced {}
                        JamiAccountPages.Network {}
                        JamiAccountPages.Devices {}
                        JamiAccountPages.Codecs {}
                        JamiAccountPages.Credentials {}
                        JamiAccountPages.Security {}
                        JamiAccountPages.Ringtones {}
                    }
                }
            }
        }
    }
}
