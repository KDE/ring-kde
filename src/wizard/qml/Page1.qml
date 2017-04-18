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

Page1Form {
    anchors.fill: parent

    property bool nextAvailable: false

    // Let the pretty little animation run its course
    Timer {
        id: createAccountTimer
        interval: 0 /*ANIM 1000*/
        running: false
        repeat: false
        onTriggered: {
            stateGroup.state = "createRegRing";
        }
    }

    Timer {
        id: importAccountTimer
        interval: 0 /*ANIM 1000*/
        running: false
        repeat: false
        onTriggered: {
            stateGroup.state = "importRing";
        }
    }

    importRingAccount.onClicked: {
        // This will move the current buttons out and the fields in
        removeAnchors()
        importAccountTimer.running = true
    }

    createRingAccount.onClicked: {
        // This will move the current buttons out and the fields in
        removeAnchors()
        createAccountTimer.running = true
    }

    createRing.onNextAvailableChanged: isNextAvailable()
    importRing.onNextAvailableChanged: isNextAvailable()

    function isNextAvailable() {
        switch(state) {
        case 'createRegRing':
            nextAvailable = createRing.nextAvailable
            break
        case 'importRing':
            nextAvailable = importRing.nextAvailable
            break
        case 'showProfile':
            nextAvailable = true
            break;
        }
    }

    function removeAnchors() {
        // Remove the anchors from the buttons so they can be animated freely
        var widgets = [
            createRingAccount,
            welcomeMessage,
            createIp2IPAccount,
            logo,
            importRingAccount,
            importSIPAccount
        ];
        for (var i = 0, len = widgets.length; i < len; i++) {
            widgets[i].x = parent.x - (widgets[i].width/2);
            widgets[i].anchors.horizontalCenter = undefined;
        }
        stateGroup.state = "hideAccountTypes";
    }
}
