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
import QtQuick 2.8
import org.kde.kirigami 2.5 as Kirigami
import QtQuick.Controls 2.2 as Controls
import QtQuick.Layouts 1.0

import org.kde.playground.kquickitemviews 1.0 as KQuickItemViews
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import org.kde.ringkde.jamiaccountview 1.0 as JamiAccountView
import org.kde.ringkde.genericutils 1.0 as GenericUtils

ListView {
    id: accountList

    property QtObject selectedAccount: null

    spacing : 10
    model: RingSession.profileModel
    delegate: codecDelegate

    JamiAccountView.Deleter {
        id: deleter
    }

    Component {
        id: codecDelegate
        Kirigami.Card {
            id: card
            x: 5
            width: accountList.width - 10
            height: content.height + 20
            implicitHeight: content.height + 20

            property var pModel : RingSession.profileModel.profilesForPerson(model.object)

            ColumnLayout {
                id: content
                height: implicitHeight
                width: parent.width
                Kirigami.Heading {
                    text: model.display
                    level:4
                }

                ListView {
                    id: profileList
                    interactive: false
                    model: pModel
                    implicitHeight: contentHeight
                    height: contentHeight
                    width: parent.width

                    delegate: Kirigami.SwipeListItem {
                        id: listItem
                        width: card.width
                        background: Rectangle {
                            id: background
                            property var listItem: parent

                            color: listItem.checked ||
                                listItem.highlighted || (
                                    listItem.supportsMouseEvents
                                    && listItem.pressed
                                    && !listItem.checked
                                    && !listItem.sectionDelegate
                                ) ? listItem.activeBackgroundColor : listItem.backgroundColor

                            visible: listItem.ListView.view ? listItem.ListView.view.highlight === null : true

                            Rectangle {
                                id: internal
                                property bool indicateActiveFocus: listItem.pressed || listItem.activeFocus || (listItem.ListView.view ? listItem.ListView.view.activeFocus : false)
                                anchors.fill: parent
                                visible: !listItem.supportsMouseEvents
                                color: listItem.activeBackgroundColor
                                opacity: (listItem.hovered || listItem.highlighted || listItem.activeFocus) && !listItem.pressed ? 0.5 : 0
                                Behavior on opacity { NumberAnimation { duration: Kirigami.Units.longDuration } }
                            }

                            Rectangle {
                                opacity: 0.9
                                anchors.fill: parent
                                color: object == accountList.selectedAccount ? Kirigami.Theme.highlightColor : registrationStateColor
                            }

                            KQuickItemViews.DecorationAdapter {
                                width: check.implicitHeight
                                height: check.implicitHeight
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                pixmap: securityLevelIcon
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
                        RowLayout {
                            width: parent.width
                            height: check.implicitHeight*2
                            Controls.CheckBox {
                                id: check
                                width: height
                                checked: model.enabled
                            }

                            KQuickItemViews.DecorationAdapter {
                                height: check.implicitHeight
                                width: check.implicitHeight
                                pixmap: decoration
                            }

                            Controls.Label {
                                text: alias
                                Layout.fillWidth: true
                            }
                        }
                        actions: [
                            Kirigami.Action {
                                text: i18n("Remove")
                                iconName: "list-remove"
                                onTriggered: {
                                    deleter.deleteAccount(object)
                                }
                            }
                        ]

                        onPressedChanged: {
                            //TODO add a popup
                            if (accountList.selectedAccount) {
                                accountList.selectedAccount.performAction(
                                    RingQtQuick.Account.CANCEL
                                )
                            }

                            accountList.selectedAccount = object
                        }
                    }
                }
            }
        }
    }

    RingQtQuick.AccountBuilder {
        id: accountBuilder
    }

    footer: GenericUtils.OutlineButtons {
        function callback(index) {
            selectedAccount = accountBuilder.buildFor(index)
        }

        width: accountList.width
        rows: 2
        padding: 5
        model: accountBuilder
        action: callback
    }

    Component.onCompleted: {
        accountList.selectedAccount = RingSession.availableAccountModel.currentDefaultAccount()
    }
}
