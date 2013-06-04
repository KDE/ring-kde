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
#include "account.h"

//Qt
#include <QtCore/QDebug>
#include <QtCore/QString>

//SFLPhone
#include "sflphone_const.h"

//SFLPhone lib
#include "dbus/configurationmanager.h"
#include "dbus/callmanager.h"
#include "dbus/videomanager.h"
#include "accountlist.h"
#include "credentialmodel.h"
#include "audiocodecmodel.h"
#include "videocodecmodel.h"

const account_function Account::stateMachineActionsOnState[6][7] = {
/*                 NOTHING              EDIT              RELOAD              SAVE               REMOVE             MODIFY             CANCEL            */
/*READY    */{ &Account::nothing, &Account::edit   , &Account::reload , &Account::nothing, &Account::remove , &Account::modify   , &Account::nothing },/**/
/*EDITING  */{ &Account::nothing, &Account::nothing, &Account::outdate, &Account::nothing, &Account::remove , &Account::modify   , &Account::cancel  },/**/
/*OUTDATED */{ &Account::nothing, &Account::nothing, &Account::nothing, &Account::nothing, &Account::remove , &Account::reloadMod, &Account::reload  },/**/
/*NEW      */{ &Account::nothing, &Account::nothing, &Account::nothing, &Account::save   , &Account::remove , &Account::nothing  , &Account::nothing },/**/
/*MODIFIED */{ &Account::nothing, &Account::nothing, &Account::nothing, &Account::save   , &Account::remove , &Account::nothing  , &Account::reload  },/**/
/*REMOVED  */{ &Account::nothing, &Account::nothing, &Account::nothing, &Account::nothing, &Account::nothing, &Account::nothing  , &Account::cancel  } /**/
/*                                                                                                                                                       */
};

///Match state name to user readable string
const QString& account_state_name(const QString& s)
{
   static const QString registered             = "Registered"               ;
   static const QString notRegistered          = "Not Registered"           ;
   static const QString trying                 = "Trying..."                ;
   static const QString error                  = "Error"                    ;
   static const QString authenticationFailed   = "Authentication Failed"    ;
   static const QString networkUnreachable     = "Network unreachable"      ;
   static const QString hostUnreachable        = "Host unreachable"         ;
   static const QString stunConfigurationError = "Stun configuration error" ;
   static const QString stunServerInvalid      = "Stun server invalid"      ;
   static const QString invalid                = "Invalid"                  ;
   
   if(s == QString(ACCOUNT_STATE_REGISTERED)       )
      return registered             ;
   if(s == QString(ACCOUNT_STATE_UNREGISTERED)     )
      return notRegistered          ;
   if(s == QString(ACCOUNT_STATE_TRYING)           )
      return trying                 ;
   if(s == QString(ACCOUNT_STATE_ERROR)            )
      return error                  ;
   if(s == QString(ACCOUNT_STATE_ERROR_AUTH)       )
      return authenticationFailed   ;
   if(s == QString(ACCOUNT_STATE_ERROR_NETWORK)    )
      return networkUnreachable     ;
   if(s == QString(ACCOUNT_STATE_ERROR_HOST)       )
      return hostUnreachable        ;
   if(s == QString(ACCOUNT_STATE_ERROR_CONF_STUN)  )
      return stunConfigurationError ;
   if(s == QString(ACCOUNT_STATE_ERROR_EXIST_STUN) )
      return stunServerInvalid      ;
   return invalid                   ;
} //account_state_name

///Constructors
Account::Account():m_pAccountId(nullptr),m_pCredentials(nullptr),m_pAudioCodecs(nullptr),m_CurrentState(READY),
m_pVideoCodecs(nullptr)
{
   CallManagerInterface& callManager = DBus::CallManager::instance();
   connect(&callManager,SIGNAL(registrationStateChanged(QString,QString,int)),this,SLOT(accountChanged(QString,QString,int)));
}

///Build an account from it'id
Account* Account::buildExistingAccountFromId(const QString& _accountId)
{
   qDebug() << "Building an account from id: " << _accountId;
   Account* a = new Account();
   a->m_pAccountId = new QString(_accountId);

   a->performAction(AccountEditAction::RELOAD);

   return a;
} //buildExistingAccountFromId

///Build an account from it's name / alias
Account* Account::buildNewAccountFromAlias(const QString& alias)
{
   qDebug() << "Building an account from alias: " << alias;
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   Account* a = new Account();
   a->m_hAccountDetails.clear();
   MapStringString tmp = configurationManager.getAccountTemplate();
   QMutableMapIterator<QString, QString> iter(tmp);
   while (iter.hasNext()) {
      iter.next();
      a->m_hAccountDetails[iter.key()] = iter.value();
   }
   a->setAccountHostname(a->m_hAccountDetails[ACCOUNT_HOSTNAME]);
   a->setAccountDetail(ACCOUNT_ALIAS,alias);
   return a;
}

///Destructor
Account::~Account()
{
   disconnect();
   delete m_pAccountId;
   if (m_pCredentials) delete m_pCredentials ;
   if (m_pAudioCodecs) delete m_pAudioCodecs ;
}


/*****************************************************************************
 *                                                                           *
 *                                   Slots                                   *
 *                                                                           *
 ****************************************************************************/

///Callback when the account state change
void Account::accountChanged(QString accountId,QString state,int)
{
   if (m_pAccountId && accountId == *m_pAccountId) {
      qDebug() << "Account" << m_pAccountId << "status changed";
      if (Account::updateState())
         emit stateChanged(getStateName(state));
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
   return (m_pAccountId == nullptr);
}

///Get this account ID
const QString Account::getAccountId() const
{
   if (isNew()) {
      qDebug() << "Error : getting AccountId of a new account.";
   }
   if (!m_pAccountId) {
      qDebug() << "Account not configured";
      return QString(); //WARNING May explode
   }
   
   return *m_pAccountId;
}

///Get current state
const QString Account::getStateName(const QString& state) const
{
   return (const QString&)account_state_name(state);
}

///Get an account detail
const QString Account::getAccountDetail(const QString& param) const
{
   if (!m_hAccountDetails.size()) {
      qDebug() << "The account list is not set";
      return QString(); //May crash, but better than crashing now
   }
   if (m_hAccountDetails.find(param) != m_hAccountDetails.end()) {
      return m_hAccountDetails[param];
   }
   else if (m_hAccountDetails.count() > 0) {
      if (param == "Account.enable") //If an account is invalid, at least does not try to register it
         return REGISTRATION_ENABLED_FALSE;
      qDebug() << "Account parameter \"" << param << "\" not found";
      return QString();
   }
   else {
      qDebug() << "Account details not found, there is " << m_hAccountDetails.count() << " details available";
      return QString();
   }
} //getAccountDetail

///Get the alias
const QString Account::getAlias() const
{
   return getAccountDetail(ACCOUNT_ALIAS);
}

///Is this account enabled
bool Account::isEnabled() const
{
   //getAccountRegistrationStatus() != ACCOUNT_STATE_UNREGISTERED should not happen, but it does, it is not managed because both client trying to 
   //set the state would fight to the daeth to set the enabled boolean
   return (getAccountDetail(ACCOUNT_ENABLED) == REGISTRATION_ENABLED_TRUE);
}

///Is this account registered
bool Account::isRegistered() const
{
   return (getAccountDetail(ACCOUNT_REGISTRATION_STATUS) == ACCOUNT_STATE_REGISTERED);
}

///Return the model index of this item
QModelIndex Account::getIndex()
{
   for (int i=0;i < AccountList::instance()->m_pAccounts->size();i++) {
      if (this == (*AccountList::instance()->m_pAccounts)[i]) {
         return AccountList::instance()->index(i,0);
      }
   }
   return QModelIndex();
}

///Return status color name
QString Account::getStateColorName() const
{
   if(getAccountRegistrationStatus() == ACCOUNT_STATE_UNREGISTERED)
      return "black";
   if(getAccountRegistrationStatus() == ACCOUNT_STATE_REGISTERED || getAccountRegistrationStatus() == ACCOUNT_STATE_READY)
      return "darkGreen";
   return "red";
}

///Return status Qt color, QColor is not part of QtCore, use using the global variant
Qt::GlobalColor Account::getStateColor() const
{
   if(getAccountRegistrationStatus() == ACCOUNT_STATE_UNREGISTERED)
      return Qt::darkGray  ;
   if(getAccountRegistrationStatus() == ACCOUNT_STATE_REGISTERED || getAccountRegistrationStatus() == ACCOUNT_STATE_READY)
      return Qt::darkGreen ;
   if(getAccountRegistrationStatus() == ACCOUNT_STATE_TRYING)
      return Qt::darkYellow;
   return Qt::darkRed;
}

///Create and return the credential model
CredentialModel* Account::getCredentialsModel()
{
   if (!m_pCredentials)
      reloadCredentials();
   return m_pCredentials;
}

///Create and return the audio codec model
AudioCodecModel* Account::getAudioCodecModel()
{
   if (!m_pAudioCodecs)
      reloadAudioCodecs();
   return m_pAudioCodecs;
}

///Create and return the video codec model
VideoCodecModel* Account::getVideoCodecModel()
{
   if (!m_pVideoCodecs)
      m_pVideoCodecs = new VideoCodecModel(this);
   return m_pVideoCodecs;
}

void Account::setAccountAlias(QString detail)
{
   bool accChanged = detail != getAccountAlias();
   setAccountDetail(ACCOUNT_ALIAS,detail);
   if (accChanged)
      emit aliasChanged(detail);
}

///Return the account hostname
QString Account::getAccountHostname() const
{
   return m_HostName;
}

///Return if the account is enabled
bool Account::isAccountEnabled() const 
{
   return (getAccountDetail(ACCOUNT_ENABLED)  == "true")?1:0;
}

///Return if the account should auto answer
bool Account::isAutoAnswer() const
{
   return (getAccountDetail(ACCOUNT_AUTOANSWER)  == "true")?1:0;
}

///Return the account user name
QString Account::getAccountUsername() const
{
   return getAccountDetail(ACCOUNT_USERNAME);
}

///Return the account mailbox address
QString Account::getAccountMailbox() const 
{
   return getAccountDetail(ACCOUNT_MAILBOX);
}

///Return the account mailbox address
QString Account::getAccountProxy() const
{
   return getAccountDetail(ACCOUNT_ROUTE);
}

///
bool Account::isAccountDisplaySasOnce() const
{ 
   return (getAccountDetail(ACCOUNT_DISPLAY_SAS_ONCE)  == "true")?1:0 ;
}

///Return the account security fallback
bool Account::isAccountSrtpRtpFallback() const
{
   return (getAccountDetail(ACCOUNT_SRTP_RTP_FALLBACK)  == "true")?1:0 ;
}

///
bool Account::isAccountZrtpDisplaySas         () const
{
   return (getAccountDetail(ACCOUNT_ZRTP_DISPLAY_SAS)  == "true")?1:0 ;
}

///Return if the other side support warning
bool Account::isAccountZrtpNotSuppWarning() const
{
   return (getAccountDetail(ACCOUNT_ZRTP_NOT_SUPP_WARNING) == "true")?1:0 ;
}

///
bool Account::isAccountZrtpHelloHash() const
{
   return (getAccountDetail(ACCOUNT_ZRTP_HELLO_HASH)  == "true")?1:0 ;
}

///Return if the account is using a STUN server
bool Account::isAccountSipStunEnabled() const
{
   return (getAccountDetail(ACCOUNT_SIP_STUN_ENABLED)  == "true")?1:0 ;
}

///Return the account STUN server
QString Account::getAccountSipStunServer() const
{
   return getAccountDetail(ACCOUNT_SIP_STUN_SERVER);
}

///Return when the account expire (require renewal)
int Account::getAccountRegistrationExpire() const
{
   return getAccountDetail(ACCOUNT_REGISTRATION_EXPIRE).toInt();
}

///Return if the published address is the same as the local one
bool Account::isPublishedSameAsLocal() const
{
   return (getAccountDetail(PUBLISHED_SAMEAS_LOCAL)  == "true")?1:0 ;
}

///Return the account published address
QString Account::getPublishedAddress() const
{
   return getAccountDetail(PUBLISHED_ADDRESS);
}

///Return the account published port
int Account::getPublishedPort() const
{
   return getAccountDetail(PUBLISHED_PORT).toUInt();
}

///Return the account tls password
QString Account::getTlsPassword() const
{
   return getAccountDetail(TLS_PASSWORD);
}

///Return the account TLS port
int Account::getTlsListenerPort() const
{
   return getAccountDetail(TLS_LISTENER_PORT).toInt();
}

///Return the account TLS certificate authority list file
QString Account::getTlsCaListFile() const
{
   return getAccountDetail(TLS_CA_LIST_FILE);
}

///Return the account TLS certificate
QString Account::getTlsCertificateFile() const
{
   return getAccountDetail(TLS_CERTIFICATE_FILE);
}

///Return the account private key
QString Account::getTlsPrivateKeyFile() const
{
   return getAccountDetail(TLS_PRIVATE_KEY_FILE);
}

///Return the account cipher
QString Account::getTlsCiphers() const
{
   return getAccountDetail(TLS_CIPHERS);
}

///Return the account TLS server name
QString Account::getTlsServerName() const
{
   return getAccountDetail(TLS_SERVER_NAME);
}

///Return the account negotiation timeout in seconds
int Account::getTlsNegotiationTimeoutSec() const
{
   return getAccountDetail(TLS_NEGOTIATION_TIMEOUT_SEC).toInt();
}

///Return the account negotiation timeout in milliseconds
int Account::getTlsNegotiationTimeoutMsec() const
{
   return getAccountDetail(TLS_NEGOTIATION_TIMEOUT_MSEC).toInt();
}

///Return the account TLS verify server
bool Account::isTlsVerifyServer() const
{
   return (getAccountDetail(TLS_VERIFY_SERVER)  == "true")?1:0;
}

///Return the account TLS verify client
bool Account::isTlsVerifyClient() const
{
   return (getAccountDetail(TLS_VERIFY_CLIENT)  == "true")?1:0;
}

///Return if it is required for the peer to have a certificate
bool Account::isTlsRequireClientCertificate() const
{
   return (getAccountDetail(TLS_REQUIRE_CLIENT_CERTIFICATE)  == "true")?1:0;
}

///Return the account TLS security is enabled
bool Account::isTlsEnable() const
{ 
   return (getAccountDetail(TLS_ENABLE)  == "true")?1:0;
}

///Return the account the TLS encryption method
int Account::getTlsMethod() const
{
   return getAccountDetail(TLS_METHOD).toInt();
}

///Return the account alias
QString Account::getAccountAlias() const
{
   return getAccountDetail(ACCOUNT_ALIAS);
}

///Return if the ringtone are enabled
bool Account::isRingtoneEnabled() const
{
   return (getAccountDetail(CONFIG_RINGTONE_ENABLED)  == "true")?1:0;
}

///Return the account ringtone path
QString Account::getRingtonePath() const
{
   return getAccountDetail(CONFIG_RINGTONE_PATH);
}

///Return the account local port
int Account::getLocalPort() const
{
   return getAccountDetail(LOCAL_PORT).toInt();
}

///Return the account local interface
QString Account::getLocalInterface() const
{
   return getAccountDetail(LOCAL_INTERFACE);
}

///Return the account registration status
QString Account::getAccountRegistrationStatus() const
{
   return getAccountDetail(ACCOUNT_REGISTRATION_STATUS);
}

///Return the account type
QString Account::getAccountType() const
{
   return getAccountDetail(ACCOUNT_TYPE);
}

///Return the DTMF type
DtmfType Account::getDTMFType() const
{
   QString type = getAccountDetail(ACCOUNT_DTMF_TYPE);
   return (type == "overrtp" || type.isEmpty())? DtmfType::OverRtp:DtmfType::OverSip;
}


/*****************************************************************************
 *                                                                           *
 *                                  Setters                                  *
 *                                                                           *
 ****************************************************************************/

///Set account details
void Account::setAccountDetails(const QHash<QString,QString>& m)
{
   m_hAccountDetails.clear();
   m_hAccountDetails = m;
   m_HostName = m[ACCOUNT_HOSTNAME];
}

///Set a specific detail
bool Account::setAccountDetail(const QString& param, const QString& val)
{
   const bool accChanged = m_hAccountDetails[param] != val;
   const QString buf = m_hAccountDetails[param];
   if (param == ACCOUNT_REGISTRATION_STATUS) {
      m_hAccountDetails[param] = val;
      if (accChanged) {
         emit detailChanged(this,param,val,buf);
      }
   }
   else {
      performAction(AccountEditAction::MODIFY);
      if (m_CurrentState == MODIFIED || m_CurrentState == NEW) {
         m_hAccountDetails[param] = val;
         if (accChanged) {
            emit detailChanged(this,param,val,buf);
         }
      }
   }
   return m_CurrentState == MODIFIED || m_CurrentState == NEW;
}

///Set the account id
void Account::setAccountId(const QString& id)
{
   qDebug() << "Setting accountId = " << m_pAccountId;
   if (! isNew())
      qDebug() << "Error : setting AccountId of an existing account.";
   m_pAccountId = new QString(id);
}

///Set account enabled
void Account::setEnabled(bool checked)
{
   setAccountEnabled(checked);
}


#ifdef ENABLE_VIDEO
void Account::setActiveVideoCodecList(const QList<VideoCodec*>& codecs);
QList<VideoCodec*> getActiveVideoCodecList();
#endif
///Set the account type, SIP or IAX
void Account::setAccountType(QString detail)
{
   setAccountDetail(ACCOUNT_TYPE ,detail);
}

///The set account hostname, it can be an hostname or an IP address
void Account::setAccountHostname(QString detail)
{
   m_HostName = detail;
   setAccountDetail(ACCOUNT_HOSTNAME, detail);
}

///Set the account username, everything is valid, some might be rejected by the PBX server
void Account::setAccountUsername(QString detail)
{
   setAccountDetail(ACCOUNT_USERNAME, detail);
}

///Set the account mailbox, usually a number, but can be anything
void Account::setAccountMailbox(QString detail)
{
   setAccountDetail(ACCOUNT_MAILBOX, detail);
}

///Set the account mailbox, usually a number, but can be anything
void Account::setAccountProxy(QString detail)
{
   setAccountDetail(ACCOUNT_ROUTE, detail);
}

///Set the main credential password
void Account::setAccountPassword(QString detail)
{
   setAccountDetail(ACCOUNT_PASSWORD, detail);
}

///Set the TLS (encryption) password
void Account::setTlsPassword(QString detail)
{
   setAccountDetail(TLS_PASSWORD, detail);
}

///Set the certificate authority list file
void Account::setTlsCaListFile(QString detail)
{
   setAccountDetail(TLS_CA_LIST_FILE, detail);
}

///Set the certificate
void Account::setTlsCertificateFile(QString detail)
{
   setAccountDetail(TLS_CERTIFICATE_FILE, detail);
}

///Set the private key
void Account::setTlsPrivateKeyFile(QString detail)
{
   setAccountDetail(TLS_PRIVATE_KEY_FILE, detail);
}

///Set the TLS cipher
void Account::setTlsCiphers(QString detail)
{
   setAccountDetail(TLS_CIPHERS, detail);
}

///Set the TLS server
void Account::setTlsServerName(QString detail)
{
   setAccountDetail(TLS_SERVER_NAME, detail);
}

///Set the stun server
void Account::setAccountSipStunServer(QString detail)
{
   setAccountDetail(ACCOUNT_SIP_STUN_SERVER, detail);
}

///Set the published address
void Account::setPublishedAddress(QString detail)
{
   setAccountDetail(PUBLISHED_ADDRESS, detail);
}

///Set the local interface
void Account::setLocalInterface(QString detail)
{
   setAccountDetail(LOCAL_INTERFACE, detail);
}

///Set the ringtone path, it have to be a valid absolute path
void Account::setRingtonePath(QString detail)
{
   setAccountDetail(CONFIG_RINGTONE_PATH, detail);
}

///Set the Tls method
void Account::setTlsMethod(int detail)
{
   setAccountDetail(TLS_METHOD ,QString::number(detail));
}

///Set the account timeout, it will be renegotiated when that timeout occur
void Account::setAccountRegistrationExpire(int detail)
{
   setAccountDetail(ACCOUNT_REGISTRATION_EXPIRE, QString::number(detail));
}

///Set TLS negotiation timeout in second
void Account::setTlsNegotiationTimeoutSec(int detail)
{
   setAccountDetail(TLS_NEGOTIATION_TIMEOUT_SEC, QString::number(detail));
}

///Set the TLS negotiation timeout in milliseconds
void Account::setTlsNegotiationTimeoutMsec(int detail)
{
   setAccountDetail(TLS_NEGOTIATION_TIMEOUT_MSEC, QString::number(detail));
}

///Set the local port for SIP/IAX communications
void Account::setLocalPort(unsigned short detail)
{
   setAccountDetail(LOCAL_PORT, QString::number(detail));
}

///Set the TLS listener port (0-2^16)
void Account::setTlsListenerPort(unsigned short detail)
{
   setAccountDetail(TLS_LISTENER_PORT, QString::number(detail));
}

///Set the published port (0-2^16)
void Account::setPublishedPort(unsigned short detail)
{
   setAccountDetail(PUBLISHED_PORT, QString::number(detail));
}

///Set if the account is enabled or not
void Account::setAccountEnabled(bool detail)
{
   setAccountDetail(ACCOUNT_ENABLED, detail?"true":"false");
}

///Set if the account should auto answer
void Account::setAutoAnswer(bool detail)
{
   setAccountDetail(ACCOUNT_AUTOANSWER, detail?"true":"false");
}

///Set the TLS verification server
void Account::setTlsVerifyServer(bool detail)
{
   setAccountDetail(TLS_VERIFY_SERVER, detail?"true":"false");
}

///Set the TLS verification client
void Account::setTlsVerifyClient(bool detail)
{
   setAccountDetail(TLS_VERIFY_CLIENT, detail?"true":"false");
}

///Set if the peer need to be providing a certificate
void Account::setTlsRequireClientCertificate(bool detail)
{
   setAccountDetail(TLS_REQUIRE_CLIENT_CERTIFICATE ,detail?"true":"false");
}

///Set if the security settings are enabled
void Account::setTlsEnable(bool detail)
{
   setAccountDetail(TLS_ENABLE ,detail?"true":"false");
}

void Account::setAccountDisplaySasOnce(bool detail)
{
   setAccountDetail(ACCOUNT_DISPLAY_SAS_ONCE, detail?"true":"false");
}

void Account::setAccountSrtpRtpFallback(bool detail)
{
   setAccountDetail(ACCOUNT_SRTP_RTP_FALLBACK, detail?"true":"false");
}

void Account::setAccountZrtpDisplaySas(bool detail)
{
   setAccountDetail(ACCOUNT_ZRTP_DISPLAY_SAS, detail?"true":"false");
}

void Account::setAccountZrtpNotSuppWarning(bool detail)
{
   setAccountDetail(ACCOUNT_ZRTP_NOT_SUPP_WARNING, detail?"true":"false");
}

void Account::setAccountZrtpHelloHash(bool detail)
{
   setAccountDetail(ACCOUNT_ZRTP_HELLO_HASH, detail?"true":"false");
}

void Account::setAccountSipStunEnabled(bool detail)
{
   setAccountDetail(ACCOUNT_SIP_STUN_ENABLED, detail?"true":"false");
}

void Account::setPublishedSameAsLocal(bool detail)
{
   setAccountDetail(PUBLISHED_SAMEAS_LOCAL, detail?"true":"false");
}

///Set if custom ringtone are enabled
void Account::setRingtoneEnabled(bool detail)
{
   setAccountDetail(CONFIG_RINGTONE_ENABLED, detail?"true":"false");
}

///Set the DTMF type
void Account::setDTMFType(DtmfType type)
{
   setAccountDetail(ACCOUNT_DTMF_TYPE,(type==OverRtp)?"overrtp":"oversip");
}


/*****************************************************************************
 *                                                                           *
 *                                  Mutator                                  *
 *                                                                           *
 ****************************************************************************/

bool Account::performAction(AccountEditAction action)
{
   AccountEditState curState = m_CurrentState;
   (this->*(stateMachineActionsOnState[m_CurrentState][action]))();
   return curState != m_CurrentState;
}

AccountEditState Account::currentState() const
{
   return m_CurrentState;
}

/**Update the account
 * @return if the state changed
 */
bool Account::updateState()
{
   if(! isNew()) {
      ConfigurationManagerInterface & configurationManager = DBus::ConfigurationManager::instance();
      const MapStringString details       = configurationManager.getAccountDetails(getAccountId()).value();
      const QString         status        = details[ACCOUNT_REGISTRATION_STATUS];
      const QString         currentStatus = getAccountRegistrationStatus();
      setAccountDetail(ACCOUNT_REGISTRATION_STATUS, status); //Update -internal- object state
      return status == currentStatus;
   }
   return true;
}

///Save the current account to the daemon
void Account::save()
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   if (isNew()) {
      MapStringString details;
      QMutableHashIterator<QString,QString> iter(m_hAccountDetails);

      while (iter.hasNext()) {
         iter.next();
         details[iter.key()] = iter.value();
      }

      const QString currentId = configurationManager.addAccount(details);

      //Be sure there is audio codec enabled to avoid obscure error messages for the user
      QVector<int> codecIdList = configurationManager.getAudioCodecList();
      foreach (const int aCodec, codecIdList) {
         QStringList codec = configurationManager.getAudioCodecDetails(aCodec);
         QModelIndex idx = m_pAudioCodecs->addAudioCodec();
         m_pAudioCodecs->setData(idx,codec[0],AudioCodecModel::NAME_ROLE       );
         m_pAudioCodecs->setData(idx,codec[1],AudioCodecModel::SAMPLERATE_ROLE );
         m_pAudioCodecs->setData(idx,codec[2],AudioCodecModel::BITRATE_ROLE    );
         m_pAudioCodecs->setData(idx,aCodec  ,AudioCodecModel::ID_ROLE         );
         m_pAudioCodecs->setData(idx, Qt::Checked ,Qt::CheckStateRole);
      }
      saveAudioCodecs();

      setAccountId(currentId);
      saveCredentials();
   }
   else {
      MapStringString tmp;
      QMutableHashIterator<QString,QString> iter(m_hAccountDetails);

      while (iter.hasNext()) {
         iter.next();
         tmp[iter.key()] = iter.value();
      }
      configurationManager.setAccountDetails(getAccountId(), tmp);
   }

   //QString id = configurationManager.getAccountDetail(getAccountId());
   if (!getAccountId().isEmpty()) {
      Account* acc =  AccountList::instance()->getAccountById(getAccountId());
      qDebug() << "Adding the new account to the account list (" << getAccountId() << ")";
      if (acc != this) {
         (*AccountList::instance()->m_pAccounts) << this;
      }

      performAction(AccountEditAction::RELOAD);
      updateState();
      m_CurrentState = READY;
   }
   #ifdef ENABLE_VIDEO
   m_pVideoCodecs->save();
   #endif
   saveAudioCodecs();
   emit changed(this);
}

///sync with the daemon, this need to be done manually to prevent reloading the account while it is being edited
void Account::reload()
{
   qDebug() << "Reloading" << getAccountId();
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   QMap<QString,QString> aDetails = configurationManager.getAccountDetails(getAccountId());

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
      setAccountHostname(m_hAccountDetails[ACCOUNT_HOSTNAME]);
   }
   m_CurrentState = READY;
   reloadCredentials();
   emit changed(this);
}

///Reload credentials from DBUS
void Account::reloadCredentials()
{
   if (!m_pCredentials) {
      m_pCredentials = new CredentialModel(this);
   }
   if (!isNew()) {
      m_pCredentials->clear();
      ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
      VectorMapStringString credentials = configurationManager.getCredentials(getAccountId());
      for (int i=0; i < credentials.size(); i++) {
         QModelIndex idx = m_pCredentials->addCredentials();
         m_pCredentials->setData(idx,credentials[i][ CONFIG_ACCOUNT_USERNAME  ],CredentialModel::NAME_ROLE    );
         m_pCredentials->setData(idx,credentials[i][ CONFIG_ACCOUNT_PASSWORD  ],CredentialModel::PASSWORD_ROLE);
         m_pCredentials->setData(idx,credentials[i][ CONFIG_ACCOUNT_REALM     ],CredentialModel::REALM_ROLE   );
      }
   }
}

///Save all credentials
void Account::saveCredentials() {
   if (m_pCredentials) {
      ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
      VectorMapStringString toReturn;
      for (int i=0; i < m_pCredentials->rowCount();i++) {
         QModelIndex idx = m_pCredentials->index(i,0);
         MapStringString credentialData;
         QString username = m_pCredentials->data(idx,CredentialModel::NAME_ROLE ).toString();
         QString realm = m_pCredentials->data(idx,CredentialModel::REALM_ROLE    ).toString();
         if (username.isEmpty()) {
            username = getAccountUsername();
            m_pCredentials->setData(idx,username,CredentialModel::NAME_ROLE );
         }
         if (realm.isEmpty()) {
            realm = '*';
            m_pCredentials->setData(idx,realm,CredentialModel::REALM_ROLE    );
         }
         credentialData[ CONFIG_ACCOUNT_USERNAME] = username;
         credentialData[ CONFIG_ACCOUNT_PASSWORD] = m_pCredentials->data(idx,CredentialModel::PASSWORD_ROLE ).toString();
         credentialData[ CONFIG_ACCOUNT_REALM   ] = realm;
         toReturn << credentialData;
      }
      configurationManager.setCredentials(getAccountId(),toReturn);
   }
}

///Reload all audio codecs
void Account::reloadAudioCodecs()
{
   if (!m_pAudioCodecs) {
      m_pAudioCodecs = new AudioCodecModel(this);
   }
   m_pAudioCodecs->clear();
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   QVector<int> codecIdList = configurationManager.getAudioCodecList();
   if (!isNew()) {
      QVector<int> activeCodecList = configurationManager.getActiveAudioCodecList(getAccountId());
      QStringList tmpNameList;

      foreach (const int aCodec, activeCodecList) {
         QStringList codec = configurationManager.getAudioCodecDetails(aCodec);
         QModelIndex idx = m_pAudioCodecs->addAudioCodec();
         m_pAudioCodecs->setData(idx,codec[0]     ,AudioCodecModel::NAME_ROLE       );
         m_pAudioCodecs->setData(idx,codec[1]     ,AudioCodecModel::SAMPLERATE_ROLE );
         m_pAudioCodecs->setData(idx,codec[2]     ,AudioCodecModel::BITRATE_ROLE    );
         m_pAudioCodecs->setData(idx,aCodec       ,AudioCodecModel::ID_ROLE         );
         m_pAudioCodecs->setData(idx, Qt::Checked ,Qt::CheckStateRole               );
         if (codecIdList.indexOf(aCodec)!=-1)
            codecIdList.remove(codecIdList.indexOf(aCodec));
      }
   }

   foreach (const int aCodec, codecIdList) {
      QStringList codec = configurationManager.getAudioCodecDetails(aCodec);
      QModelIndex idx = m_pAudioCodecs->addAudioCodec();
      m_pAudioCodecs->setData(idx,codec[0],AudioCodecModel::NAME_ROLE       );
      m_pAudioCodecs->setData(idx,codec[1],AudioCodecModel::SAMPLERATE_ROLE );
      m_pAudioCodecs->setData(idx,codec[2],AudioCodecModel::BITRATE_ROLE    );
      m_pAudioCodecs->setData(idx,aCodec  ,AudioCodecModel::ID_ROLE         );
      
      m_pAudioCodecs->setData(idx, Qt::Unchecked ,Qt::CheckStateRole);
   }
}

///Save audio codecs
void Account::saveAudioCodecs() {
   if (m_pAudioCodecs) {
      QStringList _codecList;
      for (int i=0; i < m_pAudioCodecs->rowCount();i++) {
         QModelIndex idx = m_pAudioCodecs->index(i,0);
         if (m_pAudioCodecs->data(idx,Qt::CheckStateRole) == Qt::Checked) {
            _codecList << m_pAudioCodecs->data(idx,AudioCodecModel::ID_ROLE).toString();
         }
      }

      ConfigurationManagerInterface & configurationManager = DBus::ConfigurationManager::instance();
      configurationManager.setActiveAudioCodecList(_codecList, getAccountId());
   }
}

/*****************************************************************************
 *                                                                           *
 *                                 Operator                                  *
 *                                                                           *
 ****************************************************************************/

///Are both account the same
bool Account::operator==(const Account& a)const
{
   return *m_pAccountId == *a.m_pAccountId;
}

/*****************************************************************************
 *                                                                           *
 *                                   Video                                   *
 *                                                                           *
 ****************************************************************************/
#ifdef ENABLE_VIDEO
///Save active video codecs
void Account::setActiveVideoCodecList(const QList<VideoCodec*>& codecs)
{
   QStringList codecs2;
   VideoInterface& interface = DBus::VideoManager::instance();
   foreach(VideoCodec* codec,codecs) {
      codecs2 << codecs->getName();
   }
   interface.setActiveCodecList(codecs2,m_pAccountId);
}

///Return the list of active video dodecs
QList<VideoCodec*> Account::getActiveVideoCodecList()
{
   QList<VideoCodec*> codecs;
   VideoInterface& interface = DBus::VideoManager::instance();
   const QStringList activeCodecList = interface.getActiveCodecList(m_pAccountId);
   foreach (const QString& codec, activeCodecList) {
      codecs << VideoCodec::getCodec(codec);
   }
}

#endif
