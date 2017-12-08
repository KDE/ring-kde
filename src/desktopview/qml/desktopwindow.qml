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
import Ring 1.0
import RingQmlWidgets 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0 as Controls
import org.kde.kirigami 2.2 as Kirigami
import ContactView 1.0

Kirigami.ApplicationWindow {
    id: root

    globalDrawer: Kirigami.GlobalDrawer {
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
            QmlAction {
                action: ActionCollection.newContact
            },
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
            }
        ]

        handleVisible: true
        drawerOpen: false
    }

    QActionBinder {
        id: binder
        actionCollection: ActionCollection.kactionCollection
    }

    RowLayout {
        anchors.fill: parent
        DockBar {
            id: dockBar
            Layout.fillHeight: true
            width: 48
            Layout.maximumWidth: 48
        }

        Item {
            width: 335
            Layout.fillHeight: true
            Loader {
                id: timelineView
                active: dockBar.selectedItem == "timeline"
                anchors.fill: parent
                sourceComponent: PeersTimeline {
                    anchors.fill: parent
                    onContactMethodSelected: {
                        mainPage.setContactMethod(cm)
                    }
                }
            }
            Loader {
                id: dialView
                active: dockBar.selectedItem == "call"
                anchors.fill: parent
                sourceComponent: DialView {
                    anchors.fill: parent
                }
            }
            Loader {
                id: contactView
                active: dockBar.selectedItem == "contact"
                anchors.fill: parent
                sourceComponent: ContactList {
                    anchors.fill: parent
                    onContactMethodSelected: {
                        mainPage.setContactMethod(cm)
                    }
                }
            }
        }

        MainPage {
            id: mainPage
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

    }
}
