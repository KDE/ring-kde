/*
 *   Copyright 2019 Emmanuel Lepage <emmanuel.lepage@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 3, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
import QtQuick 2.9
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import net.lvindustries.ringqtquick.media 1.0 as RingQtMedia
import org.kde.kirigami 2.4 as Kirigami

/**
 * This object hold the Banji toolbar (main) actions.
 *
 * They help to navigate the page stack and build the multimedia objects.
 */
QtObject {

    /*
     * Individual with multiple ring account, SIP URIs or Phone number require
     * special care given it isn't always obvious which to call.
     */
    function selectContactMethod(callback) {
        if (workflow.currentIndividual.requireUserSelection) {
            var component = Qt.createComponent("qrc:/callview/qml/cmselector.qml")
            if (component.status == Component.Ready) {
                var window = component.createObject(applicationWindow().contentItem)
                window.individual = workflow.currentIndividual
                window.callback = function(cm) {
                    workflow.currentContactMethod = cm
                    console.log("\n\nCALLBACK!", cm, workflow.currentContactMethod)
                    callback()
                }
                window.open()
            }
            else
                console.log("ERROR", component.status, component.errorString())
                return
        }
    }

    function showCallPage() {
        callpage.visible = true

        if (pageStack.currentItem == callpage)
            return

        showChat()

        for (var i = 0; i < pageStack.depth; i++) {
            if (pageStack.get(i) == callpage) {
                pageStack.currentIndex = i
                return
            }
        }
        pageStack.push(callpage)
    }

    function hideCall() {
        for (var i = 0; i < pageStack.depth; i++) {
            if (pageStack.get(i) == callpage) {
                pageStack.pop(callpage)
                pageStack.currentIndex = 0
                return
            }
        }
    }

    function showChat() {
        if (pageStack.currentItem == callpage)
            return

        for (var i = 0; i < pageStack.depth; i++) {
            if (pageStack.get(i) == chat) {
                pageStack.currentIndex = Kirigami.Settings.isMobile ? 1 : 0
                return
            }
        }

        pageStack.push(chat)
        pageStack.currentIndex = Kirigami.Settings.isMobile ? 1 : 0
    }

    function audioCall() {
        if (mainCallBuilder.choiceRequired)
            return selectContactMethod(audioCall)

        mainCallBuilder.audio         = true
        mainCallBuilder.video         = false
        mainCallBuilder.screenSharing = false
        workflow.showCall(mainCallBuilder.commit())
    }

    function videoCall() {
        if (mainCallBuilder.choiceRequired)
            return selectContactMethod(videoCall)

        mainCallBuilder.audio         = true
        mainCallBuilder.video         = true
        mainCallBuilder.screenSharing = false
        workflow.showCall(mainCallBuilder.commit())
    }

    function screencast() {
        if (mainCallBuilder.choiceRequired)
            return selectContactMethod(screencast)

        mainCallBuilder.audio         = true
        mainCallBuilder.video         = false
        mainCallBuilder.screenSharing = true
        workflow.showCall(mainCallBuilder.commit())
    }

    /*
     * Change the page automatically when an incoming call arrives.
     */
    property var _conn: Connections {
        target: workflow

        onCallChanged: {
            if (!workflow.call)
                hideCall()
            else
                showCallPage()
        }
    }

    readonly property Kirigami.Action chatAction: Kirigami.Action {
        iconName: "kmouth-phrase-new"
        text: i18n("Chat")
        onTriggered: {
            hideCall()
            showChat()
        }
    }

    readonly property Kirigami.Action videoCallAction: Kirigami.Action {
        iconName: "camera-web"
        text: i18n("Video call")
        enabled: availabilityTracker.canVideoCall
        onTriggered: {
            showCallPage()
            callpage.visible = true
            videoCall()
        }
    }

    readonly property Kirigami.Action shareScreenAction: Kirigami.Action {
        iconName: ":/sharedassets/outline/screen.svg"
        text: i18n("Cast screen")
        enabled: availabilityTracker.canVideoCall
        onTriggered: {
            showCallPage()
            callpage.visible = true
            screencast()
        }
    }

    readonly property Kirigami.Action audioCallAction: Kirigami.Action {
        iconName: "call-start"
        text: i18n("Audio call")
        enabled: availabilityTracker.canCall
        onTriggered: {
            showCallPage()
            callpage.visible = true
            audioCall()
        }
    }
}
