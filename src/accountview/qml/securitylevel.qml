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
import org.kde.playground.kquickview 1.0 as KQuickItemViews

Item {
    property int level: 0
    height: Kirigami.Theme.defaultFont.pointSize*3
    implicitHeight: Kirigami.Theme.defaultFont.pointSize*3
    Rectangle {
        id: securityLevel
        x: 20
        width: parent.width - 40
        height: parent.height

        color: "transparent"
        border.width: 1
        border.color: inactivePalette.text
        radius: 99
        clip: true

        ListModel {
            id: securityLevels

            ListElement { text: /*i18n(*/ "None"       }
            ListElement { text: /*i18n(*/ "Weak"       }
            ListElement { text: /*i18n(*/ "Medium"     }
            ListElement { text: /*i18n(*/ "Acceptable" }
            ListElement { text: /*i18n(*/ "Strong"     }
            ListElement { text: /*i18n(*/ "Complete"   }
        }

        Rectangle {
            height: parent.height
            width: (securityLevel.width / 6) * (level+1)
            color: level < 2 ? Kirigami.Theme.negativeTextColor : (
                    level == 2 ? Kirigami.Theme.neutralTextColor :
                        Kirigami.Theme.positiveTextColor
                )
            radius: 99
        }

        Row {
            anchors.fill: parent
            Repeater {
                model: securityLevels
                delegate: RowLayout {
                    height: securityLevel.height
                    width: securityLevel.width / 6
                    Label {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        horizontalAlignment: Text.AlignHCente
                        text: model.text
                    }
                    Rectangle {
                        Layout.fillHeight: true
                        width: 1
                        color: inactivePalette.text
                    }
                }
            }
        }
    }
}
