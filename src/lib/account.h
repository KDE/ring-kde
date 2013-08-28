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
   

   Q_PROPERTY(QString        alias                        READ alias                         WRITE setAccountAlias                )
   Q_PROPERTY(QString        type                         READ accountType                   WRITE setAccountType                 )
   Q_PROPERTY(QString        hostname                     READ hostname                      WRITE setAccountHostname             )
   Q_PROPERTY(QString        username                     READ username                      WRITE setAccountUsername             )
   Q_PROPERTY(QString        mailbox                      READ mailbox                       WRITE setAccountMailbox              )
   Q_PROPERTY(QString        proxy                        READ proxy                         WRITE setAccountProxy                )
   Q_PROPERTY(QString        tlsPassword                  READ tlsPassword                   WRITE setTlsPassword                 )
   Q_PROPERTY(QString        tlsCaListFile                READ tlsCaListFile                 WRITE setTlsCaListFile               )
   Q_PROPERTY(QString        tlsCertificateFile           READ tlsCertificateFile            WRITE setTlsCertificateFile          )
   Q_PROPERTY(QString        tlsPrivateKeyFile            READ tlsPrivateKeyFile             WRITE setTlsPrivateKeyFile           )
   Q_PROPERTY(QString        tlsCiphers                   READ tlsCiphers                    WRITE setTlsCiphers                  )
   Q_PROPERTY(QString        tlsServerName                READ tlsServerName                 WRITE setTlsServerName               )
   Q_PROPERTY(QString        sipStunServer                READ accountSipStunServer          WRITE setAccountSipStunServer        )
   Q_PROPERTY(QString        publishedAddress             READ publishedAddress              WRITE setPublishedAddress            )
   Q_PROPERTY(QString        localInterface               READ localInterface                WRITE setLocalInterface              )
   Q_PROPERTY(QString        ringtonePath                 READ ringtonePath                  WRITE setRingtonePath                )
   Q_PROPERTY(int            tlsMethod                    READ tlsMethod                     WRITE setTlsMethod                   )
   Q_PROPERTY(int            accountRegistrationExpire    READ accountRegistrationExpire     WRITE setAccountRegistrationExpire   )
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
   Q_PROPERTY(bool           displaySasOnce               READ isAccountDisplaySasOnce       WRITE setAccountDisplaySasOnce       )
   Q_PROPERTY(bool           srtpRtpFallback              READ isAccountSrtpRtpFallback      WRITE setAccountSrtpRtpFallback      )
   Q_PROPERTY(bool           zrtpDisplaySas               READ isAccountZrtpDisplaySas       WRITE setAccountZrtpDisplaySas       )
   Q_PROPERTY(bool           zrtpNotSuppWarning           READ isAccountZrtpNotSuppWarning   WRITE setAccountZrtpNotSuppWarning   )
   Q_PROPERTY(bool           zrtpHelloHash                READ isAccountZrtpHelloHash        WRITE setAccountZrtpHelloHash        )
   Q_PROPERTY(bool           sipStunEnabled               READ isAccountSipStunEnabled       WRITE setAccountSipStunEnabled       )
   Q_PROPERTY(bool           publishedSameAsLocal         READ isPublishedSameAsLocal        WRITE setPublishedSameAsLocal        )
   Q_PROPERTY(bool           ringtoneEnabled              READ isRingtoneEnabled             WRITE setRingtoneEnabled             )
   Q_PROPERTY(DtmfType       dTMFType                     READ DTMFType                      WRITE setDTMFType                    )
   Q_PROPERTY(QString        typeName                     READ accountType                   WRITE setAccountType                 )

   public:
      ///@enum AccountEditState: Manage how and when an account can be reloaded or change state
      enum AccountEditState {
         READY    =0,
         EDITING  =1,
         OUTDATED =2,
         NEW      =3,
         MODIFIED =4,
         REMOVED  =5
      };

      ///@enum AccountEditAction Actions that can be performed on the Account state
      enum AccountEditAction {
         NOTHING =0,
         EDIT    =1,
         RELOAD  =2,
         SAVE    =3,
         REMOVE  =4,
         MODIFY  =5,
         CANCEL  =6
      };

      class State {
      public:
         constexpr static const char* REGISTERED       = "REGISTERED";
         constexpr static const char* READY            = "READY";
         constexpr static const char* UNREGISTERED     = "UNREGISTERED";
         constexpr static const char* TRYING           = "TRYING";
         constexpr static const char* ERROR            = "ERROR";
         constexpr static const char* ERROR_AUTH       = "ERROR_AUTH";
         constexpr static const char* ERROR_NETWORK    = "ERROR_NETWORK";
         constexpr static const char* ERROR_HOST       = "ERROR_HOST";
         constexpr static const char* ERROR_CONF_STUN  = "ERROR_CONF_STUN";
         constexpr static const char* ERROR_EXIST_STUN = "ERROR_EXIST_STUN";
      };

      ~Account();
      //Constructors
      static Account* buildExistingAccountFromId(const QString& _accountId);
      static Account* buildNewAccountFromAlias  (const QString& alias     );

      enum Role {
         Alias                       = 100,
         Type                        = 101,
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
         AccountRegistrationExpire   = 118,
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
      };

      /**
       *Perform an action
       * @return If the state changed
       */
      bool performAction(Account::AccountEditAction action);
      Account::AccountEditState currentState() const;

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

      Q_INVOKABLE CredentialModel*        credentialsModel();
      Q_INVOKABLE AudioCodecModel*        audioCodecModel() ;
      Q_INVOKABLE VideoCodecModel*        videoCodecModel() ;

      ///Return the account hostname
      QString hostname                     () const;
      bool    isEnabled                    () const;
      bool    isAutoAnswer                 () const;
      QString username                     () const;
      QString mailbox                      () const;
      QString proxy                        () const;
      bool    isAccountDisplaySasOnce      () const;
      bool    isAccountSrtpRtpFallback     () const;
      bool    isAccountZrtpDisplaySas      () const;
      bool    isAccountZrtpNotSuppWarning  () const;
      bool    isAccountZrtpHelloHash       () const;
      bool    isAccountSipStunEnabled      () const;
      QString accountSipStunServer         () const;
      int     accountRegistrationExpire    () const;
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
      int     tlsMethod                    () const;
      bool    isRingtoneEnabled            () const;
      QString ringtonePath                 () const;
      int     localPort                    () const;
      QString localInterface               () const;
      QString accountRegistrationStatus    () const;
      QString accountType                  () const;
      DtmfType DTMFType                    () const;

      QVariant roleData                    (int role) const;

      //Setters
      void setAccountId      (const QString& id);
      #ifdef ENABLE_VIDEO
      void setActiveVideoCodecList(const QList<VideoCodec*>& codecs);
      QList<VideoCodec*> getActiveVideoCodecList();
      #endif
      void setAccountAlias                  (const QString& detail);
      void setAccountType                   (const QString& detail);
      void setAccountHostname               (const QString& detail);
      void setAccountUsername               (const QString& detail);
      void setAccountMailbox                (const QString& detail);
      void setAccountProxy                  (const QString& detail);
      void setAccountPassword               (const QString& detail);
      void setTlsPassword                   (const QString& detail);
      void setTlsCaListFile                 (const QString& detail);
      void setTlsCertificateFile            (const QString& detail);
      void setTlsPrivateKeyFile             (const QString& detail);
      void setTlsCiphers                    (const QString& detail);
      void setTlsServerName                 (const QString& detail);
      void setAccountSipStunServer          (const QString& detail);
      void setPublishedAddress              (const QString& detail);
      void setLocalInterface                (const QString& detail);
      void setRingtonePath                  (const QString& detail);
      void setTlsMethod                     (int     detail);
      void setAccountRegistrationExpire     (int     detail);
      void setTlsNegotiationTimeoutSec      (int     detail);
      void setTlsNegotiationTimeoutMsec     (int     detail);
      void setLocalPort                     (unsigned short detail);
      void setTlsListenerPort               (unsigned short detail);
      void setPublishedPort                 (unsigned short detail);
      void setAccountEnabled                (bool    detail);
      void setAutoAnswer                    (bool    detail);
      void setTlsVerifyServer               (bool    detail);
      void setTlsVerifyClient               (bool    detail);
      void setTlsRequireClientCertificate   (bool    detail);
      void setTlsEnable                     (bool    detail);
      void setAccountDisplaySasOnce         (bool    detail);
      void setAccountSrtpRtpFallback        (bool    detail);
      void setAccountZrtpDisplaySas         (bool    detail);
      void setAccountZrtpNotSuppWarning     (bool    detail);
      void setAccountZrtpHelloHash          (bool    detail);
      void setAccountSipStunEnabled         (bool    detail);
      void setPublishedSameAsLocal          (bool    detail);
      void setRingtoneEnabled               (bool    detail);
      void setDTMFType                      (DtmfType type );

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
      QString*                m_pAccountId     ;
      QHash<QString,QString>  m_hAccountDetails;
      

   public Q_SLOTS:
      void setEnabled(bool checked);

   private Q_SLOTS:
      void accountChanged(QString accountId,QString stateName, int state);

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
