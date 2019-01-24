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
import org.kde.playground.kquickitemviews 1.0 as KQuickItemViews
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import org.kde.ringkde.jamiaccountview 1.0 as JamiAccountView

/**
 * This page contains the most basic elements necessary to create a SIP or
 * Ring/Jami account.
 */
Page {
    ColumnLayout {
        anchors.fill: parent

        JamiAccountView.SecurityLevel {
            Layout.fillWidth: true
            level: securityLevel
            informationCount: object.securityEvaluationModel.informationCount
            warningCount: object.securityEvaluationModel.warningCount
            issueCount: object.securityEvaluationModel.issueCount
            errorCount: object.securityEvaluationModel.errorCount
            model: securityEvaluationModel
        }

        Kirigami.FormLayout {
            Layout.fillWidth: true

            Kirigami.Separator {
                Kirigami.FormData.label: i18n("TLS Certificates")
                Kirigami.FormData.isSection: true
            }

            TextField {
                Kirigami.FormData.label: i18n("Authority certificate(s)")
                KQuickItemViews.RoleBinder.modelRole: "tlsCaListCertificate"                             //<widget class="QLineEdit"
                KQuickItemViews.RoleBinder.objectProperty: "text"
                RingQtQuick.FieldStatus.name: "tlsCaListCertificate"
                visible: RingQtQuick.FieldStatus.available
            }

            TextField {
                Kirigami.FormData.label: i18n("Endpoint certificate")
                KQuickItemViews.RoleBinder.modelRole: "tlsCertificate"                             //<widget class="QLineEdit"
                KQuickItemViews.RoleBinder.objectProperty: "text"
                RingQtQuick.FieldStatus.name: "tlsCertificate"
                visible: RingQtQuick.FieldStatus.available
            }

            TextField {
                Kirigami.FormData.label: i18n("Private key")
                KQuickItemViews.RoleBinder.modelRole: "tlsPrivateKey"                             //<widget class="QLineEdit"
                KQuickItemViews.RoleBinder.objectProperty: "text"
                RingQtQuick.FieldStatus.name: "tlsPrivateKey"
                visible: RingQtQuick.FieldStatus.available
            }

            TextField {
                Kirigami.FormData.label: i18n("Password")
                KQuickItemViews.RoleBinder.modelRole: "tlsPassword"                             //<widget class="QLineEdit"
                KQuickItemViews.RoleBinder.objectProperty: "text"
                RingQtQuick.FieldStatus.name: "tlsPassword"
                visible: RingQtQuick.FieldStatus.available
            }

            JamiAccountView.SecurityTip {
                id: permTip
                visible: object.securityEvaluationModel.permissionWarning
                Kirigami.FormData.isSection: true
                message: i18n("Your certificates access are too permissive. Certificates should only be readable by the owner.")
                icon: "image://icon/dialog-warning"
                Button {
                    text: i18n("Fix the permissions")
                    onClicked: {
                        object.tlsCertificate.fixPermissions()
                    }
                }
                Button {
                    text: i18n("No thanks")
                    onClicked: {
                        permTip.visible = false
                    }
                }
            }

            JamiAccountView.SecurityTip {
                id: pathTip
                Kirigami.FormData.isSection: true
                visible: object.securityEvaluationModel.locationWarning
                message: i18n("A good security practice is to move the certificates to a common directory. SELinux recommends ~/.cert for this. Do you wish to move the certificate there?")
                icon: "image://icon/dialog-warning"
                Button {
                    text: i18n("Move to ~/.cert")
                    onClicked: {
                        object.tlsCertificate.moveToDotCert()
                    }
                }
                Button {
                    text: i18n("No thanks")
                    onClicked: {
                        pathTip.visible = false
                    }
                }
            }

            Kirigami.Separator {
                Kirigami.FormData.label: i18n("Faulty servers workarounds")
                Kirigami.FormData.isSection: true
            }

            CheckBox {
                Kirigami.FormData.label: i18n("Encrypt media streams (SRTP)")
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
                Kirigami.FormData.label: i18n("Encrypt negotiation (TLS)")
                KQuickItemViews.RoleBinder.modelRole: "tlsEnabled"                              //<widget class="QGroupBox"
                KQuickItemViews.RoleBinder.objectProperty: "checked"
                RingQtQuick.FieldStatus.name: "tlsEnabled"
                visible: RingQtQuick.FieldStatus.available
            }

            CheckBox {
                Kirigami.FormData.label: i18n("Verify Server")
                KQuickItemViews.RoleBinder.modelRole: "tlsVerifyServer"                         //<widget class="QCheckBox"
                KQuickItemViews.RoleBinder.objectProperty: "checked"
                RingQtQuick.FieldStatus.name: "tlsVerifyServer"
                visible: RingQtQuick.FieldStatus.available
            }

            TextField {
                Kirigami.FormData.label: i18n("Server Name")
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
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
