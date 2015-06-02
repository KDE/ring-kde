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

#include "dlgaccount.h"

//Ring
#include <accountmodel.h>
#include <account.h>
#include <protocolmodel.h>

#include "accountpages/account.h"

#include <QtGui/QPainter>

#include <KMessageBox>

DlgAccount::DlgAccount(QWidget* parent) : QWidget(parent), m_pCurrentAccount(nullptr)
{
   setupUi(this);

   m_pAccountList->setModel         ( AccountModel::instance()                   );
   m_pAccountList->setSelectionModel( AccountModel::instance()->selectionModel() );

   m_pProtocolModel = new ProtocolModel();

   m_pGlobalProto->bindToModel(m_pProtocolModel,m_pProtocolModel->selectionModel());

//    setCurrentAccount(nullptr);

   connect(m_pMoveUp  , &QToolButton::clicked,AccountModel::instance(), &AccountModel::moveUp  );
   connect(m_pMoveDown, &QToolButton::clicked,AccountModel::instance(), &AccountModel::moveDown);
   connect(m_pRemove  , &QToolButton::clicked,[this]() {
      const QModelIndex& idx = AccountModel::instance()->selectionModel()->currentIndex();
      if (KMessageBox::questionYesNo(
         this,
         tr("Are you sure you want to remove %1?").arg(idx.data(Qt::DisplayRole).toString()),
         tr("Remove account")
      ) == KMessageBox::Yes)
         AccountModel::instance()->remove(idx);
   });

   //TODO eventually remove this once it is plugged into KConfig
   connect(AccountModel::instance(), &AccountModel::editStateChanged, [this]() {
      emit updateButtons();
   });

}

DlgAccount::~DlgAccount()
{
}

void DlgAccount::setCurrentAccount(const QModelIndex& idx)
{
   setCurrentAccount(AccountModel::instance()->getAccountByModelIndex(idx));
}

void DlgAccount::setCurrentAccount(::Account* a)
{
   Pages::Account* acc = new Pages::Account(a,/*"ring/account789.ini",*/this);
   QHBoxLayout* l = new QHBoxLayout(m_pPanel);
   l->addWidget(acc);
   m_lPages["account456.ini"] = acc;

   m_pCurrentAccount = acc;
}

void DlgAccount::slotNewAddAccount()
{
   const QString newAlias = tr("New account%1").arg(AccountModel::getSimilarAliasIndex(tr("New account")));
   const Account::Protocol proto = qvariant_cast<Account::Protocol>(m_pProtocolModel->data(m_pProtocolModel->selectionModel()->currentIndex(),Qt::UserRole));
   Account* a = AccountModel::instance()->add(newAlias,proto);

   setCurrentAccount(a);

   m_pCurrentAccount->selectAlias();

} //on_button_accountAdd_clicked

void DlgAccount::cancel()
{

}

bool DlgAccount::hasChanged()
{
   return AccountModel::instance()->editState() != AccountModel::EditState::SAVED;
}

void DlgAccount::updateSettings()
{

}

void DlgAccount::updateWidgets()
{

}

#include "dlgaccount.moc"
