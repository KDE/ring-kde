
import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.6 as Kirigami
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import org.kde.ringkde.jamicontactview 1.0 as JamiContactView

Kirigami.GlobalDrawer {
    id: globalDrawer
    actions: [
        actionCollection.newContact,
        actionCollection.editAction,
    ]

    topContent: ListView {
        y: 5
        id: availableAccounts
        model: RingSession.profileModel.availableProfileModel
        height: 200
        spacing: 5
        interactive: false
        Layout.preferredWidth: globalDrawer.width
        Layout.preferredHeight: 200
        delegate: MouseArea {
            width: parent.width
            height: Kirigami.Units.iconSizes.large * 1.5
            Rectangle {
                anchors.fill: parent
                color: "#44000000"//Kirigami.Theme.backgroundColor

                RowLayout {
                    width: parent.width
                    height: Kirigami.Units.iconSizes.large
                    anchors.verticalCenter: parent.verticalCenter
                    JamiContactView.ContactPhoto {
                        width: Kirigami.Units.iconSizes.large
                        height: Kirigami.Units.iconSizes.large
                        individual: object
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Kirigami.Heading {
                            level: 2
                            text: object.bestName
                            Layout.fillWidth: true
                        }
                        Kirigami.Heading {
                            level: 3
                            color: Kirigami.Theme.positiveTextColor
                            text: "online"
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }
                    }
                }
            }
            onClicked: {
                var component = Qt.createComponent("qrc:/account/qml/presenceselector.qml")
                if (component.status == Component.Ready) {
                    var window = component.createObject(applicationWindow().contentItem)
                    window.individual = object
                    globalDrawer.drawerOpen = false
                    window.open()
                }
                else
                    console.log("ERROR", component.status, component.errorString())
            }
        }
    }
}
