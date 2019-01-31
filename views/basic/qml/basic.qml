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
import org.kde.ringkde.basicview 1.0 as BasicView
import org.kde.ringkde.jamicontactview 1.0 as JamiContactView
import org.kde.ringkde.jamiwizard 1.0 as JamiWizard
import org.kde.ringkde.jamikdeintegration 1.0 as JamiKDEIntegration
import QtQuick.Controls.Material 2.3

Kirigami.ApplicationWindow {
    width: 320
    height: 600
    id: root

    RingQtQuick.SharedModelLocker {
        id: mainPage

        onChanged: {
            chat.boo.timelineModel = timelineModel

            if (!currentIndividual)
                return;

            var idx = RingSession.peersTimelineModel.individualIndex(currentIndividual)
            list.currentIndex = idx.row
        }
    }

    BasicView.ActionCollection {
        id: actionCollection
    }

    BasicView.Contacts {
        id : mydata
        Component.onCompleted: {
            chat.model =  mydata.get(3)
        }
    }

    pageStack.initialPage: [list, chat]
    pageStack.globalToolBar.style: Kirigami.ApplicationHeaderStyle.ToolBar
    pageStack.globalToolBar.preferredHeight: Kirigami.Units.gridUnit * 3
    pageStack.defaultColumnWidth: root.width < 320 ? root.width : 320

    BasicView.ListPage {
        id: list
        model: RingSession.peersTimelineModel
    }

    BasicView.ChatPage {
        id: chat
        visible: true
    }

    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
    }

    globalDrawer: BasicView.GlobalDrawer {
        id: globalDrawer
    }

    JamiKDEIntegration.WindowEvent {
        id: events

        onRequestsConfigureAccounts: {
            var component = Qt.createComponent("qrc:/account/qml/accountdialog.qml")
            if (component.status == Component.Ready) {
                var window = component.createObject(applicationWindow().contentItem)
                window.open()
            }
            else
                console.log("ERROR", component.status, component.errorString())

        }

        onRequestsHideWindow: {
            hide()
        }

        onRequestsWizard: {
            globalDrawer.drawerOpen = false
            wizardLoader.visible    = true
            wizardLoader.active     = true
        }
    }

    /**
     * Display the wizard when all accounts are deleted.
     */
    JamiWizard.Policies {
        id: wizardPolicies
    }

    Loader {
        id: wizardLoader
        active: wizardPolicies.displayWizard
        anchors.fill: parent
        z: 999999
        visible: false
        sourceComponent: JamiWizard.Wizard {
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

}
