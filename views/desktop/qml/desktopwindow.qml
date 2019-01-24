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

import RingQmlWidgets 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0 as Controls
import org.kde.kirigami 2.2 as Kirigami
import DesktopView 1.0
import org.kde.ringkde.jamikdeintegration 1.0 as JamiKDEIntegration
import org.kde.ringkde.jamiwizard 1.0 as JamiWizard
import org.kde.ringkde.jamicontactview 1.0 as JamiContactView
import org.kde.ringkde.jamitimeline 1.0 as JamiTimeline
import org.kde.ringkde.jamiwizard 1.0 as JamiWizard
// import org.kde.ringkde.jamiaccountview 1.0 as JamiAccountView

Kirigami.ApplicationWindow {
    id: root

    width: 1024
    height: 768

    property bool wizardVisible: false

    TipModel {
        id: displayTips
    }

    /*
     * This is an abstraction mechanism to avoid coupling QML code with the
     * remaining KDE (Desktop) integration code written in C++
     */
    JamiKDEIntegration.WindowEvent {
        id: events

        onRequestsConfigureAccounts: {
            var component = Qt.createComponent("AccountDialog.qml")
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
            wizardVisible = true
            globalDrawer.drawerOpen = false
            wizardLoader.visible    = true
            wizardLoader.active     = true
        }
    }

    function showContactRequests() {
        var component = Qt.createComponent("PendingRequests.qml")
        if (component.status == Component.Ready) {
            var window = component.createObject(applicationWindow().contentItem)
            window.open()
        }
        else
            console.log("ERROR", component.status, component.errorString())
    }

    function addNewContact() {
        var component = Qt.createComponent("NewContactDialog.qml")
        if (component.status == Component.Ready) {
            var window = component.createObject(applicationWindow().contentItem)
            window.open()
        }
        else
            console.log("ERROR", component.status, component.errorString())
    }

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    SystemPalette {
        id: inactivePalette
        colorGroup: SystemPalette.Disabled
    }

    property var _regularActions: undefined
    property var _mobileActions: undefined

    // It's the only way to make it stateful
    function regularActions() {
        if (_regularActions != undefined)
            return _regularActions

        _regularActions = []

        var view = [
            "ActionCollection.showTimelineDockAction",
            "ActionCollection.showDialDockAction",
            "ActionCollection.showContactDockAction",
            "ActionCollection.showHistoryDockAction",
            "ActionCollection.showBookmarkDockAction",
            "ActionCollection.displayDialpadAction"
        ]

        var actions = [
            "ActionCollection.showWizard",
            "ActionCollection.configureAccount",
            "ActionCollection.configureShortcut",
            "ActionCollection.configureNotification",
            "ActionCollection.quitAction"
        ]

        var viewCode = "import RingQmlWidgets 1.0\n import DesktopView 1.0 \n QmlAction {text: \"View\" \n"
        for (var i = 0; i < view.length; i++)
            viewCode = viewCode + "QmlAction { action:"+view[i]+"} \n"
        viewCode += "}"

        _regularActions.push(Qt.createQmlObject(viewCode, root, "dynamicSnippet1"))

        for (var i = 0; i < actions.length; i++) {
            _regularActions.push(Qt.createQmlObject("import RingQmlWidgets 1.0;import DesktopView 1.0; QmlAction {
                action: "+actions[i]+"
            }", root, "dynamicSnippet2"))
        }

        return _regularActions
    }

    function mobileActions() {
        if (_mobileActions != undefined)
            return _mobileActions

        _mobileActions = []

        var actions = [
            "ActionCollection.showWizard",
            "ActionCollection.configureAccount",
            "ActionCollection.configureShortcut",
            "ActionCollection.configureNotification",
            "ActionCollection.quitAction"
        ]

        var viewCode = "import RingQmlWidgets 1.0\n QmlAction {text: \""+i18n("Settings")+"\" \n"
        for (var i = 0; i < actions.length; i++)
            viewCode = viewCode + "QmlAction { action:"+actions[i]+"} \n"
        viewCode += "}"

        _mobileActions.push(Qt.createQmlObject(viewCode, root, "dynamicSnippet1"))

        return _mobileActions
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
        bannerImageSource: ""
        actions: regularActions()

        Component {
            id: timelineAndAccounts
            ColumnLayout {
                width: globalDrawer.width
                anchors.fill: drawerContainer

                JamiTimeline.PeersTimeline {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    width: globalDrawer.width

                    state: "mobile"
                    onIndividualSelected: {
                        mainPage.setIndividual(ind)
                    }
                }
                AccountList {
                    height: contentHeight
                    Layout.fillWidth: true
                    enableAdd: false
                }
            }
        }

        // Yes, this is a Kirigami bug, the Item should *not* be needed, but
        // `content` cannot be resized after being created
        content: Item {
            id: drawerContainer

            Component {
                id: accountsOnly
                ColumnLayout {
                    anchors.fill: drawerContainer

                    Item {
                        width: globalDrawer.width

                        // Layouts in Layouts **** up QML, so make sure the result
                        // is what's expected by removing all ambiguities. Otherwise
                        // it's randomly corrupted
                        Layout.preferredHeight: drawerContainer.height
                            - accounts.contentHeight
                        Layout.maximumHeight: drawerContainer.height
                            - accounts.contentHeight
                        Layout.minimumHeight: drawerContainer.height
                            - accounts.contentHeight
                        height: drawerContainer.height
                            - accounts.contentHeight
                        implicitHeight: drawerContainer.height
                            - accounts.contentHeight

                        Layout.fillHeight: true
                        Layout.fillWidth: true
                    }

                    AccountList {
                        id: accounts
                        height: contentHeight
                        Layout.minimumHeight: contentHeight
                        Layout.preferredHeight: contentHeight
                        Layout.maximumHeight: contentHeight
                        Layout.fillWidth: true
                        Layout.fillHeight: false
                        enableAdd: true
                    }
                }
            }

            // The drawer is a QtQuick.Layout, it uses implicitHeight even if
            // Layout.fillHeight is set to device the "winner" that will take
            // the space. Make sure this is the loader.
            implicitHeight: 99999
            Layout.fillHeight: true
            Layout.fillWidth: true
            width: globalDrawer.width

            Loader {
                id: accountsLoader
                anchors.fill: parent
                width: globalDrawer.width
                Layout.preferredWidth: globalDrawer.width
                active: globalDrawer.drawerOpen || globalDrawer.peeking
                sourceComponent: accountsOnly
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
        sourceComponent: JamiWizard.Wizard {
            anchors.fill: parent
            z: 999999
            onVisibleChanged: {
                if (!visible) {
                    wizardLoader.visible = false
                    wizardLoader.active = false
                }
                wizardVisible = visible
            }
        }
    }

    QActionBinder {
        id: binder
        actionCollection: ActionCollection.kactionCollection
    }

    Item {
        id: desktopOverlay //TODO Qt5.10, use QQC2.Overlay
        anchors.fill: parent
        visible: false
        z: 1
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Loader {
            id: dockBar
            Layout.fillHeight: true
            Layout.maximumWidth: item ? item.width : 0
            Layout.minimumWidth: item ? item.width : 0
            Layout.preferredWidth: item ? item.width : 0
            visible: stateGroup.state != "mobile"
            active: true
            sourceComponent: DockBar {
                height: parent.height
                newHolder: newHolder
                onTimelineSelected: {
                    if (!mainPage.individual)
                        return

                    item.setCurrentIndex(mainPage.suggestedTimelineIndex)
                }
            }
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
//                     if (contactInfo != undefined)
//                         contactInfo.cachedPhoto = contactHeader.cachedPhoto
                }

                onSelectChat: {
                    mainPage.setCurrentPage(MainPage.TIMELINE)
                    timelinePage.currentInstance.focusEdit()
                }

                onSelectVideo: {
                    mainPage.setCurrentPage(MainPage.MEDIA)
                }
            }

            JamiContactView.MainPage {
                id: mainPage
                header: contactHeader
                Layout.fillHeight: true
                Layout.fillWidth: true

                onSuggestSelection: {
                    var d = dockBar
                    if (d.item) {
                        console.log("Setting timeline index:", modelIndex, individual)
                        d.item.setCurrentIndex(
                            modelIndex
                        )
                    }
                }
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
                    target: dockBar
                    active: true
                }
                PropertyChanges {
                    target: contactHeader
                    isCompact: false
                }
                PropertyChanges {
                    target: accountsLoader
                    sourceComponent: accountsOnly
                }
                PropertyChanges {
                    target: globalDrawer
                    title: "Ring KDE client"
                    titleIcon: "ring-kde"
                    bannerImageSource: ""
                    actions: regularActions()
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
                    active: false
                }
                PropertyChanges {
                    target: accountsLoader
                    sourceComponent: timelineAndAccounts
                }
                PropertyChanges {
                    target: globalDrawer
//FIXME buggy
//                     title: ""
//                     titleIcon: ""
//                     bannerImageSource: ""
                    actions: mobileActions()
                }
                PropertyChanges {
                    target: mainPage
                    mobile: true
                }
            }
        ]
    }
}
