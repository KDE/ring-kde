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
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Ring 1.0

import RingQmlWidgets 1.0

Rectangle {
    id: timelinePage

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    color: activePalette.base

    property var currentContactMethod: null

    onCurrentContactMethodChanged: currentContactMethod ?
        chatView.model = currentContactMethod.timelineModel : null

    ColumnLayout {
        anchors.fill: parent
        clip: true

        RowLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true

            ChatView {
                id: chatView
                Layout.fillHeight: true
                Layout.fillWidth: true

                textColor: activePalette.text
                bubbleBackground: activePalette.highlight
            }

            TimelineScrollbar {
                id: scrollbar
                Layout.fillHeight: true
                Layout.preferredWidth: 20
            }
        }

        ChatBox {
            id: chatBox
            Layout.fillWidth: true
            height: 120
            MessageBuilder {id: builder}

            textColor: activePalette.text
            backgroundColor: activePalette.window
            emojiColor: activePalette.highlight
        }
    }

    Connections {
        target: chatBox
        onSendMessage: {
            builder.addPayload("text/plain", message)
            builder.sendWidth(currentContactMethod)
        }
    }
}
