import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Ring 1.0

Item {

    property var currentContactMethod: null

    function getLastContacted() {
        var d = currentContactMethod.lastUsed == 0 ? qsTr("Never") :
            new Date(currentContactMethod.lastUsed * 1000).toLocaleString()

        return qsTr("<b>Last contacted on:</b> ") + d + "[?]"
    }

    function getTotalCall() {
        return qsTr("<b>Called:</b> ") + currentContactMethod.callCount +
            " (" + (currentContactMethod.totalSpentTime/60) + " minutes) [?]"
    }

    function getTotalText() {
        return  qsTr("<b>Texted:</b> ") + "N/A time [?]"
    }

    function getTotalRecording() {
        return  qsTr("<b>Recordings:</b> ") + "N/A [?]"
    }

    function getTotalScreenshot() {
        return  qsTr("<b>Screenshots:</b> ") + "N/A [?]"
    }

    onCurrentContactMethodChanged: {

        // Header
        primaryName.text = currentContactMethod.primaryName

        // Stats
        lastContactedTime.text = getLastContacted()
        totalCall.text         = getTotalCall()
        totalText.text         = getTotalText()
        totalRecording.text    = getTotalRecording()
        totalScreenshot.text   = getTotalScreenshot()

        // Contact info
        formattedName.text = currentContactMethod.person ?
            currentContactMethod.person.formattedName : ""
        firstName.text = currentContactMethod.person ?
            currentContactMethod.person.firstName : ""
        lastName.text = currentContactMethod.person ?
            currentContactMethod.person.secondName : ""
        email.text = currentContactMethod.person ?
            currentContactMethod.person.preferredEmail : ""
        organization.text = currentContactMethod.person ?
            currentContactMethod.person.organization : ""
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

        Rectangle {
            color: "gray"
            height: 100
            Layout.fillWidth: true

            RowLayout {
                anchors.fill: parent
                anchors.margins: 8

                Rectangle {
                    radius: 5
                    height: 90
                    width: 90
                    color: "white"
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Text {
                        id: primaryName
                        font.bold: true
                        font.pointSize: 16
                        text: "My name"
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Button {
                            id: button
                            text: qsTr("Call")
                            onClicked: {
                                if (currentContactMethod == null) return
                                CallModel.dialingCall(currentContactMethod)
                                    .performAction(Call.ACCEPT)
                            }
                        }

                        Button {
                            id: button2
                            text: qsTr("Video")
                            onClicked: {
                                if (currentContactMethod == null) return
                                CallModel.dialingCall(currentContactMethod)
                                    .performAction(Call.ACCEPT)
                            }
                        }

                        Button {
                            text: qsTr("Screen sharing")
                            onClicked: {
                                if (currentContactMethod == null) return
                                CallModel.dialingCall(currentContactMethod)
                                    .performAction(Call.ACCEPT)
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            color: "black"
            height: 1
            Layout.fillWidth: true
        }

        Label {
            id: lastContactedTime
        }

        Label {
            id: totalCall
        }

        Label {
            id: totalText
        }

        Label {
            id: totalRecording
        }

        Label {
            id: totalScreenshot
        }

        Rectangle {
            color: "black"
            height: 1
            Layout.fillWidth: true
        }

        GridLayout {
            columns: 2
            rowSpacing: 10
            columnSpacing: 10
            Layout.fillWidth: true

            Label {
                id: label
                text: qsTr("Formatted name:")
            }
            TextField {
                id: formattedName
            }

            Label {
                text: qsTr("Primary name:")
            }
            TextField {
                id: firstName
            }

            Label {
                text: qsTr("Last name:")
            }
            TextField {
                id: lastName
            }

            Label {
                text: qsTr("Email:")
            }
            TextField {
                id: email
            }

            Label {
                text: qsTr("Organization:")
            }
            TextField {
                id: organization
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
