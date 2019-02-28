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
import QtQuick 2.2
import QtQuick.Layouts 1.4
import QtQuick.Controls 2.2 as Controls
import org.kde.kirigami 2.6 as Kirigami
import org.kde.ringkde.basicview 1.0 as BasicView
import org.kde.ringkde.jamichatview 1.0 as JamiChatView

Kirigami.Page {
    property bool _fits: false

    // Force the toolbar style to prevent the action and drawer handles from
    // getting on top of the chatbox.
    Kirigami.Theme.colorSet: Kirigami.Theme.View
    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.ToolBar

    id: chatPage

    // Remove all padding and spacing because otherwise the separators will have holes
    spacing: 0
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0
    padding: 0

    /*
     * When there is plenty of room, move the header into the toolbar.
     */
    titleDelegate: BasicView.DesktopHeader {
        id: dheader
        visible: fits
        Layout.fillHeight: true
        photoSize: parent.parent.height - 2*Kirigami.Units.largeSpacing
        Layout.fillWidth: true
        Component.onCompleted: _fits = fits
        onFitsChanged: _fits = fits
    }

    /*
     * When there isn't enough room in the toolbar, add another row
     */
    header: Controls.ToolBar {
        visible: (!_fits)
        height: visible ? Kirigami.Units.gridUnit * 2.5 : 0

        Layout.fillWidth: true
        Layout.preferredHeight: visible ? Kirigami.Units.gridUnit * 5 : 0
        Layout.margins: 0

        BasicView.DesktopHeader {
            anchors.fill: parent
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        /*
         * This is the main chat widget with the chatbox, messages, emojis
         * and timeline scrollbar.
         */
        JamiChatView.ChatPage {
            id: chatView
            showScrollbar: pageStack.wideMode
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.bottomMargin: 0
            currentIndividual: workflow.currentIndividual

            /*
            * Display the branding when there's no content
            */
            Image {
                property real size: Math.min(400, chatPage.width*0.75)
                anchors.centerIn: parent
                verticalAlignment: Image.AlignVCenter
                opacity: chatView.empty == true ? 0.3 : 0
                visible: opacity > 0

                fillMode: Image.PreserveAspectFit
                sourceSize.width:size;sourceSize.height:size;width:size;height:size
                source: "image://SymbolicColorizer/qrc:/sharedassets/branding.svg"

                Behavior on opacity {
                    NumberAnimation {duration: 300; easing.type: Easing.InQuad}
                }
            }
        }

        Kirigami.Separator {
            Layout.fillHeight: true
        }

        /*
         * Only add a sidebar when there is more room than the chat can make use
         * of. It was decided to restrict the chat with to prevent long bubble.
         */
        Loader {
            id: sidebarLoader

            // 750 is the 600pt maximum width of the chat + width of the sidebar
            active: pageStack.wideMode
                && workflow.currentIndividual
                && (!Kirigami.Settings.isMobile)
                && parent.width > 750

            Layout.preferredWidth: active ? 250 : 0
            Layout.fillHeight: true

            sourceComponent: BasicView.SideBar {
                anchors.fill: sidebarLoader
                onSelectIndex: {
                    chatView.jumpTo(idx)
                }
            }
        }
    }

    actions {
        left : pageManager.videoCallAction
        main : pageManager.audioCallAction
        right: Kirigami.Settings.isMobile ?
            undefined : pageManager.shareScreenAction
    }

    // Not worth it on mobile
    contextualActions: Kirigami.Settings.isMobile ? [] : [
        actionCollection.bookmarkAction,
        actionCollection.shareAction,
        actionCollection.editAction,
        actionCollection.photoAction,
        actionCollection.banAction,
        actionCollection.deleteAction,
        actionCollection.learAction,
    ]
}
