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
import QtQuick 2.2
import QtQuick.Controls 2.2 as Controls
import QtQuick.Layouts 1.2 as Layouts
import org.kde.kirigami 2.4 as Kirigami

Column {
    id: root
    property var model;

    Column {
        id: comm
        width: parent.width

        Repeater {
            model: root.model.communication

            delegate: Kirigami.BasicListItem {
                id: delegate

                contentItem: Layouts.RowLayout {
                    spacing: Kirigami.Units.largeSpacing * 2
                    anchors.verticalCenter: parent.verticalCenter

                    Kirigami.Icon {
                        id: icon
                        width: Kirigami.Units.iconSizes.smallMedium
                        height: width
                        source: model.icon
                        color: "#232627"
                        Layouts.Layout.alignment: Qt.AlignVCenter
                    }

                    Column {
                        Layouts.Layout.alignment: Qt.AlignVCenter

                        Controls.Label {
                            text: model.text
                            color: model.default ? "#2980b9" : "#232627"
                        }

                        Controls.Label {
                            text: model.description
                            font.pointSize: 8
                            color: "#7f8c8d"
                        }

                    }

                    Rectangle {
                        Layouts.Layout.fillWidth: true
                        Layouts.Layout.alignment: Qt.AlignVCenter

                        Kirigami.Icon {
                            visible: typeof model.actions !== "undefined"
                            source: "kmouth-phrase-new"
                            width: Kirigami.Units.iconSizes.smallMedium
                            height: width
                            anchors.right: parent.right
                            anchors.rightMargin: Kirigami.Units.largeSpacing
                            anchors.verticalCenter: parent.verticalCenter
                            id: call
                        }
                    }
                }
            }
        }
    }

    Kirigami.Heading {
        level: 4
        visible: typeof root.model.history !== "undefined" && root.model.history.count
        text: "History"
        id: history
    }

    Column {
        width: root.width
        Repeater {
            model: root.model.history

            delegate: Kirigami.SwipeListItem {
                Row {
                    spacing: Kirigami.Units.largeSpacing * 2
                    anchors.verticalCenter: parent.verticalCenter

                    Kirigami.Icon {
                        width: Kirigami.Units.iconSizes.smallMedium
                        height: width
                        source: model.icon
                        color: "#232627"
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Column {
                        anchors.verticalCenter: parent.verticalCenter

                        Controls.Label {
                            text: model.text
                            color: "#232627"
                        }

                        Controls.Label {
                            text: model.date
                            font.pointSize: 8
                            color: "#7f8c8d"
                        }
                    }
                }
            }
        }
    }
}
