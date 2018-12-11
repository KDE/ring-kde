/***************************************************************************
 *   Copyright (C) 2014-2015 by Savoir-Faire Linux                         *
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
#include "credentials.h"

// Qt
#include <QQmlEngine>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWidget>
#include <QQmlError>

// LibRingQt
#include <account.h>
#include <credentialmodel.h>

#include <delegates/categorizeddelegate.h>
#include <ringapplication.h>

Pages::Credentials::Credentials(QWidget *parent) : PageBase(parent)
{
   setupUi(this);
   m_pDelegate = new CategorizedDelegate(m_pCredentials);
   m_pChildDelegate = new QStyledItemDelegate();
   m_pDelegate->setChildDelegate(m_pChildDelegate);
   m_pCredentials->setItemDelegate(m_pDelegate);

   connect(this,&PageBase::accountSet, this, &Pages::Credentials::slotSetAccount);

   connect(m_pCredentials             , &QTreeView::clicked    , this, &Pages::Credentials::loadInfo             );
   connect(button_add_credential      , &QToolButton::clicked  , this, &Pages::Credentials::slotAddCredential    );
   connect(button_remove_credential   , &QToolButton::clicked  , this, &Pages::Credentials::slotRemoveCredential );
   connect(edit_credential_realm_2    , &QLineEdit::textChanged, this, &Pages::Credentials::slotRealmChanged     );
   connect(edit_credential_auth_2     , &QLineEdit::textChanged, this, &Pages::Credentials::slotUserChanged      );
   connect(edit_credential_password_2 , &QLineEdit::textChanged, this, &Pages::Credentials::slotPasswdChanged    );
}

Pages::Credentials::~Credentials()
{
    m_pDelegate->setChildDelegate(nullptr);
    delete m_pChildDelegate;
    delete m_pDelegate;
}

void Pages::Credentials::slotSetAccount()
{
   m_pCredentials->setModel(account()->credentialModel());
   loadInfo();
   m_pType->bindToModel(account()->credentialModel()->availableTypeModel(),account()->credentialModel()->availableTypeSelectionModel());

   connect(account()->credentialModel(), &QAbstractItemModel::rowsInserted, m_pCredentials, &QTreeView::expandAll);

   m_pCredentials->expandAll();
}

void Pages::Credentials::slotAddCredential()
{
   m_pCredentials->setCurrentIndex(account()->credentialModel()->addCredentials());
   loadInfo();
}

void Pages::Credentials::slotRemoveCredential()
{
   account()->credentialModel()->removeCredentials(m_pCredentials->currentIndex());
   m_pCredentials->setCurrentIndex(account()->credentialModel()->index(0,0));
   loadInfo();
}

void Pages::Credentials::slotRealmChanged(const QString& text)
{
   const QModelIndex current = m_pCredentials->selectionModel()->currentIndex();
   account()->credentialModel()->setData(current,text, CredentialModel::Role::REALM);
}

void Pages::Credentials::slotUserChanged(const QString& text)
{
   const QModelIndex current = m_pCredentials->selectionModel()->currentIndex();
   account()->credentialModel()->setData(current,text, CredentialModel::Role::NAME);
}

void Pages::Credentials::slotPasswdChanged(const QString& text)
{
   const QModelIndex current = m_pCredentials->selectionModel()->currentIndex();
   account()->credentialModel()->setData(current,text, CredentialModel::Role::PASSWORD);
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

void Pages::Credentials::setAccount(Account* a)
{
}

// kate: space-indent on; indent-width 3; replace-tabs on;
