/***************************************************************************
 *   Copyright (C) 2017 by Bluesystems                                     *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/
import QtQuick 2.4
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Item {

    property alias background: background
    property alias whatsNew: swipeView

    width: 400
    height: 400

    Rectangle {
        id: background
        anchors.fill: parent
        color: "#0886a0"
        x: 51
        y: 40
        width: 200
        height: 200

        ColumnLayout {
            id: columnLayout
            anchors.fill: parent

            RowLayout {
                id: rowLayout
                clip: true
                Layout.maximumHeight: 500
                Layout.fillWidth: true

                Image {
                    id: image1
                    height: 200

                    Layout.maximumHeight: 200
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter

                    fillMode: Image.PreserveAspectFit
                    source: "beta_logo.svg"

                    Layout.fillHeight: true
                    Layout.fillWidth: true

                }

                Image {
                    id: image
                    fillMode: Image.PreserveAspectFit
                    height: width<parent.minHeight ? parent.minHeight : ( width>parent.maxHeight ? parent.maxHeight : width )
                    width: (parent.rowWidth) < parent.minWidth ? parent.minWidth : ( parent.rowWidth > parent.maxWidth ? parent.maxWidth : parent.rowWidth)

                    source: "people.svg"
                    Layout.fillHeight: true
                    Layout.fillWidth: false
                    Layout.maximumHeight: 300
                    Layout.maximumWidth: 300
                    Layout.preferredHeight: parent.height
                    Layout.preferredWidth: parent.height
                    anchors.verticalCenter: parent.verticalCenter

                }
            }

            Item {
                height: 40
            }

            SwipeView {
                id: swipeView
                clip: true
                currentIndex: tabBar.currentIndex
                Layout.fillHeight: true
                Layout.fillWidth: true


                Label {
                    wrapMode: Text.WordWrap
                    clip: true
                    text: qsTr("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, ultricies nec, pellentesque eu, pretium quis, sem. Nulla consequat massa quis enim. Donec pede justo, fringilla vel, aliquet nec, vulputate eget, arcu. In enim justo, rhoncus ut, imperdiet a, venenatis vitae, justo. Nullam dictum felis eu pede mollis pretium. Integer tincidunt. Cras dapibus. Vivamus elementum semper nisi. Aenean vulputate eleifend tellus. Aenean leo ligula, porttitor eu, consequat vitae, eleifend ac, enim. Aliquam lorem ante, dapibus in, viverra quis, feugiat a, tellus. Phasellus viverra nulla ut metus varius laoreet. Quisque rutrum. Aenean imperdiet. Etiam ultricies nisi vel augue. Curabitur ullamcorper ultricies nisi. Nam eget dui. Etiam rhoncus. Maecenas tempus, tellus eget condimentum rhoncus, sem quam semper libero, sit amet adipiscing sem neque sed ipsum. Nam quam nunc, blandit vel, luctus pulvinar, hendrerit id, lorem. Maecenas nec odio et ante tincidunt tempus. Donec vitae sapien ut libero venenatis faucibus. Nullam quis ante. Etiam sit amet orci eget eros faucibus tincidunt. Duis leo. Sed fringilla mauris sit amet nibh. Donec sodales sagittis magna. Sed consequat, leo eget bibendum sodales, augue velit cursus nunc,")
                    width:parent.parent.width
                }

                Label {
                    wrapMode: Text.WordWrap
                    clip: true
                    text: qsTr("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, ultricies nec, pellentesque eu, pretium quis, sem. Nulla consequat massa quis enim. Donec pede justo, fringilla vel, aliquet nec, vulputate eget, arcu. In enim justo, rhoncus ut, imperdiet a, venenatis vitae, justo. Nullam dictum felis eu pede mollis pretium. Integer tincidunt. Cras dapibus. Vivamus elementum semper nisi. Aenean vulputate eleifend tellus. Aenean leo ligula, porttitor eu, consequat vitae, eleifend ac, enim. Aliquam lorem ante, dapibus in, viverra quis, feugiat a, tellus. Phasellus viverra nulla ut metus varius laoreet. Quisque rutrum. Aenean imperdiet. Etiam ultricies nisi vel augue. Curabitur ullamcorper ultricies nisi. Nam eget dui. Etiam rhoncus. Maecenas tempus, tellus eget condimentum rhoncus, sem quam semper libero, sit amet adipiscing sem neque sed ipsum. Nam quam nunc, blandit vel, luctus pulvinar, hendrerit id, lorem. Maecenas nec odio et ante tincidunt tempus. Donec vitae sapien ut libero venenatis faucibus. Nullam quis ante. Etiam sit amet orci eget eros faucibus tincidunt. Duis leo. Sed fringilla mauris sit amet nibh. Donec sodales sagittis magna. Sed consequat, leo eget bibendum sodales, augue velit cursus nunc,")
                    width:parent.parent.width
                }

                Label {
                    wrapMode: Text.WordWrap
                    clip: true
                    text: qsTr("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, ultricies nec, pellentesque eu, pretium quis, sem. Nulla consequat massa quis enim. Donec pede justo, fringilla vel, aliquet nec, vulputate eget, arcu. In enim justo, rhoncus ut, imperdiet a, venenatis vitae, justo. Nullam dictum felis eu pede mollis pretium. Integer tincidunt. Cras dapibus. Vivamus elementum semper nisi. Aenean vulputate eleifend tellus. Aenean leo ligula, porttitor eu, consequat vitae, eleifend ac, enim. Aliquam lorem ante, dapibus in, viverra quis, feugiat a, tellus. Phasellus viverra nulla ut metus varius laoreet. Quisque rutrum. Aenean imperdiet. Etiam ultricies nisi vel augue. Curabitur ullamcorper ultricies nisi. Nam eget dui. Etiam rhoncus. Maecenas tempus, tellus eget condimentum rhoncus, sem quam semper libero, sit amet adipiscing sem neque sed ipsum. Nam quam nunc, blandit vel, luctus pulvinar, hendrerit id, lorem. Maecenas nec odio et ante tincidunt tempus. Donec vitae sapien ut libero venenatis faucibus. Nullam quis ante. Etiam sit amet orci eget eros faucibus tincidunt. Duis leo. Sed fringilla mauris sit amet nibh. Donec sodales sagittis magna. Sed consequat, leo eget bibendum sodales, augue velit cursus nunc,")
                    width:parent.parent.width
                }

                Label {
                    wrapMode: Text.WordWrap
                    clip: true
                    text: qsTr("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, ultricies nec, pellentesque eu, pretium quis, sem. Nulla consequat massa quis enim. Donec pede justo, fringilla vel, aliquet nec, vulputate eget, arcu. In enim justo, rhoncus ut, imperdiet a, venenatis vitae, justo. Nullam dictum felis eu pede mollis pretium. Integer tincidunt. Cras dapibus. Vivamus elementum semper nisi. Aenean vulputate eleifend tellus. Aenean leo ligula, porttitor eu, consequat vitae, eleifend ac, enim. Aliquam lorem ante, dapibus in, viverra quis, feugiat a, tellus. Phasellus viverra nulla ut metus varius laoreet. Quisque rutrum. Aenean imperdiet. Etiam ultricies nisi vel augue. Curabitur ullamcorper ultricies nisi. Nam eget dui. Etiam rhoncus. Maecenas tempus, tellus eget condimentum rhoncus, sem quam semper libero, sit amet adipiscing sem neque sed ipsum. Nam quam nunc, blandit vel, luctus pulvinar, hendrerit id, lorem. Maecenas nec odio et ante tincidunt tempus. Donec vitae sapien ut libero venenatis faucibus. Nullam quis ante. Etiam sit amet orci eget eros faucibus tincidunt. Duis leo. Sed fringilla mauris sit amet nibh. Donec sodales sagittis magna. Sed consequat, leo eget bibendum sodales, augue velit cursus nunc,")
                    width:parent.parent.width
                }
            }

            TabBar {
                id: tabBar
                clip: true
                currentIndex: swipeView.currentIndex
                anchors.horizontalCenter: parent.horizontalCenter
                TabButton {
                    contentItem: Item {}

                    background: Rectangle {
                        color: "blue"
                        implicitWidth: 20
                        implicitHeight: 20
                        radius: 10
                    }
                }
                TabButton {
                    contentItem: Item {}

                    background: Rectangle {
                        color: "blue"
                        implicitWidth: 20
                        implicitHeight: 20
                        radius: 10
                    }
                }
                TabButton {
                    contentItem: Item {}

                    background: Rectangle {
                        color: "blue"
                        implicitWidth: 20
                        implicitHeight: 20
                        radius: 10
                    }
                }
                TabButton {
                    contentItem: Item {}

                    background: Rectangle {
                        color: "blue"
                        implicitWidth: 20
                        implicitHeight: 20
                        radius: 10
                    }
                }
                background: Item {}
            }

            Label {
                id: label
                color: "white"
                text: qsTr("Press anywhere to begin")
                font.pointSize: 12
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
                font.bold: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }
        }
    }
}
