import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Ring 1.0

Item {
    id: scrollbar

    Rectangle {
        radius: 99
        color: "black"
        width: parent.width
        height: 65
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            timelineOverlay.height = height
            timelineOverlay.visible = true
        }
        onExited:  timelineOverlay.visible = false

        Rectangle {
            id: timelineOverlay
            color: "orange"
            width: 100
            x: -100
            height: scrollbar.height
            visible: false
        }
    }
}
