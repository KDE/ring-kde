pragma Singleton
import QtQuick 2.7

QtObject {
    id: style

    property SystemPalette activePalette: new SystemPalette()

    property FontMetrics chatBubbleMetrics: new FontMetrics("Noto Color Emoji")


    function bubbleRect(text, maximum) {
//         if (!this.chatBubbleMetrics) {
//             this.chatBubbleMetrics = new FontMetrics
//         }

        console.log("\n\nSD",style, this.chatBubbleMetrics, this.activePalette)
        var rect = style.chatBubbleMetrics.boundingRect(text)

        if (rect.width > maximum*0.66) {
            var row = Math.ceil(rect.width/(maximum*0.66))
            rect.width   = maximum*0.66
            rect.height += row*chatBubbleMetrics.height
        }

        rect.width  = Math.max(style.dateWidth, rect.width)
        rect.height = Math.max(rect.height, 50)

        return rect
    }

    property var  bubbleBackground: blendColor()
    property var  bubbleForeground: ""
    property var  unreadBackground: ""
    property var  unreadForeground: ""
    property int  dateWidth: 0
    property date currentDate: new Date()

    function blendColor() {
        var base2 = activePalette.highlight
        base2     = Qt.rgba(base2.r, base2.g, base2.b, 0.3)
        var base1 = Qt.tint(activePalette.base, base2)

        chatView.bubbleBackground = base1
        chatView.unreadBackground = Qt.tint(activePalette.base, "#33BB0000")
        chatView.bubbleForeground = activePalette.text
        chatView.unreadForeground = activePalette.text

        return base1
    }

    Component.onCompleted: {
//         if (!this.chatBubbleMetrics) {
//             this.chatBubbleMetrics = new FontMetrics
//         }
        style.dateWidth = style.chatBubbleMetrics.boundingRect(currentDate.toLocaleDateString()) + 100
        chatBubbleMetrics.font.family = "Noto Color Emoji"
        activePalette.colorGroup = SystemPalette.Active
    }
}
