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

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QtCore/QList>

//Qt
class QString;

//SFLPhone
#include "videocodecmodel.h"
#include "keyexchangemodel.h"
#include "tlsmethodmodel.h"
#include "sflphone_const.h"
#include "typedefs.h"
// #include "dbus/metatypes.h"
class CredentialModel;
class AudioCodecModel;
class VideoCodecModel;

const QString& account_state_name(const QString& s);

typedef void (Account::*account_function)();

///@enum DtmfType Different method to send the DTMF (key sound) to the peer
enum DtmfType {
   OverRtp,
   OverSip
};
Q_ENUMS(DtmfType)

///Account: a daemon account (SIP or AIX)
class LIB_EXPORT Account : public QObject {
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop

   //Properties
   Q_PROPERTY(QString        alias                        READ alias                         WRITE setAlias                       )
   Q_PROPERTY(Account::Protocol protocol                  READ protocol                      WRITE setProtocol                    )
   Q_PROPERTY(QString        hostname                     READ hostname                      WRITE setHostname                    )
   Q_PROPERTY(QString        username                     READ username                      WRITE setUsername                    )
   Q_PROPERTY(QString        mailbox                      READ mailbox                       WRITE setMailbox                     )
   Q_PROPERTY(QString        proxy                        READ proxy                         WRITE setProxy                       )
   Q_PROPERTY(QString        tlsPassword                  READ tlsPassword                   WRITE setTlsPassword                 )
   Q_PROPERTY(QString        tlsCaListFile                READ tlsCaListFile                 WRITE setTlsCaListFile               )
   Q_PROPERTY(QString        tlsCertificateFile           READ tlsCertificateFile            WRITE setTlsCertificateFile          )
   Q_PROPERTY(QString        tlsPrivateKeyFile            READ tlsPrivateKeyFile             WRITE setTlsPrivateKeyFile           )
   Q_PROPERTY(QString        tlsCiphers                   READ tlsCiphers                    WRITE setTlsCiphers                  )
   Q_PROPERTY(QString        tlsServerName                READ tlsServerName                 WRITE setTlsServerName               )
   Q_PROPERTY(QString        sipStunServer                READ sipStunServer                 WRITE setSipStunServer               )
   Q_PROPERTY(QString        publishedAddress             READ publishedAddress              WRITE setPublishedAddress            )
   Q_PROPERTY(QString        localInterface               READ localInterface                WRITE setLocalInterface              )
   Q_PROPERTY(QString        ringtonePath                 READ ringtonePath                  WRITE setRingtonePath                )
   Q_PROPERTY(TlsMethodModel::Type tlsMethod              READ tlsMethod                     WRITE setTlsMethod                   )
   Q_PROPERTY(KeyExchangeModel::Type keyExchange          READ keyExchange                   WRITE setKeyExchange                 )
   Q_PROPERTY(int            registrationExpire           READ registrationExpire            WRITE setRegistrationExpire          )
   Q_PROPERTY(int            tlsNegotiationTimeoutSec     READ tlsNegotiationTimeoutSec      WRITE setTlsNegotiationTimeoutSec    )
   Q_PROPERTY(int            tlsNegotiationTimeoutMsec    READ tlsNegotiationTimeoutMsec     WRITE setTlsNegotiationTimeoutMsec   )
   Q_PROPERTY(int            localPort                    READ localPort                     WRITE setLocalPort                   )
   Q_PROPERTY(int            tlsListenerPort              READ tlsListenerPort               WRITE setTlsListenerPort             )
   Q_PROPERTY(int            publishedPort                READ publishedPort                 WRITE setPublishedPort               )
   Q_PROPERTY(bool           enabled                      READ isEnabled                     WRITE setEnabled                     )
   Q_PROPERTY(bool           autoAnswer                   READ isAutoAnswer                  WRITE setAutoAnswer                  )
   Q_PROPERTY(bool           tlsVerifyServer              READ isTlsVerifyServer             WRITE setTlsVerifyServer             )
   Q_PROPERTY(bool           tlsVerifyClient              READ isTlsVerifyClient             WRITE setTlsVerifyClient             )
   Q_PROPERTY(bool           tlsRequireClientCertificate  READ isTlsRequireClientCertificate WRITE setTlsRequireClientCertificate )
   Q_PROPERTY(bool           tlsEnable                    READ isTlsEnable                   WRITE setTlsEnable                   )
   Q_PROPERTY(bool           displaySasOnce               READ isDisplaySasOnce              WRITE setDisplaySasOnce              )
   Q_PROPERTY(bool           srtpRtpFallback              READ isSrtpRtpFallback             WRITE setSrtpRtpFallback             )
   Q_PROPERTY(bool           zrtpDisplaySas               READ isZrtpDisplaySas              WRITE setZrtpDisplaySas              )
   Q_PROPERTY(bool           zrtpNotSuppWarning           READ isZrtpNotSuppWarning          WRITE setZrtpNotSuppWarning          )
   Q_PROPERTY(bool           zrtpHelloHash                READ isZrtpHelloHash               WRITE setZrtpHelloHash               )
   Q_PROPERTY(bool           sipStunEnabled               READ isSipStunEnabled              WRITE setSipStunEnabled              )
   Q_PROPERTY(bool           publishedSameAsLocal         READ isPublishedSameAsLocal        WRITE setPublishedSameAsLocal        )
   Q_PROPERTY(bool           ringtoneEnabled              READ isRingtoneEnabled             WRITE setRingtoneEnabled             )
   Q_PROPERTY(DtmfType       dTMFType                     READ DTMFType                      WRITE setDTMFType                    )
//    Q_PROPERTY(QString        typeName                     READ type                          WRITE setType                        )
   Q_PROPERTY(bool           presenceStatus               READ presenceStatus                                                     )
   Q_PROPERTY(QString        presenceMessage              READ presenceMessage                                                    )

   public:
      ///@enum AccountEditState: Manage how and when an account can be reloaded or change state
      enum AccountEditState {
         READY    = 0,
         EDITING  = 1,
         OUTDATED = 2,
         NEW      = 3,
         MODIFIED = 4,
         REMOVED  = 5
      };

      ///@enum AccountEditAction Actions that can be performed on the Account state
      enum AccountEditAction {
         NOTHING = 0,
         EDIT    = 1,
         RELOAD  = 2,
         SAVE    = 3,
         REMOVE  = 4,
         MODIFY  = 5,
         CANCEL  = 6
      };

      class State {
      public:
         constexpr static const char* REGISTERED       = "REGISTERED"      ;
         constexpr static const char* READY            = "READY"           ;
         constexpr static const char* UNREGISTERED     = "UNREGISTERED"    ;
         constexpr static const char* TRYING           = "TRYING"          ;
         constexpr static const char* ERROR            = "ERROR"           ;
         constexpr static const char* ERROR_AUTH       = "ERROR_AUTH"      ;
         constexpr static const char* ERROR_NETWORK    = "ERROR_NETWORK"   ;
         constexpr static const char* ERROR_HOST       = "ERROR_HOST"      ;
         constexpr static const char* ERROR_CONF_STUN  = "ERROR_CONF_STUN" ;
         constexpr static const char* ERROR_EXIST_STUN = "ERROR_EXIST_STUN";
      };

      ~Account();
      //Constructors
      static Account* buildExistingAccountFromId(const QString& _accountId);
      static Account* buildNewAccountFromAlias  (const QString& alias     );

      enum Role {
         Alias                       = 100,
         Proto                       = 101,
         Hostname                    = 102,
         Username                    = 103,
         Mailbox                     = 104,
         Proxy                       = 105,
         TlsPassword                 = 107,
         TlsCaListFile               = 108,
         TlsCertificateFile          = 109,
         TlsPrivateKeyFile           = 110,
         TlsCiphers                  = 111,
         TlsServerName               = 112,
         SipStunServer               = 113,
         PublishedAddress            = 114,
         LocalInterface              = 115,
         RingtonePath                = 116,
         TlsMethod                   = 117,
         KeyExchange                 = 190,
         RegistrationExpire          = 118,
         TlsNegotiationTimeoutSec    = 119,
         TlsNegotiationTimeoutMsec   = 120,
         LocalPort                   = 121,
         TlsListenerPort             = 122,
         PublishedPort               = 123,
         Enabled                     = 124,
         AutoAnswer                  = 125,
         TlsVerifyServer             = 126,
         TlsVerifyClient             = 127,
         TlsRequireClientCertificate = 128,
         TlsEnable                   = 129,
         DisplaySasOnce              = 130,
         SrtpRtpFallback             = 131,
         ZrtpDisplaySas              = 132,
         ZrtpNotSuppWarning          = 133,
         ZrtpHelloHash               = 134,
         SipStunEnabled              = 135,
         PublishedSameAsLocal        = 136,
         RingtoneEnabled             = 137,
         dTMFType                    = 138,
         Id                          = 139,
         Object                      = 140,
         TypeName                    = 141,
         PresenceStatus              = 142,
         PresenceMessage             = 143,
      };

      class MapField {
      public:
         constexpr static const char* ID                     = "Account.id"                   ;
         constexpr static const char* TYPE                   = "Account.type"                 ;
         constexpr static const char* ALIAS                  = "Account.alias"                ;
         constexpr static const char* ENABLED                = "Account.enable"               ;
         constexpr static const char* MAILBOX                = "Account.mailbox"              ;
         constexpr static const char* DTMF_TYPE              = "Account.dtmfType"             ;
         constexpr static const char* AUTOANSWER             = "Account.autoAnswer"           ;
         constexpr static const char* HOSTNAME               = "Account.hostname"             ;
         constexpr static const char* USERNAME               = "Account.username"             ;
         constexpr static const char* ROUTE                  = "Account.routeset"             ;
         constexpr static const char* PASSWORD               = "Account.password"             ;
         constexpr static const char* REALM                  = "Account.realm"                ;
         constexpr static const char* LOCAL_INTERFACE        = "Account.localInterface"       ;
         constexpr static const char* PUBLISHED_SAMEAS_LOCAL = "Account.publishedSameAsLocal" ;
         constexpr static const char* LOCAL_PORT             = "Account.localPort"            ;
         constexpr static const char* PUBLISHED_PORT         = "Account.publishedPort"        ;
         constexpr static const char* PUBLISHED_ADDRESS      = "Account.publishedAddress"     ;
         class Registration {
         public:
            constexpr static const char* EXPIRE              = "Account.registrationExpire"   ;
            constexpr static const char* STATUS              = "Account.registrationStatus"   ;
         };
         class Ringtone {
         public:
            constexpr static const char* PATH                = "Account.ringtonePath"         ;
            constexpr static const char* ENABLED             = "Account.ringtoneEnabled"      ;
         };
      };

      class ProtocolName {
      public:
         constexpr static const char* SIP = "SIP";
         constexpr static const char* IAX = "IAX";
      };

      enum class Protocol {
         SIP = 0,
         IAX = 1,
      };
      Q_ENUMS(Protocol)

      /**
       *Perform an action
       * @return If the state changed
       */
      bool performAction(Account::AccountEditAction action);
      Account::AccountEditState state() const;

      //Getters
      bool            isNew()                             const;
      const QString   id()                                const;
      const QString   stateName(const QString& state)     const;
      const QString   accountDetail(const QString& param) const;
      const QString   alias()                             const;
      bool            isRegistered()                      const;
      QModelIndex     index()                                  ;
      QString         stateColorName()                    const;
      QVariant        stateColor()                        const;

      Q_INVOKABLE CredentialModel* credentialsModel() const;
      Q_INVOKABLE AudioCodecModel* audioCodecModel () const;
      Q_INVOKABLE VideoCodecModel* videoCodecModel () const;

      //Getters
      QString hostname                     () const;
      bool    isEnabled                    () const;
      bool    isAutoAnswer                 () const;
      QString username                     () const;
      QString mailbox                      () const;
      QString proxy                        () const;
      QString password                     () const;
      bool    isDisplaySasOnce             () const;
      bool    isSrtpRtpFallback            () const;
      bool    isZrtpDisplaySas             () const;
      bool    isZrtpNotSuppWarning         () const;
      bool    isZrtpHelloHash              () const;
      bool    isSipStunEnabled             () const;
      QString sipStunServer                () const;
      int     registrationExpire           () const;
      bool    isPublishedSameAsLocal       () const;
      QString publishedAddress             () const;
      int     publishedPort                () const;
      QString tlsPassword                  () const;
      int     tlsListenerPort              () const;
      QString tlsCaListFile                () const;
      QString tlsCertificateFile           () const;
      QString tlsPrivateKeyFile            () const;
      QString tlsCiphers                   () const;
      QString tlsServerName                () const;
      int     tlsNegotiationTimeoutSec     () const;
      int     tlsNegotiationTimeoutMsec    () const;
      bool    isTlsVerifyServer            () const;
      bool    isTlsVerifyClient            () const;
      bool    isTlsRequireClientCertificate() const;
      bool    isTlsEnable                  () const;
      bool    isRingtoneEnabled            () const;
      QString ringtonePath                 () const;
      int     localPort                    () const;
      QString localInterface               () const;
      QString registrationStatus           () const;
      DtmfType DTMFType                    () const;
      bool    presenceStatus               () const;
      QString presenceMessage              () const;
      Account::Protocol      protocol      () const;
      TlsMethodModel::Type   tlsMethod     () const;
      KeyExchangeModel::Type keyExchange   () const;
      QVariant roleData            (int role) const;

      //Setters
      void setId      (const QString& id);
      void setAlias                         (const QString& detail);
      void setProtocol                      (Account::Protocol proto);
      void setHostname                      (const QString& detail);
      void setUsername                      (const QString& detail);
      void setMailbox                       (const QString& detail);
      void setProxy                         (const QString& detail);
      void setPassword                      (const QString& detail);
      void setTlsPassword                   (const QString& detail);
      void setTlsCaListFile                 (const QString& detail);
      void setTlsCertificateFile            (const QString& detail);
      void setTlsPrivateKeyFile             (const QString& detail);
      void setTlsCiphers                    (const QString& detail);
      void setTlsServerName                 (const QString& detail);
      void setSipStunServer                 (const QString& detail);
      void setPublishedAddress              (const QString& detail);
      void setLocalInterface                (const QString& detail);
      void setRingtonePath                  (const QString& detail);
      void setTlsMethod                     (TlsMethodModel::Type   detail);
      void setKeyExchange                   (KeyExchangeModel::Type detail);
      void setRegistrationExpire            (int  detail);
      void setTlsNegotiationTimeoutSec      (int  detail);
      void setTlsNegotiationTimeoutMsec     (int  detail);
      void setLocalPort                     (unsigned short detail);
      void setTlsListenerPort               (unsigned short detail);
      void setPublishedPort                 (unsigned short detail);
      void setAutoAnswer                    (bool detail);
      void setTlsVerifyServer               (bool detail);
      void setTlsVerifyClient               (bool detail);
      void setTlsRequireClientCertificate   (bool detail);
      void setTlsEnable                     (bool detail);
      void setDisplaySasOnce                (bool detail);
      void setSrtpRtpFallback               (bool detail);
      void setZrtpDisplaySas                (bool detail);
      void setZrtpNotSuppWarning            (bool detail);
      void setZrtpHelloHash                 (bool detail);
      void setSipStunEnabled                (bool detail);
      void setPublishedSameAsLocal          (bool detail);
      void setRingtoneEnabled               (bool detail);
      void setDTMFType                      (DtmfType type);

      void setRoleData(int role, const QVariant& value);

      //Updates
      virtual bool updateState();

      //Operators
      bool operator==(const Account&)const;

      //Mutator
      Q_INVOKABLE void saveCredentials  ();
      Q_INVOKABLE void saveAudioCodecs  ();
      Q_INVOKABLE void reloadCredentials();
      Q_INVOKABLE void reloadAudioCodecs();

   protected:
      //Constructors
      Account();

      //Attributes
      QString                 m_AccountId      ;
      QHash<QString,QString>  m_hAccountDetails;


   public Q_SLOTS:
      void setEnabled(bool checked);

   private Q_SLOTS:
      void accountChanged(const QString& accountId, const QString& stateName, int state);

   private:

      //Setters
      void setAccountDetails (const QHash<QString,QString>& m          );
      bool setAccountDetail  (const QString& param, const QString& val );

      //State actions
      void nothing() {};
      void edit()    {m_CurrentState = EDITING ;emit changed(this);};
      void modify()  {m_CurrentState = MODIFIED;emit changed(this);};
      void remove()  {m_CurrentState = REMOVED ;emit changed(this);};
      void cancel()  {m_CurrentState = READY   ;emit changed(this);};
      void outdate() {m_CurrentState = OUTDATED;emit changed(this);};
      void reload();
      void save();
      void reloadMod() {reload();modify();};

      CredentialModel* m_pCredentials;
      AudioCodecModel* m_pAudioCodecs;
      VideoCodecModel* m_pVideoCodecs;
      AccountEditState m_CurrentState;
      static const account_function stateMachineActionsOnState[6][7];

      //Cached account details (as they are called too often for the hash)
      QString m_HostName;


   Q_SIGNALS:
      ///The account state (Invalif,Trying,Registered) changed
      void stateChanged(QString state);
      void detailChanged(Account* a,QString name,QString newVal, QString oldVal);
      void changed(Account* a);
      ///The alias changed, take effect instantaneously
      void aliasChanged(const QString&);
};
// Q_DISABLE_COPY(Account)
Q_DECLARE_METATYPE(Account*)
#endif
