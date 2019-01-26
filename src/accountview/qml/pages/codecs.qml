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
import QtQuick 2.8
import org.kde.kirigami 2.5 as Kirigami
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Page {
    property var account: object

    Kirigami.CardsGridView {
        id: gridView
        minimumColumnWidth: 160
        cellWidth: 160
        cellHeight: 160

        Component {
            id: codecDelegate
            Item {
                id: content
                implicitWidth: card.width + 60
                implicitHeight: card.height + 60
                width: 200
                Kirigami.Card {
                    id: card
                    anchors.centerIn: parent
                    height: info.height + 20
                    width: info.implicitWidth + 20
                    RowLayout {
                        id: info
                        anchors.centerIn: parent
                        Kirigami.ListItemDragHandle {
                            listView: gridView
                            listItem: content
                            Layout.fillHeight: true
                        }
                        ColumnLayout {
                            height: implicitHeight
                            CheckBox {
                                text: name
                                checked: model.enabled
                                onCheckedChanged: {
                                    model.enabled = checked
                                }
                            }
                            Label {
                                text: "<b>Bitrate: </b>"+bitrate
                            }
                            Label {
                                text: "<b>Type: </b>"+type
                            }
                        }
                    }
                }
            }
        }

        anchors.fill: parent
        model: account ? account.codecModel : null
        delegate: codecDelegate
    }
}
