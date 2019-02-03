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
import org.kde.ringkde.jamisearch 1.0 as JamiSearch

Kirigami.Page {
    id: peerListPage
    property alias currentIndex: list.currentIndex;
    property bool displayWelcome: false

    spacing: 0
    leftPadding: 0; rightPadding: 0; topPadding: 0;bottomPadding: 0; padding: 0

    signal search()

    Kirigami.Theme.colorSet: Kirigami.Theme.View

    header: Layouts.ColumnLayout {
        visible: globalTroubleshoot.sourceComponent != null
        height: visible && globalTroubleshoot.active ? implicitHeight : 0
        width: peerListPage.width
        spacing: Kirigami.Units.largeSpacing

        JamiTroubleShooting.GlobalTroubleshoot {
            id: globalTroubleshoot
            Layouts.Layout.fillWidth: true
            Layouts.Layout.margins: Kirigami.Units.largeSpacing
        }

        Item {
            visible: globalTroubleshoot.sourceComponent != null
            height: 10
        }
    }

    title: i18n("Address book")

    background: Rectangle {
        color: Kirigami.Theme.backgroundColor
    }

    titleDelegate: Item {
        id: header
        implicitHeight: parent.parent.height - 2*Kirigami.Units.largeSpacing
        implicitWidth: 10

        JamiSearch.SearchBox {
            id: headerSearchbox
            searchView: _searchView
            anchors.centerIn: parent
            anchors.margins: Kirigami.Units.largeSpacing
            width: parent.width - 2 * Kirigami.Units.largeSpacing
            height: headerSearchbox.focus ? parent.height : parent.height * 1.5
            z: 9999

            Connections {
                target: peerListPage
                onSearch: {
                    headerSearchbox.forceFocus()
                }
            }
        }

        JamiSearch.Overlay {
            id: _searchView
            source: peerListPage
            searchBox: headerSearchbox
            width: peerListPage.width
            height: peerListPage.height + header.height
            x: -(peerListPage.width - header.width)
            y: -Kirigami.Units.largeSpacing

            onDisplayWelcomeChanged: {
                peerListPage.displayWelcome = displayWelcome
            }

            onContactMethodSelected: {
                mainPage.currentContactMethod = cm
                var idx = RingSession.peersTimelineModel.individualIndex(cm.individual)
                list.currentIndex = idx.row
            }

            //HACK obey god dammit
            onHeightChanged: {
                height = peerListPage.height + header.height
                x = -(peerListPage.width - header.width)
                width = peerListPage.width
            }

            //HACK obey god dammit
            onActiveChanged: {
                if (!active)
                    return

                width = peerListPage.width
                height = peerListPage.height + header.height
                x = -(peerListPage.width - header.width)
            }

            z: 9998
        }
    }

    BasicView.List {
        id: list
        width: parent.width
        height: parent.height
    }
}
