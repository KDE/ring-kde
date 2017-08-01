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
import RingQmlWidgets 1.0
import QtQuick.Controls 2.0

GridLayout {
    id: screenSharing
    property QtObject call: null

    columns: 2

    Text {
        color: "white"
        text: qsTr("Screen")
    }
    ComboBox {
        id: screens
        Layout.fillWidth: true
        model: ["First", "Second", "Third"]
    }

    Text {
        color: "white"
        text: qsTr("X offset")
    }
    SpinBox {
        id: xOffset
        editable: true
        from: 0
        to: 99999
    }

    Text {
        color: "white"
        text: qsTr("Y offset")
    }
    SpinBox {
        id: yOffset
        editable: true
        from: 0
        to: 99999
    }

    Text {
        color: "white"
        text: qsTr("Width")
    }
    SpinBox {
        id: width
        editable: true
        from: 0
        to: 99999
    }

    Text {
        color: "white"
        text: qsTr("Height")
    }
    SpinBox {
        id: height
        editable: true
        from: 0
        to: 99999
    }

    Item {}

    Item {
        Layout.fillHeight: true
    }
    Item {}

    Item {
        Layout.fillWidth: true
        height: shareButton.height
        Button {
            id: shareButton
            anchors.right: parent.right
            anchors.top: parent.top
            text: "Share"
            onClicked: {
                if (!screenSharing.call)
                    return

                if (!screenSharing.call.sourceModel)
                    return

                screenSharing.call.sourceModel.setDisplay(
                    screens.currentIndex,
                    Qt.rect(
                        xOffset.value,
                        yOffset.value,
                        width.value,
                        height.value
                    )
                )
            }
        }
    }
}
