/***************************************************************************
 *   Copyright (C) 2019 by Bluesystems                                     *
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
import QtQuick 2.9

import org.kde.ringkde.genericutils 1.0 as GenericUtils
import QtQuick.Layouts 1.2 as Layouts

Layouts.ColumnLayout {
    property QtObject individual: null

    Layouts.Layout.topMargin: 0
    Layouts.Layout.leftMargin: 0
    Layouts.Layout.rightMargin: 0

    GenericUtils.ActionSwitch {
        icon: "mail-message"
        text: "Write email"
        checkable: false
    }

    GenericUtils.ActionSwitch {
        icon: "delete"
        text: "Delete contact"
        checkable: false
    }

    GenericUtils.ActionSwitch {
        icon: "edit-clear-history"
        text: "Clear history"
        checkable: false
    }

    GenericUtils.ActionSwitch {
        icon: "favorite"
        text: "Bookmark"
        checkable: true
    }

    GenericUtils.ActionSwitch {
        icon: "im-kick-user"
        text: "Block"
        checkable: true
    }
}
