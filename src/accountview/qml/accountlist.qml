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
import RingQmlWidgets 1.0
import Ring 1.0
import QtQuick.Controls 2.0
import ContactView 1.0
import org.kde.kirigami 2.2 as Kirigami

ListView {
    id: accountList
    interactive: false
    height: contentHeight
    implicitHeight: contentHeight
    model: AccountModel

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
            Label {
                Layout.fillWidth: true
                Layout.fillHeight: true
                text: error
                color: activePalette.text
                wrapMode: Text.WordWrap
                width: parent.width
            }
        }
    }

    Component {
        id: defaultAccountComponent
        RowLayout {
            CheckBox {
                Layout.fillHeight: true
                Layout.preferredWidth: Kirigami.Units.fontMetrics.height * 2
                Layout.minimumWidth: Kirigami.Units.fontMetrics.height * 2
                checked: obj.enabled
                onClicked: {
                    obj.enabled = checked
                    obj.performAction(Account.SAVE)
                }
            }

            Rectangle {
                height: Kirigami.Units.fontMetrics.height * 0.66
                width: Kirigami.Units.fontMetrics.height * 0.66
                radius: 99
                color: obj.registrationState == Account.READY ?
                    Kirigami.Theme.positiveTextColor : (
                        obj.registrationState == Account.TRYING ?
                            Kirigami.Theme.neutralTextColor : Kirigami.Theme.negativeTextColor
                    )
            }

            Label {
                Layout.fillHeight: true
                Layout.fillWidth: true
                text: obj.alias
            }

            PixmapWrapper {
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

    Component {
        id: errorComponent
        RowLayout {
            anchors.fill: parent

            CheckBox {
                Layout.fillHeight: true
                Layout.preferredWidth: Kirigami.Units.fontMetrics.height * 2
                Layout.minimumWidth: Kirigami.Units.fontMetrics.height * 2
                checked: obj.enabled
                onClicked: {
                    obj.enabled = checked
                    obj.performAction(Account.SAVE)
                }
            }

            Rectangle {
                height: Kirigami.Units.fontMetrics.height * 0.66
                width: Kirigami.Units.fontMetrics.height * 0.66
                radius: 99
                color: obj.registrationState == Account.READY ?
                    Kirigami.Theme.positiveTextColor : (
                        obj.registrationState == Account.TRYING ?
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

                Label {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    text: obj.alias
                }
            }

            PixmapWrapper {
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

    delegate: Kirigami.SwipeListItem {
        property bool hasProblems: object.enabled && registrationState != Account.READY

        height: Kirigami.Units.fontMetrics.height * (hasProblems ? 3.5 : 2)
        width: accountList.width

        actions: [
            Kirigami.Action {
                iconName: "message-news"
                text: i18n("Status logs")
                onTriggered: {
                    object.performAction(Account.RELOAD)
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
                text: i18n("Remove")
                onTriggered: {
                    //TODO
                }
            },
            Kirigami.Action {
                iconName: "view-refresh"
                text: i18n("Reload")
                onTriggered: {
                    object.performAction(Account.RELOAD)
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
    }

    footer: OutlineButton {
        id: mainArea
        height: fontMetrics.height * 3.5
        expandedHeight: fontMetrics.height * 3.5
        sideMargin: 2
        width: parent.width
        label: i18n("Add an account")
        topPadding: 2
        onClicked: {
            if (phoneNumbers.model) {
                contactBuilder.addEmptyPhoneNumber(phoneNumbers.person)
                numbers.currentIndex = numbers.count - 1
                numbers.currentItem.state = "edit"
            }
            else
                console.log("No contact, not implemented")
        }
    }
}
