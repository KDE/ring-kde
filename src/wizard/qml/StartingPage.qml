import QtQuick 2.4

StartingPageForm {
    id: startingPage

    /*Behavior on y {
        NumberAnimation {
            easing.type: Easing.InQuad
            duration: 500
            onRunningChanged: {
                if (!running){
                    visible = false
                }
            }
        }
    }*/

    MouseArea {
        anchors.fill: parent
        onClicked: {
            x = 0
            y = 0
            width = parent.width
            height = parent.height
            parent.visible = false /*ANIM remove*/
            startingPage.anchors.fill = undefined
            startingPage.state = "hidden"
        }
    }

    Timer {
        id: createAccountTimer
        interval: 2000
        repeat: true
        running: true
        onTriggered: {
            whatsNew.currentIndex = whatsNew.count-1 == whatsNew.currentIndex ?
                        0 : whatsNew.currentIndex+1
        }
    }

    states: [
        State {
            name: "hidden"
            PropertyChanges {
                target: startingPage
                y: -startingPage.height
            }
        }
    ]
}
