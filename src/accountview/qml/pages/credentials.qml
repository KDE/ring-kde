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
import QtQuick 2.8
import org.kde.kirigami 2.5 as Kirigami
import org.kde.playground.kquickitemviews 1.0 as KQuickItemViews
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import org.kde.ringkde.genericutils 1.0 as GenericUtils

Page {
    property var account: object
    property var details: credentialDetails

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }


    SystemPalette {
        id: inactivePalette
        colorGroup: SystemPalette.Inactive
    }

    ColumnLayout {
        anchors.fill: parent
        Label {
            Layout.fillWidth: true
            text: i18n("This tab help manage accounts with multiple credentials. If you have multiple accounts. Do not use this feature, just create multiple accounts in Ring-KDE. Nor use this dialog if you only have one set of credentials. In that case, the \"Basic\" tab will provide everything you need.")
        }

        KQuickItemViews.TreeView {
            id: gridView
            Layout.fillWidth: true
            Layout.preferredHeight: contentHeight

            model: account ? account.credentialModel : null

            delegate: Loader {
                Component {
                    id: categoryDelegate
                    ColumnLayout {
                        width: gridView.width
                        Item {
                            height: 10
                        }
                        Kirigami.Heading {
                            text: display
                        }
                    }
                }

                Component {
                    id: credentialDelegate
                    Kirigami.SwipeListItem {
                        ColumnLayout {
                            Label {
                                text: display
                            }
                            RowLayout {
                                Text {
                                    color: inactivePalette.text
                                    text: "Realm: <i>"+realm+"</i>"
                                }
                                Item {
                                    Layout.fillWidth: true
                                }
                            }
                        }
                        actions: [
                            Kirigami.Action {
                                text: i18n("Remove")
                                iconName: "list-remove"
                                onTriggered: {
                                    account.credentialModel.removeCredentials(
                                        rootIndex
                                    )
                                }
                            },
                            Kirigami.Action {
                                text: i18n("Edit")
                                iconName: "document-edit"
                                onTriggered: print("Action 1 clicked")
                            },
                            Kirigami.Action {
                                text: i18n("Move up")
                                iconName: "go-up"
                                onTriggered: print("Action 1 clicked")
                            },
                            Kirigami.Action {
                                text: i18n("Move down")
                                iconName: "go-down"
                                onTriggered: print("Action 1 clicked")
                            }
                        ]

                        onPressedChanged: {
                            details.modelIndex = rootIndex
                        }
                    }
                }

                sourceComponent: isCategory ? categoryDelegate : credentialDelegate
            }

       }

        GenericUtils.OutlineButtons {
            GenericUtils.TreeHelper {
                id: creationHelper
                model: account ? account.credentialModel.availableTypeModel : null
            }

            Layout.fillWidth: true
            height: 50
            padding: 5
            implicitHeight: 50
            model: account ? account.credentialModel.availableTypeModel : null
            action: function(index) {
                account.credentialModel.addCredentials(
                    creationHelper.getIndex(index, undefined)
                )
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        GroupBox {
            title: i18n("Details")
            Layout.fillWidth: true
            height: implicitHeight
            implicitHeight: 130 //FIXME
            Layout.preferredHeight: implicitHeight
            Layout.minimumHeight: implicitHeight
            Layout.maximumHeight: implicitHeight
            KQuickItemViews.IndexView {
                id: credentialDetails
                anchors.fill: parent
                implicitHeight: 0
                height: 0
                delegate: Kirigami.FormLayout {
                    anchors.fill: parent

                    TextField {
                        Kirigami.FormData.label: i18n("Realm")
                        KQuickItemViews.RoleBinder.modelRole: "realm"
                        KQuickItemViews.RoleBinder.objectProperty: "text"
                    }

                    TextField {
                        Kirigami.FormData.label: i18n("User (auth) name")
                        KQuickItemViews.RoleBinder.modelRole: "name"
                        KQuickItemViews.RoleBinder.objectProperty: "text"
                    }

                    TextField {
                        Kirigami.FormData.label: i18n("Password")
                        KQuickItemViews.RoleBinder.modelRole: "password"
                        KQuickItemViews.RoleBinder.objectProperty: "text"
                        echoMode: TextInput.Password
                    }

                }
            }
        }

    }
}
