/****************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                          *
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
#include "account.h"

//Qt
#include <QtCore/QDebug>
#include <QtCore/QObject>
#include <QtCore/QString>

//SFLPhone
#include "sflphone_const.h"

//SFLPhone lib
#include "dbus/configurationmanager.h"
#include "dbus/callmanager.h"
#include "dbus/videomanager.h"
#include "visitors/accountlistcolorvisitor.h"
#include "certificate.h"
#include "accountmodel.h"
#include "credentialmodel.h"
#include "audio/codecmodel.h"
#include "video/codecmodel2.h"
#include "ringtonemodel.h"
#include "phonenumber.h"
#include "phonedirectorymodel.h"
#include "presencestatusmodel.h"
#include "uri.h"
#include "securityvalidationmodel.h"
#define TO_BOOL ?"true":"false"
#define IS_TRUE == "true"

typedef void (AccountPrivate::*account_function)();

class AccountPrivate : public QObject
{
public:
   Q_OBJECT
   Q_DECLARE_PUBLIC(Account)

   //Constructor
   AccountPrivate(Account* acc);

   //Attributes
   QString                 m_AccountId      ;
   QHash<QString,QString>  m_hAccountDetails;
   PhoneNumber*            m_pAccountNumber ;

   Account*                q_ptr       ;

   //Setters
   void setAccountDetails (const QHash<QString,QString>& m          );
   bool setAccountDetail  (const QString& param, const QString& val );

   //Getters
   const QString accountDetail(const QString& param) const;

   //Helpers
   inline void changeState(Account::EditState state);

   //State actions
   void performAction(Account::EditAction action);
   void nothing() {};
   void edit()    {changeState(Account::EditState::EDITING );};
   void modify()  {changeState(Account::EditState::MODIFIED);};
   void remove()  {changeState(Account::EditState::REMOVED );};
   void cancel()  {changeState(Account::EditState::READY   );};
   void outdate() {changeState(Account::EditState::OUTDATED);};
   void reload();
   void save();
   void reloadMod() {reload();modify();};

   CredentialModel*          m_pCredentials     ;
   Audio::CodecModel*        m_pAudioCodecs     ;
   Video::CodecModel2*        m_pVideoCodecs     ;
   RingToneModel*            m_pRingToneModel   ;
   KeyExchangeModel*         m_pKeyExchangeModel;
   SecurityValidationModel*  m_pSecurityValidationModel;
   Account::EditState m_CurrentState;

   // State machines
   static const account_function stateMachineActionsOnState[6][7];

   //Cached account details (as they are called too often for the hash)
   mutable QString      m_HostName;
   mutable QString      m_LastErrorMessage;
   mutable int          m_LastErrorCode;
   mutable int          m_VoiceMailCount;
   mutable Certificate* m_pCaCert;
   mutable Certificate* m_pTlsCert;
   mutable Certificate* m_pPrivateKey;

public Q_SLOTS:
      void slotPresentChanged        (bool  present  );
      void slotPresenceMessageChanged(const QString& );
      void slotUpdateCertificate     (               );
};

#define AP &AccountPrivate
const account_function AccountPrivate::stateMachineActionsOnState[6][7] = {
/*               NOTHING        EDIT         RELOAD        SAVE        REMOVE      MODIFY         CANCEL       */
/*READY    */{ AP::nothing, AP::edit   , AP::reload , AP::nothing, AP::remove , AP::modify   , AP::nothing },/**/
/*EDITING  */{ AP::nothing, AP::nothing, AP::outdate, AP::nothing, AP::remove , AP::modify   , AP::cancel  },/**/
/*OUTDATED */{ AP::nothing, AP::nothing, AP::nothing, AP::nothing, AP::remove , AP::reloadMod, AP::reload  },/**/
/*NEW      */{ AP::nothing, AP::nothing, AP::nothing, AP::save   , AP::remove , AP::nothing  , AP::nothing },/**/
/*MODIFIED */{ AP::nothing, AP::nothing, AP::nothing, AP::save   , AP::remove , AP::nothing  , AP::reload  },/**/
/*REMOVED  */{ AP::nothing, AP::nothing, AP::nothing, AP::nothing, AP::nothing, AP::nothing  , AP::cancel  } /**/
/*                                                                                                                                                       */
};
#undef AP

AccountPrivate::AccountPrivate(Account* acc) : QObject(acc),q_ptr(acc),m_pCredentials(nullptr),m_pAudioCodecs(nullptr),
m_pVideoCodecs(nullptr),m_LastErrorCode(-1),m_VoiceMailCount(0),m_pRingToneModel(nullptr),
m_CurrentState(Account::EditState::READY),
m_pAccountNumber(nullptr),m_pKeyExchangeModel(nullptr),m_pSecurityValidationModel(nullptr),m_pCaCert(nullptr),m_pTlsCert(nullptr),
m_pPrivateKey(nullptr)
{
   Q_Q(Account);
}

void AccountPrivate::changeState(Account::EditState state) {
   Q_Q(Account);
   m_CurrentState = state;
   emit q_ptr->changed(q_ptr);
}


///Constructors
Account::Account():QObject(AccountModel::instance()),d_ptr(new AccountPrivate(this))

{
}

///Build an account from it'id
Account* Account::buildExistingAccountFromId(const QString& _accountId)
{
//    qDebug() << "Building an account from id: " << _accountId;
   Account* a = new Account();
   a->d_ptr->m_AccountId = _accountId;
   a->d_ptr->setObjectName(_accountId);

   a->performAction(EditAction::RELOAD);

   return a;
} //buildExistingAccountFromId

///Build an account from it's name / alias
Account* Account::buildNewAccountFromAlias(const QString& alias)
{
   qDebug() << "Building an account from alias: " << alias;
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   Account* a = new Account();
   a->d_ptr->m_hAccountDetails.clear();
   a->d_ptr->m_hAccountDetails[Account::MapField::ENABLED] = "false";
   a->d_ptr->m_pAccountNumber = const_cast<PhoneNumber*>(PhoneNumber::BLANK());
   MapStringString tmp = configurationManager.getAccountTemplate();
   QMutableMapIterator<QString, QString> iter(tmp);
   while (iter.hasNext()) {
      iter.next();
      a->d_ptr->m_hAccountDetails[iter.key()] = iter.value();
   }
   a->setHostname(a->d_ptr->m_hAccountDetails[Account::MapField::HOSTNAME]);
   a->d_ptr->setAccountDetail(Account::MapField::ALIAS,alias);
   a->setObjectName(a->id());
   return a;
}

///Destructor
Account::~Account()
{
   disconnect();
   if (d_ptr->m_pCredentials) delete d_ptr->m_pCredentials ;
   if (d_ptr->m_pAudioCodecs) delete d_ptr->m_pAudioCodecs ;
}


/*****************************************************************************
 *                                                                           *
 *                                   Slots                                   *
 *                                                                           *
 ****************************************************************************/

void AccountPrivate::slotPresentChanged(bool present)
{
   Q_UNUSED(present)
   emit q_ptr->changed(q_ptr);
}

void AccountPrivate::slotPresenceMessageChanged(const QString& message)
{
   Q_UNUSED(message)
   emit q_ptr->changed(q_ptr);
}

void AccountPrivate::slotUpdateCertificate()
{
   Certificate* cert = qobject_cast<Certificate*>(sender());
   if (cert) {
      switch (cert->type()) {
         case Certificate::Type::AUTHORITY:
            if (accountDetail(Account::MapField::TLS::CA_LIST_FILE) != cert->path().toString())
               setAccountDetail(Account::MapField::TLS::CA_LIST_FILE, cert->path().toString());
            break;
         case Certificate::Type::USER:
            if (accountDetail(Account::MapField::TLS::CERTIFICATE_FILE) != cert->path().toString())
               setAccountDetail(Account::MapField::TLS::CERTIFICATE_FILE, cert->path().toString());
            break;
         case Certificate::Type::PRIVATE_KEY:
            if (accountDetail(Account::MapField::TLS::PRIVATE_KEY_FILE) != cert->path().toString())
               setAccountDetail(Account::MapField::TLS::PRIVATE_KEY_FILE, cert->path().toString());
            break;
         case Certificate::Type::NONE:
            break;
      };
   }
}

/*****************************************************************************
 *                                                                           *
 *                                  Getters                                  *
 *                                                                           *
 ****************************************************************************/

///IS this account new
bool Account::isNew() const
{
   return (d_ptr->m_AccountId == nullptr) || d_ptr->m_AccountId.isEmpty();
}

///Get this account ID
const QString Account::id() const
{
   if (isNew()) {
      qDebug() << "Error : getting AccountId of a new account.";
   }
   if (d_ptr->m_AccountId.isEmpty()) {
      qDebug() << "Account not configured";
      return QString(); //WARNING May explode
   }

   return d_ptr->m_AccountId;
}

///Get current state
const QString Account::toHumanStateName() const
{
   const QString s = d_ptr->m_hAccountDetails[Account::MapField::Registration::STATUS];

   static const QString registered             = tr("Registered"               );
   static const QString notRegistered          = tr("Not Registered"           );
   static const QString trying                 = tr("Trying..."                );
   static const QString error                  = tr("Error"                    );
   static const QString authenticationFailed   = tr("Authentication Failed"    );
   static const QString networkUnreachable     = tr("Network unreachable"      );
   static const QString hostUnreachable        = tr("Host unreachable"         );
   static const QString stunConfigurationError = tr("Stun configuration error" );
   static const QString stunServerInvalid      = tr("Stun server invalid"      );
   static const QString serviceUnavailable     = tr("Service unavailable"      );
   static const QString notAcceptable          = tr("Unacceptable"             );
   static const QString invalid                = tr("Invalid"                  );
   static const QString requestTimeout         = tr("Request Timeout"          );

   if(s == Account::State::REGISTERED       )
      return registered             ;
   if(s == Account::State::UNREGISTERED     )
      return notRegistered          ;
   if(s == Account::State::TRYING           )
      return trying                 ;
   if(s == Account::State::ERROR            )
      return d_ptr->m_LastErrorMessage.isEmpty()?error:d_ptr->m_LastErrorMessage;
   if(s == Account::State::ERROR_AUTH       )
      return authenticationFailed   ;
   if(s == Account::State::ERROR_NETWORK    )
      return networkUnreachable     ;
   if(s == Account::State::ERROR_HOST       )
      return hostUnreachable        ;
   if(s == Account::State::ERROR_CONF_STUN  )
      return stunConfigurationError ;
   if(s == Account::State::ERROR_EXIST_STUN )
      return stunServerInvalid      ;
   if(s == Account::State::ERROR_SERVICE_UNAVAILABLE )
      return serviceUnavailable     ;
   if(s == Account::State::ERROR_NOT_ACCEPTABLE      )
      return notAcceptable          ;
   if(s == Account::State::REQUEST_TIMEOUT           )
      return requestTimeout         ;
   return invalid                   ;
}

///Get an account detail
const QString AccountPrivate::accountDetail(const QString& param) const
{
   if (!m_hAccountDetails.size()) {
      qDebug() << "The account details is not set";
      return QString(); //May crash, but better than crashing now
   }
   if (m_hAccountDetails.find(param) != m_hAccountDetails.end()) {
      return m_hAccountDetails[param];
   }
   else if (m_hAccountDetails.count() > 0) {
      if (param == Account::MapField::ENABLED) //If an account is invalid, at least does not try to register it
         return Account::RegistrationEnabled::NO;
      if (param == Account::MapField::Registration::STATUS) //If an account is new, then it is unregistered
         return Account::State::UNREGISTERED;
      if (q_ptr->protocol() != Account::Protocol::IAX) //IAX accounts lack some fields, be quiet
         qDebug() << "Account parameter \"" << param << "\" not found";
      return QString();
   }
   else {
      qDebug() << "Account details not found, there is " << m_hAccountDetails.count() << " details available";
      return QString();
   }
} //accountDetail

///Get the alias
const QString Account::alias() const
{
   return d_ptr->accountDetail(Account::MapField::ALIAS);
}

///Is this account registered
bool Account::isRegistered() const
{
   return (d_ptr->accountDetail(Account::MapField::Registration::STATUS) == Account::State::REGISTERED);
}

///Return the model index of this item
QModelIndex Account::index()
{
   for (int i=0;i < AccountModel::instance()->size();i++) {
      if (this == (*AccountModel::instance())[i]) {
         return AccountModel::instance()->index(i,0);
      }
   }
   return QModelIndex();
}

///Return status color name
QString Account::stateColorName() const
{
   if(registrationStatus() == Account::State::UNREGISTERED)
      return "black";
   if(registrationStatus() == Account::State::REGISTERED || registrationStatus() == Account::State::READY)
      return "darkGreen";
   return "red";
}

///Return status Qt color, QColor is not part of QtCore, use using the global variant
QVariant Account::stateColor() const
{
   if (AccountModel::instance()->colorVisitor()) {
      return AccountModel::instance()->colorVisitor()->getColor(this);
   }
   return QVariant();
}

///Create and return the credential model
CredentialModel* Account::credentialsModel() const
{
   if (!d_ptr->m_pCredentials)
      const_cast<Account*>(this)->reloadCredentials();
   return d_ptr->m_pCredentials;
}

///Create and return the audio codec model
Audio::CodecModel* Account::audioCodecModel() const
{
   if (!d_ptr->m_pAudioCodecs)
      const_cast<Account*>(this)->reloadAudioCodecs();
   return d_ptr->m_pAudioCodecs;
}

///Create and return the video codec model
Video::CodecModel2* Account::videoCodecModel() const
{
   if (!d_ptr->m_pVideoCodecs)
      d_ptr->m_pVideoCodecs = new Video::CodecModel2(const_cast<Account*>(this));
   return d_ptr->m_pVideoCodecs;
}

RingToneModel* Account::ringToneModel() const
{
   if (!d_ptr->m_pRingToneModel)
      d_ptr->m_pRingToneModel = new RingToneModel(const_cast<Account*>(this));
   return d_ptr->m_pRingToneModel;
}

KeyExchangeModel* Account::keyExchangeModel() const
{
   if (!d_ptr->m_pKeyExchangeModel) {
      d_ptr->m_pKeyExchangeModel = new KeyExchangeModel(const_cast<Account*>(this));
   }
   return d_ptr->m_pKeyExchangeModel;
}

SecurityValidationModel* Account::securityValidationModel() const
{
   if (!d_ptr->m_pSecurityValidationModel) {
      d_ptr->m_pSecurityValidationModel = new SecurityValidationModel(const_cast<Account*>(this));
   }
   return d_ptr->m_pSecurityValidationModel;
}

void Account::setAlias(const QString& detail)
{
   const bool accChanged = detail != alias();
   d_ptr->setAccountDetail(Account::MapField::ALIAS,detail);
   if (accChanged)
      emit aliasChanged(detail);
}

///Return the account hostname
QString Account::hostname() const
{
   return d_ptr->m_HostName;
}

///Return if the account is enabled
bool Account::isEnabled() const
{
   return d_ptr->accountDetail(Account::MapField::ENABLED) IS_TRUE;
}

///Return if the account should auto answer
bool Account::isAutoAnswer() const
{
   return d_ptr->accountDetail(Account::MapField::AUTOANSWER) IS_TRUE;
}

///Return the account user name
QString Account::username() const
{
   return d_ptr->accountDetail(Account::MapField::USERNAME);
}

///Return the account mailbox address
QString Account::mailbox() const
{
   return d_ptr->accountDetail(Account::MapField::MAILBOX);
}

///Return the account mailbox address
QString Account::proxy() const
{
   return d_ptr->accountDetail(Account::MapField::ROUTE);
}


QString Account::password() const
{
   switch (protocol()) {
      case Account::Protocol::SIP:
         if (credentialsModel()->rowCount())
            return credentialsModel()->data(credentialsModel()->index(0,0),CredentialModel::Role::PASSWORD).toString();
      case Account::Protocol::IAX:
         return d_ptr->accountDetail(Account::MapField::PASSWORD);
   };
   return "";
}

///
bool Account::isDisplaySasOnce() const
{
   return d_ptr->accountDetail(Account::MapField::ZRTP::DISPLAY_SAS_ONCE) IS_TRUE;
}

///Return the account security fallback
bool Account::isSrtpRtpFallback() const
{
   return d_ptr->accountDetail(Account::MapField::SRTP::RTP_FALLBACK) IS_TRUE;
}

//Return if SRTP is enabled or not
bool Account::isSrtpEnabled() const
{
   return d_ptr->accountDetail(Account::MapField::SRTP::ENABLED) IS_TRUE;
}

///
bool Account::isZrtpDisplaySas         () const
{
   return d_ptr->accountDetail(Account::MapField::ZRTP::DISPLAY_SAS) IS_TRUE;
}

///Return if the other side support warning
bool Account::isZrtpNotSuppWarning() const
{
   return d_ptr->accountDetail(Account::MapField::ZRTP::NOT_SUPP_WARNING) IS_TRUE;
}

///
bool Account::isZrtpHelloHash() const
{
   return d_ptr->accountDetail(Account::MapField::ZRTP::HELLO_HASH) IS_TRUE;
}

///Return if the account is using a STUN server
bool Account::isSipStunEnabled() const
{
   return d_ptr->accountDetail(Account::MapField::STUN::ENABLED) IS_TRUE;
}

///Return the account STUN server
QString Account::sipStunServer() const
{
   return d_ptr->accountDetail(Account::MapField::STUN::SERVER);
}

///Return when the account expire (require renewal)
int Account::registrationExpire() const
{
   return d_ptr->accountDetail(Account::MapField::Registration::EXPIRE).toInt();
}

///Return if the published address is the same as the local one
bool Account::isPublishedSameAsLocal() const
{
   return d_ptr->accountDetail(Account::MapField::PUBLISHED_SAMEAS_LOCAL) IS_TRUE;
}

///Return the account published address
QString Account::publishedAddress() const
{
   return d_ptr->accountDetail(Account::MapField::PUBLISHED_ADDRESS);
}

///Return the account published port
int Account::publishedPort() const
{
   return d_ptr->accountDetail(Account::MapField::PUBLISHED_PORT).toUInt();
}

///Return the account tls password
QString Account::tlsPassword() const
{
   return d_ptr->accountDetail(Account::MapField::TLS::PASSWORD);
}

///Return the account TLS port
int Account::tlsListenerPort() const
{
   return d_ptr->accountDetail(Account::MapField::TLS::LISTENER_PORT).toInt();
}

///Return the account TLS certificate authority list file
Certificate* Account::tlsCaListCertificate() const
{
   if (!d_ptr->m_pCaCert) {
      d_ptr->m_pCaCert = new Certificate(Certificate::Type::AUTHORITY,this);
      connect(d_ptr->m_pCaCert,SIGNAL(changed()),d_ptr.data(),SLOT(slotUpdateCertificate()));
   }
   const_cast<Account*>(this)->d_ptr->m_pCaCert->setPath(d_ptr->accountDetail(Account::MapField::TLS::CA_LIST_FILE));
   return d_ptr->m_pCaCert;
}

///Return the account TLS certificate
Certificate* Account::tlsCertificate() const
{
   if (!d_ptr->m_pTlsCert) {
      d_ptr->m_pTlsCert = new Certificate(Certificate::Type::USER,this);
      connect(d_ptr->m_pTlsCert,SIGNAL(changed()),d_ptr.data(),SLOT(slotUpdateCertificate()));
   }
   const_cast<Account*>(this)->d_ptr->m_pTlsCert->setPath(d_ptr->accountDetail(Account::MapField::TLS::CERTIFICATE_FILE));
   return d_ptr->m_pTlsCert;
}

///Return the account private key
Certificate* Account::tlsPrivateKeyCertificate() const
{
   if (!d_ptr->m_pPrivateKey) {
      d_ptr->m_pPrivateKey = new Certificate(Certificate::Type::PRIVATE_KEY,this);
      connect(d_ptr->m_pPrivateKey,SIGNAL(changed()),d_ptr.data(),SLOT(slotUpdateCertificate()));
   }
   const_cast<Account*>(this)->d_ptr->m_pPrivateKey->setPath(d_ptr->accountDetail(Account::MapField::TLS::PRIVATE_KEY_FILE));
   return d_ptr->m_pPrivateKey;
}

///Return the account cipher
QString Account::tlsCiphers() const
{
   return d_ptr->accountDetail(Account::MapField::TLS::CIPHERS);
}

///Return the account TLS server name
QString Account::tlsServerName() const
{
   return d_ptr->accountDetail(Account::MapField::TLS::SERVER_NAME);
}

///Return the account negotiation timeout in seconds
int Account::tlsNegotiationTimeoutSec() const
{
   return d_ptr->accountDetail(Account::MapField::TLS::NEGOTIATION_TIMEOUT_SEC).toInt();
}

///Return the account negotiation timeout in milliseconds
int Account::tlsNegotiationTimeoutMsec() const
{
   return d_ptr->accountDetail(Account::MapField::TLS::NEGOTIATION_TIMEOUT_MSEC).toInt();
}

///Return the account TLS verify server
bool Account::isTlsVerifyServer() const
{
   return (d_ptr->accountDetail(Account::MapField::TLS::VERIFY_SERVER) IS_TRUE);
}

///Return the account TLS verify client
bool Account::isTlsVerifyClient() const
{
   return (d_ptr->accountDetail(Account::MapField::TLS::VERIFY_CLIENT) IS_TRUE);
}

///Return if it is required for the peer to have a certificate
bool Account::isTlsRequireClientCertificate() const
{
   return (d_ptr->accountDetail(Account::MapField::TLS::REQUIRE_CLIENT_CERTIFICATE) IS_TRUE);
}

///Return the account TLS security is enabled
bool Account::isTlsEnabled() const
{ 
   return (d_ptr->accountDetail(Account::MapField::TLS::ENABLED) IS_TRUE);
}

///Return the account the TLS encryption method
TlsMethodModel::Type Account::tlsMethod() const
{
   const QString value = d_ptr->accountDetail(Account::MapField::TLS::METHOD);
   return TlsMethodModel::fromDaemonName(value);
}

///Return the key exchange mechanism
KeyExchangeModel::Type Account::keyExchange() const
{
   return KeyExchangeModel::fromDaemonName(d_ptr->accountDetail(Account::MapField::SRTP::KEY_EXCHANGE));
}

///Return if the ringtone are enabled
bool Account::isRingtoneEnabled() const
{
   return (d_ptr->accountDetail(Account::MapField::Ringtone::ENABLED) IS_TRUE);
}

///Return the account ringtone path
QString Account::ringtonePath() const
{
   return d_ptr->accountDetail(Account::MapField::Ringtone::PATH);
}

///Return the last error message received
QString Account::lastErrorMessage() const
{
   return d_ptr->m_LastErrorMessage;
}

///Return the last error code (useful for debugging)
int Account::lastErrorCode() const
{
   return d_ptr->m_LastErrorCode;
}

///Return the account local port
int Account::localPort() const
{
   return d_ptr->accountDetail(Account::MapField::LOCAL_PORT).toInt();
}

///Return the number of voicemails
int Account::voiceMailCount() const
{
   return d_ptr->m_VoiceMailCount;
}

///Return the account local interface
QString Account::localInterface() const
{
   return d_ptr->accountDetail(Account::MapField::LOCAL_INTERFACE);
}

///Return the account registration status
QString Account::registrationStatus() const
{
   return d_ptr->accountDetail(Account::MapField::Registration::STATUS);
}

///Return the account type
Account::Protocol Account::protocol() const
{
   const QString str = d_ptr->accountDetail(Account::MapField::TYPE);
   if (str.isEmpty() || str == Account::ProtocolName::SIP)
      return Account::Protocol::SIP;
   else if (str == Account::ProtocolName::IAX)
      return Account::Protocol::IAX;
   qDebug() << "Warning: unhandled protocol name" << str << ", defaulting to SIP";
   return Account::Protocol::SIP;
}

///Return the DTMF type
DtmfType Account::DTMFType() const
{
   QString type = d_ptr->accountDetail(Account::MapField::DTMF_TYPE);
   return (type == "overrtp" || type.isEmpty())? DtmfType::OverRtp:DtmfType::OverSip;
}

bool Account::presenceStatus() const
{
   return d_ptr->m_pAccountNumber->isPresent();
}

QString Account::presenceMessage() const
{
   return d_ptr->m_pAccountNumber->presenceMessage();
}

bool Account::supportPresencePublish() const
{
   return d_ptr->accountDetail(Account::MapField::Presence::SUPPORT_PUBLISH) IS_TRUE;
}

bool Account::supportPresenceSubscribe() const
{
   return d_ptr->accountDetail(Account::MapField::Presence::SUPPORT_SUBSCRIBE) IS_TRUE;
}

bool Account::presenceEnabled() const
{
   return d_ptr->accountDetail(Account::MapField::Presence::ENABLED) IS_TRUE;
}

bool Account::isVideoEnabled() const
{
   return d_ptr->accountDetail(Account::MapField::Video::ENABLED) IS_TRUE;
}

int Account::videoPortMax() const
{
   return d_ptr->accountDetail(Account::MapField::Video::PORT_MAX).toInt();
}

int Account::videoPortMin() const
{
   return d_ptr->accountDetail(Account::MapField::Video::PORT_MIN).toInt();
}

int Account::audioPortMin() const
{
   return d_ptr->accountDetail(Account::MapField::Audio::PORT_MIN).toInt();
}

int Account::audioPortMax() const
{
   return d_ptr->accountDetail(Account::MapField::Audio::PORT_MAX).toInt();
}

QString Account::userAgent() const
{
   return d_ptr->accountDetail(Account::MapField::USER_AGENT);
}

QVariant Account::roleData(int role) const
{
   switch(role) {
      case Account::Role::Alias:
         return alias();
      case Account::Role::Proto:
         return static_cast<int>(protocol());
      case Account::Role::Hostname:
         return hostname();
      case Account::Role::Username:
         return username();
      case Account::Role::Mailbox:
         return mailbox();
      case Account::Role::Proxy:
         return proxy();
//       case Password:
//          return accountPassword();
      case Account::Role::TlsPassword:
         return tlsPassword();
      case Account::Role::TlsCaListCertificate:
         return tlsCaListCertificate()->path().toLocalFile();
      case Account::Role::TlsCertificate:
         return tlsCertificate()->path().toLocalFile();
      case Account::Role::TlsPrivateKeyCertificate:
         return tlsPrivateKeyCertificate()->path().toLocalFile();
      case Account::Role::TlsCiphers:
         return tlsCiphers();
      case Account::Role::TlsServerName:
         return tlsServerName();
      case Account::Role::SipStunServer:
         return sipStunServer();
      case Account::Role::PublishedAddress:
         return publishedAddress();
      case Account::Role::LocalInterface:
         return localInterface();
      case Account::Role::RingtonePath:
         return ringtonePath();
      case Account::Role::TlsMethod:
         return static_cast<int>(tlsMethod());
      case Account::Role::RegistrationExpire:
         return registrationExpire();
      case Account::Role::TlsNegotiationTimeoutSec:
         return tlsNegotiationTimeoutSec();
      case Account::Role::TlsNegotiationTimeoutMsec:
         return tlsNegotiationTimeoutMsec();
      case Account::Role::LocalPort:
         return localPort();
      case Account::Role::TlsListenerPort:
         return tlsListenerPort();
      case Account::Role::PublishedPort:
         return publishedPort();
      case Account::Role::Enabled:
         return isEnabled();
      case Account::Role::AutoAnswer:
         return isAutoAnswer();
      case Account::Role::TlsVerifyServer:
         return isTlsVerifyServer();
      case Account::Role::TlsVerifyClient:
         return isTlsVerifyClient();
      case Account::Role::TlsRequireClientCertificate:
         return isTlsRequireClientCertificate();
      case Account::Role::TlsEnabled:
         return isTlsEnabled();
      case Account::Role::DisplaySasOnce:
         return isDisplaySasOnce();
      case Account::Role::SrtpRtpFallback:
         return isSrtpRtpFallback();
      case Account::Role::ZrtpDisplaySas:
         return isZrtpDisplaySas();
      case Account::Role::ZrtpNotSuppWarning:
         return isZrtpNotSuppWarning();
      case Account::Role::ZrtpHelloHash:
         return isZrtpHelloHash();
      case Account::Role::SipStunEnabled:
         return isSipStunEnabled();
      case Account::Role::PublishedSameAsLocal:
         return isPublishedSameAsLocal();
      case Account::Role::RingtoneEnabled:
         return isRingtoneEnabled();
      case Account::Role::dTMFType:
         return DTMFType();
      case Account::Role::Id:
         return id();
      case Account::Role::Object: {
         QVariant var;
         var.setValue(const_cast<Account*>(this));
         return var;
      }
      case Account::Role::TypeName:
         return static_cast<int>(protocol());
      case Account::Role::PresenceStatus:
         return PresenceStatusModel::instance()->currentStatus();
      case Account::Role::PresenceMessage:
         return PresenceStatusModel::instance()->currentMessage();
      default:
         return QVariant();
   }
}


/*****************************************************************************
 *                                                                           *
 *                                  Setters                                  *
 *                                                                           *
 ****************************************************************************/

///Set account details
void AccountPrivate::setAccountDetails(const QHash<QString,QString>& m)
{
   m_hAccountDetails.clear();
   m_hAccountDetails = m;
   m_HostName = m[Account::MapField::HOSTNAME];
}

///Set a specific detail
bool AccountPrivate::setAccountDetail(const QString& param, const QString& val)
{
   const bool accChanged = m_hAccountDetails[param] != val;
   const QString buf = m_hAccountDetails[param];
   if (param == Account::MapField::Registration::STATUS) {
      m_hAccountDetails[param] = val;
      if (accChanged) {
         emit q_ptr->detailChanged(q_ptr,param,val,buf);
      }
   }
   else {
      q_ptr->performAction(Account::EditAction::MODIFY);
      if (m_CurrentState == Account::EditState::MODIFIED || m_CurrentState == Account::EditState::NEW) {
         m_hAccountDetails[param] = val;
         if (accChanged) {
            emit q_ptr->detailChanged(q_ptr,param,val,buf);
         }
      }
   }
   return m_CurrentState == Account::EditState::MODIFIED || m_CurrentState == Account::EditState::NEW;
}

///Set the account id
void Account::setId(const QString& id)
{
   qDebug() << "Setting accountId = " << d_ptr->m_AccountId;
   if (! isNew())
      qDebug() << "Error : setting AccountId of an existing account.";
   d_ptr->m_AccountId = id;
}

///Set the account type, SIP or IAX
void Account::setProtocol(Account::Protocol proto)
{
   switch (proto) {
      case Account::Protocol::SIP:
         d_ptr->setAccountDetail(Account::MapField::TYPE ,Account::ProtocolName::SIP);
         break;
      case Account::Protocol::IAX:
         d_ptr->setAccountDetail(Account::MapField::TYPE ,Account::ProtocolName::IAX);
         break;
   };
}

///The set account hostname, it can be an hostname or an IP address
void Account::setHostname(const QString& detail)
{
   if (d_ptr->m_HostName != detail) {
      d_ptr->m_HostName = detail;
      d_ptr->setAccountDetail(Account::MapField::HOSTNAME, detail);
   }
}

///Set the account username, everything is valid, some might be rejected by the PBX server
void Account::setUsername(const QString& detail)
{
   d_ptr->setAccountDetail(Account::MapField::USERNAME, detail);
}

///Set the account mailbox, usually a number, but can be anything
void Account::setMailbox(const QString& detail)
{
   d_ptr->setAccountDetail(Account::MapField::MAILBOX, detail);
}

///Set the account mailbox, usually a number, but can be anything
void Account::setProxy(const QString& detail)
{
   d_ptr->setAccountDetail(Account::MapField::ROUTE, detail);
}

///Set the main credential password
void Account::setPassword(const QString& detail)
{
   switch (protocol()) {
      case Account::Protocol::SIP:
         if (credentialsModel()->rowCount())
            credentialsModel()->setData(credentialsModel()->index(0,0),detail,CredentialModel::Role::PASSWORD);
         else {
            const QModelIndex idx = credentialsModel()->addCredentials();
            credentialsModel()->setData(idx,detail,CredentialModel::Role::PASSWORD);
         }
         break;
      case Account::Protocol::IAX:
         d_ptr->setAccountDetail(Account::MapField::PASSWORD, detail);
         break;
   };
}

///Set the TLS (encryption) password
void Account::setTlsPassword(const QString& detail)
{
   d_ptr->setAccountDetail(Account::MapField::TLS::PASSWORD, detail);
}

///Set the certificate authority list file
void Account::setTlsCaListCertificate(Certificate* cert)
{
   d_ptr->m_pCaCert = cert; //FIXME memory leak
   d_ptr->setAccountDetail(Account::MapField::TLS::CA_LIST_FILE, cert?cert->path().toLocalFile():QString());
}

///Set the certificate
void Account::setTlsCertificate(Certificate* cert)
{
   d_ptr->m_pTlsCert = cert; //FIXME memory leak
   d_ptr->setAccountDetail(Account::MapField::TLS::CERTIFICATE_FILE, cert?cert->path().toLocalFile():QString());
}

///Set the private key
void Account::setTlsPrivateKeyCertificate(Certificate* cert)
{
   d_ptr->m_pPrivateKey = cert; //FIXME memory leak
   d_ptr->setAccountDetail(Account::MapField::TLS::PRIVATE_KEY_FILE, cert?cert->path().toLocalFile():QString());
}

///Set the TLS cipher
void Account::setTlsCiphers(const QString& detail)
{
   d_ptr->setAccountDetail(Account::MapField::TLS::CIPHERS, detail);
}

///Set the TLS server
void Account::setTlsServerName(const QString& detail)
{
   d_ptr->setAccountDetail(Account::MapField::TLS::SERVER_NAME, detail);
}

///Set the stun server
void Account::setSipStunServer(const QString& detail)
{
   d_ptr->setAccountDetail(Account::MapField::STUN::SERVER, detail);
}

///Set the published address
void Account::setPublishedAddress(const QString& detail)
{
   d_ptr->setAccountDetail(Account::MapField::PUBLISHED_ADDRESS, detail);
}

///Set the local interface
void Account::setLocalInterface(const QString& detail)
{
   d_ptr->setAccountDetail(Account::MapField::LOCAL_INTERFACE, detail);
}

///Set the ringtone path, it have to be a valid absolute path
void Account::setRingtonePath(const QString& detail)
{
   d_ptr->setAccountDetail(Account::MapField::Ringtone::PATH, detail);
}

///Set the number of voice mails
void Account::setVoiceMailCount(int count)
{
   d_ptr->m_VoiceMailCount = count;
}

///Set the last error message to be displayed as status instead of "Error"
void Account::setLastErrorMessage(const QString& message)
{
   d_ptr->m_LastErrorMessage = message;
}

///Set the last error code
void Account::setLastErrorCode(int code)
{
   d_ptr->m_LastErrorCode = code;
}

///Set the Tls method
void Account::setTlsMethod(TlsMethodModel::Type detail)
{
   d_ptr->setAccountDetail(Account::MapField::TLS::METHOD ,TlsMethodModel::toDaemonName(detail));
}

///Set the Tls method
void Account::setKeyExchange(KeyExchangeModel::Type detail)
{
   d_ptr->setAccountDetail(Account::MapField::SRTP::KEY_EXCHANGE ,KeyExchangeModel::toDaemonName(detail));
}

///Set the account timeout, it will be renegotiated when that timeout occur
void Account::setRegistrationExpire(int detail)
{
   d_ptr->setAccountDetail(Account::MapField::Registration::EXPIRE, QString::number(detail));
}

///Set TLS negotiation timeout in second
void Account::setTlsNegotiationTimeoutSec(int detail)
{
   d_ptr->setAccountDetail(Account::MapField::TLS::NEGOTIATION_TIMEOUT_SEC, QString::number(detail));
}

///Set the TLS negotiation timeout in milliseconds
void Account::setTlsNegotiationTimeoutMsec(int detail)
{
   d_ptr->setAccountDetail(Account::MapField::TLS::NEGOTIATION_TIMEOUT_MSEC, QString::number(detail));
}

///Set the local port for SIP/IAX communications
void Account::setLocalPort(unsigned short detail)
{
   d_ptr->setAccountDetail(Account::MapField::LOCAL_PORT, QString::number(detail));
}

///Set the TLS listener port (0-2^16)
void Account::setTlsListenerPort(unsigned short detail)
{
   d_ptr->setAccountDetail(Account::MapField::TLS::LISTENER_PORT, QString::number(detail));
}

///Set the published port (0-2^16)
void Account::setPublishedPort(unsigned short detail)
{
   d_ptr->setAccountDetail(Account::MapField::PUBLISHED_PORT, QString::number(detail));
}

///Set if the account is enabled or not
void Account::setEnabled(bool detail)
{
   d_ptr->setAccountDetail(Account::MapField::ENABLED, (detail)TO_BOOL);
}

///Set if the account should auto answer
void Account::setAutoAnswer(bool detail)
{
   d_ptr->setAccountDetail(Account::MapField::AUTOANSWER, (detail)TO_BOOL);
}

///Set the TLS verification server
void Account::setTlsVerifyServer(bool detail)
{
   d_ptr->setAccountDetail(Account::MapField::TLS::VERIFY_SERVER, (detail)TO_BOOL);
}

///Set the TLS verification client
void Account::setTlsVerifyClient(bool detail)
{
   d_ptr->setAccountDetail(Account::MapField::TLS::VERIFY_CLIENT, (detail)TO_BOOL);
}

///Set if the peer need to be providing a certificate
void Account::setTlsRequireClientCertificate(bool detail)
{
   d_ptr->setAccountDetail(Account::MapField::TLS::REQUIRE_CLIENT_CERTIFICATE ,(detail)TO_BOOL);
}

///Set if the security settings are enabled
void Account::setTlsEnabled(bool detail)
{
   d_ptr->setAccountDetail(Account::MapField::TLS::ENABLED ,(detail)TO_BOOL);
}

void Account::setDisplaySasOnce(bool detail)
{
   d_ptr->setAccountDetail(Account::MapField::ZRTP::DISPLAY_SAS_ONCE, (detail)TO_BOOL);
}

void Account::setSrtpRtpFallback(bool detail)
{
   d_ptr->setAccountDetail(Account::MapField::SRTP::RTP_FALLBACK, (detail)TO_BOOL);
}

void Account::setSrtpEnabled(bool detail)
{
   d_ptr->setAccountDetail(Account::MapField::SRTP::ENABLED, (detail)TO_BOOL);
}

void Account::setZrtpDisplaySas(bool detail)
{
   d_ptr->setAccountDetail(Account::MapField::ZRTP::DISPLAY_SAS, (detail)TO_BOOL);
}

void Account::setZrtpNotSuppWarning(bool detail)
{
   d_ptr->setAccountDetail(Account::MapField::ZRTP::NOT_SUPP_WARNING, (detail)TO_BOOL);
}

void Account::setZrtpHelloHash(bool detail)
{
   d_ptr->setAccountDetail(Account::MapField::ZRTP::HELLO_HASH, (detail)TO_BOOL);
}

void Account::setSipStunEnabled(bool detail)
{
   d_ptr->setAccountDetail(Account::MapField::STUN::ENABLED, (detail)TO_BOOL);
}

void Account::setPublishedSameAsLocal(bool detail)
{
   d_ptr->setAccountDetail(Account::MapField::PUBLISHED_SAMEAS_LOCAL, (detail)TO_BOOL);
}

///Set if custom ringtone are enabled
void Account::setRingtoneEnabled(bool detail)
{
   d_ptr->setAccountDetail(Account::MapField::Ringtone::ENABLED, (detail)TO_BOOL);
}

void Account::setPresenceEnabled(bool enable)
{
   d_ptr->setAccountDetail(Account::MapField::Presence::ENABLED, (enable)TO_BOOL);
   emit presenceEnabledChanged(enable);
}

///Use video by default when available
void Account::setVideoEnabled(bool enable)
{
   d_ptr->setAccountDetail(Account::MapField::Video::ENABLED, (enable)TO_BOOL);
}

void Account::setAudioPortMax(int port )
{
   d_ptr->setAccountDetail(Account::MapField::Audio::PORT_MAX, QString::number(port));
}

void Account::setAudioPortMin(int port )
{
   d_ptr->setAccountDetail(Account::MapField::Audio::PORT_MIN, QString::number(port));
}

void Account::setVideoPortMax(int port )
{
   d_ptr->setAccountDetail(Account::MapField::Video::PORT_MAX, QString::number(port));
}

void Account::setVideoPortMin(int port )
{
   d_ptr->setAccountDetail(Account::MapField::Video::PORT_MIN, QString::number(port));
}

void Account::setUserAgent(const QString& agent)
{
   d_ptr->setAccountDetail(Account::MapField::USER_AGENT, agent);
}

///Set the DTMF type
void Account::setDTMFType(DtmfType type)
{
   d_ptr->setAccountDetail(Account::MapField::DTMF_TYPE,(type==OverRtp)?"overrtp":"oversip");
}

void Account::setRoleData(int role, const QVariant& value)
{
   switch(role) {
      case Account::Role::Alias:
         setAlias(value.toString());
         break;
      case Account::Role::Proto: {
         const int proto = value.toInt();
         setProtocol((proto>=0&&proto<=1)?static_cast<Account::Protocol>(proto):Account::Protocol::SIP);
         break;
      }
      case Account::Role::Hostname:
         setHostname(value.toString());
         break;
      case Account::Role::Username:
         setUsername(value.toString());
         break;
      case Account::Role::Mailbox:
         setMailbox(value.toString());
         break;
      case Account::Role::Proxy:
         setProxy(value.toString());
         break;
//       case Password:
//          accountPassword();
      case Account::Role::TlsPassword:
         setTlsPassword(value.toString());
         break;
      case Account::Role::TlsCaListCertificate: {
         const QString path = value.toString();
         if ((tlsCaListCertificate() && tlsCaListCertificate()->path() != QUrl(path)) || !tlsCaListCertificate()) {
            tlsCaListCertificate()->setPath(path);
         }
         break;
      }
      case Account::Role::TlsCertificate: {
         const QString path = value.toString();
         if ((tlsCertificate() && tlsCertificate()->path() != QUrl(path)) || !tlsCertificate())
            tlsCertificate()->setPath(path);
      }
         break;
      case Account::Role::TlsPrivateKeyCertificate: {
         const QString path = value.toString();
         if ((tlsPrivateKeyCertificate() && tlsPrivateKeyCertificate()->path() != QUrl(path)) || !tlsPrivateKeyCertificate())
            tlsPrivateKeyCertificate()->setPath(path);
      }
         break;
      case Account::Role::TlsCiphers:
         setTlsCiphers(value.toString());
         break;
      case Account::Role::TlsServerName:
         setTlsServerName(value.toString());
         break;
      case Account::Role::SipStunServer:
         setSipStunServer(value.toString());
         break;
      case Account::Role::PublishedAddress:
         setPublishedAddress(value.toString());
         break;
      case Account::Role::LocalInterface:
         setLocalInterface(value.toString());
         break;
      case Account::Role::RingtonePath:
         setRingtonePath(value.toString());
         break;
      case Account::Role::TlsMethod: {
         const int method = value.toInt();
         setTlsMethod(method<=TlsMethodModel::instance()->rowCount()?static_cast<TlsMethodModel::Type>(method):TlsMethodModel::Type::DEFAULT);
         break;
      }
      case Account::Role::KeyExchange: {
         const int method = value.toInt();
         setKeyExchange(method<=keyExchangeModel()->rowCount()?static_cast<KeyExchangeModel::Type>(method):KeyExchangeModel::Type::NONE);
         break;
      }
      case Account::Role::RegistrationExpire:
         setRegistrationExpire(value.toInt());
         break;
      case Account::Role::TlsNegotiationTimeoutSec:
         setTlsNegotiationTimeoutSec(value.toInt());
         break;
      case Account::Role::TlsNegotiationTimeoutMsec:
         setTlsNegotiationTimeoutMsec(value.toInt());
         break;
      case Account::Role::LocalPort:
         setLocalPort(value.toInt());
         break;
      case Account::Role::TlsListenerPort:
         setTlsListenerPort(value.toInt());
         break;
      case Account::Role::PublishedPort:
         setPublishedPort(value.toInt());
         break;
      case Account::Role::Enabled:
         setEnabled(value.toBool());
         break;
      case Account::Role::AutoAnswer:
         setAutoAnswer(value.toBool());
         break;
      case Account::Role::TlsVerifyServer:
         setTlsVerifyServer(value.toBool());
         break;
      case Account::Role::TlsVerifyClient:
         setTlsVerifyClient(value.toBool());
         break;
      case Account::Role::TlsRequireClientCertificate:
         setTlsRequireClientCertificate(value.toBool());
         break;
      case Account::Role::TlsEnabled:
         setTlsEnabled(value.toBool());
         break;
      case Account::Role::DisplaySasOnce:
         setDisplaySasOnce(value.toBool());
         break;
      case Account::Role::SrtpRtpFallback:
         setSrtpRtpFallback(value.toBool());
         break;
      case Account::Role::ZrtpDisplaySas:
         setZrtpDisplaySas(value.toBool());
         break;
      case Account::Role::ZrtpNotSuppWarning:
         setZrtpNotSuppWarning(value.toBool());
         break;
      case Account::Role::ZrtpHelloHash:
         setZrtpHelloHash(value.toBool());
         break;
      case Account::Role::SipStunEnabled:
         setSipStunEnabled(value.toBool());
         break;
      case Account::Role::PublishedSameAsLocal:
         setPublishedSameAsLocal(value.toBool());
         break;
      case Account::Role::RingtoneEnabled:
         setRingtoneEnabled(value.toBool());
         break;
      case Account::Role::dTMFType:
         setDTMFType((DtmfType)value.toInt());
         break;
      case Account::Role::Id:
         setId(value.toString());
         break;
   }
}


/*****************************************************************************
 *                                                                           *
 *                                  Mutator                                  *
 *                                                                           *
 ****************************************************************************/

void AccountPrivate::performAction(Account::EditAction action)
{
   (this->*(stateMachineActionsOnState[(int)m_CurrentState][(int)action]))();//FIXME don't use integer cast
}

bool Account::performAction(EditAction action)
{
   Account::EditState curState = d_ptr->m_CurrentState;
   d_ptr->performAction(action);
   return curState != d_ptr->m_CurrentState;
}

Account::EditState Account::state() const
{
   return d_ptr->m_CurrentState;
}

/**Update the account
 * @return if the state changed
 */
bool Account::updateState()
{
   if(! isNew()) {
      ConfigurationManagerInterface & configurationManager = DBus::ConfigurationManager::instance();
      const MapStringString details       = configurationManager.getAccountDetails(id()).value();
      const QString         status        = details[Account::MapField::Registration::STATUS];
      const QString         currentStatus = registrationStatus();
      d_ptr->setAccountDetail(Account::MapField::Registration::STATUS, status); //Update -internal- object state
      return status == currentStatus;
   }
   return true;
}

///Save the current account to the daemon
void AccountPrivate::save()
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   if (q_ptr->isNew()) {
      MapStringString details;
      QMutableHashIterator<QString,QString> iter(m_hAccountDetails);

      while (iter.hasNext()) {
         iter.next();
         details[iter.key()] = iter.value();
      }

      const QString currentId = configurationManager.addAccount(details);

      //Be sure there is audio codec enabled to avoid obscure error messages for the user
      const QVector<int> codecIdList = configurationManager.getAudioCodecList();
      foreach (const int aCodec, codecIdList) {
         const QStringList codec = configurationManager.getAudioCodecDetails(aCodec);
         const QModelIndex idx = m_pAudioCodecs->add();
         m_pAudioCodecs->setData(idx,codec[0],Audio::CodecModel::Role::NAME       );
         m_pAudioCodecs->setData(idx,codec[1],Audio::CodecModel::Role::SAMPLERATE );
         m_pAudioCodecs->setData(idx,codec[2],Audio::CodecModel::Role::BITRATE    );
         m_pAudioCodecs->setData(idx,aCodec  ,Audio::CodecModel::Role::ID         );
         m_pAudioCodecs->setData(idx, Qt::Checked ,Qt::CheckStateRole);
      }
      q_ptr->saveAudioCodecs();

      q_ptr->setId(currentId);
      q_ptr->saveCredentials();
   } //New account
   else { //Existing account
      MapStringString tmp;
      QMutableHashIterator<QString,QString> iter(m_hAccountDetails);

      while (iter.hasNext()) {
         iter.next();
         tmp[iter.key()] = iter.value();
      }
      configurationManager.setAccountDetails(q_ptr->id(), tmp);
   }

   if (!q_ptr->id().isEmpty()) {
      Account* acc =  AccountModel::instance()->getById(q_ptr->id());
      qDebug() << "Adding the new account to the account list (" << q_ptr->id() << ")";
      if (acc != q_ptr) {
         AccountModel::instance()->add(q_ptr);
      }

      q_ptr->performAction(Account::EditAction::RELOAD);
      q_ptr->updateState();
      m_CurrentState = Account::EditState::READY;
   }
   #ifdef ENABLE_VIDEO
   q_ptr->videoCodecModel()->save();
   #endif
   q_ptr->saveAudioCodecs();
   emit q_ptr->changed(q_ptr);
}

///sync with the daemon, this need to be done manually to prevent reloading the account while it is being edited
void AccountPrivate::reload()
{
   if (!q_ptr->isNew()) {
      if (m_hAccountDetails.size())
         qDebug() << "Reloading" << q_ptr->id() << q_ptr->alias();
      else
         qDebug() << "Loading" << q_ptr->id();
      ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
      QMap<QString,QString> aDetails = configurationManager.getAccountDetails(q_ptr->id());

      if (!aDetails.count()) {
         qDebug() << "Account not found";
      }
      else {
         m_hAccountDetails.clear();
         QMutableMapIterator<QString, QString> iter(aDetails);
         while (iter.hasNext()) {
            iter.next();
            m_hAccountDetails[iter.key()] = iter.value();
         }
         q_ptr->setHostname(m_hAccountDetails[Account::MapField::HOSTNAME]);
      }
      m_CurrentState = Account::EditState::READY;

      const QString currentUri = QString("%1@%2").arg(q_ptr->username()).arg(m_HostName);
      if (!m_pAccountNumber || (m_pAccountNumber && m_pAccountNumber->uri() != currentUri)) {
         if (m_pAccountNumber) {
            disconnect(m_pAccountNumber,SIGNAL(presenceMessageChanged(QString)),this,SLOT(slotPresenceMessageChanged(QString)));
            disconnect(m_pAccountNumber,SIGNAL(presentChanged(bool)),this,SLOT(slotPresentChanged(bool)));
         }
         m_pAccountNumber = PhoneDirectoryModel::instance()->getNumber(currentUri,q_ptr);
         m_pAccountNumber->setType(PhoneNumber::Type::ACCOUNT);
         connect(m_pAccountNumber,SIGNAL(presenceMessageChanged(QString)),this,SLOT(slotPresenceMessageChanged(QString)));
         connect(m_pAccountNumber,SIGNAL(presentChanged(bool)),this,SLOT(slotPresentChanged(bool)));
      }

      //If the credential model is loaded, then update it
      if (m_pCredentials)
         q_ptr->reloadCredentials();
      emit q_ptr->changed(q_ptr);
   }
}

///Reload credentials from DBUS
void Account::reloadCredentials()
{
   if (!d_ptr->m_pCredentials) {
      d_ptr->m_pCredentials = new CredentialModel(this);
   }
   if (!isNew()) {
      d_ptr->m_pCredentials->clear();
      ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
      VectorMapStringString credentials = configurationManager.getCredentials(id());
      for (int i=0; i < credentials.size(); i++) {
         QModelIndex idx = d_ptr->m_pCredentials->addCredentials();
         d_ptr->m_pCredentials->setData(idx,credentials[i][ Account::MapField::USERNAME ],CredentialModel::Role::NAME    );
         d_ptr->m_pCredentials->setData(idx,credentials[i][ Account::MapField::PASSWORD ],CredentialModel::Role::PASSWORD);
         d_ptr->m_pCredentials->setData(idx,credentials[i][ Account::MapField::REALM    ],CredentialModel::Role::REALM   );
      }
   }
}

///Save all credentials
void Account::saveCredentials() {
   if (d_ptr->m_pCredentials) {
      ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
      VectorMapStringString toReturn;
      for (int i=0; i < d_ptr->m_pCredentials->rowCount();i++) {
         QModelIndex idx = d_ptr->m_pCredentials->index(i,0);
         MapStringString credentialData;
         QString user = d_ptr->m_pCredentials->data(idx,CredentialModel::Role::NAME).toString();
         QString realm = d_ptr->m_pCredentials->data(idx,CredentialModel::Role::REALM).toString();
         if (user.isEmpty()) {
            user = username();
            d_ptr->m_pCredentials->setData(idx,user,CredentialModel::Role::NAME);
         }
         if (realm.isEmpty()) {
            realm = '*';
            d_ptr->m_pCredentials->setData(idx,realm,CredentialModel::Role::REALM);
         }
         credentialData[ Account::MapField::USERNAME ] = user;
         credentialData[ Account::MapField::PASSWORD ] = d_ptr->m_pCredentials->data(idx,CredentialModel::Role::PASSWORD).toString();
         credentialData[ Account::MapField::REALM    ] = realm;
         toReturn << credentialData;
      }
      configurationManager.setCredentials(id(),toReturn);
   }
}

///Reload all audio codecs
void Account::reloadAudioCodecs()
{
   if (!d_ptr->m_pAudioCodecs) {
      d_ptr->m_pAudioCodecs = new Audio::CodecModel(this);
   }
   d_ptr->m_pAudioCodecs->reload();
}

///Save audio codecs
void Account::saveAudioCodecs() {
   if (d_ptr->m_pAudioCodecs)
      d_ptr->m_pAudioCodecs->save();
}

/*****************************************************************************
 *                                                                           *
 *                                 Operator                                  *
 *                                                                           *
 ****************************************************************************/

///Are both account the same
bool Account::operator==(const Account& a)const
{
   return d_ptr->m_AccountId == a.d_ptr->m_AccountId;
}

/*****************************************************************************
 *                                                                           *
 *                                   Video                                   *
 *                                                                           *
 ****************************************************************************/

#undef TO_BOOL
#undef IS_TRUE
#include <account.moc>
