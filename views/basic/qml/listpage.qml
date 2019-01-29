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
import QtQuick.Controls 2.2 as Controls
import org.kde.kirigami 2.4 as Kirigami
import QtQuick.Layouts 1.2 as Layouts
import org.kde.ringkde.basicview 1.0 as BasicView
import org.kde.ringkde.jamitroubleshooting 1.0 as JamiTroubleShooting
import org.kde.ringkde.jamitimeline 1.0 as JamiTimeline

Kirigami.Page {
    id: peerListPage
    property alias currentIndex: list.currentIndex;
    property alias model: list.model

    spacing: 0
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0
    padding: 0

    JamiTimeline.SearchOverlay {
        id: searchView
        source: peerListPage
        visible: true
        width: peerListPage.width
        height: peerListPage.height

        onContactMethodSelected: {
            mainPage.currentContactMethod = cm
            setCurrentIndex(RingSession.peersTimelineModel.individualIndex(cm.individual))
        }

        z:100000
    }

    header: Layouts.ColumnLayout {
        height: globalTroubleshoot.active ? implicitHeight : 0
        width: peerListPage.width
        spacing: Kirigami.Units.largeSpacing

        JamiTroubleShooting.GlobalTroubleshoot {
            id: globalTroubleshoot
            Layouts.Layout.fillWidth: true
            Layouts.Layout.margins: Kirigami.Units.largeSpacing
        }

        Item {
            height: 10
        }
    }

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    title: i18n("Address book")

    background: Rectangle {
        color: Kirigami.Theme.backgroundColor
    }

    titleDelegate: Item {
        clip: true
        id: header
        implicitHeight: parent.parent.height - 2*Kirigami.Units.largeSpacing
        implicitWidth: 10


        JamiTimeline.SearchBox {
            id: searchBox
            searchView: searchView
            anchors.centerIn: parent
            anchors.margins: Kirigami.Units.largeSpacing
            width: parent.width - 2 * Kirigami.Units.largeSpacing
            z: 9999
        }
    }

    BasicView.List {
        id: list
        width: parent.width
        height: parent.height
    }

    actions {
        main: actionCollection.newContact
    }
}
