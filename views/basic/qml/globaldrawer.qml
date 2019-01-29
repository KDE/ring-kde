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

    actions: [
        actionCollection.settings,
        actionCollection.newContact,
        actionCollection.editAction,
        actionCollection.darkMode,
        actionCollection.lightMode,
    ]

    topContent: ListView {
        y: 5
        id: availableAccounts
        model: RingSession.profileModel.availableProfileModel
        interactive: false
        height: contentHeight
        spacing: 5
        Layout.preferredWidth: globalDrawer.width
        Layout.preferredHeight: contentHeight

        delegate: MouseArea {
            width: parent.width
            height: Kirigami.Units.iconSizes.large * 1.5
            Rectangle {
                anchors.fill: parent
                color: Qt.darker(Kirigami.Theme.backgroundColor, 1.2)

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
