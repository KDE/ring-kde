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
import org.kde.kirigami 2.2 as Kirigami

Text {
    property var align: Qt.LeftEdge
    property bool compact: false
    property string section: ""
    property string recentDate: ""

    anchors.margins:10
    text: section + (section == i18n("Never") ?
        "" :", <b>" + (recentDate ? recentDate.toLocaleString(Qt.locale(), "d MMM") : "")+ "</b>")
    leftPadding: 10
    rightPadding: 10
    height: (compact ? 1.5 : 3) *Kirigami.Units.fontMetrics.height
    verticalAlignment: Text.AlignVCenter
    color: Kirigami.Theme.textColor
    x: 10

    Rectangle {
        height: 30
        width: 100
        border.width: 1
        border.color: Kirigami.Theme.textColor
        color: "transparent"
        radius: 1
        z: -10
        anchors.fill: parent
    }

    Rectangle {
        width: 2
        height: parent.height
        color: "#993558"
        anchors.left: align == Qt.LeftEdge ? parent.left : undefined
        anchors.right: align == Qt.RightEdge ? parent.right : undefined
    }
}
