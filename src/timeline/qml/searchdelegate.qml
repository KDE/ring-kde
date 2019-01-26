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
import QtQuick.Controls 2.0 as Controls

import org.kde.kirigami 2.0 as Kirigami
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import net.lvindustries.ringqtquick.models 1.0 as RingQtModels
import org.kde.ringkde.jamicontactview 1.0 as JamiContactView

Item {
    property QtObject contactMethod: object
    property double buttonHeight: 30
    property double labelHeight: fontMetrics.height*2
    property bool showPhoto: true
    property bool showControls: true
    property bool showSeparator: true
    property bool isSelectable: selectable
    height: rows.implicitHeight + 10 //10 == 2*margins

    opacity: selectable ? 1 : 0.6

    Behavior on opacity {
        NumberAnimation {duration: 200}
    }

    function getSourceColor(src) {
        if (src == RingQtModels.NumberCompletionModel.FROM_BOOKMARKS)
            return "#cfa02a"
        if (src == RingQtModels.NumberCompletionModel.FROM_HISTORY)
            return "#be3411"
        if (src == RingQtModels.NumberCompletionModel.FROM_CONTACTS)
            return "#14883b"
        if (src == RingQtModels.NumberCompletionModel.FROM_WEB)
            return "#2c53bd"

        return "red"
    }

    Rectangle {
        anchors.margins: 5
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        border.width: 1
        border.color: Kirigami.Theme.textColor
        color: "transparent"
        anchors.fill: parent
        radius: 5

        ColumnLayout {
            id: rows
            spacing: 5
            anchors.fill: parent

            Item {
                height: 5
            }

            RowLayout {
                anchors.topMargin: 5
                anchors.bottomMargin: 5
                anchors.fill: parent

                Item {
                    height: 32
                    width: 20
                    Rectangle {
                        width: 10
                        height: 10
                        radius: 99
                        anchors.centerIn: parent

                        color: getSourceColor(entrySource)
                    }
                }

                JamiContactView.ContactPhoto {
                    width:  32
                    height: 32
                    implicitHeight: 32
                    implicitWidth: 32
                    anchors.margins: 5
                    contactMethod: object
                }

                Text {
                    Layout.fillWidth: true
                    height: 32
                    id: displayNameLabel
                    text: display
                    color: Kirigami.Theme.textColor
                    verticalAlignment: Text.AlignHCenter
                    font.bold: true
                }
            }

            Layout.fillHeight: true
            Layout.fillWidth: true

            Rectangle {
                id: uriSeparator
                height: 1
                Layout.fillWidth: true
                color: Kirigami.Theme.textColor
            }

            Text {
                Layout.fillWidth: true
                color: Kirigami.Theme.textColor
                text: "  "+(registeredName == "" ? uri : registeredName)
                height: implicitHeight * 2

                Controls.BusyIndicator {
                    id: busyIndicator
                    visible: nameStatus == RingQtModels.NumberCompletionModel.IN_PROGRESS
                    anchors.right: searchStatus.left
                    anchors.rightMargin: 2
                    height: searchStatus.implicitHeight * 2
                    width: searchStatus.implicitHeight * 2
                    //anchors.top: uriSeparator.bottom
                }

                Text {
                    id: searchStatus
                    anchors.right: parent.right
                    anchors.rightMargin: 5
                    color: nameStatus == RingQtModels.NumberCompletionModel.SUCCESS ?
                        Kirigami.Theme.positiveTextColor : (nameStatus == RingQtModels.NumberCompletionModel.IN_PROGRESS ?
                        Kirigami.Theme.neutralTextColor :
                        Kirigami.Theme.negativeTextColor)
                    visible: supportsRegistry
                    text: nameStatusString
                }
            }

            Rectangle {
                visible: accountAlias != ""
                height: 1
                Layout.fillWidth: true
                color: Kirigami.Theme.textColor
            }

            Rectangle {
                color: Kirigami.Theme.highlightColor
                radius: 99
                anchors.right: parent.right
                anchors.rightMargin: 5
                height: accountAliasText.implicitHeight + 4
                visible: accountAlias != ""
                width: accountAliasText.implicitWidth + accountAliasText.implicitHeight
                Text {
                    id: accountAliasText
                    anchors.centerIn: parent
                    anchors.leftMargin: 16
                    anchors.rightMargin: 16
                    text: accountAlias
                    color: Kirigami.Theme.highlightedTextColor
                }
            }

            Item {
                height: 2
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                // Display an error message when the selected element doesn't exist
                if (!selectable) {
                    displayNotFoundMessage()
                    return
                }

                contactMethodSelected(RingSession.individualDirectory.fromTemporary(contactMethod))
                hide()
            }
        }
    }
}
