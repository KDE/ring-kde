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

import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0 as Controls
import org.kde.kirigami 2.2 as Kirigami
import QtQuick.Window 2.2
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import org.kde.ringkde.jamicontactview 1.0 as JamiContactView
import org.kde.ringkde.jamicontactview 1.0 as JamiContactView

import DesktopView 1.0

Item {
    id: topLevel

    width: Math.min(335, root.width-48) + dockBar.width

    property var newHolder: null

    function setCurrentIndex(index) {
        if (dockLoader.sourceComponent != timelineViewComponent)
            return

        dockLoader.item.setCurrentIndex(index)
    }

    signal timelineSelected()

    RowLayout {
        id: dockLayout
        spacing: 0
        anchors.fill: parent

        Image {
            id: dockBar

            property string selectedItem: "timeline"

            source: "gui/icons/assets/toolbar_bg.png"
            fillMode: Image.Tile
            width: 48
            height: parent.height

            Layout.fillHeight: true
            Layout.maximumWidth: width

            onSelectedItemChanged: {
                switch(selectedItem) {
                    case "timeline":
                        dockLoader.sourceComponent = timelineViewComponent
                        break
                    case "call"    :
                        dockLoader.sourceComponent = dialViewComponent
                        break
                    case "contact" :
                        dockLoader.sourceComponent = contactViewComponent
                        break
                    case "bookmark":
                        dockLoader.sourceComponent = bookmarkViewComponent
                        break
                    case "history" :
                        dockLoader.sourceComponent = historyViewComponent
                        break
                }
            }

            Behavior on width {
                NumberAnimation {duration: 200;  easing.type: Easing.OutQuad }
            }

            DockModel {
                id: icons
            }

            FontMetrics {
                id: fontMetrics
            }

            Column {
                Item {
                    width: dockBar.width
                    height: Math.max(
                        dockBar.width * 1.33,
                        availableAccounts.count*(dockBar.width+5) + 6
                    )

                    ListView {
                        y: 5
                        id: availableAccounts
                        model: RingSession.profileModel.availableProfileModel
                        height: 200
                        spacing: 5
                        interactive: false
                        delegate: MouseArea {
                            width: dockBar.width
                            height: dockBar.width
                            JamiContactView.ContactPhoto {
                                anchors.centerIn: parent
                                width: dockBar.width - 3
                                height: dockBar.width - 3
                                individual: object
                                defaultColor: "#f2f2f2ff" //same as the SVG
                            }
                            onClicked: {
                                var component = Qt.createComponent("PresenceSelector.qml")
                                if (component.status == Component.Ready) {
                                    var window = component.createObject(applicationWindow().contentItem)
                                    window.individual = object
                                    window.open()
                                }
                                else
                                    console.log("ERROR", component.status, component.errorString())
                            }
                        }
                    }

                    Loader {
                        active: availableAccounts.count == 0 || !RingSession.profileModel.hasAvailableProfiles
                        anchors.centerIn: parent
                        sourceComponent: Rectangle {
                            height: 48
                            width: 48
                            border.width: 2
                            border.color: Kirigami.Theme.negativeTextColor
                            color: "transparent"
                            radius: 99

                            Image {
                                anchors.fill: parent
                                sourceSize.width: 48
                                sourceSize.height: 48
                                anchors.verticalCenter: parent.verticalCenter
                                source: "image://SymbolicColorizer/:/sharedassets/outline/warning.svg"
                            }
                        }
                    }

                    Rectangle {
                        height: 1
                        width: parent.width
                        color: "#939393"
                        anchors.bottom: parent.bottom
                    }
                }

                Repeater {
                    model: icons
                    Rectangle {
                        id: actionIcon
                        color: dockBar.selectedItem == identifier ? "#111111" : "transparent"
                        height: dockBar.width + 10
                        width: dockBar.width

                        Image {
                            anchors.verticalCenter: parent.verticalCenter
                            source: decoration
                            width: 48
                            height: 48
                            sourceSize.width: 48
                            sourceSize.height: 48
                            fillMode: Image.PreserveAspectFit
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                dockBar.selectedItem = identifier
                            }
                        }

                        Rectangle {
                            color: "red"
                            radius: 99
                            x: 2
                            y: 2
                            width: fontMetrics.height + 4
                            height: fontMetrics.height + 4
                            visible: activeCount > 0
                            Text {
                                color: "white"
                                anchors.centerIn: parent
                                font.bold: true
                                text: activeCount
                            }
                        }
                    }
                }
            }
        }

        Component {
            id: timelineViewComponent
            PeersTimeline {
                anchors.fill: parent
                state: ""
                onIndividualSelected: {
                    mainPage.setIndividual(ind)
                }
            }
        }
        Component {
            id: dialViewComponent
            DialView {
                anchors.fill: parent
                onSelectCall: {
                    mainPage.showVideo(call)
                }
            }
        }
        Component {
            id: contactViewComponent
            ContactList {
                anchors.fill: parent
                onIndividualSelected: {
                    mainPage.setIndividual(ind)
                }
            }
        }
        Component {
            id: bookmarkViewComponent
            BookmarkList {
                anchors.fill: parent
                onContactMethodSelected: {
                    mainPage.setContactMethod(cm)
                }
            }
        }
        Component {
            id: historyViewComponent
            HistoryTimeline {
                anchors.fill: parent
                onContactMethodSelected: {
                    mainPage.setContactMethod(cm)
                }
            }
        }

        ColumnLayout {
            id: dockHolder
            width: Math.min(335, root.width-48)
            Layout.fillHeight: true
            visible: true

            // This is a placeholder for the searchbox
            Item {
                Layout.fillWidth: true
                Layout.minimumHeight: 32
                Layout.maximumHeight: 32
            }

            Loader {
                id: dockLoader

                sourceComponent: timelineViewComponent
                Layout.fillWidth: true
                Layout.fillHeight: true

                onItemChanged: {
                    if (item && sourceComponent == timelineViewComponent)
                        timelineSelected()
                }
            }
        }
    }

    SearchOverlay {
        id: searchView
        source: parent
        anchors.fill: parent
        onContactMethodSelected: {
            mainPage.setContactMethod(cm)
            setCurrentIndex(RingSession.peersTimelineModel.individualIndex(cm.individual))
        }
    }
}
