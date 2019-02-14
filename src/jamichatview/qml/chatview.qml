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

import org.kde.kirigami 2.2 as Kirigami
import org.kde.playground.kquickitemviews 1.0 as KQuickItemViews
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import net.lvindustries.ringqtquick.models 1.0 as RingQtModels
import org.kde.ringkde.jamichatview 1.0 as JamiChatView
import org.kde.ringkde.genericutils 1.0 as GenericUtils

KQuickItemViews.HierarchyView {
    id: chatView
    clip: true

    property bool forceTime: false
    property var bubbleBackground: blendColor()
    property var bubbleForeground: ""
    property var unreadBackground: ""
    property var unreadForeground: ""
    property alias slideshow: slideshow
    property bool displayExtraTimePrivate: moving || dragging || forceTime
    property bool displayExtraTime: false

    onDisplayExtraTimePrivateChanged: {
        if (displayExtraTimePrivate)
            displayExtraTime  = true
        else
            dateTimer.running = true
    }

    Timer {
        id: dateTimer
        interval: 1500
        repeat: false
        onTriggered: displayExtraTime = false
    }

    model: RingQtQuick.TimelineFilter {
        individual: mainPage.currentIndividual
        showCalls: false
        showEmptyGroups: true
        showMessages: true
        initDelay: 33
    }

    function blendColor() {
        chatView.bubbleBackground = Qt.tint(
            Kirigami.Theme.backgroundColor,
            Kirigami.Theme.highlightColor //base1
        )
        chatView.unreadBackground = Qt.tint(
            Kirigami.Theme.backgroundColor, "#99BB0000"
        )

        chatView.bubbleForeground = Kirigami.Theme.highlightedTextColor
        chatView.unreadForeground = Kirigami.Theme.highlightedTextColor

        return chatView.bubbleBackground
    }

    JamiChatView.Slideshow {
        id: slideshow
    }

    // Display something when the chat is empty
    Text {
        color: Kirigami.Theme.textColor
        text: i18n("There is nothing yet, enter a message below or place a call using the buttons\nfound in the header")
        anchors.centerIn: parent
        visible: chatView.empty
        horizontalAlignment: Text.AlignHCenter
    }

    Component {
        id: messageDelegate
        Loader {
            id: chatLoader

            // Create a delegate for each type
            Component {
                id: sectionDelegate
                JamiChatView.TextMessageGroup {
                    width: chatView.width
                }
            }

            Component {
                id: snapshotGroupDelegate
                JamiChatView.Snapshots {
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
                JamiChatView.CallGroup {
                    width: chatView.width
                }
            }

            Component {
                id: categoryDelegate
                JamiChatView.CategoryHeader {
                    width: chatView.width
                }
            }

            Component {
                id: textDelegate
                JamiChatView.TextBubble {

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
                if (nodeType == RingQtModels.IndividualTimelineModel.TIME_CATEGORY)
                    return categoryDelegate

                if (nodeType == RingQtModels.IndividualTimelineModel.TEXT_MESSAGE)
                    return textDelegate

                if (nodeType == RingQtModels.IndividualTimelineModel.SNAPSHOT_GROUP)
                    return snapshotGroupDelegate

                if (nodeType == RingQtModels.IndividualTimelineModel.SECTION_DELIMITER)
                    return sectionDelegate

                if (
                    nodeType == RingQtModels.IndividualTimelineModel.CALL_GROUP ||
                    nodeType == RingQtModels.IndividualTimelineModel.RECORDINGS
                )
                    return callDelegate
            }

            sourceComponent: selectDelegate()
        }
    }

    delegate: messageDelegate
}
