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
AccountListModel::AccountListModel(QStringList & _accountIds) : QAbstractListModel(QCoreApplication::instance()),m_pColorVisitor(nullptr),m_pDefaultAccount(nullptr)
{
   m_pAccounts = new QVector<Account*>();
   for (int i = 0; i < _accountIds.size(); ++i) {
      Account* a = Account::buildExistingAccountFromId(_accountIds[i]);
      (*m_pAccounts) += a;
      emit dataChanged(index(size()-1,0),index(size()-1,0));
      connect(a,SIGNAL(changed(Account*)),this,SLOT(accountChanged(Account*)));
   }
   CallManagerInterface&          callManager          = DBus::CallManager::instance();
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();

   connect(&callManager         , SIGNAL(registrationStateChanged(QString,QString,int)) ,this,SLOT(accountChanged(QString,QString,int)));
   connect(&configurationManager, SIGNAL(accountsChanged())                             ,this,SLOT(updateAccounts())                   );
   setupRoleName();
}

///Constructors
///@param fill Whether to fill the list with accounts from configurationManager or not.
AccountListModel::AccountListModel(bool fill) : QAbstractListModel(QCoreApplication::instance()),m_pColorVisitor(nullptr),m_pDefaultAccount(nullptr)
{
   m_pAccounts = new QVector<Account *>();
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
   foreach(Account* a,*m_pAccounts) {
      delete a;
   }
   delete m_pAccounts;
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
         if (!getAccountById(accountIds[i])) {
            Account* acc = Account::buildExistingAccountFromId(accountIds[i]);
            m_pAccounts->insert(i, acc);
            connect(acc,SIGNAL(changed(Account*)),this,SLOT(accountChanged(Account*)));
            emit dataChanged(index(i,0),index(size()-1));
         }
      }
      foreach (Account* acc, *m_pAccounts) {
         int idx =accountIds.indexOf(acc->accountId());
         if (idx == -1 && (acc->currentState() == Account::AccountEditState::READY || acc->currentState() == Account::AccountEditState::REMOVED)) {
            m_pAccounts->remove(idx);
            emit dataChanged(index(idx - 1, 0), index(m_pAccounts->size()-1, 0));
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
   int idx = (*m_pAccounts).indexOf(a);
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
   for (int i = 0; i < m_pAccounts->size(); i++) {
      current = (*m_pAccounts)[i];
      if (!(*m_pAccounts)[i]->isNew() && (current->currentState() != Account::AccountEditState::NEW || current->currentState() != Account::AccountEditState::MODIFIED || current->currentState() != Account::AccountEditState::OUTDATED))
         removeAccount(current);
   }
   //ask for the list of accounts ids to the configurationManager
   QStringList accountIds = configurationManager.getAccountList().value();
   for (int i = 0; i < accountIds.size(); ++i) {
      Account* a = Account::buildExistingAccountFromId(accountIds[i]);
      m_pAccounts->insert(i, a);
      emit dataChanged(index(i,0),index(size()-1,0));
      connect(a,SIGNAL(changed(Account*)),this,SLOT(accountChanged(Account*)));
   }
} //update

///Update accounts
void AccountListModel::updateAccounts()
{
   qDebug() << "updateAccounts";
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   QStringList accountIds = configurationManager.getAccountList().value();
   //m_pAccounts->clear();
   for (int i = 0; i < accountIds.size(); ++i) {
      Account* acc = getAccountById(accountIds[i]);
      if (!acc) {
         qDebug() << "updateAccounts " << accountIds[i];
         Account* a = Account::buildExistingAccountFromId(accountIds[i]);
         (*m_pAccounts) += a;
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
   const QStringList accountIds= QStringList(configurationManager.getAccountList().value());

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

   configurationManager.setAccountsOrder(getOrderedList());
}

///Move account up
bool AccountListModel::accountUp( int idx )
{
   if(idx > 0 && idx <= rowCount()) {
      Account* account = getAccountAt(idx);
      m_pAccounts->remove(idx);
      m_pAccounts->insert(idx - 1, account);
      emit dataChanged(this->index(idx - 1, 0, QModelIndex()), this->index(idx, 0, QModelIndex()));
      return true;
   }
   return false;
}

///Move account down
bool AccountListModel::accountDown( int idx )
{
   if(idx >= 0 && idx < rowCount()) {
      Account* account = getAccountAt(idx);
      m_pAccounts->remove(idx);
      m_pAccounts->insert(idx + 1, account);
      emit dataChanged(this->index(idx, 0, QModelIndex()), this->index(idx + 1, 0, QModelIndex()));
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


/*****************************************************************************
 *                                                                           *
 *                                  Getters                                  *
 *                                                                           *
 ****************************************************************************/

///Get all accounts
const QVector<Account*>& AccountListModel::getAccounts()
{
   return *m_pAccounts;
}

///Get a single account
const Account* AccountListModel::getAccountAt (int i) const
{
   return (*m_pAccounts)[i];
}

///Get a single account
Account* AccountListModel::getAccountAt (int i)
{
   return (*m_pAccounts)[i];
}

///Get a serialized string of all accounts
QString AccountListModel::getOrderedList() const
{
   QString order;
   for( int i = 0 ; i < size() ; i++) {
      order += getAccountAt(i)->accountId() + '/';
   }
   return order;
}

///Get account using its ID
Account* AccountListModel::getAccountById(const QString& id) const
{
   if(id.isEmpty())
          return nullptr;
   for (int i = 0; i < m_pAccounts->size(); ++i) {
      if (!(*m_pAccounts)[i]->isNew() && (*m_pAccounts)[i]->accountId() == id)
         return (*m_pAccounts)[i];
   }
   return nullptr;
}

///Get account with a specific state
QVector<Account*> AccountListModel::getAccountsByState(const QString& state)
{
   QVector<Account *> v;
   for (int i = 0; i < m_pAccounts->size(); ++i) {
      if ((*m_pAccounts)[i]->accountRegistrationStatus() == state)
         v += (*m_pAccounts)[i];
   }
   return v;
}

///Get a list of all registerred account
QVector<Account*> AccountListModel::registeredAccounts() const
{
   qDebug() << "registeredAccounts";
   QVector<Account*> registeredAccountsVector;
   Account* current;
   for (int i = 0; i < m_pAccounts->count(); ++i) {
      current = (*m_pAccounts)[i];
      if(current->accountRegistrationStatus() == ACCOUNT_STATE_REGISTERED) {
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
   for (int i = 0; i < m_pAccounts->count(); ++i) {
      current = (*m_pAccounts)[i];
      if(current && current->accountRegistrationStatus() == ACCOUNT_STATE_REGISTERED && current->isAccountEnabled())
         return current;
      else if (current && (current->accountRegistrationStatus() == ACCOUNT_STATE_READY) && m_pAccounts->count() == 1)
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
   return m_pAccounts->size();
}

///Return the current account
Account* AccountListModel::currentAccount()
{
   Account* priorAccount = m_spPriorAccount;
   if(priorAccount && priorAccount->accountRegistrationStatus() == ACCOUNT_STATE_REGISTERED && priorAccount->isAccountEnabled() ) {
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

   const Account * account = (*m_pAccounts)[idx.row()];
   if(idx.column() == 0 && (role == Qt::DisplayRole || role == Qt::EditRole))
      return QVariant(account->alias());
   else if(idx.column() == 0 && role == Qt::CheckStateRole) {
      return QVariant(account->isEnabled() ? Qt::Checked : Qt::Unchecked);
   }
   else if (role == Qt::BackgroundRole) {
      if (m_pColorVisitor)
         return m_pColorVisitor->getColor(account);
      else {
         QVariant var = account->stateColor();
         return account->stateColor();
      }
   }
   else if(idx.column() == 0 && role == Qt::DecorationRole && m_pColorVisitor) {
      return m_pColorVisitor->getIcon(account);
   }
   return QVariant();
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
   return m_pAccounts->size();
}

Account* AccountListModel::getAccountByModelIndex(const QModelIndex& item) const
{
   return (*m_pAccounts)[item.row()];
}

///Return the prior account
Account* AccountListModel::getPriorAccount()
{
   return m_spPriorAccount;
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
      if (a->accountAlias().left(alias.size()) == alias)
         count++;
   }
   bool found = true;
   do {
      found = false;
      foreach (Account* a, instance()->getAccounts()) {
         if (a->accountAlias() == alias+QString(" (%1)").arg(count)) {
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
   (*m_pAccounts) += a;
   
   emit dataChanged(index(m_pAccounts->size()-1,0), index(m_pAccounts->size()-1,0));
   return a;
}

///Remove an account
void AccountListModel::removeAccount(Account* account)
{
   if (not account) return;
   qDebug() << "Removing" << m_pAccounts;
   int aindex = m_pAccounts->indexOf(account);
   m_pAccounts->remove(aindex);
   emit dataChanged(index(aindex,0), index(m_pAccounts->size()-1,0));
}

void AccountListModel::removeAccount( QModelIndex idx )
{
   removeAccount(getAccountByModelIndex(idx));
}

///Set the previous account used
void AccountListModel::setPriorAccount(const Account* account) {
   bool changed = (account && m_spPriorAccount != account) || (!account && m_spPriorAccount);
   m_spPriorAccount = (Account*)(account);
   if (changed)
      emit priorAccountChanged(currentAccount());
}

///Set model data
bool AccountListModel::setData(const QModelIndex& idx, const QVariant& value, int role)
{
   if (idx.isValid() && idx.column() == 0 && role == Qt::CheckStateRole) {
      bool prevEnabled = (*m_pAccounts)[idx.row()]->isEnabled();
      (*m_pAccounts)[idx.row()]->setEnabled(value.toBool());
      emit dataChanged(idx, idx);
      if (prevEnabled != value.toBool())
         emit accountEnabledChanged((*m_pAccounts)[idx.row()]);
      emit dataChanged(idx, idx);
      return true;
   }
   else if ( role == Qt::EditRole ) {
      bool changed = value.toString() != data(idx,Qt::EditRole);
      if (changed) {
         (*m_pAccounts)[idx.row()]->setAccountAlias(value.toString());
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
   return (*m_pAccounts)[i];
}

///Get the account from its index
Account* AccountListModel::operator[] (int i)
{
   return (*m_pAccounts)[i];
}
