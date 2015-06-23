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

Pages::Security::Security(QWidget *parent) : PageBase(parent)
{
   setupUi(this);
   frame->setVisible(false);
   frame_2->setVisible(false);

   connect(this,&PageBase::accountSet,[this]() {
      combo_security_STRP->bindToModel(account()->keyExchangeModel(),account()->keyExchangeModel()->selectionModel());
      m_pCiphers->setModel(account()->cipherModel());
      combo_tls_method->bindToModel(account()->tlsMethodModel(),account()->tlsMethodModel()->selectionModel());
      m_pSecurityIssues->setModel(account()->securityEvaluationModel());
   });

   connect(m_pViewCa, &QToolButton::clicked, [this]() {
      CertificateViewer* c = new CertificateViewer(account()->tlsCaListCertificate());
      c->show();
   });

   connect(m_pViewCert, &QToolButton::clicked, [this]() {
      CertificateViewer* c = new CertificateViewer(account()->tlsCertificate());
      c->show();
   });
}
