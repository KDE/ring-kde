import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Ring 1.0

Item {

    property var currentContactMethod: null

    onCurrentContactMethodChanged: {
        contactHeader.currentContactMethod = currentContactMethod
        contactInfo.currentContactMethod = currentContactMethod
    }

    RowLayout {
        anchors.topMargin: 5
        anchors.rightMargin: 5
        anchors.top: parent.top
        anchors.right: parent.right
        z: 100

        TextField {
            id: textField1
            placeholderText: qsTr("search box")
        }

        Button {
            id: button1
            text: qsTr("Search")
        }
    }

    ColumnLayout {
        anchors.fill: parent

        ContactHeader {
            id: contactHeader
        }

        ContactInfo {
            id: contactInfo
        }
    }
}
