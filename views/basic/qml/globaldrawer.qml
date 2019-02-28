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
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.6 as Kirigami
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import org.kde.ringkde.jamicontactview 1.0 as JamiContactView

Kirigami.GlobalDrawer {
    id: globalDrawer

    topPadding: 0

    actions: [
        actionCollection.settings,
        actionCollection.autoanswer,
        actionCollection.contactRequests,
        actionCollection.quitAction,
    ]

    /**
     * List the active account profile.
     *
     * A profile is "our own" contact. There is one or more account per profile.
     */
    topContent: ListView {
        y: 5
        id: availableAccounts
        model: RingSession.profileModel.availableProfileModel
        interactive: false
        height: contentHeight
        spacing: 5
        Layout.preferredWidth: globalDrawer.width
        Layout.preferredHeight: contentHeight
        Layout.leftMargin: -globalDrawer.leftPadding

        delegate: ToolBar {
            width: parent.width
            height: Kirigami.Units.iconSizes.large * 1.5
            MouseArea {
                anchors.fill: parent

                RowLayout {
                    width: parent.width
                    height: Kirigami.Units.iconSizes.large
                    anchors.verticalCenter: parent.verticalCenter
                    JamiContactView.ContactPhoto {
                        width: Kirigami.Units.iconSizes.large
                        height: Kirigami.Units.iconSizes.large
                        individual: object
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Kirigami.Heading {
                            level: 2
                            text: object.bestName
                            Layout.fillWidth: true
                        }
                        Kirigami.Heading {
                            level: 3
                            color: Kirigami.Theme.positiveTextColor
                            text: "online"
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }
                    }

                    ColumnLayout {
                        Layout.fillHeight: true
                        Layout.preferredWidth: Kirigami.Units.iconSizes.small

                        Kirigami.Icon {
                            Layout.preferredHeight: Kirigami.Units.iconSizes.small
                            Layout.preferredWidth: Kirigami.Units.iconSizes.small
                            source: "view-refresh"
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    console.log("RELOAD", model.object)
                                    RingSession.profileModel.reloadAllAccount(
                                    model.object
                                    )
                                }
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                        }
                    }
                }

                onClicked: {
                    var component = Qt.createComponent("qrc:/account/qml/presenceselector.qml")
                    if (component.status == Component.Ready) {
                        var window = component.createObject(applicationWindow().contentItem)
                        window.individual = object
                        globalDrawer.drawerOpen = false
                        window.open()
                    }
                    else
                        console.log("ERROR", component.status, component.errorString())
                }
            }
        }
    }
}
