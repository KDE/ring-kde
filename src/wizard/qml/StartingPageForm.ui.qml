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
                Layout.fillHeight: true
                Layout.fillWidth: true

                RowLayout {
                    Image {
                        Layout.fillHeight: true
                        source: "image://icon/channel-secure-symbolic"
                        Layout.preferredWidth: 100
                        fillMode: Image.PreserveAspectFit
                    }

                    Label {
                        wrapMode: Text.WordWrap
                        clip: true
                        text: i18n("Ring-KDE allows you to communicate privately with your friends. It uses peer to peer technologies popularised by BitTorrent to create a decentralized network of users. There is no central server and all communications are end-to-end encrypted.")
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }

                RowLayout {
                    Image {
                        Layout.fillHeight: true
                        source: "image://icon/text-directory"
                        Layout.preferredWidth: 100
                        fillMode: Image.PreserveAspectFit
                    }

                    Label {
                        wrapMode: Text.WordWrap
                        clip: true
                        text: i18n("Ring-KDE stores all data locally using standard formats such as JSON, vCard, MIME, PCM/WAV, INI and X-509. No data is ever uploaded to external servers.")
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }

                RowLayout {
                    Image {
                        Layout.fillHeight: true
                        source: "image://icon/call-start"
                        Layout.preferredWidth: 100
                        fillMode: Image.PreserveAspectFit
                    }

                    Label {
                        wrapMode: Text.WordWrap
                        clip: true
                        text: i18n("Ring-KDE supports your office phone account. Answers, transfer and record your phone calls directly from KDE.")
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }

                RowLayout {
                    Image {
                        Layout.fillHeight: true
                        source: "image://icon/standard-connector"
                        Layout.preferredWidth: 100
                        fillMode: Image.PreserveAspectFit
                    }

                    Label {
                        wrapMode: Text.WordWrap
                        clip: true
                        text: i18n("Ring is based on open standards and it can interoperate with other SIP compliant systems.")
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }

                RowLayout {
                    Image {
                        Layout.fillHeight: true
                        source: "image://icon/text-x-python"
                        Layout.preferredWidth: 100
                        fillMode: Image.PreserveAspectFit
                    }

                    Label {
                        wrapMode: Text.WordWrap
                        clip: true
                        text: i18n("Ring offers an user friendly DBus API to automate all aspects of the application.")
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }

                RowLayout {
                    Image {
                        Layout.fillHeight: true
                        source: "image://icon/call-start"
                        Layout.preferredWidth: 100
                        fillMode: Image.PreserveAspectFit
                    }

                    Label {
                        wrapMode: Text.WordWrap
                        clip: true
                        text: i18n("Ring-KDE can call real phone numbers. Cheap credits are available from thousand of providers worldwide. Use your favorite search engine to locate a local SIP phone provider.")
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }
            }

            Label {
                id: label
                color: "white"
                text: i18n("Press anywhere to begin")
                font.pointSize: 12
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
                font.bold: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }
        }
    }
}
