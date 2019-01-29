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
    property var model;
    property var currentIndividual: null
    property var boo: chatView
    property alias showContactDetails: detail.active
    property alias editContact: form.active

    Kirigami.Theme.colorSet: Kirigami.Theme.View

    id: chatPage

    spacing: 0
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0
    padding: 0
    title: "Susan Hoffman"

    titleDelegate: BasicView.DesktopHeader {
        id: dheader
        visible: fits
        onVisibleChanged: header.visible = !visible
    }

    header: BasicView.Header {
        id: header
        visible: !pageStack.wideMode
        Layout.fillWidth: true
        height: visible ? Kirigami.Units.gridUnit * 5 : 0
        Layout.preferredHeight: visible ? Kirigami.Units.gridUnit * 5 : 0
        Layout.margins: 0
    }

    JamiChatView.ChatPage {
        id: chatView
        anchors.fill: parent
        currentIndividual: mainPage.currentIndividual
        timelineModel: mainPage.timelineModel
    }

    Loader {
        active: false
        id: detail
        sourceComponent: BasicView.DetailPage {
            model: chatPage.model
            onSheetOpenChanged: detail.active = sheetOpen
            Component.onCompleted: sheetOpen = true
        }
    }

    Loader {
        active: false
        id: form
        sourceComponent: BasicView.DetailPage {
            model: chatPage.model
            onSheetOpenChanged: form.active = sheetOpen
            Component.onCompleted: {sheetOpen = true}
        }
    }

    actions {
        left : actionCollection.mailAction
        main : actionCollection.callAction
        right: actionCollection.chatAction
    }

    contextualActions: [
        actionCollection.bookmarkAction,
        actionCollection.shareAction,
        actionCollection.editAction,
        actionCollection.photoAction,
        actionCollection.banAction,
        actionCollection.deleteAction,
        actionCollection.learAction,
    ]
}
