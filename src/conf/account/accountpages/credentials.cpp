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
#include "credentials.h"

#include <account.h>
#include <credentialmodel.h>

Pages::Credentials::Credentials(QWidget *parent) : PageBase(parent)
{
   setupUi(this);

   connect(this,&PageBase::accountSet,[this]() {
      m_pCredentials->setModel(account()->credentialModel());
      loadInfo();
   });

   connect(m_pCredentials, &QListView::clicked, this, &Pages::Credentials::loadInfo);

   connect(button_add_credential, &QToolButton::clicked,[this]() {
      m_pCredentials->setCurrentIndex(account()->credentialModel()->addCredentials());
      loadInfo();
   });

   connect(button_remove_credential, &QToolButton::clicked,[this]() {
      account()->credentialModel()->removeCredentials(m_pCredentials->currentIndex());
      m_pCredentials->setCurrentIndex(account()->credentialModel()->index(0,0));
      loadInfo();
   });

   connect(edit_credential_realm_2, &QLineEdit::textChanged,[this](const QString& text) {
      const QModelIndex current = m_pCredentials->selectionModel()->currentIndex();
      account()->credentialModel()->setData(current,text, CredentialModel::Role::REALM);
   });

   connect(edit_credential_auth_2, &QLineEdit::textChanged,[this](const QString& text) {
      const QModelIndex current = m_pCredentials->selectionModel()->currentIndex();
      account()->credentialModel()->setData(current,text, CredentialModel::Role::NAME);
   });

   connect(edit_credential_password_2, &QLineEdit::textChanged,[this](const QString& text) {
      const QModelIndex current = m_pCredentials->selectionModel()->currentIndex();
      account()->credentialModel()->setData(current,text, CredentialModel::Role::PASSWORD);
   });
}


void Pages::Credentials::loadInfo()
{
   const QModelIndex idx = m_pCredentials->selectionModel()->currentIndex();
   if (idx.isValid()) {
      edit_credential_password_2->blockSignals(true);
      edit_credential_auth_2->blockSignals(true);
      edit_credential_realm_2->blockSignals(true);
      edit_credential_realm_2->setText       ( m_pCredentials->model()->data(idx,CredentialModel::Role::REALM)    .toString());
      edit_credential_auth_2->setText        ( m_pCredentials->model()->data(idx,CredentialModel::Role::NAME)     .toString());
      edit_credential_password_2->setText    ( m_pCredentials->model()->data(idx,CredentialModel::Role::PASSWORD) .toString());
      edit_credential_realm_2->blockSignals(false);
      edit_credential_auth_2->blockSignals(false);
      edit_credential_password_2->blockSignals(false);
   }
   else if (account()->credentialModel()->rowCount()) {
      m_pCredentials->setCurrentIndex(account()->credentialModel()->index(0,0));
      loadInfo();
   }
}