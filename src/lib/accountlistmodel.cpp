/****************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                          *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>          *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Lesser General Public             *
 *   License as published by the Free Software Foundation; either           *
 *   version 2.1 of the License, or (at your option) any later version.     *
 *                                                                          *
 *   This library is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

//Parent
#include "accountlistmodel.h"
 
//SFLPhone
#include "sflphone_const.h"

//Qt
#include <QtCore/QObject>

//SFLPhone library
#include "dbus/configurationmanager.h"
#include "dbus/callmanager.h"
#include "visitors/accountlistcolorvisitor.h"

AccountListModel* AccountListModel::m_spAccountList   = nullptr;
Account*     AccountListModel::m_spPriorAccount   = nullptr     ;

QVariant AccountListNoCheckProxyModel::data(const QModelIndex& idx,int role ) const
{
   if (role == Qt::CheckStateRole) {
      return QVariant();
   }
   return AccountListModel::instance()->data(idx,role);
}

bool AccountListNoCheckProxyModel::setData( const QModelIndex& idx, const QVariant &value, int role)
{
   return AccountListModel::instance()->setData(idx,value,role);
}

Qt::ItemFlags AccountListNoCheckProxyModel::flags (const QModelIndex& idx) const
{
   return AccountListModel::instance()->flags(idx);
}

int AccountListNoCheckProxyModel::rowCount(const QModelIndex& parentIdx ) const
{
   return AccountListModel::instance()->rowCount(parentIdx);
}

///Constructors
///@param fill Whether to fill the list with accounts from configurationManager or not.
AccountListModel::AccountListModel(bool fill) : QAbstractListModel(QCoreApplication::instance()),m_pColorVisitor(nullptr),m_pDefaultAccount(nullptr)
{
   if(fill)
      updateAccounts();
   CallManagerInterface& callManager = DBus::CallManager::instance();
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();

   connect(&callManager         , SIGNAL(registrationStateChanged(QString,QString,int)),this,SLOT(accountChanged(QString,QString,int)));
   connect(&configurationManager, SIGNAL(accountsChanged())                            ,this,SLOT(updateAccounts())                   );
   setupRoleName();
}

///Destructor
AccountListModel::~AccountListModel()
{
   foreach(Account* a,m_lAccounts) {
      delete a;
   }
}

void AccountListModel::setupRoleName()
{
   QHash<int, QByteArray> roles = roleNames();
   roles.insert(Account::Role::Alias                    ,QByteArray("alias"                         ));
   roles.insert(Account::Role::Type                     ,QByteArray("type"                          ));
   roles.insert(Account::Role::Hostname                 ,QByteArray("hostname"                      ));
   roles.insert(Account::Role::Username                 ,QByteArray("username"                      ));
   roles.insert(Account::Role::Mailbox                  ,QByteArray("mailbox"                       ));
   roles.insert(Account::Role::Proxy                    ,QByteArray("proxy"                         ));
   roles.insert(Account::Role::TlsPassword              ,QByteArray("tlsPassword"                   ));
   roles.insert(Account::Role::TlsCaListFile            ,QByteArray("tlsCaListFile"                 ));
   roles.insert(Account::Role::TlsCertificateFile       ,QByteArray("tlsCertificateFile"            ));
   roles.insert(Account::Role::TlsPrivateKeyFile        ,QByteArray("tlsPrivateKeyFile"             ));
   roles.insert(Account::Role::TlsCiphers               ,QByteArray("tlsCiphers"                    ));
   roles.insert(Account::Role::TlsServerName            ,QByteArray("tlsServerName"                 ));
   roles.insert(Account::Role::SipStunServer            ,QByteArray("sipStunServer"                 ));
   roles.insert(Account::Role::PublishedAddress         ,QByteArray("publishedAddress"              ));
   roles.insert(Account::Role::LocalInterface           ,QByteArray("localInterface"                ));
   roles.insert(Account::Role::RingtonePath             ,QByteArray("ringtonePath"                  ));
   roles.insert(Account::Role::TlsMethod                ,QByteArray("tlsMethod"                     ));
   roles.insert(Account::Role::AccountRegistrationExpire,QByteArray("accountRegistrationExpire"     ));
   roles.insert(Account::Role::TlsNegotiationTimeoutSec ,QByteArray("tlsNegotiationTimeoutSec"      ));
   roles.insert(Account::Role::TlsNegotiationTimeoutMsec,QByteArray("tlsNegotiationTimeoutMsec"     ));
   roles.insert(Account::Role::LocalPort                ,QByteArray("localPort"                     ));
   roles.insert(Account::Role::TlsListenerPort          ,QByteArray("tlsListenerPort"               ));
   roles.insert(Account::Role::PublishedPort            ,QByteArray("publishedPort"                 ));
   roles.insert(Account::Role::Enabled                  ,QByteArray("enabled"                       ));
   roles.insert(Account::Role::AutoAnswer               ,QByteArray("autoAnswer"                    ));
   roles.insert(Account::Role::TlsVerifyServer          ,QByteArray("tlsVerifyServer"               ));
   roles.insert(Account::Role::TlsVerifyClient          ,QByteArray("tlsVerifyClient"               ));
   roles.insert(Account::Role::TlsRequireClientCertificate,QByteArray("tlsRequireClientCertificate" ));
   roles.insert(Account::Role::TlsEnable                ,QByteArray("tlsEnable"                     ));
   roles.insert(Account::Role::DisplaySasOnce           ,QByteArray("displaySasOnce"                ));
   roles.insert(Account::Role::SrtpRtpFallback          ,QByteArray("srtpRtpFallback"               ));
   roles.insert(Account::Role::ZrtpDisplaySas           ,QByteArray("zrtpDisplaySas"                ));
   roles.insert(Account::Role::ZrtpNotSuppWarning       ,QByteArray("zrtpNotSuppWarning"            ));
   roles.insert(Account::Role::ZrtpHelloHash            ,QByteArray("zrtpHelloHash"                 ));
   roles.insert(Account::Role::SipStunEnabled           ,QByteArray("sipStunEnabled"                ));
   roles.insert(Account::Role::PublishedSameAsLocal     ,QByteArray("publishedSameAsLocal"          ));
   roles.insert(Account::Role::RingtoneEnabled          ,QByteArray("ringtoneEnabled"               ));
   roles.insert(Account::Role::dTMFType                 ,QByteArray("dTMFType"                      ));
   roles.insert(Account::Role::Id                       ,QByteArray("id"                            ));
   roles.insert(Account::Role::Object                   ,QByteArray("object"                        ));
   roles.insert(Account::Role::TypeName                 ,QByteArray("typeName"                      ));
   setRoleNames(roles);
}

///Singleton
AccountListModel* AccountListModel::instance()
{
   if (not m_spAccountList) {
      m_spAccountList = new AccountListModel(true);
   }
   return m_spAccountList;
}

///Static destructor
void AccountListModel::destroy()
{
   if (m_spAccountList)
      delete m_spAccountList;
   m_spAccountList = nullptr;
}

///Account status changed
void AccountListModel::accountChanged(const QString& account,const QString& state, int code)
{
   Q_UNUSED(code)
   qDebug() << "Account" << account << "status changed to" << state;
   Account* a = getAccountById(account);
   if (!a) {
      ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
      QStringList accountIds = configurationManager.getAccountList().value();
      for (int i = 0; i < accountIds.size(); ++i) {
         if ((!getAccountById(accountIds[i])) && m_lDeletedAccounts.indexOf(accountIds[i]) == -1) {
            Account* acc = Account::buildExistingAccountFromId(accountIds[i]);
            m_lAccounts.insert(i, acc);
            connect(acc,SIGNAL(changed(Account*)),this,SLOT(accountChanged(Account*)));
            emit dataChanged(index(i,0),index(size()-1));
            emit layoutChanged();
         }
      }
      foreach (Account* acc, m_lAccounts) {
         int idx =accountIds.indexOf(acc->id());
         if (idx == -1 && (acc->currentState() == Account::AccountEditState::READY || acc->currentState() == Account::AccountEditState::REMOVED)) {
            m_lAccounts.remove(idx);
            emit dataChanged(index(idx - 1, 0), index(m_lAccounts.size()-1, 0));
            emit layoutChanged();
         }
      }
   }
   if (a)
      emit accountStateChanged(a,a->stateName(state));
   else
      qDebug() << "Account not found";
}

///Tell the model something changed
void AccountListModel::accountChanged(Account* a)
{
   int idx = m_lAccounts.indexOf(a);
   if (idx != -1) {
      emit dataChanged(index(idx, 0), index(idx, 0));
   }
}


/*****************************************************************************
 *                                                                           *
 *                                  Mutator                                  *
 *                                                                           *
 ****************************************************************************/

///Update accounts
void AccountListModel::update()
{
   ConfigurationManagerInterface & configurationManager = DBus::ConfigurationManager::instance();
   Account* current;
   QList<Account*> tmp;
   for (int i = 0; i < m_lAccounts.size(); i++)
      tmp << m_lAccounts[i];

   for (int i = 0; i < tmp.size(); i++) {
      current = tmp[i];
      if (!current->isNew() && (current->currentState() != Account::AccountEditState::NEW 
         && current->currentState() != Account::AccountEditState::MODIFIED
         && current->currentState() != Account::AccountEditState::OUTDATED))
         removeAccount(current);
   }
   //ask for the list of accounts ids to the configurationManager
   QStringList accountIds = configurationManager.getAccountList().value();
   for (int i = 0; i < accountIds.size(); ++i) {
      if (m_lDeletedAccounts.indexOf(accountIds[i]) == -1) {
         Account* a = Account::buildExistingAccountFromId(accountIds[i]);
         m_lAccounts.insert(i, a);
         emit dataChanged(index(i,0),index(size()-1,0));
         connect(a,SIGNAL(changed(Account*)),this,SLOT(accountChanged(Account*)));
         emit layoutChanged();
      }
   }
} //update

///Update accounts
void AccountListModel::updateAccounts()
{
   qDebug() << "updateAccounts";
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   QStringList accountIds = configurationManager.getAccountList().value();
   //m_lAccounts.clear();
   for (int i = 0; i < accountIds.size(); ++i) {
      Account* acc = getAccountById(accountIds[i]);
      if (!acc) {
         qDebug() << "updateAccounts " << accountIds[i];
         Account* a = Account::buildExistingAccountFromId(accountIds[i]);
         m_lAccounts += a;
         connect(a,SIGNAL(changed(Account*)),this,SLOT(accountChanged(Account*)));
         emit dataChanged(index(size()-1,0),index(size()-1,0));
      }
      else {
         acc->performAction(Account::AccountEditAction::RELOAD);
      }
   }
   emit accountListUpdated();
} //updateAccounts

///Save accounts details and reload it
void AccountListModel::save()
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   const QStringList accountIds = QStringList(configurationManager.getAccountList().value());

   //create or update each account from accountList
   for (int i = 0; i < size(); i++) {
      Account* current = (*this)[i];
      //current->save();
      current->performAction(Account::AccountEditAction::SAVE);
   }

   //remove accounts that are in the configurationManager but not in the client
   for (int i = 0; i < accountIds.size(); i++) {
      if(!getAccountById(accountIds[i])) {
         configurationManager.removeAccount(accountIds[i]);
      }
   }

   //Set account order
   QString order;
   for( int i = 0 ; i < size() ; i++)
      order += getAccountAt(i)->id() + '/';
   configurationManager.setAccountsOrder(order);
   m_lDeletedAccounts.clear();
}

///Move account up
bool AccountListModel::accountUp( int idx )
{
   if(idx > 0 && idx <= rowCount()) {
      Account* account = getAccountAt(idx);
      m_lAccounts.remove(idx);
      m_lAccounts.insert(idx - 1, account);
      emit dataChanged(this->index(idx - 1, 0, QModelIndex()), this->index(idx, 0, QModelIndex()));
      emit layoutChanged();
      return true;
   }
   return false;
}

///Move account down
bool AccountListModel::accountDown( int idx )
{
   if(idx >= 0 && idx < rowCount()) {
      Account* account = getAccountAt(idx);
      m_lAccounts.remove(idx);
      m_lAccounts.insert(idx + 1, account);
      emit dataChanged(this->index(idx, 0, QModelIndex()), this->index(idx + 1, 0, QModelIndex()));
      emit layoutChanged();
      return true;
   }
   return false;
}

///Try to register all enabled accounts
void AccountListModel::registerAllAccounts()
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   configurationManager.registerAllAccounts();
}

///Cancel all modifications
void AccountListModel::cancel() {
   foreach (Account* a, getAccounts()) {
      if (a->currentState() == Account::AccountEditState::MODIFIED || a->currentState() == Account::AccountEditState::OUTDATED)
         a->performAction(Account::AccountEditAction::CANCEL);
   }
   m_lDeletedAccounts.clear();
}


/*****************************************************************************
 *                                                                           *
 *                                  Getters                                  *
 *                                                                           *
 ****************************************************************************/

///Get all accounts
const QVector<Account*>& AccountListModel::getAccounts()
{
   return m_lAccounts;
}

///Get a single account
Account* AccountListModel::getAccountAt (int i) const
{
   return m_lAccounts[i];
}

///Get account using its ID
Account* AccountListModel::getAccountById(const QString& id) const
{
   if(id.isEmpty())
      return nullptr;
   for (int i = 0; i < m_lAccounts.size(); ++i) {
      if (!m_lAccounts[i]->isNew() && m_lAccounts[i]->id() == id)
         return m_lAccounts[i];
   }
   return nullptr;
}

///Get account with a specific state
QVector<Account*> AccountListModel::getAccountsByState(const QString& state)
{
   QVector<Account *> v;
   for (int i = 0; i < m_lAccounts.size(); ++i) {
      if (m_lAccounts[i]->accountRegistrationStatus() == state)
         v += m_lAccounts[i];
   }
   return v;
}

///Get a list of all registerred account
QVector<Account*> AccountListModel::registeredAccounts() const
{
   qDebug() << "registeredAccounts";
   QVector<Account*> registeredAccountsVector;
   Account* current;
   for (int i = 0; i < m_lAccounts.count(); ++i) {
      current = m_lAccounts[i];
      if(current->accountRegistrationStatus() == Account::State::REGISTERED) {
         qDebug() << current->alias() << " : " << current;
         registeredAccountsVector.append(current);
      }
   }
   return registeredAccountsVector;
}

///Get the first registerred account (default account)
Account* AccountListModel::firstRegisteredAccount() const
{
   Account* current;
   for (int i = 0; i < m_lAccounts.count(); ++i) {
      current = m_lAccounts[i];
      if(current && current->accountRegistrationStatus() == Account::State::REGISTERED && current->isEnabled())
         return current;
      else if (current && (current->accountRegistrationStatus() == Account::State::READY) && m_lAccounts.count() == 1)
         return current;
//       else if (current && !(current->accountRegistrationStatus()() == ACCOUNT_STATE_READY)) {
//          qDebug() << "Account " << ((current)?current->accountId():"") << " is not registered ("
//          << ((current)?current->accountRegistrationStatus()():"") << ") State:"
//          << ((current)?current->accountRegistrationStatus()():"");
//       }
   }
   return nullptr;
}

///Get the account size
int AccountListModel::size() const
{
   return m_lAccounts.size();
}

///Return the current account
Account* AccountListModel::currentAccount()
{
   Account* priorAccount = m_spPriorAccount;
   if(priorAccount && priorAccount->accountRegistrationStatus() == Account::State::REGISTERED && priorAccount->isEnabled() ) {
      return priorAccount;
   }
   else {
      Account* a = instance()->firstRegisteredAccount();
      if (!a)
         a = instance()->getAccountById("IP2IP");
      instance()->setPriorAccount(a);
      return a;
   }
} //getCurrentAccount

///Get data from the model
QVariant AccountListModel::data ( const QModelIndex& idx, int role) const
{
   if (!idx.isValid() || idx.row() < 0 || idx.row() >= rowCount())
      return QVariant();

   const Account* account = m_lAccounts[idx.row()];
   if(idx.column() == 0 && (role == Qt::DisplayRole || role == Qt::EditRole))
      return QVariant(account->alias());
   else if(idx.column() == 0 && role == Qt::CheckStateRole)
      return QVariant(account->isEnabled() ? Qt::Checked : Qt::Unchecked);
   else if (role == Qt::BackgroundRole)
      return (m_pColorVisitor)?m_pColorVisitor->getColor(account):account->stateColor();
   else if(idx.column() == 0 && role == Qt::DecorationRole && m_pColorVisitor)
      return m_pColorVisitor->getIcon(account);
   else
      return account->roleData(role);
} //data

///Flags for "idx"
Qt::ItemFlags AccountListModel::flags(const QModelIndex& idx) const
{
   if (idx.column() == 0)
      return QAbstractItemModel::flags(idx) | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
   return QAbstractItemModel::flags(idx);
}

///Number of account
int AccountListModel::rowCount(const QModelIndex& parentIdx) const
{
   Q_UNUSED(parentIdx);
   return m_lAccounts.size();
}

Account* AccountListModel::getAccountByModelIndex(const QModelIndex& item) const
{
   return m_lAccounts[item.row()];
}

AccountListColorVisitor* AccountListModel::colorVisitor()
{
   return m_pColorVisitor;
}

///Return the default account (used for contact lookup)
Account* AccountListModel::getDefaultAccount() const
{
   return m_pDefaultAccount;
}

///Generate an unique suffix to prevent multiple account from sharing alias
QString AccountListModel::getSimilarAliasIndex(const QString& alias)
{
   int count = 0;
   foreach (Account* a, instance()->getAccounts()) {
      if (a->alias().left(alias.size()) == alias)
         count++;
   }
   bool found = true;
   do {
      found = false;
      foreach (Account* a, instance()->getAccounts()) {
         if (a->alias() == alias+QString(" (%1)").arg(count)) {
            count++;
            found = false;
            break;
         }
      }
   } while(found);
   if (count)
      return QString(" (%1)").arg(count);
   return QString();
}


/*****************************************************************************
 *                                                                           *
 *                                  Setters                                  *
 *                                                                           *
 ****************************************************************************/

///Add an account
Account* AccountListModel::addAccount(const QString& alias)
{
   Account* a = Account::buildNewAccountFromAlias(alias);
   connect(a,SIGNAL(changed(Account*)),this,SLOT(accountChanged(Account*)));
   m_lAccounts += a;

   emit dataChanged(index(m_lAccounts.size()-1,0), index(m_lAccounts.size()-1,0));
   return a;
}

///Remove an account
void AccountListModel::removeAccount(Account* account)
{
   if (not account) return;
   qDebug() << "Removing" << account->alias() << account->id();
   const int aindex = m_lAccounts.indexOf(account);
   m_lAccounts.remove(aindex);
   m_lDeletedAccounts << account->id();
   if (currentAccount() == account)
      setPriorAccount(getAccountById("IP2IP"));
   delete account;
   emit dataChanged(index(aindex,0), index(m_lAccounts.size()-1,0));
   emit layoutChanged();
}

void AccountListModel::removeAccount( QModelIndex idx )
{
   removeAccount(getAccountByModelIndex(idx));
}

///Set the previous account used
void AccountListModel::setPriorAccount(const Account* account) {
   const bool changed = (account && m_spPriorAccount != account) || (!account && m_spPriorAccount);
   m_spPriorAccount = (Account*)(account);
   if (changed)
      emit priorAccountChanged(currentAccount());
}

///Set model data
bool AccountListModel::setData(const QModelIndex& idx, const QVariant& value, int role)
{
   if (idx.isValid() && idx.column() == 0 && role == Qt::CheckStateRole) {
      const bool prevEnabled = m_lAccounts[idx.row()]->isEnabled();
      m_lAccounts[idx.row()]->setEnabled(value.toBool());
      emit dataChanged(idx, idx);
      if (prevEnabled != value.toBool())
         emit accountEnabledChanged(m_lAccounts[idx.row()]);
      emit dataChanged(idx, idx);
      return true;
   }
   else if ( role == Qt::EditRole ) {
      bool changed = value.toString() != data(idx,Qt::EditRole);
      if (changed) {
         m_lAccounts[idx.row()]->setAlias(value.toString());
         emit dataChanged(idx, idx);
      }
   }
   return false;
}

///Set QAbstractItemModel BackgroundRole visitor
void AccountListModel::setColorVisitor(AccountListColorVisitor* visitor)
{
   m_pColorVisitor = visitor;
}

///Set the default account (used for contact lookup)
void AccountListModel::setDefaultAccount(Account* a)
{
   if (a != m_pDefaultAccount)
      emit defaultAccountChanged(a);
   m_pDefaultAccount = a;
}


/*****************************************************************************
 *                                                                           *
 *                                 Operator                                  *
 *                                                                           *
 ****************************************************************************/

///Get the account from its index
const Account* AccountListModel::operator[] (int i) const
{
   return m_lAccounts[i];
}

///Get the account from its index
Account* AccountListModel::operator[] (int i)
{
   return m_lAccounts[i];
}

///Get accoutn by id
Account* AccountListModel::operator[] (const QString& i) {
   return getAccountById(i);
}