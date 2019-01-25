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
import org.kde.ringkde.basicview 1.0 as BasicView

Kirigami.ApplicationWindow {
    width: 320
    height: 600
    id: root

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
        model: mydata
    }

    BasicView.ChatPage {
        id: chat
        visible: true
    }

    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
     }

    globalDrawer: Kirigami.GlobalDrawer {
        actions: [
            actionCollection.newContact,
            actionCollection.editAction,
        ]
    }

}
