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
#include "dbus/metatypes.h"
class CredentialModel;
class AudioCodecModel;
class VideoCodecModel;

const QString& account_state_name(const QString& s);

typedef void (Account::*account_function)();

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

///@enum DtmfType Different method to send the DTMF (key sound) to the peer
enum DtmfType {
   OverRtp,
   OverSip
};

///Account: a daemon account (SIP or AIX)
class LIB_EXPORT Account : public QObject {
   Q_OBJECT

   public:
      ~Account();
      //Constructors
      static Account* buildExistingAccountFromId(const QString& _accountId);
      static Account* buildNewAccountFromAlias  (const QString& alias     );

      /**
       *Perform an action
       * @return If the state changed
       */
      bool performAction(AccountEditAction action);
      AccountEditState currentState() const;

      //Getters
      bool                    isNew()                                const;
      const QString           getAccountId()                         const;
      const QString           getStateName(const QString& state)     const;
      const QString           getAccountDetail(const QString& param) const;
      const QString           getAlias()                             const;
      bool                    isEnabled()                            const;
      bool                    isRegistered()                         const;
      QModelIndex             getIndex()                                  ;
      QString                 getStateColorName()                    const;
      Qt::GlobalColor         getStateColor()                        const;

      CredentialModel*        getCredentialsModel();
      AudioCodecModel*        getAudioCodecModel();
      VideoCodecModel*        getVideoCodecModel();

      ///Return the account hostname
      QString getAccountHostname              () const;
      bool    isAccountEnabled                () const;
      bool    isAutoAnswer                    () const;
      QString getAccountUsername              () const;
      QString getAccountMailbox               () const;
      QString getAccountProxy                 () const;
      bool    isAccountDisplaySasOnce         () const;
      bool    isAccountSrtpRtpFallback        () const;
      bool    isAccountZrtpDisplaySas         () const;
      bool    isAccountZrtpNotSuppWarning     () const;
      bool    isAccountZrtpHelloHash          () const;
      bool    isAccountSipStunEnabled         () const;
      QString getAccountSipStunServer         () const;
      int     getAccountRegistrationExpire    () const;
      bool    isPublishedSameAsLocal          () const;
      QString getPublishedAddress             () const;
      int     getPublishedPort                () const;
      QString getTlsPassword                  () const;
      int     getTlsListenerPort              () const;
      QString getTlsCaListFile                () const;
      QString getTlsCertificateFile           () const;
      QString getTlsPrivateKeyFile            () const;
      QString getTlsCiphers                   () const;
      QString getTlsServerName                () const;
      int     getTlsNegotiationTimeoutSec     () const;
      int     getTlsNegotiationTimeoutMsec    () const;
      bool    isTlsVerifyServer               () const;
      bool    isTlsVerifyClient               () const;
      bool    isTlsRequireClientCertificate   () const;
      bool    isTlsEnable                     () const;
      int     getTlsMethod                    () const;
      QString getAccountAlias                 () const;
      bool    isRingtoneEnabled               () const;
      QString getRingtonePath                 () const;
      int     getLocalPort                    () const;
      QString getLocalInterface               () const;
      QString getAccountRegistrationStatus    () const;
      QString getAccountType                  () const;
      DtmfType getDTMFType                    () const;
   
      //Setters
      void setAccountId      (const QString& id                        );
      #ifdef ENABLE_VIDEO
      void setActiveVideoCodecList(const QList<VideoCodec*>& codecs);
      QList<VideoCodec*> getActiveVideoCodecList();
      #endif
      void setAccountAlias                  (QString detail);
      void setAccountType                   (QString detail);
      void setAccountHostname               (QString detail);
      void setAccountUsername               (QString detail);
      void setAccountMailbox                (QString detail);
      void setAccountProxy                  (QString detail);
      void setAccountPassword               (QString detail);
      void setTlsPassword                   (QString detail);
      void setTlsCaListFile                 (QString detail);
      void setTlsCertificateFile            (QString detail);
      void setTlsPrivateKeyFile             (QString detail);
      void setTlsCiphers                    (QString detail);
      void setTlsServerName                 (QString detail);
      void setAccountSipStunServer          (QString detail);
      void setPublishedAddress              (QString detail);
      void setLocalInterface                (QString detail);
      void setRingtonePath                  (QString detail);
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
   
      //Updates
      virtual bool updateState();
   
      //Operators
      bool operator==(const Account&)const;

      //Mutator
      void saveCredentials  ();
      void saveAudioCodecs  ();
      void reloadCredentials();
      void reloadAudioCodecs();

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
      void edit()   {m_CurrentState = EDITING ;emit changed(this);};
      void modify() {m_CurrentState = MODIFIED;emit changed(this);};
      void remove() {m_CurrentState = REMOVED ;emit changed(this);};
      void cancel() {m_CurrentState = READY   ;emit changed(this);};
      void outdate(){m_CurrentState = OUTDATED;emit changed(this);};
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
#endif
