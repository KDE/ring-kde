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
import QtQuick.Controls 2.2 as Controls
import org.kde.kirigami 2.4 as Kirigami
import QtQuick.Layouts 1.9 as Layouts
import org.kde.ringkde.basicview 1.0 as BasicView
import org.kde.ringkde.jamicontactview 1.0 as JamiContactView
import org.kde.ringkde.jaminotification 1.0 as JamiNotification
import org.kde.ringkde.jamihistoryview 1.0 as JamiHistoryView
import org.kde.ringkde.genericutils 1.0 as GenericUtils

Controls.ToolBar {
    id: sideBar
    property QtObject individual: mainPage.currentIndividual

    Layouts.ColumnLayout {
        anchors.fill: parent
        spacing: Kirigami.Units.largeSpacing*2

        BasicView.DesktopHeader {
            Layouts.Layout.preferredWidth: parent.width
            Layouts.Layout.preferredHeight: Kirigami.Units.iconSizes.large*1.5
            textColor: "white"
            photoSize: Kirigami.Units.iconSizes.large*1.5
        }

        Kirigami.Separator {
            Layouts.Layout.fillWidth: true
            color: "white"
        }

        Controls.TabBar {
            id: tabs
            property real underlineHeight: 4
            Layouts.Layout.fillWidth: true
            Layouts.Layout.preferredHeight: implicitHeight + underlineHeight*1.5
            currentIndex: swipeView.currentIndex

            background: Item {
                // Force the Material underline style in all theme
                Rectangle {
                    height: tabs.underlineHeight
                    anchors.bottom: parent.bottom
                    width: sideBar.width/3
                    color: Kirigami.Theme.highlightedTextColor
                    x: tabs.currentIndex*(sideBar.width/3)
                    Behavior on x {
                        NumberAnimation {duration: 200}
                    }
                }

            }

            Controls.TabButton {
                width: sideBar.width/3
                background: Kirigami.Icon {
                    source: "help-about"
                    height: parent.height - tabs.underlineHeight*1.5
                }
            }

            Controls.TabButton {
                width: sideBar.width/3
                background: Kirigami.Icon {
                    source: "shallow-history"
                    height: parent.height - tabs.underlineHeight*1.5
                }
            }

            Controls.TabButton {
                width: sideBar.width/3
                background: Kirigami.Icon {
                    source: "favorite"
                    height: parent.height - tabs.underlineHeight*1.5
                }
            }
        }

        Controls.SwipeView {
            id: swipeView
            clip: true
            currentIndex: tabs.currentIndex
            Layouts.Layout.fillWidth: true
            Layouts.Layout.fillHeight: true

            Item {
//                 background: Item{}

                Layouts.ColumnLayout {
                    spacing: Kirigami.Units.largeSpacing*2

                    Kirigami.Heading {
                        level: 1
                        text: "Actions"
                        color: "white"

                        Layouts.Layout.fillWidth: true

                        elide: Text.ElideRight

                        //show only when at least half of the string has been painted: use
                        //opacity as using visible it won't correctly recalculate the width
                        opacity: width > implicitWidth/2

                    }

                    JamiContactView.CommonActions {
                        individual: mainPage.individual
                        Layouts.Layout.fillWidth: true
                    }

                    Kirigami.Heading {
                        level: 1
                        text: "Notifications"
                        color: "white"

                        Layouts.Layout.fillWidth: true

                        elide: Text.ElideRight

                        //show only when at least half of the string has been painted: use
                        //opacity as using visible it won't correctly recalculate the width
                        opacity: width > implicitWidth/2

                    }

                    JamiNotification.IndividualSettings {
                        individual: mainPage.individual
                        Layouts.Layout.fillWidth: true
                    }

                    Kirigami.Heading {
                        level: 1
                        text: "Statistics"
                        color: "white"

                        Layouts.Layout.fillWidth: true

                        elide: Text.ElideRight

                        //show only when at least half of the string has been painted: use
                        //opacity as using visible it won't correctly recalculate the width
                        opacity: width > implicitWidth/2

                    }

                    JamiContactView.Statistics {
                        individual: mainPage.individual
                        Layouts.Layout.fillWidth: true
                        labelColor: Kirigami.Theme.textColor
                    }

                    Item {
                        Layouts.Layout.fillHeight: true
                    }
                }
            }

            Loader {
                width: swipeView.width
                height: swipeView.height
                active: currentIndex == Controls.SwipeView.index
                sourceComponent: JamiHistoryView.RecordingHistory {
                    anchors.fill: parent
                    individual: mainPage.currentIndividual
                }
            }

            Item {}
        }
    }
}
