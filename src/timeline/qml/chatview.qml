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

import RingQmlWidgets 1.0

ListView {
    id: chatView
    clip: true

//     property QtObject model : null
    property var textColor: "blue"
    property var bubbleBackground: "red"

    TreeHelper {
        id: treeHelper
    }

    function getIndex(model, index) {
        var m = model.modelIndex(index)
        return m// treeHelper.getIndex(0, m)
    }

    Component {
        id: messageDelegate

        ColumnLayout {
            width: parent.width
            CategoryHeader {
                width: 99999999999
            }
            Repeater {
                id: childrenView
                Layout.fillWidth: true

                model: VisualDataModel {
                    id: childrenVisualDataModel
                    model: chatView.model
                    Component.onCompleted: {
                        childrenView.model.rootIndex = childrenView.model.modelIndex(index)
                    }

                    delegate: TextMessageGroup {
                        width: chatView.width
                        id: groupDelegate
                        model: chatView.model
                        rootIndex: getIndex(childrenView.model, index)
                    }
                }
            }
        }
    }

    delegate: messageDelegate
}
