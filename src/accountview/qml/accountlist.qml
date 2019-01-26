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
import QtQuick.Layouts 1.0

import QtQuick.Controls 2.2 as Controls
import org.kde.ringkde.genericutils 1.0 as GenericUtils
import org.kde.ringkde.jamicontactview 1.0 as JamiContactView
import org.kde.kirigami 2.2 as Kirigami
import org.kde.playground.kquickitemviews 1.0 as KQuickItemViews
import net.lvindustries.ringqtquick 1.0 as RingQtQuick

ListView {
    id: accountList
    interactive: false
    height: totalHeight
    implicitHeight: totalHeight
    model: RingSession.accountModel

    property real totalHeight: footerItem + contentHeight + headerItem + 10

    property real footerItem: 0
    property real headerItem: 0

    property bool enableAdd: true

    Component {
        id: accountErrorComponent
        RowLayout {
            anchors.fill: parent

            Image {
                Layout.fillHeight: true
                fillMode: Image.PreserveAspectFit
                width: parent.width
                sourceSize.width: 22
                sourceSize.height: 22
                horizontalAlignment: Image.AlignHCenter
                source: "image://SymbolicColorizer/:/sharedassets/outline/warning.svg"
            }
            Controls.Label {
                Layout.fillWidth: true
                Layout.fillHeight: true
                text: error
                color: Kirigami.Theme.textColor
                wrapMode: Text.WordWrap
                width: parent.width
            }
        }
    }

    Component {
        id: defaultAccountComponent
        RowLayout {
            Controls.CheckBox {
                Layout.fillHeight: true
                Layout.preferredWidth: Kirigami.Units.fontMetrics.height * 2
                Layout.minimumWidth: Kirigami.Units.fontMetrics.height * 2
                checked: obj.enabled
                onClicked: {
                    obj.enabled = checked
                    obj.performAction(RingQtQuick.Account.SAVE)
                }
            }

            Rectangle {
                height: Kirigami.Units.fontMetrics.height * 0.66
                width: Kirigami.Units.fontMetrics.height * 0.66
                radius: 99
                color: obj.registrationState == RingQtQuick.Account.READY ?
                    Kirigami.Theme.positiveTextColor : (
                        obj.registrationState == RingQtQuick.Account.TRYING ?
                            Kirigami.Theme.neutralTextColor : Kirigami.Theme.negativeTextColor
                    )
            }

            Controls.Label {
                Layout.fillHeight: true
                Layout.fillWidth: true
                text: obj.alias
            }

            Item {
                Layout.fillHeight: true
                KQuickItemViews.DecorationAdapter {
                    pixmap: secIcon
                    anchors.verticalCenter: parent.verticalCenter
                    height: (Kirigami.Units.fontMetrics.height * 2) - 6
                    width:  (Kirigami.Units.fontMetrics.height * 2) - 6
                }
            }

            // Do not cover the handle
            Item {
                width: (Kirigami.Units.fontMetrics.height * 2)
            }
        }
    }

    Component {
        id: errorComponent
        RowLayout {
            anchors.fill: parent

            Controls.CheckBox {
                Layout.fillHeight: true
                Layout.preferredWidth: Kirigami.Units.fontMetrics.height * 2
                Layout.minimumWidth: Kirigami.Units.fontMetrics.height * 2
                checked: obj.enabled
                onClicked: {
                    obj.enabled = checked
                    obj.performAction(RingQtQuick.Account.SAVE)
                }
            }

            Rectangle {
                height: Kirigami.Units.fontMetrics.height * 0.66
                width: Kirigami.Units.fontMetrics.height * 0.66
                radius: 99
                color: obj.registrationState == RingQtQuick.Account.READY ?
                    Kirigami.Theme.positiveTextColor : (
                        obj.registrationState == RingQtQuick.Account.TRYING ?
                        Kirigami.Theme.neutralTextColor : Kirigami.Theme.negativeTextColor
                    )
            }

            ColumnLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true

                Loader {
                    property string error: obj.lastErrorMessage
                    property int code: obj.lastErrorCode
                    id: accountError
                    Layout.minimumHeight: active ? Kirigami.Units.fontMetrics.height * 1.5 : 0
                    Layout.fillWidth: true
                    active: true

                    sourceComponent: accountErrorComponent
                }

                Controls.Label {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    text: obj.alias
                }
            }

            KQuickItemViews.DecorationAdapter {
                pixmap: secIcon
                anchors.verticalCenter: parent.verticalCenter
                height: (Kirigami.Units.fontMetrics.height * 2) - 6
                width:  (Kirigami.Units.fontMetrics.height * 2) - 6
            }

            // Do not cover the handle
            Item {
                width: (Kirigami.Units.fontMetrics.height * 2)
            }
        }
    }

    Loader {
        id: accountDeleter
        property string name: ""
        property var account: ""
        active: false
        sourceComponent: Controls.Dialog {
            height: 150
            parent: applicationWindow().contentItem
            x: applicationWindow().contentItem.width / 2 - width/2
            y: applicationWindow().contentItem.height / 2 - height/2
            standardButtons: Controls.Dialog.Ok | Controls.Dialog.Cancel
            closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
            modal: true
            title: i18n("Delete an account")

            Controls.Label {
                text: i18n("<center>Are you sure you want to delete the account called ")
                    + name + i18n(". <br><br> This cannot be undone and you will lose the account <b>permanently</b></center>")
            }

            onAccepted: {
                accountDeleter.active = false
                RingSession.accountModel.remove(account)
                RingSession.accountModel.save()
                accountDeleter.account = null
            }

            onRejected: {
                accountDeleter.active = false
                accountDeleter.account = null
            }
        }
    }

    delegate: Kirigami.SwipeListItem {
        property bool hasProblems: object.enabled && registrationState != RingQtQuick.Account.READY

        height: Kirigami.Units.fontMetrics.height * (hasProblems ? 3.5 : 2)
        width: accountList.width

        actions: [
            Kirigami.Action {
                iconName: "message-news"
                text: i18n("Status logs")
                onTriggered: {
                    object.performAction(RingQtQuick.Account.RELOAD)
                }
            },
            Kirigami.Action {
                iconName: "document-share"
                text: i18n("Share")
                onTriggered: {
                    //TODO
                }
            },
            Kirigami.Action {
                iconName: "edit-delete"
                text: i18n("Delete")
                onTriggered: {
                    accountDeleter.name = alias
                    accountDeleter.account = object
                    accountDeleter.active = true
                    accountDeleter.item.open()
                    //applicationWindow().globalDrawer.drawerOpen = false //FIXME
                }
            },
            Kirigami.Action {
                iconName: "view-refresh"
                text: i18n("Reload")
                onTriggered: {
                    object.performAction(RingQtQuick.Account.RELOAD)
                }
            }
        ]

        Loader {
            anchors.fill: parent
            property QtObject obj: object
            property var secIcon: securityLevelIcon
            sourceComponent: hasProblems ?
                errorComponent : defaultAccountComponent
        }
    }

    header: Kirigami.Heading {
        level: 2
        text: i18n("Accounts")

        Component.onCompleted: {
            headerItem = Math.max(height, implicitHeight)
        }

        onHeightChanged: {
            headerItem = Math.max(height, implicitHeight)
        }
    }

    footer: GenericUtils.OutlineButton {
        id: mainArea
        height: enableAdd ? fontMetrics.height * 3.5 : 0
        expandedHeight: fontMetrics.height * 3.5
        sideMargin: 2
        width: parent.width
        label: i18n("Add an account")
        topPadding: 2
        visible: enableAdd
        onClicked: {
            applicationWindow().globalDrawer.drawerOpen = false
            ActionCollection.showWizard.trigger()
        }

        Component.onCompleted: {
            footerItem = Math.max(height, implicitHeight)
        }

        onHeightChanged: {
            footerItem = Math.max(height, implicitHeight)
        }
    }
}
