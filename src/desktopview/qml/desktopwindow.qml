/***************************************************************************
 *   Copyright (C) 2017-2018 by Bluesystems                                *
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
import Ring 1.0
import RingQmlWidgets 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0 as Controls
import org.kde.kirigami 2.2 as Kirigami
import ContactView 1.0

Kirigami.ApplicationWindow {
    id: root

    width: 1024
    height: 768

    function showWizard() {
        globalDrawer.drawerOpen = false
        wizardLoader.visible    = true
        wizardLoader.active     = true
    }

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    SystemPalette {
        id: inactivePalette
        colorGroup: SystemPalette.Inactive
    }


    //TODO use kirigami
    Rectangle {
        id: newHolder
        property bool show: false
        property alias container: mobileHolderContent
        width: 0
        height: parent.height
        color: activePalette.base
        z: 10000
        visible: false
        state: ""
        clip: true

        Behavior on width {
            NumberAnimation {duration: 200;  easing.type: Easing.OutQuad }
        }

        ColumnLayout {
            anchors.fill: parent
            Item {
                height: 30

                Layout.fillWidth: true
                Layout.minimumHeight: 30

                Text {
                    text: i18n("Hide")
                    color: activePalette.text
                    anchors.centerIn: parent
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        newHolder.show = false
                    }
                }


            }
            Item {
                id: mobileHolderContent
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }

        states: [
            State {
                name: ""
                when: stateGroup.state != "mobile"
                PropertyChanges {
                    target: newHolder
                    visible: false
                    width: 0
                }
            },
            State {
                name: "active"
                when: stateGroup.state == "mobile" && !newHolder.show
                PropertyChanges {
                    target: newHolder
                    visible: true
                }
            },
            State {
                name: "visible"
                extend: "active"
                when: stateGroup.state == "mobile" && newHolder.show
                PropertyChanges {
                    target: newHolder
                    width: 300
                    x: 0
                }
            }
        ]
    }

    globalDrawer: Kirigami.GlobalDrawer {
        id: globalDrawer
        title: "Ring KDE client"
        titleIcon: "ring-kde"
        actions: [
            QmlAction {
                text: "View"
                QmlAction {
                    action: ActionCollection.showTimelineDockAction
                }
                QmlAction {
                    action: ActionCollection.showDialDockAction
                }
                QmlAction {
                    action: ActionCollection.showContactDockAction
                }
                QmlAction {
                    action: ActionCollection.showHistoryDockAction
                }
                QmlAction {
                    action: ActionCollection.showBookmarkDockAction
                }
            },
//             QmlAction {
//                 action: ActionCollection.newContact
//             },
            QmlAction {
                action: ActionCollection.showWizard
            },
            QmlAction {
                action: ActionCollection.configureRing
            },
            QmlAction {
                action: ActionCollection.configureShortcut
            },
            QmlAction {
                action: ActionCollection.configureNotification
            },
            QmlAction {
                action: ActionCollection.quitAction
            }
        ]

        // Yes, this is a Kirigami bug, the Item should *not* be needed, but
        // `content` cannot be resized after being created
        content: Loader {
            id: testLoader
            width: globalDrawer.width
            Layout.preferredWidth: globalDrawer.width
            active: globalDrawer.drawerOpen
            sourceComponent: AccountList {
                height: contentHeight
                width: globalDrawer.width
            }
        }
        handleVisible: true
        drawerOpen: false
    }

    FontMetrics {
        id: fontMetrics
    }

    Loader {
        id: wizardLoader
        active: false
        anchors.fill: parent
        z: 999999
        visible: false
        sourceComponent: Wizard {
            anchors.fill: parent
            z: 999999
            onVisibleChanged: {
                if (!visible) {
                    wizardLoader.visible = false
                    wizardLoader.active = false
                }
            }
        }
    }

    QActionBinder {
        id: binder
        actionCollection: ActionCollection.kactionCollection
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        DockBar {
            id: dockBar
            newHolder: newHolder
            Layout.fillHeight: true
            Layout.maximumWidth: width
        }

        ColumnLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true

            ContactHeader {
                id: contactHeader
                backgroundColor: activePalette.alternateBase
                isMobile: stateGroup.state == "mobile"
                textColor: activePalette.text
                Layout.maximumHeight: height
                Layout.minimumHeight: height

                onCachedPhotoChanged: {
                    contactInfo.cachedPhoto = contactHeader.cachedPhoto
                }

                onSelectChat: {
                    mainPage.setCurrentPage(MainPage.TIMELINE)
                    timelinePage.currentInstance.focusEdit()
                }

                onSelectVideo: {
                    mainPage.setCurrentPage(MainPage.MEDIA)
                }
            }

            MainPage {
                id: mainPage
                header: contactHeader
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }

    }

    onClosing: {
        close.accepted = false
        hide()
    }

    StateGroup {
        id: stateGroup

        states: [
            State {
                name: ""
                PropertyChanges {
                    target: contactHeader
                    isCompact: false
                }
            },
            State {
                name: "compact"
                when: root.height < 700 && root.width >= 500
                PropertyChanges {
                    target: contactHeader
                    isCompact: true
                }
            },
            State {
                name: "mobile"
                extend: "compact"
                when: root.width < 500
                PropertyChanges {
                    target: dockBar
                    state: "mobile"
                }
                PropertyChanges {
                    target: mainPage
                    mobile: true
                }
            }
        ]
    }
}
