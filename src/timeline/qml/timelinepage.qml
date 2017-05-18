import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Ring 1.0

import RingQmlWidgets 1.0

Item {
    property var currentContactMethod: null

    onCurrentContactMethodChanged: currentContactMethod ?
        chatView.model = currentContactMethod.textRecording.instantMessagingModel : null

    ColumnLayout {
        anchors.fill: parent
        clip: true

        RowLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true

            ChatView {
                id: chatView
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            TimelineScrollbar {
                id: scrollbar
                Layout.fillHeight: true
                Layout.preferredWidth: 20
            }
        }

        ChatBox {
            id: chatBox
            Layout.fillWidth: true
            height: 120
            MessageBuilder {id: builder}
        }
    }

    Connections {
        target: chatBox
        onSendMessage: {
            builder.addPayload("text/plain", message)
            builder.sendWidth(currentContactMethod)
        }
    }
}
