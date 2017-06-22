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

    /*TreeHelper {
        id: treeHelper
    }*/

    function getIndex(model, index) {
        var m = model.modelIndex(index)
        return m// treeHelper.getIndex(0, m)
    }

    function bestDelegate(parent, model2, type, index) {
        var component = Qt.createComponent("TextMessageGroup.qml", parent);

        if (component.status == Component.Ready) {
            component.width     = chatView.width
            component.model     = chatView.model
            component.rootIndex = getIndex(model2, index)
        }

        return component
    }

    Component {
        id: messageDelegate
        Loader {
            asynchronous: true
            visible: status == Loader.Ready

            sourceComponent: ColumnLayout {
                width: parent.width
                CategoryHeader {
                    width: chatView.width
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

                        delegate: Component {
                            Loader {
                                asynchronous: true
                                visible: status == Loader.Ready

                                Component {
                                    id: troll
                                    TextMessageGroup {
                                        width: chatView.width
                                        id: groupDelegate
                                        model: chatView.model
                                        rootIndex: getIndex(childrenView.model, modelIndex)
                                    }

                                }

                                Component {
                                    id: troll2
                                    CallGroup {
                                        width: chatView.width
                                        id: groupDelegate
                                        model: chatView.model
                                        rootIndex: getIndex(childrenView.model, modelIndex)
                                    }

                                }

                                property int modelIndex: index
                                sourceComponent: nodeType == PeerTimelineModel.SECTION_DELIMITER ? troll : troll2
                            }
                        }

                    }
                }
            }
        }
    }

    ModelScrollAdapter {
        id: scrollAdapter
        target: chatView
    }

    onModelChanged: {
        scrollAdapter.model = model
    }

    delegate: messageDelegate
}
