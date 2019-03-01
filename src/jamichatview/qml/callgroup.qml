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
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0 as Controls
import org.kde.kirigami 2.2 as Kirigami

import org.kde.ringkde.jamichatview 1.0 as JamiChatView
import org.kde.ringkde.jamitimelinebase 1.0 as JamiTimelineBase

ColumnLayout {
    id: textGroupDelegate

    JamiChatView.GroupHeader {
        type: "call"
        Layout.margins: 4
        Layout.fillWidth: true
        Layout.preferredHeight: 38
    }

    Loader {
        id: callGroup
        Layout.alignment: Qt.AlignHCenter

        Component {
            id: mediumSize
            JamiTimelineBase.MultiCall {
                modelIndex: rootIndex
                width: callCount*32
                count: callCount
            }
        }

        Component {
            id: singleSize
            Item {
                anchors.centerIn: callGroup
                implicitWidth: content.implicitWidth + 2 + 4*Kirigami.Units.largeSpacing
                implicitHeight: content.implicitHeight + 2 + 4*Kirigami.Units.largeSpacing

                Rectangle {
                    radius: 7
                    anchors.centerIn: parent
                    width: content.implicitWidth + 2 + 2*Kirigami.Units.largeSpacing
                    height: content.implicitHeight + 2 + 2*Kirigami.Units.largeSpacing
                    opacity: 0.3
                    color: Kirigami.Theme.highlightColor
                    border.width: 1
                    border.color: isMissed ?
                        Kirigami.Theme.negativeTextColor : Kirigami.Theme.textColor
                }

                GridLayout {
                    id: content
                    columns: 2
                    rows: 2
                    anchors.centerIn: parent
                    JamiTimelineBase.MultiCall {
                        Layout.preferredHeight: 2.5*Kirigami.Units.fontMetrics.height
                        Layout.preferredWidth: 2.5*Kirigami.Units.fontMetrics.height
                        Layout.alignment: Qt.AlignHCenter
                        modelIndex: rootIndex
                        count: callCount
                        Layout.rowSpan: 2
                    }
                    Controls.Label {
                        text: formattedDate
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.alignment: Qt.AlignVCenter
                    }
                    Controls.Label {
                        text: i18n("Lasted ") + totalActiveTime + i18n(" seconds")
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.alignment: Qt.AlignVCenter
                    }
                }
            }
        }

        Component {
            id: summary
            GridLayout {
                rows: 4
                columns: 2
                Layout.alignment: Qt.AlignHCenter
                Kirigami.Icon {
                    Layout.preferredWidth: Kirigami.Units.iconSizes.medium
                    Layout.preferredHeight: Kirigami.Units.iconSizes.medium
                    source: ":/sharedassets/phone_dark/incoming.svg"
                }
                Controls.Label {
                    text: "<b>" + incomingCount + i18n("</b> incoming calls")
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignVCenter
                }
                Kirigami.Icon {
                    Layout.preferredWidth: Kirigami.Units.iconSizes.medium
                    Layout.preferredHeight: Kirigami.Units.iconSizes.medium
                    source: ":/sharedassets/phone_dark/outgoing.svg"
                }
                Controls.Label {
                    text: "<b>" +incomingCount + i18n("</b> outgoing calls")
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignVCenter
                }
                Kirigami.Icon {
                    Layout.preferredWidth: Kirigami.Units.iconSizes.medium
                    Layout.preferredHeight: Kirigami.Units.iconSizes.medium
                    source: ":/sharedassets/phone_dark/missed_incoming.svg"
                }
                Controls.Label {
                    text: "<b>" +missedCount + i18n("</b> missed calls")
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignVCenter
                }
                Item {
                    height: Kirigami.Units.largeSpacing
                    Layout.columnSpan: 2
                }
            }
        }

        function selectType() {
            var cc = callCount

            if (cc > 1 && cc < 10)
                return mediumSize

            if (cc == 1)
                return singleSize

            return summary
        }

        // reload when the call count changes
        sourceComponent: callCount ? selectType() : 0
        active: sourceComponent
    }

    Item {
        height: 3
    }

    JamiChatView.GroupFooter {
        Layout.fillWidth: true
    }
}
