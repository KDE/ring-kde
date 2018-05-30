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
import QtQuick 2.7
import Ring 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.2
import org.kde.kirigami 2.2 as Kirigami
import RingQmlWidgets 1.0

Dialog {
    id: dialog
    parent: applicationWindow().contentItem
    x: applicationWindow().contentItem.width / 2 - width/2
    y: applicationWindow().contentItem.height / 2 - height/2
    width: applicationWindow().contentItem.width * 0.66
    height: applicationWindow().contentItem.height * 0.66

    // As on Qt 5.9.5, QML crashes on a double free when Close is used
    standardButtons: Dialog.NoButton

    property int rowCount: AccountModel.incomingContactRequestModel.size

    function selectContactRequest(index, obj, modelIndex) {
        contactInfo.individual = obj.peer.individual
    }

    Component {
        id: contactRequestDelegate
        ContactCard {
            selectionCallback: selectContactRequest
            Component.onCompleted: {
                if (index == contactRequestList.currentIndex)
                    contactInfo.individual = object.peer.individual
            }

            Component {
                id: buttons
                Row {
                    z: 2
                    anchors.rightMargin: 15
                    Button {
                        text: i18n("Accept")
                        onClicked: {
                            object.accept()
                            if (AccountModel.incomingContactRequestModel.size == 0)
                                dialog.close()
                        }
                    }
                    Button {
                        text: i18n("Decline")
                        onClicked: {
                            object.discard()
                        }
                    }
                    Button {
                        text: i18n("Block")
                        onClicked: {
                            object.block()
                        }
                    }
                }
            }

            rightControls: buttons
        }
    }

    ColumnLayout {
        anchors.fill: parent
        ListView {
            id: contactRequestList
            Layout.minimumHeight: Math.min(contentHeight, 150)
            Layout.preferredHeight: contentHeight
            Layout.maximumHeight: 300
            Layout.fillWidth: true
            model: AccountModel.incomingContactRequestModel
            delegate: contactRequestDelegate
            currentIndex: 0

            highlight: Item {
                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 5
                    anchors.leftMargin: 10
                    anchors.rightMargin: 10
                    radius: 5
                    color: activePalette.highlight
                }
            }
        }

        ContactInfo {
            id: contactInfo
            Layout.fillWidth: true
            Layout.fillHeight: true
            showStat: false
            showImage: true
            showSave: false
            editable: false
            forcedState: "profile"
            defaultName: ""
        }

        RowLayout {
            Item {
                Layout.fillWidth: true
            }
            Button {
                text: i18n("Close")
                onClicked: {
                    dialog.close()
                }
            }
        }
    }
}
