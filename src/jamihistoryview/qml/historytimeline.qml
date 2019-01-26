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

import org.kde.ringkde.jamicontactview 1.0 as JamiContactView
import org.kde.kirigami 2.2 as Kirigami
import net.lvindustries.ringqtquick 1.0 as RingQtQuick

Kirigami.Page {
    id: bookmarkList
    signal contactMethodSelected(var cm)

    leftPadding: 0
    topPadding: 0
    bottomPadding: 0
    rightPadding: 0

    FontMetrics {
        id: fontMetrics
    }

    Rectangle {
        color: Kirigami.Theme.backgroundColor
        anchors.fill: parent

        ListView {
            id: treeView
            anchors.fill: parent
            width: parent.width
            highlightFollowsCurrentItem: true

            model: RingSession.eventModel


            section.property: "formattedLastUsed"
            section.criteria: ViewSection.FullString
            section.delegate: Text {
                color: Kirigami.Theme.textColor
                text: section
            }

            highlight: Item {

                anchors.topMargin: 5
                anchors.bottomMargin: 5
                anchors.leftMargin: 30
                anchors.rightMargin: 40

                Rectangle {
                    anchors.fill: parent
                    anchors.topMargin: 5
                    anchors.bottomMargin: 5
                    anchors.leftMargin: 30
                    anchors.rightMargin: 40
                    radius: 10
                    color: Kirigami.Theme.highlightColor
                }
            }

            delegate: Item {
                height: 5*fontMetrics.height + 13
                width: parent.width

                Rectangle {
                    width: 1
                    color: Kirigami.Theme.textColor
                    height: parent.height
                    x: 10
                }

                Rectangle {
                    radius: 99
                    color: Kirigami.Theme.backgroundColor
                    border.width: 1
                    border.color: Kirigami.Theme.textColor
                    width: 16
                    height: 16
                    y: 10
                    x: 3 // (16 - 10) / 2

                    Rectangle {
                        id: demandsAttention
                        radius: 99
                        color: Kirigami.Theme.textColor
                        anchors.centerIn: parent
                        height: 8
                        width: 8
                    }
                }

                Rectangle {
                    border.color: Kirigami.Theme.textColor
                    border.width: 1
                    anchors.fill: parent

                    anchors.topMargin: 5
                    anchors.bottomMargin: 5
                    anchors.leftMargin: 30
                    anchors.rightMargin: 40

                    color: "transparent"
                    radius: 10

                    ColumnLayout {
                        anchors.fill: parent
                        RowLayout {
                            Layout.fillWidth: true
                            height: 3*fontMetrics.height

                            Item {
                                anchors.margins: 4

                                height: parent.height
                                width:  parent.height

                                JamiContactView.ContactPhoto {
                                    anchors.margins: 3
                                    anchors.fill: parent
                                    event: object
                                    defaultColor: ListView.isCurrentItem ?
                                        Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
                                    drawEmptyOutline: false
                                }
                            }

                            Text {
                                text: bestName
                                clip: true
                                font.bold : true
                                Layout.fillWidth: true
                                anchors.verticalCenter: parent.verticalCenter
                                color: ListView.isCurrentItem ?
                                    Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
                            }
                        }

                        Rectangle {
                            color: Kirigami.Theme.textColor
                            height:1
                            Layout.fillWidth: true
                        }

                        Text {
                            Layout.fillWidth: true
                            text: formattedDate

                            height: 2*fontMetrics.height
                            leftPadding: 10

                            verticalAlignment: Text.AlignVCenter
                            color: ListView.isCurrentItem ?
                                Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
                        }

                        Item {
                            Layout.fillHeight: true
                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        treeView.currentIndex = index
                    }
                }
            }
//             sortingEnabled: true
        }
    }
}
