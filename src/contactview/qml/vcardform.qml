/***************************************************************************
 *   Copyright (C) 2017-2018 by Bluesystems                                *
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
import net.lvindustries.ringqtquick 1.0 as RingQtQuick

GridLayout {
    id: mainInfo

    property var currentContactMethod: null
    property var currentPerson: null
    property bool editing: true
    property real preferredHeight: implicitHeight
    property var individual: null

    property bool hasTarget: individual || currentContactMethod || currentPerson

    property alias name: formattedName.text

    signal changed()

    columns: 2
    rowSpacing: 10
    columnSpacing: 10

    function syncDetails(person) {
        if ((!currentContactMethod) && (!currentPerson))
            return

        // Changing the CM will flush the content, preserve it
        var old_formattedName  = formattedName.text
        var old_firstName      = firstName.text
        var old_secondName     = lastName.text
        var old_preferredEmail = email.text
//         var old_organization   = organization.text

        // Create a real contact method in case this is a temporary one
        if (currentContactMethod && currentContactMethod.type == RingQtQuick.ContactMethod.TEMPORARY)
            currentContactMethod = RingSession.individualDirectory.getNumber(
                currentContactMethod.uri,
                null,
                currentContactMethod.account
            )

        person.formattedName  = old_formattedName
        person.firstName      = old_firstName
        person.secondName     = old_secondName
        person.preferredEmail = old_preferredEmail
//         person.organization   = old_organization

        currentPerson = person
    }

    onCurrentContactMethodChanged: {
        if (!currentContactMethod)
            return

        currentPerson = currentContactMethod.person
    }

    onCurrentPersonChanged: {
        formattedName.text = currentPerson ?
            currentPerson.formattedName : ""
        firstName.text = currentPerson ?
            currentPerson.firstName : ""
        lastName.text = currentPerson ?
            currentPerson.secondName : ""
        email.text = currentPerson ?
            currentPerson.preferredEmail : ""
//         organization.text = currentPerson ?
//             currentPerson.organization : ""

    }

    onIndividualChanged: {
        if (formattedName.text == "" && individual)
            formattedName.text = individual.bestName
    }

    Component.onCompleted: {
        if (formattedName.text == "" && individual)
            formattedName.text = individual.bestName
    }

    Label {
        id: label
        text: i18n("Formatted name:")
        color: labelColor ? labelColor : activePalette.text
    }

    TextField {
        id: formattedName
        readOnly: hasTarget && !mainInfo.editing
        onTextChanged: {
            mainInfo.changed()
        }
    }

    Label {
        text: i18n("Primary name:")
        color: labelColor ? labelColor : activePalette.text
    }
    TextField {
        id: firstName
        readOnly: hasTarget && !mainInfo.editing
        onTextChanged: {
            mainInfo.changed()
        }
    }

    Label {
        text: i18n("Last name:")
        color: labelColor ? labelColor : activePalette.text
    }
    TextField {
        id: lastName
        readOnly: hasTarget && !mainInfo.editing
        onTextChanged: {
            mainInfo.changed()
        }
    }

    Label {
        text: i18n("Email:")
        color: labelColor ? labelColor : activePalette.text
    }
    TextField {
        id: email
        readOnly: hasTarget && !mainInfo.editing
        onTextChanged: {
            mainInfo.changed()
        }
    }

//     OutlineButton {
//         id: addButton
//         height: 54
//         sideMargin: 2
//         Layout.columnSpan: 2
//         width: formattedName.width + label.width + 30
//         label: i18n("Add a field")
//         topPadding: 2
//         visible: mainInfo.editing
//     }
}
