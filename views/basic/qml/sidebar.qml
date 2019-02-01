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
import QtQuick.Layouts 1.2 as Layouts
import org.kde.ringkde.basicview 1.0 as BasicView
import org.kde.ringkde.jamicontactview 1.0 as JamiContactView

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
            Layouts.Layout.fillWidth: true

            Controls.TabButton {
                text: "Foo"
            }
            Controls.TabButton {
                text: "Bar"
            }
            Controls.TabButton {
                text: "Baz"
            }
        }

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

        BasicView.ActionSwitch {
            icon: "mail-message"
            text: "Write email"
            checkable: false
        }

        BasicView.ActionSwitch {
            icon: "delete"
            text: "Delete contact"
            checkable: false
        }

        BasicView.ActionSwitch {
            icon: "edit-clear-history"
            text: "Clear history"
            checkable: false
        }

        BasicView.ActionSwitch {
            icon: "favorite"
            text: "Bookmark"
            checkable: true
        }

        BasicView.ActionSwitch {
            icon: "im-kick-user"
            text: "Block"
            checkable: true
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

        BasicView.ActionSwitch {
            icon: "preferences-desktop-notification"
            text: "On new messages"
        }

        BasicView.ActionSwitch {
            icon: "preferences-desktop-notification"
            text: "On presence changes"
        }

        BasicView.ActionSwitch {
            icon: "preferences-desktop-notification"
            text: "Ignore incoming calls"
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

        Row {
            Item {width: Kirigami.Units.smallSpacing; height: 1}
            Kirigami.Icon {
                source: "appointment-new"
                height: Kirigami.Units.iconSizes.smallMedium
                width: Kirigami.Units.iconSizes.smallMedium
            }
            Item {width: Kirigami.Units.smallSpacing; height: 1}
            Controls.Label {
                color: "white"; text:  mainPage.currentIndividual?
                    mainPage.currentIndividual.formattedLastUsedTime : ""
            }
            Layouts.Layout.fillWidth: true
        }

        Row {
            Item {width: Kirigami.Units.smallSpacing; height: 1}
            Kirigami.Icon {
                source: "dialog-messages"
                height: Kirigami.Units.iconSizes.smallMedium
                width: Kirigami.Units.iconSizes.smallMedium
            }
            Item {width: Kirigami.Units.smallSpacing; height: 1}
            Controls.Label {
                color: "white"; text: "Texted "+ (mainPage.currentIndividual?
                    mainPage.currentIndividual.textMessageCount : 0) + " time"
            }
            Layouts.Layout.fillWidth: true
        }

        Row {
            Item {width: Kirigami.Units.smallSpacing; height: 1}
            Kirigami.Icon {
                source: "dialog-messages"
                height: Kirigami.Units.iconSizes.smallMedium
                width: Kirigami.Units.iconSizes.smallMedium
            }
            Item {width: Kirigami.Units.smallSpacing; height: 1}
            Controls.Label {
                color: "white"; text: (mainPage.currentIndividual?
                    mainPage.currentIndividual.unreadTextMessageCount : 0) + " unread messages"
            }
            Layouts.Layout.fillWidth: true
        }

        Row {
            Item {width: Kirigami.Units.smallSpacing; height: 1}
            Kirigami.Icon {
                source: "call-start"
                height: Kirigami.Units.iconSizes.smallMedium
                width: Kirigami.Units.iconSizes.smallMedium
            }
            Item {width: Kirigami.Units.smallSpacing; height: 1}
            Controls.Label {
                color: "white"; text: "Called "+ (mainPage.currentIndividual?
                    mainPage.currentIndividual.callCount : 0) + " time"
            }
            Layouts.Layout.fillWidth: true
        }

        Row {
            Item {width: Kirigami.Units.smallSpacing; height: 1}
            Kirigami.Icon {
                source: "call-start"
                height: Kirigami.Units.iconSizes.smallMedium
                width: Kirigami.Units.iconSizes.smallMedium
            }
            Item {width: Kirigami.Units.smallSpacing; height: 1}
            Controls.Label {
                color: "white"; text: "Spoken "+ Math.ceil((mainPage.currentIndividual?
                    mainPage.currentIndividual.totalSpentTime : 0)/60) + " minutes"
            }
            Layouts.Layout.fillWidth: true
        }

        Item {
            Layouts.Layout.fillHeight: true
        }
    }
}
