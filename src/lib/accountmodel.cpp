/****************************************************************************
 *   Copyright (C) 2009-2014 by Savoir-Faire Linux                          *
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
#include "accountmodel.h"

//SFLPhone
#include "sflphone_const.h"

//Qt
#include <QtCore/QObject>

//SFLPhone library
#include "dbus/configurationmanager.h"
#include "dbus/callmanager.h"
#include "visitors/accountlistcolorvisitor.h"

class AccountModelPrivate : public QObject
{
   Q_OBJECT
   Q_DECLARE_PUBLIC(AccountModel)

public:
   //Constructor
   AccountModelPrivate(AccountModel* parent);
   void init();
   void setupRoleName();

   //Helpers
   Account* firstRegisteredAccount() const;

   //Attributes
   AccountModel*            q_ptr             ;
   QVector<Account*>        m_lAccounts       ;
   AccountListColorVisitor* m_pColorVisitor   ;
   QStringList              m_lDeletedAccounts;
   Account*                 m_pIP2IP          ;

public Q_SLOTS:
   void slotAccountChanged(const QString& account,const QString& state, int code);
   void slotAccountChanged(Account* a);
   void slotVoiceMailNotify( const QString& accountID , int count );
   void slotAccountPresenceEnabledChanged(bool state);
};

AccountModel* AccountModel::m_spAccountList  = nullptr;
Account*      AccountModel::m_spPriorAccount = nullptr;

QVariant AccountListNoCheckProxyModel::data(const QModelIndex& idx,int role ) const
{
   if (role == Qt::CheckStateRole) {
      return QVariant();
   }
   return AccountModel::instance()->data(idx,role);
}

bool AccountListNoCheckProxyModel::setData( const QModelIndex& idx, const QVariant &value, int role)
{
   return AccountModel::instance()->setData(idx,value,role);
}

Qt::ItemFlags AccountListNoCheckProxyModel::flags (const QModelIndex& idx) const
{
   const QModelIndex& src = AccountModel::instance()->index(idx.row(),idx.column());
   if (!idx.row() || AccountModel::instance()->data(src,Qt::CheckStateRole) == Qt::Unchecked)
      return Qt::NoItemFlags;
   return AccountModel::instance()->flags(idx);
}

int AccountListNoCheckProxyModel::rowCount(const QModelIndex& parentIdx ) const
{
   return AccountModel::instance()->rowCount(parentIdx);
}

AccountModelPrivate::AccountModelPrivate(AccountModel* parent) : QObject(parent),q_ptr(parent),
m_pColorVisitor(nullptr),m_pIP2IP(nullptr)
{
   setupRoleName();
}

///Constructors
///@param fill Whether to fill the list with accounts from configurationManager or not.
AccountModel::AccountModel() : QAbstractListModel(QCoreApplication::instance())
,d_ptr(new AccountModelPrivate(this))
{

}

///Prevent constructor loop
void AccountModelPrivate::init()
{
   q_ptr->updateAccounts();
   CallManagerInterface& callManager = DBus::CallManager::instance();
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();

   connect(&configurationManager, SIGNAL(sipRegistrationStateChanged(QString,QString,int)),this ,
      SLOT(slotAccountChanged(QString,QString,int)));
   connect(&configurationManager, SIGNAL(accountsChanged())                               ,q_ptr,
      SLOT(updateAccounts())                  );
   connect(&callManager         , SIGNAL(voiceMailNotify(QString,int))                    ,this ,
      SLOT(slotVoiceMailNotify(QString,int))  );

}

///Destructor
AccountModel::~AccountModel()
{
   while(d_ptr->m_lAccounts.size()) {
      Account* a = d_ptr->m_lAccounts[0];
      d_ptr->m_lAccounts.remove(0);
      delete a;
   }
}

void AccountModelPrivate::setupRoleName()
{
   QHash<int, QByteArray> roles = q_ptr->roleNames();
   roles.insert(Account::Role::Alias                    ,QByteArray("alias"                         ));
   roles.insert(Account::Role::Proto                    ,QByteArray("protocol"                      ));
   roles.insert(Account::Role::Hostname                 ,QByteArray("hostname"                      ));
   roles.insert(Account::Role::Username                 ,QByteArray("username"                      ));
   roles.insert(Account::Role::Mailbox                  ,QByteArray("mailbox"                       ));
   roles.insert(Account::Role::Proxy                    ,QByteArray("proxy"                         ));
   roles.insert(Account::Role::TlsPassword              ,QByteArray("tlsPassword"                   ));
   roles.insert(Account::Role::TlsCaListCertificate     ,QByteArray("tlsCaListCertificate"          ));
   roles.insert(Account::Role::TlsCertificate           ,QByteArray("tlsCertificate"                ));
   roles.insert(Account::Role::TlsPrivateKeyCertificate ,QByteArray("tlsPrivateKeyCertificate"      ));
   roles.insert(Account::Role::TlsCiphers               ,QByteArray("tlsCiphers"                    ));
   roles.insert(Account::Role::TlsServerName            ,QByteArray("tlsServerName"                 ));
   roles.insert(Account::Role::SipStunServer            ,QByteArray("sipStunServer"                 ));
   roles.insert(Account::Role::PublishedAddress         ,QByteArray("publishedAddress"              ));
   roles.insert(Account::Role::LocalInterface           ,QByteArray("localInterface"                ));
   roles.insert(Account::Role::RingtonePath             ,QByteArray("ringtonePath"                  ));
   roles.insert(Account::Role::TlsMethod                ,QByteArray("tlsMethod"                     ));
   roles.insert(Account::Role::RegistrationExpire       ,QByteArray("registrationExpire"            ));
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
   roles.insert(Account::Role::TlsEnabled               ,QByteArray("tlsEnabled"                    ));
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
   roles.insert(Account::Role::PresenceStatus           ,QByteArray("presenceStatus"                ));
   roles.insert(Account::Role::PresenceMessage          ,QByteArray("presenceMessage"               ));

   q_ptr->setRoleNames(roles);
}

///Get the IP2IP account
Account* AccountModel::ip2ip() const
{
   if (!d_ptr->m_pIP2IP) {
      foreach(Account* a, d_ptr->m_lAccounts) {
         if (a->id() == Account::ProtocolName::IP2IP)
            d_ptr->m_pIP2IP = a;
      }
   }
   return d_ptr->m_pIP2IP;
}

///Singleton
AccountModel* AccountModel::instance()
{
   if (! m_spAccountList) {
      m_spAccountList = new AccountModel();
      m_spAccountList->d_ptr->init();
   }
   return m_spAccountList;
}

///Static destructor
void AccountModel::destroy()
{
   if (m_spAccountList)
      delete m_spAccountList;
   m_spAccountList = nullptr;
}

///Account status changed
void AccountModelPrivate::slotAccountChanged(const QString& account,const QString& state, int code)
{
   Account* a = q_ptr->getById(account);

   if (!a || (a && a->registrationStatus() != state )) {
      if (state != "OK") //Do not polute the log
         qDebug() << "Account" << account << "status changed to" << state;
   }
   if (!a) {
      ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
      QStringList accountIds = configurationManager.getAccountList().value();
      for (int i = 0; i < accountIds.size(); ++i) {
         if ((!q_ptr->getById(accountIds[i])) && m_lDeletedAccounts.indexOf(accountIds[i]) == -1) {
            Account* acc = Account::buildExistingAccountFromId(accountIds[i]);
            m_lAccounts.insert(i, acc);
            connect(acc,SIGNAL(changed(Account*)),this,SLOT(slotAccountChanged(Account*)));
            connect(acc,SIGNAL(presenceEnabledChanged(bool)),this,SLOT(slotAccountPresenceEnabledChanged(bool)));
            emit q_ptr->dataChanged(q_ptr->index(i,0),q_ptr->index(q_ptr->size()-1));
            emit q_ptr->layoutChanged();
         }
      }
      foreach (Account* acc, m_lAccounts) {
         const int idx =accountIds.indexOf(acc->id());
         if (idx == -1 && (acc->state() == Account::EditState::READY || acc->state() == Account::EditState::REMOVED)) {
            m_lAccounts.remove(idx);
            emit q_ptr->dataChanged(q_ptr->index(idx - 1, 0), q_ptr->index(m_lAccounts.size()-1, 0));
            emit q_ptr->layoutChanged();
         }
      }
   }
   else {
      const bool isRegistered = a->isRegistered();
      a->updateState();
      emit a->stateChanged(a->toHumanStateName());
      const QModelIndex idx = a->index();
      emit q_ptr->dataChanged(idx, idx);
      const bool regStateChanged = isRegistered != a->isRegistered();
      if (regStateChanged && (code == 502 || code == 503)) {
         emit q_ptr->badGateway();
      }
      else if (regStateChanged)
         emit q_ptr->registrationChanged(a,a->isRegistered());

      //Keep the error message
      a->setLastErrorMessage(state);
      a->setLastErrorCode(code);

      emit q_ptr->accountStateChanged(a,a->toHumanStateName());
   }

}

///Tell the model something changed
void AccountModelPrivate::slotAccountChanged(Account* a)
{
   int idx = m_lAccounts.indexOf(a);
   if (idx != -1) {
      emit q_ptr->dataChanged(q_ptr->index(idx, 0), q_ptr->index(idx, 0));
   }
}


/*****************************************************************************
 *                                                                           *
 *                                  Mutator                                  *
 *                                                                           *
 ****************************************************************************/


///When a new voice mail is available
void AccountModelPrivate::slotVoiceMailNotify(const QString &accountID, int count)
{
   Account* a = q_ptr->getById(accountID);
   if (a) {
      a->setVoiceMailCount(count);
      emit q_ptr->voiceMailNotify(a,count);
   }
}

///Propagate account presence state
void AccountModelPrivate::slotAccountPresenceEnabledChanged(bool state)
{
   Q_UNUSED(state)
   emit q_ptr->presenceEnabledChanged(q_ptr->isPresenceEnabled());
}

///Update accounts
void AccountModel::update()
{
   ConfigurationManagerInterface & configurationManager = DBus::ConfigurationManager::instance();
   QList<Account*> tmp;
   for (int i = 0; i < d_ptr->m_lAccounts.size(); i++)
      tmp << d_ptr->m_lAccounts[i];

   for (int i = 0; i < tmp.size(); i++) {
      Account* current = tmp[i];
      if (!current->isNew() && (current->state() != Account::EditState::NEW
         && current->state() != Account::EditState::MODIFIED
         && current->state() != Account::EditState::OUTDATED))
         remove(current);
   }
   //ask for the list of accounts ids to the configurationManager
   const QStringList accountIds = configurationManager.getAccountList().value();
   for (int i = 0; i < accountIds.size(); ++i) {
      if (d_ptr->m_lDeletedAccounts.indexOf(accountIds[i]) == -1) {
         Account* a = Account::buildExistingAccountFromId(accountIds[i]);
         d_ptr->m_lAccounts.insert(i, a);
         emit dataChanged(index(i,0),index(size()-1,0));
         connect(a,SIGNAL(changed(Account*)),d_ptr.data(),SLOT(slotAccountChanged(Account*)));
         connect(a,SIGNAL(presenceEnabledChanged(bool)),d_ptr.data(),SLOT(slotAccountPresenceEnabledChanged(bool)));
         emit layoutChanged();
      }
   }
} //update

///Update accounts
void AccountModel::updateAccounts()
{
   qDebug() << "Updating all accounts";
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   QStringList accountIds = configurationManager.getAccountList().value();
   //m_lAccounts.clear();
   for (int i = 0; i < accountIds.size(); ++i) {
      Account* acc = getById(accountIds[i]);
      if (!acc) {
         Account* a = Account::buildExistingAccountFromId(accountIds[i]);
         d_ptr->m_lAccounts += a;
         connect(a,SIGNAL(changed(Account*)),d_ptr.data(),SLOT(slotAccountChanged(Account*)));
         connect(a,SIGNAL(presenceEnabledChanged(bool)),d_ptr.data(),SLOT(slotAccountPresenceEnabledChanged(bool)));
         emit dataChanged(index(size()-1,0),index(size()-1,0));
      }
      else {
         acc->performAction(Account::EditAction::RELOAD);
      }
   }
   emit accountListUpdated();
} //updateAccounts

///Save accounts details and reload it
void AccountModel::save()
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   const QStringList accountIds = QStringList(configurationManager.getAccountList().value());

   //create or update each account from accountList
   for (int i = 0; i < size(); i++) {
      Account* current = (*this)[i];
      current->performAction(Account::EditAction::SAVE);
   }

   //remove accounts that are in the configurationManager but not in the client
   for (int i = 0; i < accountIds.size(); i++) {
      if(!getById(accountIds[i])) {
         configurationManager.removeAccount(accountIds[i]);
      }
   }

   //Set account order
   QString order;
   for( int i = 0 ; i < size() ; i++)
      order += d_ptr->m_lAccounts[i]->id() + '/';
   configurationManager.setAccountsOrder(order);
   d_ptr->m_lDeletedAccounts.clear();
}

///Move account up
bool AccountModel::moveUp( const QModelIndex& idx )
{
   int row = idx.row();
   if(row > 0 && row <= rowCount()) {
      Account* account = d_ptr->m_lAccounts[row];
      d_ptr->m_lAccounts.remove(row);
      d_ptr->m_lAccounts.insert(row - 1, account);
      emit dataChanged(this->index(row - 1, 0, QModelIndex()), this->index(row, 0, QModelIndex()));
      emit layoutChanged();
      return true;
   }
   return false;
}

///Move account down
bool AccountModel::moveDown( const QModelIndex& idx )
{
   int row = idx.row();
   if(row >= 0 && row < rowCount()) {
      Account* account = d_ptr->m_lAccounts[row];
      d_ptr->m_lAccounts.remove(row);
      d_ptr->m_lAccounts.insert(row + 1, account);
      emit dataChanged(this->index(row, 0, QModelIndex()), this->index(row + 1, 0, QModelIndex()));
      emit layoutChanged();
      return true;
   }
   return false;
}

///Try to register all enabled accounts
void AccountModel::registerAllAccounts()
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   configurationManager.registerAllAccounts();
}

///Cancel all modifications
void AccountModel::cancel() {
   foreach (Account* a, d_ptr->m_lAccounts) {
      if (a->state() == Account::EditState::MODIFIED || a->state() == Account::EditState::OUTDATED)
         a->performAction(Account::EditAction::CANCEL);
   }
   d_ptr->m_lDeletedAccounts.clear();
}


/*****************************************************************************
 *                                                                           *
 *                                  Getters                                  *
 *                                                                           *
 ****************************************************************************/

///Get account using its ID
Account* AccountModel::getById(const QString& id) const
{
   Q_ASSERT(!id.isEmpty());
   for (int i = 0; i < d_ptr->m_lAccounts.size(); i++) {
      Account* acc = d_ptr->m_lAccounts[i];
      if (acc && !acc->isNew() && acc->id() == id)
         return acc;
   }
   return nullptr;
}

///Get the first registerred account (default account)
Account* AccountModelPrivate::firstRegisteredAccount() const
{
   for (int i = 0; i < m_lAccounts.count(); ++i) {
      Account* current = m_lAccounts[i];
      if(current && current->registrationStatus() == Account::State::REGISTERED && current->isEnabled())
         return current;
      else if (current && (current->registrationStatus() == Account::State::READY) && m_lAccounts.count() == 1)
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
int AccountModel::size() const
{
   return d_ptr->m_lAccounts.size();
}

///Return the current account
Account* AccountModel::currentAccount()
{
   Account* priorAccount = m_spPriorAccount;
   if(priorAccount && priorAccount->registrationStatus() == Account::State::REGISTERED && priorAccount->isEnabled() ) {
      return priorAccount;
   }
   else {
      Account* a = instance()->d_ptr->firstRegisteredAccount();
      if (!a)
         a = instance()->getById(Account::ProtocolName::IP2IP);
      instance()->setPriorAccount(a);
      return a;
   }
} //getCurrentAccount

///Get data from the model
QVariant AccountModel::data ( const QModelIndex& idx, int role) const
{
   if (!idx.isValid() || idx.row() < 0 || idx.row() >= rowCount())
      return QVariant();

   const Account* account = d_ptr->m_lAccounts[idx.row()];
   if(idx.column() == 0 && (role == Qt::DisplayRole || role == Qt::EditRole))
      return QVariant(account->alias());
   else if(idx.column() == 0 && role == Qt::CheckStateRole)
      return QVariant(account->isEnabled() ? Qt::Checked : Qt::Unchecked);
   else if (role == Qt::BackgroundRole)
      return (d_ptr->m_pColorVisitor)?d_ptr->m_pColorVisitor->getColor(account):account->stateColor();
   else if(idx.column() == 0 && role == Qt::DecorationRole && d_ptr->m_pColorVisitor)
      return d_ptr->m_pColorVisitor->getIcon(account);
   else
      return account->roleData(role);
} //data

///Flags for "idx"
Qt::ItemFlags AccountModel::flags(const QModelIndex& idx) const
{
   if (idx.column() == 0)
      return QAbstractItemModel::flags(idx) | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
   return QAbstractItemModel::flags(idx);
}

///Number of account
int AccountModel::rowCount(const QModelIndex& parentIdx) const
{
   Q_UNUSED(parentIdx);
   return d_ptr->m_lAccounts.size();
}

Account* AccountModel::getAccountByModelIndex(const QModelIndex& item) const
{
   if (!item.isValid())
      return nullptr;
   return d_ptr->m_lAccounts[item.row()];
}

AccountListColorVisitor* AccountModel::colorVisitor()
{
   return d_ptr->m_pColorVisitor;
}

///Generate an unique suffix to prevent multiple account from sharing alias
QString AccountModel::getSimilarAliasIndex(const QString& alias)
{
   int count = 0;
   foreach (Account* a, instance()->d_ptr->m_lAccounts) {
      if (a->alias().left(alias.size()) == alias)
         count++;
   }
   bool found = true;
   do {
      found = false;
      foreach (Account* a, instance()->d_ptr->m_lAccounts) {
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

bool AccountModel::isPresenceEnabled() const
{
   foreach(Account* a, d_ptr->m_lAccounts) {
      if (a->presenceEnabled())
         return true;
   }
   return false;
}

bool AccountModel::isPresencePublishSupported() const
{
   foreach(Account* a,d_ptr->m_lAccounts) {
      if (a->supportPresencePublish())
         return true;
   }
   return false;
}

bool AccountModel::isPresenceSubscribeSupported() const
{
   foreach(Account* a,d_ptr->m_lAccounts) {
      if (a->supportPresenceSubscribe())
         return true;
   }
   return false;
}


/*****************************************************************************
 *                                                                           *
 *                                  Setters                                  *
 *                                                                           *
 ****************************************************************************/

///Add an account
Account* AccountModel::add(const QString& alias)
{
   Account* a = Account::buildNewAccountFromAlias(alias);
   connect(a,SIGNAL(changed(Account*)),d_ptr.data(),SLOT(slotAccountChanged(Account*)));
   d_ptr->m_lAccounts += a;
   connect(a,SIGNAL(presenceEnabledChanged(bool)),d_ptr.data(),SLOT(slotAccountPresenceEnabledChanged(bool)));

   emit dataChanged(index(d_ptr->m_lAccounts.size()-1,0), index(d_ptr->m_lAccounts.size()-1,0));
   return a;
}

///Remove an account
void AccountModel::remove(Account* account)
{
   if (not account) return;
   qDebug() << "Removing" << account->alias() << account->id();
   const int aindex = d_ptr->m_lAccounts.indexOf(account);
   d_ptr->m_lAccounts.remove(aindex);
   d_ptr->m_lDeletedAccounts << account->id();
   if (currentAccount() == account)
      setPriorAccount(getById(Account::ProtocolName::IP2IP));
   emit dataChanged(index(aindex,0), index(d_ptr->m_lAccounts.size()-1,0));
   emit layoutChanged();
   //delete account;
}

void AccountModel::remove(const QModelIndex& idx )
{
   remove(getAccountByModelIndex(idx));
}

///Set the previous account used
void AccountModel::setPriorAccount(const Account* account) {
   const bool changed = (account && m_spPriorAccount != account) || (!account && m_spPriorAccount);
   m_spPriorAccount = const_cast<Account*>(account);
   if (changed)
      emit priorAccountChanged(currentAccount());
}

///Set model data
bool AccountModel::setData(const QModelIndex& idx, const QVariant& value, int role)
{
   if (idx.isValid() && idx.column() == 0 && role == Qt::CheckStateRole) {
      const bool prevEnabled = d_ptr->m_lAccounts[idx.row()]->isEnabled();
      d_ptr->m_lAccounts[idx.row()]->setEnabled(value.toBool());
      emit dataChanged(idx, idx);
      if (prevEnabled != value.toBool())
         emit accountEnabledChanged(d_ptr->m_lAccounts[idx.row()]);
      emit dataChanged(idx, idx);
      return true;
   }
   else if ( role == Qt::EditRole ) {
      if (value.toString() != data(idx,Qt::EditRole)) {
         d_ptr->m_lAccounts[idx.row()]->setAlias(value.toString());
         emit dataChanged(idx, idx);
      }
   }
   return false;
}

///Set QAbstractItemModel BackgroundRole visitor
void AccountModel::setColorVisitor(AccountListColorVisitor* visitor)
{
   d_ptr->m_pColorVisitor = visitor;
}


/*****************************************************************************
 *                                                                           *
 *                                 Operator                                  *
 *                                                                           *
 ****************************************************************************/

///Get the account from its index
const Account* AccountModel::operator[] (int i) const
{
   return d_ptr->m_lAccounts[i];
}

///Get the account from its index
Account* AccountModel::operator[] (int i)
{
   return d_ptr->m_lAccounts[i];
}

///Get accoutn by id
Account* AccountModel::operator[] (const QString& i) {
   return getById(i);
}

void AccountModel::add(Account* acc)
{
   d_ptr->m_lAccounts << acc;
}

#include <accountmodel.moc>
