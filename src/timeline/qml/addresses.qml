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

Rectangle {
    id: phoneNumbers
    property alias model : numbers.model

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    SystemPalette {
        id: inactivePalette
        colorGroup: SystemPalette.Disabled
    }

    color: activePalette.base

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
                            color: inactivePalette.text
                        }
                        Text {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            text: addressLine
                            color: activePalette.text
                        }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Text {
                            text: zipCode
                            color: activePalette.text
                        }
                        Text {
                            text: city
                            Layout.fillWidth: true
                            color: activePalette.text
                        }
                        Text {
                            text: state
                            color: activePalette.text
                        }
                        Text {
                            text: country
                            color: activePalette.text
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
