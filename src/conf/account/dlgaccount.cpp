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

#include "dlgaccount.h"

// Qt
#include <QtWidgets/QFileDialog>

// KDE
#include <KPasswordDialog>
#include <KLocalizedString>

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

   m_pCategoryDelegate = new CategorizedDelegate(m_pAccountList);
   m_pCategoryDelegate->setChildDelegate(new RightIconDelegate(this, (int)Account::Role::SecurityLevelIcon, 0.2f));

   m_pAccountList->setItemDelegate(m_pCategoryDelegate);

   m_pProtocolModel = new ExtendedProtocolModel(this);

   m_pGlobalProto->bindToModel(m_pProtocolModel, m_pProtocolModel->selectionModel());

//    setCurrentAccount(nullptr);

   connect(m_pMoveUp  , &QToolButton::clicked,&AccountModel::instance(), &AccountModel::moveUp  );
   connect(m_pMoveDown, &QToolButton::clicked,&AccountModel::instance(), &AccountModel::moveDown);
   connect(m_pRemove  , &QToolButton::clicked, this, &DlgAccount::slotRemoveAccount );

   connect(&AccountModel::instance(), &AccountModel::editStateChanged, this, &DlgAccount::slotUpdateButtons);

   connect(&ProfileModel::instance(), &ProfileModel::rowsInserted, this, &DlgAccount::slotExpand);

   connect(m_pPanel, &Pages::Account::changed, [this]() {
      m_HasChanged = true;
      updateButtons();
   });

   slotSetAccount(m_pAccountList->selectionModel()->currentIndex());

   connect(m_pAccountList->selectionModel(), &QItemSelectionModel::currentChanged, this, &DlgAccount::slotSetAccount);

   slotExpand();
}

DlgAccount::~DlgAccount()
{
    delete m_pCategoryDelegate;
}

void DlgAccount::slotSetAccount(const QModelIndex& idx)
{
   m_pPanel->setAccount(idx);
}

void DlgAccount::slotExpand()
{
   m_pAccountList->expandAll();

   if (!m_pAccountList->selectionModel()->currentIndex().isValid()) {
      for (int i = 0; i < m_pAccountList->model()->rowCount(); i++) {
         const QModelIndex idx = m_pAccountList->model()->index(i, 0);
         if (m_pAccountList->model()->rowCount(idx)) {
            const QModelIndex defaultAcc = m_pAccountList->model()->index(0, 0, idx);
            m_pAccountList->selectionModel()->setCurrentIndex(defaultAcc, QItemSelectionModel::ClearAndSelect);
         }
      }
   }
}

void DlgAccount::slotRemoveAccount()
{
   static const QString message = i18n("Are you sure you want to remove %1?");
   // Check if the selected element is a profile
   QModelIndex idx = ProfileModel::instance().sortedProxySelectionModel()->currentIndex();
   if (idx.isValid() && !idx.parent().isValid()) {
      if (QMessageBox::warning(this, i18n("Remove profile"),
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

      if (QMessageBox::warning(this, i18n("Remove account"),
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

   // Add profile or import accounts
   if ((int)proto == ((int)Account::Protocol::COUNT__) + (int)ExtendedProtocolModel::ExtendedRole::PROFILE) {
      const bool ret = ProfileModel::instance().add();

      if (!ret)
          return;

      const QModelIndex idx = ProfileModel::instance().index(ProfileModel::instance().rowCount(),0);

      if (idx.isValid()) {
         m_pAccountList->selectionModel()->setCurrentIndex(idx, QItemSelectionModel::ClearAndSelect);
      }

      return;
   }
   else if ((int)proto == ((int)Account::Protocol::COUNT__) + (int)ExtendedProtocolModel::ExtendedRole::IMPORT) {
      const QString path = QFileDialog::getOpenFileName(this, i18n("Import accounts"),  QDir::currentPath());

      if (!path.isEmpty()) {
         KPasswordDialog dlg(this);
         dlg.setPrompt(i18n("Enter the password"));

         if( !dlg.exec() )
             return;

         AccountModel::instance().importAccounts(path, dlg.password());

      }

      return;
   }

   const QString newAlias
         = i18n("New account%1",
                (AccountModel::getSimilarAliasIndex(i18n("New account"))));
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
   AccountModel::instance().cancel();
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
