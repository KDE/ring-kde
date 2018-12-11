/***************************************************************************
*   Copyright (C) 2018 by Bluesystems                                     *
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
import QtQuick 2.8
import org.kde.kirigami 2.5 as Kirigami
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import org.kde.playground.kquickview 1.0 as KQuickItemViews
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import "../" as AccountWidgets

/**
 * This page contains the most basic elements necessary to create a SIP or
 * Ring/Jami account.
 */
Page {
    ColumnLayout {
        anchors.fill: parent

        AccountWidgets.SecurityLevel {
            Layout.fillWidth: true
            level: securityLevel
        }

        GroupBox {
            title: i18n("Security related suggestions")
            Layout.fillWidth: true
            Layout.preferredHeight: 250
            ListView {
                id: securityIssues
                anchors.fill: parent
                model: securityEvaluationModel
                delegate: Kirigami.SwipeListItem {
                    id: swipeItem
                    width: parent.width
                    RowLayout {
                        width: parent.width
                        KQuickItemViews.DecorationAdapter {
                            pixmap: decoration
                            Layout.preferredWidth: swipeItem.implicitHeight
                            Layout.minimumWidth: swipeItem.implicitHeight
                        }
                        Label {
                            text: display
                            Layout.fillWidth: true
                        }
                    }
                }
            }
        }

        Kirigami.FormLayout {
            Layout.fillWidth: true

            CheckBox {
                Kirigami.FormData.label: i18n("Encr&amp;ypt media streams (SRTP)")
                KQuickItemViews.RoleBinder.modelRole: "srtpEnabled"                             //
                KQuickItemViews.RoleBinder.objectProperty: "checked"
                RingQtQuick.FieldStatus.name: "srtpEnabled"
                visible: RingQtQuick.FieldStatus.available
            }

            CheckBox {
                Kirigami.FormData.label: i18n("Fallback on RTP on SDES failure")
                KQuickItemViews.RoleBinder.modelRole: "srtpRtpFallback"                         //<widget class="QCheckBox"
                KQuickItemViews.RoleBinder.objectProperty: "checked"
                RingQtQuick.FieldStatus.name: "srtpRtpFallback"
                visible: RingQtQuick.FieldStatus.available
            }

            CheckBox {
                Kirigami.FormData.label: i18n("Encrypt negotiation (&amp;TLS)")
                KQuickItemViews.RoleBinder.modelRole: "tlsEnabled"                              //<widget class="QGroupBox"
                KQuickItemViews.RoleBinder.objectProperty: "checked"
                RingQtQuick.FieldStatus.name: "tlsEnabled"
                visible: RingQtQuick.FieldStatus.available
            }

            CheckBox {
                Kirigami.FormData.label: i18n("")
                KQuickItemViews.RoleBinder.modelRole: "tlsVerifyServer"                         //<widget class="QCheckBox"
                KQuickItemViews.RoleBinder.objectProperty: "checked"
                RingQtQuick.FieldStatus.name: "tlsVerifyServer"
                visible: RingQtQuick.FieldStatus.available
            }

            TextField {
                Kirigami.FormData.label: i18n("")
                KQuickItemViews.RoleBinder.modelRole: "tlsPassword"                             //<widget class="QLineEdit"
                KQuickItemViews.RoleBinder.objectProperty: "text"
                RingQtQuick.FieldStatus.name: "tlsPassword"
                visible: RingQtQuick.FieldStatus.available
            }

            TextField {
                Kirigami.FormData.label: i18n("")
                KQuickItemViews.RoleBinder.modelRole: "tlsServerName"                           //<widget class="QLineEdit"
                KQuickItemViews.RoleBinder.objectProperty: "text"
                RingQtQuick.FieldStatus.name: "tlsServerName"
                visible: RingQtQuick.FieldStatus.available
            }

            CheckBox {
                Kirigami.FormData.label: i18n("Require a certificate for incoming TLS connections")
                KQuickItemViews.RoleBinder.modelRole: "tlsRequireClientCertificate"             //<widget class="QCheckBox"
                KQuickItemViews.RoleBinder.objectProperty: "checked"
                RingQtQuick.FieldStatus.name: "tlsRequireClientCertificate"
                visible: RingQtQuick.FieldStatus.available
            }

            SpinBox {
                Kirigami.FormData.label: i18n("Negotiation timeout")
                KQuickItemViews.RoleBinder.objectProperty: "value"
                KQuickItemViews.RoleBinder.modelRole: "tlsNegotiationTimeoutSec"                //<widget class="QSpinBox"
                RingQtQuick.FieldStatus.name: "tlsNegotiationTimeoutSec"
                visible: RingQtQuick.FieldStatus.available
            }

            CheckBox {
                Kirigami.FormData.label: i18n("Verify incoming certificates (server side)")
                KQuickItemViews.RoleBinder.modelRole: "tlsVerifyClient"                         //<widget class="QCheckBox"
                RingQtQuick.FieldStatus.name: "tlsVerifyClient"
                visible: RingQtQuick.FieldStatus.available
                KQuickItemViews.RoleBinder.objectProperty: "checked"
            }

            TextField {
                Kirigami.FormData.label: i18n("")
                KQuickItemViews.RoleBinder.modelRole: "tlsCaListCertificate"   //<widget class="FileSelector"
                RingQtQuick.FieldStatus.name: "tlsCaListCertificate"
                visible: RingQtQuick.FieldStatus.available
                KQuickItemViews.RoleBinder.objectProperty: "text"
            }

            TextField {
                Kirigami.FormData.label: i18n("")
                KQuickItemViews.RoleBinder.modelRole: "tlsCertificate"       //<widget class="FileSelector"
                RingQtQuick.FieldStatus.name: "tlsCertificate"
                visible: RingQtQuick.FieldStatus.available
                KQuickItemViews.RoleBinder.objectProperty: "text"
            }

            TextField {
                Kirigami.FormData.label: i18n("")
                KQuickItemViews.RoleBinder.modelRole: "tlsPrivateKey"       //<widget class="FileSelector"
                RingQtQuick.FieldStatus.name: "tlsPrivateKey"
                visible: RingQtQuick.FieldStatus.available
                KQuickItemViews.RoleBinder.objectProperty: "text"
            }

//                 KQuickItemViews.RoleBinder.modelRole: "allowedCertificatesModel"         //<widget class="ContextListView"
//                 KQuickItemViews.RoleBinder.modelRole: "bannedCertificatesModel"          //<widget class="ContextListView"
//                 KQuickItemViews.RoleBinder.modelRole: "knownCertificateModel"            //<widget class="ContextListView"

                //KQuickItemViews.RoleBinder.modelRole: "alias"
                //KQuickItemViews.RoleBinder.objectProperty: "text"
                //Kirigami.FormData.label: i18n("Alias")

        }

        Item {
            Layout.fillHeight: true
        }
    }
}
