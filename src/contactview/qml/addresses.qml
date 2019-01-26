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


Item {
    id: phoneNumbers
    property alias model : numbers.model

    ColumnLayout {
        anchors.fill: parent

        ListView {
            id: numbers
            Layout.fillWidth: true
            Layout.fillHeight: true
            anchors.margins: 3
            delegate: Item {
                height: 60
                width: phoneNumbers.width
                ColumnLayout {
                    anchors.fill: parent
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Text {
                            text: type
                            color: Kirigami.Theme.textColor
                        }
                        Text {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            text: addressLine
                            color: Kirigami.Theme.textColor
                        }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Text {
                            text: zipCode
                            color: Kirigami.Theme.textColor
                        }
                        Text {
                            text: city
                            Layout.fillWidth: true
                            color: Kirigami.Theme.textColor
                        }
                        Text {
                            text: state
                            color: Kirigami.Theme.textColor
                        }
                        Text {
                            text: country
                            color: Kirigami.Theme.textColor
                        }
                        Item {
                            Layout.preferredWidth: 5
                        }
                    }
                }
            }
        }
    }
}
