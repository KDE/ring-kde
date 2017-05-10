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

    ColumnLayout {
        anchors.fill: parent

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

    Button {
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 5
        text: qsTr("Save")
        onClicked: {
            if ((!currentContactMethod) || (!currentContactMethod.person))
                return

            currentContactMethod.person.formattedName  = formattedName.text
            currentContactMethod.person.firstName      = firstName.text
            currentContactMethod.person.secondName     = lastName.text
            currentContactMethod.person.preferredEmail = email.text
            currentContactMethod.person.organization   = organization.text

            currentContactMethod.save()
        }
    }
}
