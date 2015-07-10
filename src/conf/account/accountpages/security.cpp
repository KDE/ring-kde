/***************************************************************************
 *   Copyright (C) 2014-2015 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
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
#include "security.h"

#include <account.h>
#include <credentialmodel.h>

#include <ciphermodel.h>

#include <widgets/certificateviewer.h>

#define FAILED == Certificate::CheckValues::FAILED
Pages::Security::Security(QWidget *parent) : PageBase(parent)
{
   setupUi(this);
   frame->setVisible(false);
   frame_2->setVisible(false);

   label_3->setPixmap(QIcon::fromTheme("dialog-information").pixmap(QSize(22,22)));
   label_2->setPixmap(QIcon::fromTheme("dialog-information").pixmap(QSize(22,22)));

   connect(this,&PageBase::accountSet,[this]() {
      combo_security_STRP->bindToModel(account()->keyExchangeModel(),account()->keyExchangeModel()->selectionModel());
      combo_tls_method->bindToModel(account()->tlsMethodModel(),account()->tlsMethodModel()->selectionModel());
      m_pSecurityIssues->setModel(account()->securityEvaluationModel());

      //Display the permission fixer
      frame_2->setVisible(
         (account()->tlsCertificate() && (
            account()->tlsCertificate()->arePrivateKeyStoragePermissionOk          () FAILED ||
            account()->tlsCertificate()->arePublicKeyStoragePermissionOk           () FAILED ||
            account()->tlsCertificate()->arePrivateKeyDirectoryPermissionsOk       () FAILED ||
            account()->tlsCertificate()->arePublicKeyDirectoryPermissionsOk        () FAILED  )
         ) ||
         (account()->tlsCaListCertificate() && (
            account()->tlsCaListCertificate()->arePrivateKeyStoragePermissionOk    () FAILED ||
            account()->tlsCaListCertificate()->arePublicKeyStoragePermissionOk     () FAILED ||
            account()->tlsCaListCertificate()->arePrivateKeyDirectoryPermissionsOk () FAILED ||
            account()->tlsCaListCertificate()->arePublicKeyDirectoryPermissionsOk  () FAILED  )
         )
      );

      frame->setVisible(
         (account()->tlsCertificate() && (
            account()->tlsCertificate()->arePrivateKeyStorageLocationOk            () FAILED ||
            account()->tlsCertificate()->arePublicKeyStorageLocationOk             () FAILED ||
            account()->tlsCertificate()->arePrivateKeySelinuxAttributesOk          () FAILED ||
            account()->tlsCertificate()->arePublicKeySelinuxAttributesOk           () FAILED  )
         ) ||
         (account()->tlsCaListCertificate() && (
            account()->tlsCaListCertificate()->arePrivateKeyStorageLocationOk      () FAILED ||
            account()->tlsCaListCertificate()->arePublicKeyStorageLocationOk       () FAILED ||
            account()->tlsCaListCertificate()->arePrivateKeySelinuxAttributesOk    () FAILED ||
            account()->tlsCaListCertificate()->arePublicKeySelinuxAttributesOk     () FAILED  )
         )
      );

      tabWidget->setVisible(account()->protocol() == Account::Protocol::RING);

      const bool cipherVisible = account()->roleState(Account::Role::CipherModel) != Account::RoleState::UNAVAILABLE;
      m_pDefaultCiphers->setChecked( account()->cipherModel()->useDefault());
      m_pCipherModel->setModel(account()->cipherModel());
      m_pCipherModel->setVisible(cipherVisible && !account()->cipherModel()->useDefault());
      m_pDefaultCiphers->setVisible(cipherVisible);
   });

   connect(m_pViewCa, &QToolButton::clicked, [this]() {
      CertificateViewer* c = new CertificateViewer(account()->tlsCaListCertificate());
      c->show();
   });

   connect(m_pViewCert, &QToolButton::clicked, [this]() {
      CertificateViewer* c = new CertificateViewer(account()->tlsCertificate());
      c->show();
   });

   connect(m_pFixCertPB  , &QPushButton::clicked, [this]() {
      if (account()->tlsCertificate()) {
         if (!account()->tlsCertificate()->fixPermissions())
            qWarning() << "Changing" << account()->tlsCertificate()->path() << "permissions failed";
         frame_2->hide();
      }
   });

   connect(m_pMoveCertPB , &QPushButton::clicked, [this]() {
      if (account()->tlsCertificate()) {
         if (!account()->tlsCertificate()->moveToDotCert())
            qWarning() << "Moving" << account()->tlsCertificate()->path() << "failed";
         frame->hide();
      }
   });

   for (const QAbstractItemView* v : {lrcfg_knownCertificateModel, lrcfg_bannedCertificatesModel, lrcfg_allowedCertificatesModel}) {
      connect(v, &QAbstractItemView::doubleClicked,[this](const QModelIndex& idx) {
         CertificateViewer* c = new CertificateViewer(idx);
         c->show();
      });
   }
}
#undef FAILED
