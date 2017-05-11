import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Ring 1.0

Item {
    property var currentContactMethod: null

    onCurrentContactMethodChanged: currentContactMethod ?
        callsModels.model = currentContactMethod.callsModel : null

    function selectIcon(isMissed, direction) {
        if (isMissed && direction == Call.INCOMING)
            return "sharedassets/phone_dark/missed_incoming.svg"
        else if (isMissed && direction == Call.OUTGOING)
            return "sharedassets/phone_dark/missed_outgoing.svg"
        else if (direction == Call.INCOMING)
            return "sharedassets/phone_dark/incoming.svg"
        else
            return "sharedassets/phone_dark/outgoing.svg"
    }

    Component {
        id: historyDelegate
        Item {
            height: 30
            width: parent.width

            RowLayout {
                Image {
                    source: selectIcon(missed, direction)
                    Layout.fillHeight: true
                    width: 30
                }
                Column {
                    Text { text: formattedDate }
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Text {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 5
                text: length
            }
        }
    }

    ListView {
        id: callsModels
        anchors.fill: parent
        delegate: historyDelegate
    }
}
