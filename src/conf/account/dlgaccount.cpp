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
#include <profilemodel.h>
#include <account.h>
#include <protocolmodel.h>

#include "accountpages/account.h"

#include <delegates/righticondelegate.h>
#include <delegates/categorizeddelegate.h>

#include <model/extendedprotocolmodel.h>

#include <QtGui/QPainter>
#include <QtWidgets/QMessageBox>

#include <QtCore/QAbstractProxyModel>


DlgAccount::DlgAccount(QWidget* parent) : QWidget(parent),m_HasChanged(false)
{
   setupUi(this);
   m_pAccountList->setModel         ( ProfileModel::instance().sortedProxyModel         () );
   m_pAccountList->setSelectionModel( ProfileModel::instance().sortedProxySelectionModel() );

   CategorizedDelegate* delegate = new CategorizedDelegate(m_pAccountList);
   delegate->setChildDelegate(new RightIconDelegate(this, (int)Account::Role::SecurityLevelIcon, 0.2f));

   m_pAccountList->setItemDelegate(delegate);

   m_pProtocolModel = new ExtendedProtocolModel(this);

   m_pGlobalProto->bindToModel(m_pProtocolModel, m_pProtocolModel->selectionModel());

//    setCurrentAccount(nullptr);

   connect(m_pMoveUp  , &QToolButton::clicked,&AccountModel::instance(), &AccountModel::moveUp  );
   connect(m_pMoveDown, &QToolButton::clicked,&AccountModel::instance(), &AccountModel::moveDown);
   connect(m_pRemove  , &QToolButton::clicked, this, &DlgAccount::slotRemoveAccount );

   connect(&AccountModel::instance(), &AccountModel::editStateChanged, this, &DlgAccount::slotUpdateButtons);

   const QModelIndex idx = m_pAccountList->model()->index(0,0,m_pAccountList->model()->index(0,0));
   m_pAccountList->expandAll();

   connect(&ProfileModel::instance(), &ProfileModel::rowsInserted, this, &DlgAccount::slotExpand);

   connect(m_pPanel, &Pages::Account::changed, [this]() {
      m_HasChanged = true;
      updateButtons();
   });

   m_pAccountList->selectionModel()->setCurrentIndex(idx, QItemSelectionModel::ClearAndSelect);

   slotSetAccount(m_pAccountList->selectionModel()->currentIndex());

   connect(m_pAccountList->selectionModel(), &QItemSelectionModel::currentChanged, this, &DlgAccount::slotSetAccount);
}

DlgAccount::~DlgAccount()
{
}

void DlgAccount::slotSetAccount(const QModelIndex& idx)
{
   m_pPanel->setAccount(idx);
}

void DlgAccount::slotExpand()
{
   m_pAccountList->expandAll();
}

void DlgAccount::slotRemoveAccount()
{
   static const QString message = tr("Are you sure you want to remove %1?");
   // Check if the selectied element is a profile
   QModelIndex idx = ProfileModel::instance().sortedProxySelectionModel()->currentIndex();
   if (idx.isValid() && !idx.parent().isValid()) {
      if (QMessageBox::warning(this, tr("Remove profile"),
        message.arg(idx.data(Qt::DisplayRole).toString()),
        QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes
      )
         if (ProfileModel::instance().remove(idx)) {
            m_pAccountList->selectionModel()->setCurrentIndex(
               m_pAccountList->model()->index(0,0), QItemSelectionModel::ClearAndSelect
            );
         }
   }
   else {
   // Remove an account
      idx = AccountModel::instance().selectionModel()->currentIndex();

      if (!idx.isValid())
         return;

      if (QMessageBox::warning(this, tr("Remove account"),
         message.arg(idx.data(Qt::DisplayRole).toString()),
         QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes
      )
         AccountModel::instance().remove(idx);
   }
}

void DlgAccount::slotUpdateButtons()
{
   emit updateButtons();
}

void DlgAccount::slotNewAddAccount()
{
   const Account::Protocol proto = qvariant_cast<Account::Protocol>(
      m_pProtocolModel->data(m_pProtocolModel->selectionModel()->currentIndex(),Qt::UserRole)
   );

   //Add profile
   if (proto == Account::Protocol::COUNT__) {
      const QModelIndex idx = ProfileModel::instance().add();

      if (idx.isValid()) {
         m_pAccountList->selectionModel()->setCurrentIndex(idx, QItemSelectionModel::ClearAndSelect);
      }

      return;
   }

   const QString newAlias = tr("New account%1").arg(AccountModel::getSimilarAliasIndex(tr("New account")));
   Account* a = AccountModel::instance().add(newAlias,proto);

   QModelIndex accIdx = ProfileModel::instance().mapFromSource(a->index());
   accIdx = static_cast<QAbstractProxyModel*>(
      ProfileModel::instance().sortedProxyModel()
   )->mapFromSource(accIdx);

   ProfileModel::instance().sortedProxySelectionModel()->setCurrentIndex(
      accIdx, QItemSelectionModel::ClearAndSelect
   );

   m_pPanel->setAccount(a);

   m_pPanel->selectAlias();

} //on_button_accountAdd_clicked

void DlgAccount::cancel()
{
   for (int i=0; i < AccountModel::instance().size(); i++) {
      Account* a = AccountModel::instance()[i];

      switch(a->editState()) {
         case Account::EditState::MODIFIED_INCOMPLETE:
         case Account::EditState::MODIFIED_COMPLETE  :
         case Account::EditState::NEW                :
            a << Account::EditAction::CANCEL;
            break;
         case Account::EditState::OUTDATED           :
            a << Account::EditAction::RELOAD;
            break;
         case Account::EditState::READY              :
         case Account::EditState::REMOVED            :
         case Account::EditState::EDITING            :
         case Account::EditState::COUNT__            :
            break;
      }
   }
}

bool DlgAccount::hasChanged()
{
   return m_HasChanged || AccountModel::instance().editState() != AccountModel::EditState::SAVED;
}

void DlgAccount::updateSettings()
{
   m_pPanel->updateSettings();
   AccountModel::instance().save();
   m_HasChanged = false;
}

void DlgAccount::updateWidgets()
{
   m_HasChanged = false;
}

#include "dlgaccount.moc"
