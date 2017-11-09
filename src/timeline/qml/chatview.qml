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

HierarchyView {
    id: chatView
    clip: true

    property var treeHelper: _treeHelper

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    TreeHelper {
        id: _treeHelper
    }

    property var bubbleBackground: blendColor()
    property var bubbleForeground: ""
    property var unreadBackground: ""
    property var unreadForeground: ""
    property alias slideshow: slideshow

    function blendColor() {
        var base2 = activePalette.highlight
        base2 = Qt.rgba(base2.r, base2.g, base2.b, 0.3)
        var base1 = Qt.tint(activePalette.base, base2)

        chatView.bubbleBackground = base1
        chatView.unreadBackground = Qt.tint(activePalette.base, "#33BB0000")
        chatView.bubbleForeground = activePalette.text
        chatView.unreadForeground = activePalette.text

        return base1
    }

    Slideshow {
        id: slideshow
    }

    Component {
        id: messageDelegate
        Loader {
            id: chatLoader

            // Create a delegate for each type
            Component {
                id: sectionDelegate
                TextMessageGroup {
                    width: chatView.width
                }
            }

            Component {
                id: snapshotGroupDelegate
                Snapshots {
                    width: chatView.width
                    onViewImage: {
                        chatView.slideshow.active = true
                        chatView.slideshow.model = model
                        chatView.slideshow.source = path
                    }
                }
            }

            Component {
                id: callDelegate
                CallGroup {
                    width: chatView.width
                }
            }

            Component {
                id: categoryDelegate
                CategoryHeader {
                    width: chatView.width
                }
            }

            Component {
                id: textDelegate
                TextBubble {

                    background: isRead ?
                        chatView.bubbleBackground : chatView.unreadBackground
                    foreground: isRead ?
                        chatView.bubbleForeground : chatView.unreadForeground
                    width: chatView.width
                    onClicked: {
                        chatView.treeHelper.setData(rootIndex, true, "isRead")
                    }
                }
            }

            // Some elements don't have delegates because they are handled
            // by their parent delegates
            function selectDelegate() {
                if (nodeType == PeerTimelineModel.TIME_CATEGORY)
                    return categoryDelegate

                if (nodeType == PeerTimelineModel.TEXT_MESSAGE)
                    return textDelegate

                if (nodeType == PeerTimelineModel.SNAPSHOT_GROUP)
                    return snapshotGroupDelegate

                if (nodeType == PeerTimelineModel.SECTION_DELIMITER)
                    return sectionDelegate

                if (nodeType == PeerTimelineModel.CALL_GROUP)
                    return callDelegate
            }

            sourceComponent: selectDelegate()
        }
    }

    delegate: messageDelegate
}
