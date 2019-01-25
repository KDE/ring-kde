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

Layouts.ColumnLayout {
    property var model

    id: root
    spacing: 2 * Kirigami.Units.largeSpacing

    Controls.Label {
        id: header
        text: "Edit details"
        font.pointSize: 16
    }

    Kirigami.FormLayout {
        id: form
        Layouts.Layout.fillWidth: true

        Controls.TextField {
            Kirigami.FormData.label: i18n("Firstname:")
            text: model.firstname
        }

        Controls.TextField {
            Kirigami.FormData.label: i18n("Lastname:")
            text: model.lastname
        }

        Kirigami.Separator {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: i18n("Phone")
        }

        Repeater {
            model: root.model.communication
            delegate: Controls.TextField {
                visible: model.type === "phone"
                Kirigami.FormData.label: model.description + ":"
                text: model.text
            }
        }

        Kirigami.Separator {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: i18n("Email")
        }

        Repeater {
            model: root.model.communication
            delegate: Controls.TextField {
                visible: model.type === "email"
                Kirigami.FormData.label: model.description + ":"
                text: model.text
            }
        }

        Kirigami.Separator {
            Kirigami.FormData.isSection: true
        }

        Controls.Switch {
            Kirigami.FormData.label: i18n("Additional fields")
        }

    }

    Controls.Button {
        text: i18n("Save")
        Layouts.Layout.alignment: Qt.AlignRight
        anchors.rightMargin: Kirigami.Units.largeSpacing
    }
}
