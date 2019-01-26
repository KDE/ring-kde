/******************************************************************************
 *   Copyright (C) 2012 by Savoir-Faire Linux                                 *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>   *
 *                                                                            *
 *   This library is free software; you can redistribute it and/or            *
 *   modify it under the terms of the GNU Lesser General Public               *
 *   License as published by the Free Software Foundation; either             *
 *   version 2.1 of the License, or (at your option) any later version.       *
 *                                                                            *
 *   This library is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU        *
 *   Lesser General Public License for more details.                          *
 *                                                                            *
 *   You should have received a copy of the Lesser GNU General Public License *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *****************************************************************************/
import QtQuick 2.0
import org.kde.kirigami 2.2 as Kirigami


Column {
    property QtObject call: null

    Image {
        height: 22
        fillMode: Image.PreserveAspectFit
        width: parent.width
        sourceSize.width: 22
        sourceSize.height: 22
        horizontalAlignment: Image.AlignHCenter
        source: "image://SymbolicColorizer/:/sharedassets/outline/warning.svg"
    }
    Text {
        text: call.lastErrorMessage + " (" + call.lastErrorCode + ")"
        color: Kirigami.Theme.textColor
        wrapMode: Text.WordWrap
        width: parent.width
    }
}
