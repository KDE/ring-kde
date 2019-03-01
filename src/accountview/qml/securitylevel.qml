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
import QtQuick 2.8
import org.kde.kirigami 2.5 as Kirigami
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import org.kde.playground.kquickitemviews 1.0 as KQuickItemViews

/**
 * Display how secure the account is and how to improve it.
 */
Item {
    id: securityLevel
    property int level: 0
    property int informationCount: 0
    property int warningCount: 0
    property int issueCount: 0
    property int errorCount: 0
    property bool displayIssues: false
    property QtObject model: null
    property real issuesHeight: 0
    height: Kirigami.Theme.defaultFont.pointSize*5 + issuesHeight
    implicitHeight: Kirigami.Theme.defaultFont.pointSize*5 + issuesHeight

    ColumnLayout {
        width: parent.width - 10
        height: parent.height
        anchors.horizontalCenter: parent.horizontalCenter

        Behavior on height {
            NumberAnimation {duration: 200}
        }

        Rectangle {
            id: securityLevelIndicator
            x: 20
            anchors.leftMargin: 20
            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Theme.defaultFont.pointSize*3 + issuesHeight

            color: "transparent"
            border.width: 1
            border.color: Kirigami.Theme.textColor
            radius: displayIssues ? 5 : 99
            clip: true

            Behavior on radius {
                NumberAnimation {duration: 200}
            }

            Behavior on Layout.preferredHeight {
                NumberAnimation { duration: 200 }
            }

            ListModel {
                id: securityLevels

                ListElement { text: /*i18n(*/ "None"       }
                ListElement { text: /*i18n(*/ "Weak"       }
                ListElement { text: /*i18n(*/ "Medium"     }
                ListElement { text: /*i18n(*/ "Acceptable" }
                ListElement { text: /*i18n(*/ "Strong"     }
                ListElement { text: /*i18n(*/ "Complete"   }
            }

            Rectangle {
                height: Kirigami.Theme.defaultFont.pointSize*3
                width: (securityLevelIndicator.width / 6) * (level+1)
                color: level < 2 ? Kirigami.Theme.negativeTextColor : (
                        level == 2 ? Kirigami.Theme.neutralTextColor :
                            Kirigami.Theme.positiveTextColor
                    )
                radius: 99

                Behavior on width {
                    NumberAnimation {duration: 200}
                }
            }

            MouseArea {
                width: parent.width
                height: Kirigami.Theme.defaultFont.pointSize*3
                Row {
                    anchors.fill: parent
                    Repeater {
                        model: securityLevels
                        delegate: RowLayout {
                            height: Kirigami.Theme.defaultFont.pointSize*3
                            width: securityLevelIndicator.width / 6
                            Text {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                text: model.text
                                font.bold: true
                                color: Kirigami.Theme.textColor
                            }
                            Rectangle {
                                Layout.fillHeight: true
                                width: 1
                                color: Kirigami.Theme.textColor
                            }
                        }
                    }
                }

                onClicked: {
                    displayIssues = !displayIssues
                }
            }

            Loader {
                y: Kirigami.Theme.defaultFont.pointSize*3
                x: 1
                active: displayIssues
                sourceComponent: ListView {
                    id: securityIssues
                    interactive: false
                    model: securityLevel.model
                    height: contentHeight
                    width: securityLevelIndicator.width - 2
                    delegate: Kirigami.SwipeListItem {
                        id: swipeItem
                        width: parent.width
                        RowLayout {
                            anchors.fill: parent
                            KQuickItemViews.DecorationAdapter {
                                pixmap: decoration
                                Layout.preferredWidth: swipeItem.implicitHeight
                                Layout.minimumWidth: swipeItem.implicitHeight
                                Layout.fillHeight: true
                            }
                            Label {
                                text: display
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                            }
                        }
                    }

                    onContentHeightChanged: {
                        // 2 is to avoid hiding the border
                        securityLevel.issuesHeight = contentHeight + 2
                    }
                }

                onActiveChanged: {
                    if (!active)
                        securityLevel.issuesHeight = 0
                }
            }
        }

        MouseArea {
            Layout.preferredHeight: Kirigami.Theme.defaultFont.pointSize*2
            Layout.fillWidth: true
            RowLayout {
                anchors.fill: parent

                Item { Layout.fillWidth: true; height: 1 }

                Image {
                    Layout.preferredHeight: Kirigami.Theme.defaultFont.pointSize*2
                    Layout.preferredWidth: Kirigami.Theme.defaultFont.pointSize*2
                    source: "image://icon/dialog-information"
                }

                Label {
                    Layout.fillWidth: true
                    text: informationCount + i18n(" tips")
                }

                Image {
                    Layout.preferredHeight: Kirigami.Theme.defaultFont.pointSize*2
                    Layout.preferredWidth: Kirigami.Theme.defaultFont.pointSize*2
                    source: "image://icon/dialog-warning"
                }

                Label {
                    Layout.fillWidth: true
                    text: warningCount + i18n(" warnings")
                }

                Image {
                    Layout.preferredHeight: Kirigami.Theme.defaultFont.pointSize*2
                    Layout.preferredWidth: Kirigami.Theme.defaultFont.pointSize*2
                    source: "image://icon/task-attempt"
                }

                Label {
                    Layout.fillWidth: true
                    text: issueCount + i18n(" issues")
                }

                Image {
                    Layout.preferredHeight: Kirigami.Theme.defaultFont.pointSize*2
                    Layout.preferredWidth: Kirigami.Theme.defaultFont.pointSize*2
                    source: "image://icon/dialog-error"
                }

                Label {
                    Layout.fillWidth: true
                    text: errorCount + i18n(" errors")
                }

                Item { Layout.fillWidth: true; height: 1 }
            }

            onClicked: {
                displayIssues = !displayIssues
            }
        }
    }
}
