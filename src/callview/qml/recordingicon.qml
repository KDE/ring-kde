/***************************************************************************
 *   Copyright (C) 2018 by Bluesystems                                     *
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
import QtQuick.Layouts 1.0
import org.kde.kirigami 2.2 as Kirigami
import Ring 1.0

Loader {
    active: call
        && call.lifeCycleState == Call.PROGRESS
        && call.recordingAV

    sourceComponent: RowLayout {
        height: 30

        Timer {
            interval: 1000
            running: true
            repeat: true
            onTriggered: {
                redCircle.visible = !redCircle.visible
            }
        }

        Item {
            height: 30
            width: 30
            Rectangle {
                id: redCircle
                radius: 99
                height: 30
                width: 30
                color: Kirigami.Theme.negativeTextColor
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        Text {
            text: i18n("Recording")
            color: "white"
            font.bold: true
            font.pointSize: 14
            anchors.verticalCenter: parent.verticalCenter
        }
        Item {
            height: 1
            width: 6
        }
    }
}
