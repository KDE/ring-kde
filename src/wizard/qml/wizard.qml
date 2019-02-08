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

import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import org.kde.ringkde.jamiwizard 1.0 as JamiWizard

Rectangle {
    id: applicationWindow
    visible: true
    color: "#004d61"

    property string previousState: ""
    signal wizardFinished()

    JamiWizard.Splash {
        id: startingPage
        anchors.fill: parent
        anchors.bottomMargin: footer.height
        z: 2

        onVisibleChanged: {
            if (!visible) {
                // Force the geometry while the animation is running, anchor it after
                footer.y = applicationWindow.height + footer.height

                // Begin the first animation
                /*ANIM footer.state = "shown"*/
                footer.state = "shown"
            }
        }

        onQuit: {
            applicationWindow.visible = false
        }
    }

    JamiWizard.StartPage {
        id: frontPage
        anchors.fill: parent
        anchors.bottomMargin: footer.height
        visible: true
    }

    JamiWizard.ButtonBar {
        id: footer
        height: 45
        width: applicationWindow.width
        visible: false
    }

    onWidthChanged: { //HACK
        footer.width = width
    }

    // Display or hide the "next/finish" button in the bar
    Connections {
        target: frontPage
        onNextAvailableChanged: {
            footer.state = frontPage.nextAvailable ? "nextAvailable" : (
                frontPage.busy ? "busy" : "locked"
            )
        }
    }

    // Use a better label when next is unavailable because it's busy
    Connections {
        target: frontPage
        onBusyChanged: {
            footer.state = frontPage.nextAvailable ? "nextAvailable" : (
                frontPage.busy ? "busy" : "locked"
            )
        }
    }

    // Close the wizard and let the user configure things manually
    Connections {
        target: footer
        onSkip: {
            wizardFinished()
        }
    }

    // Try to restore the previous page
    Connections {
        target: footer.backButton
        onClicked: {
            var oldPreviousState = previousState
            previousState = stateGroup.state

            switch (stateGroup.state) {
                case 'createRegRing':
                case 'importRing':
                    stateGroup.state = "*"
                    break;
                case 'showProfile':
                    previousState = "*"
                    footer.state = "nextUnavailable" //FIXME
                    stateGroup.state = oldPreviousState
                    break;
            }
        }
    }

    // Show the profile configuration
    Connections {
        target: footer.nextButton
        onClicked: {
            switch (stateGroup.state) {
                case 'createRegRing':
                    frontPage.createRing.commitAccount()
                    break;
                case 'importRing':
                    frontPage.importRing.createAccount()
                    break;
                case '*':
                    stateGroup.state = "*"
                    stateGroup.state = "showProfile"
                    footer.state = "finish"
                    break;
                case 'showProfile':
                    var acc = frontPage.createRing.account
                    if (acc && acc.alias == "")
                        frontPage.createRing.account.alias = frontPage.createRing.account.profile.formattedName

                    frontPage.profilePage.save()

                    wizardFinished()
                    break;
            }
        }
    }

    Connections {
        target: frontPage.createRing
        onRegistrationCompleted: {
            stateGroup.state = "showProfile"
            footer.state = "finish"
            account.createProfile()

            frontPage.profilePage.individual = account.profile.individual
        }
        onAccountChanged: {
            var acc = frontPage.createRing.account

            if ((!acc) || acc.registrationType != RingQtQuick.Account.READY)
                return

            if (!acc.profile)
                acc.createProfile()

            //frontPage.profilePage.currentPerson = acc.profile
            frontPage.profilePage.individual = acc.profile.individual
        }
    }

    Connections {
        target: frontPage.importRing
        onRegistrationCompleted: {
            stateGroup.state = "showProfile"
            footer.state = "finish"
        }
        onAccountChanged: {
            var acc = frontPage.importRing.account

            if (!acc)
                return

            frontPage.profilePage.currentPerson = acc.profile
        }
    }

    StateGroup {
        id: stateGroup
        states: [
            // Hide all buttons
            State {
                name: "hideAccountTypes"

                PropertyChanges {
                    target: frontPage.createRingAccount
                    x: -436
                    y: 322
                    anchors.horizontalCenter: undefined
                    opacity: 0
                }

                PropertyChanges {
                    target: frontPage.createAnonRingAccount
                    x: 836
                    y: -322
                    anchors.horizontalCenter: undefined
                    opacity: 0
                }

                PropertyChanges {
                    target: frontPage.importRingAccount
                    x: -383
                    y: 575
                    opacity: 0
                }

                PropertyChanges {
                    target: frontPage.importSIPAccount
                    x: 771
                    y: 439
                    opacity: 0
                    text: i18n("Import a SIP account")
                }

                PropertyChanges {
                    target: frontPage.createIp2IPAccount
                    x: 771
                    y: 96
                    opacity: 0
                }

                PropertyChanges {
                    target: frontPage.logo
                    x: applicationWindow.width/2 - frontPage.welcomeMessage.width/2
                    y: 8
                }

                PropertyChanges {
                    target: frontPage.welcomeMessage
                    x: applicationWindow.width/2 - 114/2
                    y: 8
                }
            },

            // Create a new Ring account
            State {
                name: "createRegRing"
                extend: "hideAccountTypes"

                PropertyChanges {
                    target: frontPage
                    state: "createRegRing"
                }

                PropertyChanges {
                    target: frontPage.createRing
                    opacity: 1
                    visible: true

                    // Use raw [x,y] to avoid following the animation
                    x: frontPage.width/2 - width/2

                }
            },

            // Import an existing account from another device
            State {
                name: "importRing"
                extend: "hideAccountTypes"

                PropertyChanges {
                    target: frontPage
                    state: "importRing"
                }

                PropertyChanges {
                    target: frontPage.importRing
                    opacity: 1
                    visible: true

                    // Use raw [x,y] to avoid following the animation
                    x: frontPage.width/2 - width/2
                }
            },

            // Last page, the shared profile information
            State {
                name: "showProfile"
                extend: "hideAccountTypes"
                PropertyChanges {
                    target: frontPage
                    state: "showProfile"
                }

                PropertyChanges {
                    target: frontPage.profilePage
                    opacity: 1
                    visible: true
                }

                PropertyChanges {
                    target: frontPage.importRing
                    opacity: 0
                    visible: false
                }

                PropertyChanges {
                    target: frontPage.createRing
                    opacity: 0
                    visible: false
                }
            }
        ]

        transitions: [
            Transition {
                from: "*"; to: "hideAccountTypes"
                NumberAnimation {
                    targets: [
                        frontPage.importRingAccount,
                        frontPage.importSIPAccount,
                        frontPage.createIp2IPAccount,
                        frontPage.createRingAccount,
                        frontPage.createAnonRingAccount,
                        frontPage.logo,
                        frontPage.welcomeMessage
                    ]
                    easing.type: Easing.OutBounce
                    properties: "x,y";
                    duration: 1000
                }

                // Fade-out the irrelevant buttons
                NumberAnimation {
                    targets: [
                        frontPage.importRingAccount,
                        frontPage.importSIPAccount,
                        frontPage.createIp2IPAccount,
                        frontPage.createRingAccount,
                        frontPage.createAnonRingAccount,
                        frontPage.logo,
                        frontPage.welcomeMessage
                    ]
                    easing.type: Easing.Linear
                    properties: "opacity";
                    duration: 500
                    onStopped: {
                        importRingAccount.visible = false;
                        importSIPAccount.visible = false;
                        createIp2IPAccount.visible = false;
                        createRingAccount.visible = false;
                        createAnonRingAccount.visible = false;
                    }
                }
            },

            // TO DIALOG
            Transition {
                from: "hideAccountTypes"; to: "createRegRing"

                // Slide-in the new account form
                NumberAnimation {
                    target: frontPage.createRing
                    easing.type: Easing.OutQuad
                    properties: "x,opacity";
                    duration: 700
                }
            },

            Transition {
                from: "hideAccountTypes"; to: "importRing"

                // Slide-in the new account form
                NumberAnimation {
                    target: frontPage.importRing
                    easing.type: Easing.OutQuad
                    properties: "x,opacity";
                    duration: 700
                }
            },

            // BACK TO FIRST PAGE
            Transition {
                from: "createRegRing"; to: "*"

                // Slide-in the new account form
                NumberAnimation {
                    target: frontPage.createRing
                    easing.type: Easing.OutQuad
                    properties: "x,opacity";
                    duration: 700
                }
            },

            Transition {
                from: "importRing"; to: "*"

                // Slide-in the new account form
                NumberAnimation {
                    target: frontPage.importRing
                    easing.type: Easing.OutQuad
                    properties: "x,opacity";
                    duration: 700
                }
            }

        ]
    }
}
