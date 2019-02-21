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
import QtQuick 2.2
import QtQuick.Layouts 1.4
import QtQuick.Controls 2.2 as Controls
import org.kde.kirigami 2.6 as Kirigami
import org.kde.ringkde.basicview 1.0 as BasicView
import org.kde.ringkde.jamichatview 1.0 as JamiChatView
import org.kde.ringkde.jamicallview 1.0 as JamiCallView
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import net.lvindustries.ringqtquick.media 1.0 as RingQtMedia

Kirigami.Page {
    spacing: 0
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0
    padding: 0
    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.ToolBar
    Kirigami.Theme.colorSet: Kirigami.Theme.View

    titleDelegate: BasicView.DesktopHeader {
        id: dheader
        Layout.fillWidth: true
    }

    /**
     * Get an ongoing call if it exists or request a dialing call to be created.
     */
    function getCall(cm) {
        return workflow.call && workflow.call.lifeCycleState != RingQtQuick.Call.FINISHED ?
            workflow.call : RingSession.callModel.dialingCall(cm)
    }

    /**
     * An individual can have multiple phone numbers or Ring/Jami accounts.
     *
     * Pick one.
     */
    function getDefaultCm() {
        if (workflow.currentContactMethod)
            return workflow.currentContactMethod

        if (workflow.currentIndividual)
            return workflow.currentIndividual.mainContactMethod

        return null
    }

    function callCommon(media) {
        if (!workflow.currentIndividual)
            return

        var cm = getDefaultCm()

        if (!cm)
            cm = workflow.currentIndividual.preferredContactMethod(media)

        if (!cm) {
            console.log("Failed to find a proper contact method for", workflow.currentIndividual)
            return
        }

        if (cm.hasInitCall) {
            workflow.showCall(cm.firstActiveCall)
            return
        }

        var call = getCall(cm)

        call.performAction(RingQtQuick.Call.ACCEPT)
    }

    function audioCall() {
        callCommon(RingQtQuick.Media.AUDIO)
    }

    function videoCall() {
        callCommon(RingQtQuick.Media.VIDEO)
    }

    function screencast() {
        callCommon(RingQtQuick.Media.VIDEO)
    }

    JamiCallView.CallView {
        id: callview
        anchors.fill: parent
        individual: workflow.currentIndividual
        mode: "CONVERSATION"
        call: workflow.call

        Connections {
            target: workflow
            onCallChanged: {
                callview.call = workflow.call
            }
        }

        onCallWithAudio: {
            var cm = getDefaultCm()

            if (!cm)
                return

            audioCall()
        }
        onCallWithVideo: {
            var cm = getDefaultCm()

            if (!cm)
                return

            videoCall()
        }
        onCallWithScreen: {
            var cm = getDefaultCm()

            if (!cm)
                return

            screencast()
        }
    }

    actions {
        main : actionCollection.chatAction
    }

    /**
     * Not worth it on mobile, they are the same as in the call toolbar.
     */
    contextualActions: Kirigami.Settings.isMobile ? [] : [
        ActionCollection.holdAction        ,
        ActionCollection.recordAction      ,
        ActionCollection.muteCaptureAction ,
        ActionCollection.mutePlaybackAction,
        ActionCollection.hangupAction      ,
        ActionCollection.transferAction    ,
        ActionCollection.acceptAction      ,
        ActionCollection.newCallAction
    ]
}
