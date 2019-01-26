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
import org.kde.ringkde.jamitimeline 1.0 as JamiTimeline
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import org.kde.kirigami 2.2 as Kirigami

ListView {
    id: findPeers
    clip: true

    FontMetrics {
        id: fontMetrics
    }

    Component {
        id: searchDelegate
        JamiTimeline.SearchDelegate {
            width: findPeers.width
            buttonHeight: fontMetrics.height + 12
            labelHeight: fontMetrics.height
        }
    }

    highlight: Item {
        anchors.topMargin: 5
        anchors.bottomMargin: 5
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        Rectangle {
            anchors.fill: parent
            anchors.topMargin: 5
            anchors.bottomMargin: 5
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            radius: 5
            color: Kirigami.Theme.highlightColor
        }
    }

    model: RingSession.numberCompletionModel
    delegate: searchDelegate
//     add: Transition {
//         NumberAnimation {
//             properties: "y"
//             duration: 100
//         }
//     }
//     addDisplaced: Transition {
//         NumberAnimation {
//             properties: "y"
//             duration: 100
//         }
//     }
}
