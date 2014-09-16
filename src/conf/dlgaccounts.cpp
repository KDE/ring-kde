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
#include "dlgaccounts.h"

//Qt
#include <QtCore/QString>
#include <qglobal.h>
#include <QtGui/QTableWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QWidget>

//KDE
#include <KConfigDialog>
#include <KDebug>
#include <KStandardDirs>
#include <KInputDialog>
#include <KLocale>
#include <KIcon>
#include <KMessageBox>
#include <KColorScheme>

//SFLPhone
#include "klib/kcfg_settings.h"
#include "conf/configurationdialog.h"
#include "lib/dbus/configurationmanager.h"
#include "sflphoneview.h"
#include "klib/tipmanager.h"
#include "klib/tip.h"
#include "lib/sflphone_const.h"
#include "lib/credentialmodel.h"
#include "lib/audiocodecmodel.h"
#include "lib/securityvalidationmodel.h"
#include "lib/accountlistmodel.h"
#include "lib/keyexchangemodel.h"
#include "lib/ringtonemodel.h"
#include "lib/tlsmethodmodel.h"
#include "lib/certificate.h"
#include "lib/securityvalidationmodel.h"
#include "lib/categorizedaccountmodel.h"
#include "../delegates/ringtonedelegate.h"
#include "../delegates/categorizeddelegate.h"

#define ACC account->

//OS
#ifdef Q_WS_WIN // MS Windows version
   #include <windows.h>
#endif
#ifdef Q_WS_X11
   #include <X11/XKBlib.h>
   #include <QX11Info>
#endif

///Constructor
DlgAccounts::DlgAccounts(KConfigDialog* parent)
 : QWidget(parent),m_IsLoading(false),m_pPKII(nullptr)
{
   m_IsLoading++;
   setupUi(this);
   button_accountUp->setIcon         ( KIcon( "go-up"       ) );
   button_accountDown->setIcon       ( KIcon( "go-down"     ) );
   m_pVCodecUpPB->setIcon            ( KIcon( "go-up"       ) );
   m_pVCodecDownPB->setIcon          ( KIcon( "go-down"     ) );
   button_accountAdd->setIcon        ( KIcon( "list-add"    ) );
   button_accountRemove->setIcon     ( KIcon( "list-remove" ) );
   button_add_credential->setIcon    ( KIcon( "list-add"    ) );
   button_remove_credential->setIcon ( KIcon( "list-remove" ) );
   button_audiocodecUp->setIcon      ( KIcon( "go-up"       ) );
   button_audiocodecDown->setIcon    ( KIcon( "go-down"     ) );
   treeView_accountList->setModel(CategorizedAccountModel::instance());
   CategorizedDelegate* m_pCategoryDelegate = new CategorizedDelegate(treeView_accountList);
   QStyledItemDelegate* m_pItemDelegate     = new QStyledItemDelegate;
   m_pCategoryDelegate->setChildDelegate(m_pItemDelegate);
   treeView_accountList->setItemDelegate(m_pCategoryDelegate);
   treeView_accountList->expandAll();

   //BEGIN Temporarely disable advanced security widgets
   m_pSecurityIssues->setVisible(false);
   frame->setVisible(false);
   frame_2->setVisible(false);
   //END

   m_pInfoIconL->setPixmap(KIcon("dialog-information").pixmap(QSize(32,32)));
   label_message_icon->setPixmap(KIcon("dialog-information").pixmap(QSize(24,24)));
   m_pCancelFix->setIcon(KIcon("dialog-close"));
   m_pCancelMove->setIcon(KIcon("dialog-close"));
   m_pMoveCertPB->setIcon(KIcon("folder-sync"));
   m_pFixCertPB->setIcon(KIcon("configure"));
   label_3->setPixmap(KIcon("dialog-information").pixmap(QSize(22,22)));
   label_2->setPixmap(KIcon("dialog-information").pixmap(QSize(22,22)));

   //Add an info tip in the account list
   m_pTipManager = new TipManager(treeView_accountList);
   m_pTip = new Tip(i18n("To add an account, press the \"Add\" button below. Use the "
   "⬆ up and ⬇ down button to change the default account priority."),this);
   m_pTipManager->setCurrentTip(m_pTip);

   m_pRingTonePath->setMode(KFile::File | KFile::ExistingOnly);
   m_pRingTonePath->lineEdit()->setObjectName("m_pRingTonePath");
   m_pRingTonePath->lineEdit()->setReadOnly(true);

   file_tls_authority->lineEdit()->setPlaceholderText(i18n("Usually called \"ca.crt\" or \"cacert.pem\""));
   file_tls_endpoint->lineEdit()->setPlaceholderText(i18n("A .pem or .crt"));
   file_tls_private_key->lineEdit()->setPlaceholderText(i18n("A .key file"));
   file_tls_authority->lineEdit()->setClearButtonShown(false);
   file_tls_endpoint->lineEdit()->setClearButtonShown(false);
   file_tls_private_key->lineEdit()->setClearButtonShown(false);

#if false
   //BEGIN Temporarely disable advanced security widgets
   //Authority
   m_pAuthorityII = new IssuesIcon(file_tls_authority->lineEdit());
   m_pAuthorityII->setupForLineEdit(file_tls_authority->lineEdit());
   connect(m_pAuthorityII,SIGNAL(selectFlaw(QModelIndex)),m_pSecurityIssues->view(),SLOT(setCurrentIndex(QModelIndex)));

   //End point
   m_pEndCertII = new IssuesIcon(file_tls_endpoint->lineEdit());
   m_pEndCertII->setupForLineEdit(file_tls_endpoint->lineEdit());
   connect(m_pEndCertII,SIGNAL(selectFlaw(QModelIndex)),m_pSecurityIssues->view(),SLOT(setCurrentIndex(QModelIndex)));

   //Private key
   m_pPKII = new IssuesIcon(file_tls_private_key->lineEdit());
   m_pPKII->setupForLineEdit(file_tls_private_key->lineEdit());
   connect(m_pPKII,SIGNAL(selectFlaw(QModelIndex)),m_pSecurityIssues->view(),SLOT(setCurrentIndex(QModelIndex)));

   connect(m_pVerifyServer,SIGNAL(selectFlaw(QModelIndex)),m_pSecurityIssues->view(),SLOT(setCurrentIndex(QModelIndex)));
   connect(m_pVerifyClient,SIGNAL(selectFlaw(QModelIndex)),m_pSecurityIssues->view(),SLOT(setCurrentIndex(QModelIndex)));
   connect(m_pReqTLS,SIGNAL(selectFlaw(QModelIndex)),m_pSecurityIssues->view(),SLOT(setCurrentIndex(QModelIndex)));
#endif
//END
//    loadAccountList();
   accountListHasChanged = false;

   combo_tls_method->setModel(TlsMethodModel::instance());

   m_pRingtoneListLW->horizontalHeader()->setResizeMode(0,QHeaderView::Stretch);
   m_pRingtoneListLW->horizontalHeader()->setResizeMode(1,QHeaderView::ResizeToContents);
   m_pRingtoneListLW->setItemDelegate(new RingToneDelegate(m_pRingtoneListLW));

   //SLOTS
   //                     SENDER                            SIGNAL                       RECEIVER              SLOT                          /
   /**/connect(edit1_alias,                       SIGNAL(textEdited(QString))            , this   , SLOT(changedAccountList())              );
   /**/connect(edit1_alias,                       SIGNAL(textEdited(QString))            , this   , SLOT(changeAlias(QString))              );
   /**/connect(edit2_protocol,                    SIGNAL(activated(int))                 , this   , SLOT(changedAccountList())              );
   /**/connect(edit3_server,                      SIGNAL(textEdited(QString))            , this   , SLOT(changedAccountList())              );
   /**/connect(edit4_user,                        SIGNAL(textEdited(QString))            , this   , SLOT(changedAccountList())              );
   /**/connect(edit4_user,                        SIGNAL(textEdited(QString))            , this   , SLOT(updateFirstCredential(QString))    );
   /**/connect(edit5_password,                    SIGNAL(textEdited(QString))            , this   , SLOT(changedAccountList())              );
   /**/connect(edit6_mailbox,                     SIGNAL(textEdited(QString))            , this   , SLOT(changedAccountList())              );
   /**/connect(m_pProxyLE,                        SIGNAL(textEdited(QString))            , this   , SLOT(changedAccountList())              );
   /**/connect(m_pProxyCK,                        SIGNAL(clicked(bool))                  , this   , SLOT(changedAccountList())              );
   /**/connect(m_pPresenceCK,                     SIGNAL(clicked(bool))                  , this   , SLOT(changedAccountList())              );
   /**/connect(m_pDTMFOverRTP,                    SIGNAL(clicked(bool))                  , this   , SLOT(changedAccountList())              );
   /**/connect(m_pDTMFOverSIP,                    SIGNAL(clicked(bool))                  , this   , SLOT(changedAccountList())              );
   /**/connect(m_pAutoAnswer,                     SIGNAL(clicked(bool))                  , this   , SLOT(changedAccountList())              );
   /**/connect(m_pEnableVideo,                    SIGNAL(clicked(bool))                  , this   , SLOT(changedAccountList())              );
   /**/connect(spinbox_regExpire,                 SIGNAL(valueChanged(int))              , this   , SLOT(changedAccountList())              );
   /**/connect(spinBox_pa_published_port,         SIGNAL(valueChanged(int))              , this   , SLOT(changedAccountList())              );
   /**/connect(comboBox_ni_local_address,         SIGNAL(currentIndexChanged(int))       , this   , SLOT(changedAccountList())              );
   /**/connect(spinBox_ni_local_port,             SIGNAL(valueChanged(int))              , this   , SLOT(changedAccountList())              );
   /**/connect(button_accountUp,                  SIGNAL(clicked())                      , this   , SLOT(changedAccountList())              );
   /**/connect(button_accountDown,                SIGNAL(clicked())                      , this   , SLOT(changedAccountList())              );
   /**/connect(button_accountAdd,                 SIGNAL(clicked())                      , this   , SLOT(otherAccountChanged())             );
   /**/connect(button_accountRemove,              SIGNAL(clicked())                      , this   , SLOT(otherAccountChanged())             );
   /**/connect(button_audiocodecDown,             SIGNAL(clicked())                      , this   , SLOT(changedAccountList())              );
   /**/connect(m_pDefaultAccount,                 SIGNAL(clicked(bool))                  , this   , SLOT(changedAccountList())              );
   /**/connect(checkbox_stun,                     SIGNAL(clicked(bool))                  , this   , SLOT(changedAccountList())              );
   /**/connect(button_audiocodecUp,               SIGNAL(clicked())                      , this   , SLOT(changedAccountList())              );
   /**/connect(lineEdit_pa_published_address,     SIGNAL(textEdited(QString))            , this   , SLOT(changedAccountList())              );
   /**/connect(lineEdit_pa_published_address,     SIGNAL(textEdited(QString))            , this   , SLOT(changedAccountList())              );
   /**/connect(edit_tls_private_key_password,     SIGNAL(textEdited(QString))            , this   , SLOT(changedAccountList())              );
   /**/connect(m_pUserAgent,                      SIGNAL(textEdited(QString))            , this   , SLOT(changedAccountList())              );
   /**/connect(spinbox_tls_listener,              SIGNAL(valueChanged(int))              , this   , SLOT(changedAccountList())              );
   /**/connect(m_pBitrateSB,                      SIGNAL(valueChanged(int))              , this   , SLOT(changedAccountList())              );
   /**/connect(file_tls_authority,                SIGNAL(textChanged(QString))           , this   , SLOT(changedAccountList())              );
   /**/connect(file_tls_endpoint,                 SIGNAL(textChanged(QString))           , this   , SLOT(changedAccountList())              );
   /**/connect(file_tls_private_key,              SIGNAL(textChanged(QString))           , this   , SLOT(changedAccountList())              );
   /**/connect(combo_tls_method,                  SIGNAL(currentIndexChanged(int))       , this   , SLOT(changedAccountList())              );
   /**/connect(combo_security_STRP,               SIGNAL(currentIndexChanged(int))       , this   , SLOT(changedAccountList())              );
   /**/connect(edit_tls_cipher,                   SIGNAL(textEdited(QString))            , this   , SLOT(changedAccountList())              );
   /**/connect(edit_tls_outgoing,                 SIGNAL(textEdited(QString))            , this   , SLOT(changedAccountList())              );
   /**/connect(spinbox_tls_timeout_sec,           SIGNAL(valueChanged(int))              , this   , SLOT(changedAccountList())              );
   /**/connect(spinbox_tls_timeout_msec,          SIGNAL(valueChanged(int))              , this   , SLOT(changedAccountList())              );
   /**/connect(check_tls_incoming,                SIGNAL(clicked(bool))                  , this   , SLOT(changedAccountList())              );
   /**/connect(check_tls_answer,                  SIGNAL(clicked(bool))                  , this   , SLOT(changedAccountList())              );
   /**/connect(check_tls_requier_cert,            SIGNAL(clicked(bool))                  , this   , SLOT(changedAccountList())              );
   /**/connect(group_security_tls,                SIGNAL(clicked(bool))                  , this   , SLOT(changedAccountList())              );
   /**/connect(groupbox_STRP_keyexchange,         SIGNAL(clicked(bool))                  , this   , SLOT(changedAccountList())              );
   /**/connect(radioButton_pa_same_as_local,      SIGNAL(clicked(bool))                  , this   , SLOT(changedAccountList())              );
   /**/connect(radioButton_pa_custom,             SIGNAL(clicked(bool))                  , this   , SLOT(changedAccountList())              );
   /**/connect(m_pUseCustomFileCK,                SIGNAL(clicked(bool))                  , this   , SLOT(changedAccountList())              );
   /**/connect(edit_credential_realm,             SIGNAL(textEdited(QString))            , this   , SLOT(changedAccountList())              );
   /**/connect(edit_credential_auth,              SIGNAL(textEdited(QString))            , this   , SLOT(changedAccountList())              );
   /**/connect(edit_credential_password,          SIGNAL(textEdited(QString))            , this   , SLOT(changedAccountList())              );
   /**/connect(edit_tls_private_key_password,     SIGNAL(textEdited(QString))            , this   , SLOT(changedAccountList())              );
   /**/connect(m_pMinAudioPort,                   SIGNAL(valueChanged(int))              , this   , SLOT(changedAccountList())              );
   /**/connect(m_pMaxAudioPort,                   SIGNAL(valueChanged(int))              , this   , SLOT(changedAccountList())              );
   /**/connect(m_pMinVideoPort,                   SIGNAL(valueChanged(int))              , this   , SLOT(changedAccountList())              );
   /**/connect(m_pMaxVideoPort,                   SIGNAL(valueChanged(int))              , this   , SLOT(changedAccountList())              );
   /**/connect(this,                              SIGNAL(updateButtons())                , parent , SLOT(updateButtons())                   );
   /**/connect(combo_security_STRP,               SIGNAL(currentIndexChanged(int))       , this   , SLOT(updateCombo(int))                  );
   /**/connect(button_add_credential,             SIGNAL(clicked())                      , this   , SLOT(addCredential())                   );
   /**/connect(button_remove_credential,          SIGNAL(clicked())                      , this   , SLOT(removeCredential())                );
   /**/connect(edit5_password,                    SIGNAL(textEdited(QString))            , this   , SLOT(main_password_field_changed())     );
   /**/connect(edit_credential_password,          SIGNAL(textEdited(QString))            , this   , SLOT(main_credential_password_changed()));
   /**/connect(button_audiocodecUp,               SIGNAL(clicked())                      , this   , SLOT(moveAudioCodecUp())                );
   /**/connect(button_audiocodecDown,             SIGNAL(clicked())                      , this   , SLOT(moveAudioCodecDown())              );
   /**/connect(m_pVCodecUpPB,                     SIGNAL(clicked())                      , this   , SLOT(moveVideoCodecUp())                );
   /**/connect(m_pVCodecDownPB,                   SIGNAL(clicked())                      , this   , SLOT(moveVideoCodecDown())              );
   /**/connect(AccountListModel::instance(),      SIGNAL(accountEnabledChanged(Account*)), this   , SLOT(otherAccountChanged())             );
   /**/connect(AccountListModel::instance(),      SIGNAL(accountStateChanged(Account*,QString)), this   , SLOT(updateStatusLabel(Account*)) );
   /*                                                                                                                                       */

   connect(treeView_accountList->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(accountListChanged(QModelIndex,QModelIndex)) );
   connect(treeView_accountList->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(updateAccountListCommands())                 );
   connect(treeView_accountList->model()         , SIGNAL(dataChanged(QModelIndex,QModelIndex))   , this, SLOT(updateStatusLabel())                         );


   //Disable control
   connect(radioButton_pa_same_as_local,   SIGNAL(clicked(bool)) , this , SLOT(enablePublished()));
   connect(radioButton_pa_custom,          SIGNAL(clicked(bool)) , this , SLOT(enablePublished()));


   if (CategorizedAccountModel::instance()->index(0,0).isValid()) {
      if (CategorizedAccountModel::instance()->index(0,0,CategorizedAccountModel::instance()->index(0,0)).isValid())
         treeView_accountList->setCurrentIndex(CategorizedAccountModel::instance()->index(0,0,CategorizedAccountModel::instance()->index(0,0)));
      else //IP2IP
         treeView_accountList->setCurrentIndex(CategorizedAccountModel::instance()->index(0,0,CategorizedAccountModel::instance()->index(1,0)));
      loadAccount(treeView_accountList->currentIndex());
   }
   m_IsLoading--;
} //DlgAccounts

///Destructor
DlgAccounts::~DlgAccounts()
{
   //accountList->disconnect();
   //if (accountList) delete accountList;
   delete m_pTipManager;
   delete m_pTip;
}

///Save an account using the values from the widgets
void DlgAccounts::saveAccount(const QModelIndex& item)
{
   const QModelIndex srcIdx = CategorizedAccountModel::instance()->mapToSource(item);

   if(!srcIdx.isValid()) {
      kDebug() << "Attempting to save details of an account from a NULL item";
      return;
   }
   Account* account = AccountListModel::instance()->getAccountByModelIndex(srcIdx);

   if(!account) {
      kDebug() << "Attempting to save details of an unexisting account : " << (srcIdx.data(Qt::DisplayRole).toString());
      return;
   }

   //There is no point to save something that is unaltered, all it will cause is daemon corruption
   if ( ACC state() != Account::AccountEditState::NEW and ACC state() != Account::AccountEditState::MODIFIED) {
      kDebug() << "Nothing to be saved";
      return;
   }
   m_IsLoading++;

   if (!m_pProxyCK->isChecked()) {
      m_pProxyLE->setText(QString());
   }

   const KeyExchangeModel::Type currentKeyExchange = (!groupbox_STRP_keyexchange->isChecked())?KeyExchangeModel::Type::NONE:static_cast<KeyExchangeModel::Type>(combo_security_STRP->currentIndex());

   //ACCOUNT DETAILS
   //                                                                     WIDGET VALUE                                 /
   /**/ ACC setProtocol                    ( static_cast<Account::Protocol>(edit2_protocol->currentIndex())           );
   /**/ ACC setAlias                       ( edit1_alias->text()                                                      );
   /**/ ACC setHostname                    ( edit3_server->text()                                                     );
   /**/ ACC setUsername                    ( edit4_user->text()                                                       );
   /**/ ACC setPassword                    ( edit5_password->text()                                                   );
   /**/ ACC setMailbox                     ( edit6_mailbox->text()                                                    );
   /**/ ACC setProxy                       ( m_pProxyLE->text()                                                       );
   /**/ ACC setPresenceEnabled             ( m_pPresenceCK->isChecked()                                               );
   /**/ ACC setEnabled                     ( item.data(Qt::CheckStateRole).toBool()                                   );
   /**/ ACC setRegistrationExpire          ( spinbox_regExpire->value()                                               );
   /**/                                                                                                             /**/
   /*                                            Security                                                             */
   /**/ ACC setTlsPassword                 ( edit_tls_private_key_password->text()                                    );
   /**/ ACC setTlsListenerPort             ( spinbox_tls_listener->value()                                            );
   /**/ ACC setTlsMethod                   ( static_cast<TlsMethodModel::Type>(combo_tls_method->currentIndex())      );
   /**/ ACC setTlsCiphers                  ( edit_tls_cipher->text()                                                  );
   /**/ ACC setTlsServerName               ( edit_tls_outgoing->text()                                                );
   /**/ ACC setTlsNegotiationTimeoutSec    ( spinbox_tls_timeout_sec->value()                                         );
   /**/ ACC setTlsNegotiationTimeoutMsec   ( spinbox_tls_timeout_msec->value()                                        );
   /**/ ACC setKeyExchange                 ( currentKeyExchange                                                       );
   /**/ ACC setTlsVerifyServer             ( check_tls_incoming->isChecked()                                          );
   /**/ ACC setTlsVerifyClient             ( check_tls_answer->isChecked()                                            );
   /**/ ACC setTlsRequireClientCertificate ( check_tls_requier_cert->isChecked()                                      );
   /**/ ACC setTlsEnabled                  ( group_security_tls->isChecked()                                          );
   /**/ ACC setDisplaySasOnce              ( checkbox_ZRTP_Ask_user->isChecked()                                      );
   /**/ ACC setSrtpRtpFallback             ( checkbox_SDES_fallback_rtp->isChecked()                                  );
   /**/ ACC setZrtpDisplaySas              ( checkbox_ZRTP_display_SAS->isChecked()                                   );
   /**/ ACC setZrtpNotSuppWarning          ( checkbox_ZRTP_warn_supported->isChecked()                                );
   /**/ ACC setZrtpHelloHash               ( checkbox_ZTRP_send_hello->isChecked()                                    );
   /**/ ACC setSipStunEnabled              ( checkbox_stun->isChecked()                                               );
   /**/ ACC setPublishedSameAsLocal        ( radioButton_pa_same_as_local->isChecked()                                );
   /**/ ACC setSipStunServer               ( line_stun->text()                                                        );
   /**/ ACC setPublishedPort               ( spinBox_pa_published_port->value()                                       );
   /**/ ACC setPublishedAddress            ( lineEdit_pa_published_address ->text()                                   );
   /**/ ACC setLocalInterface              ( comboBox_ni_local_address->currentText()                                 );
   /**/ ACC setRingtoneEnabled             ( m_pEnableRingtoneGB->isChecked()                                         );
   /**/ ACC setDTMFType                    ( m_pDTMFOverRTP->isChecked()?DtmfType::OverRtp:DtmfType::OverSip          );
   /**/ ACC setAutoAnswer                  ( m_pAutoAnswer->isChecked()                                               );
   /**/ ACC setLocalPort                   ( spinBox_ni_local_port->value()                                           );
   /**/ ACC setSrtpEnabled                 ( groupbox_STRP_keyexchange->isChecked()                                   );
   /**/ ACC setAudioPortMax                ( m_pMaxAudioPort->value()                                                 );
   /**/ ACC setAudioPortMin                ( m_pMinAudioPort->value()                                                 );
   /**/ ACC setVideoPortMax                ( m_pMaxVideoPort->value()                                                 );
   /**/ ACC setVideoPortMin                ( m_pMinVideoPort->value()                                                 );
   /**/ ACC setUserAgent                   ( m_pUserAgent->text()                                                     );
   /**/ ACC setVideoEnabled                ( m_pEnableVideo->isChecked()                                              );
   //                                                                                                                  /

   /**/ ACC tlsCaListCertificate()->setPath( file_tls_authority->text()                                               );
   /**/ ACC tlsCertificate ()->setPath     ( file_tls_endpoint->text()                                                );
   /**/ ACC tlsPrivateKeyCertificate()->setPath( file_tls_private_key->text()                                         );


   if (m_pDefaultAccount->isChecked()) {
      ConfigurationSkeleton::setDefaultAccountId(ACC id());
      AccountListModel::instance()->setDefaultAccount(account);
   }

   //Ringtone
   if (!m_pUseCustomFileCK->isChecked()) {
      ACC setRingtonePath( m_pRingtoneListLW->currentIndex().data(RingToneModel::Role::FullPath).toString() );
   }
   else {
      ACC setRingtonePath( m_pRingTonePath->url().path() );
   }

   if (m_pCodecsLW->currentIndex().isValid())
      m_pCodecsLW->model()->setData(m_pCodecsLW->currentIndex(),m_pBitrateSB->value(),VideoCodecModel::BITRATE_ROLE);
   saveCredential();
   m_IsLoading--;
} //saveAccount

void DlgAccounts::cancel()
{
   AccountListModel::instance()->cancel();
}

///Load an account, set all field to the right value
void DlgAccounts::loadAccount(QModelIndex item)
{
   const QModelIndex srcItem = CategorizedAccountModel::instance()->mapToSource(item);
   if(! srcItem.isValid() ) {
      kDebug() << "Attempting to load details of an account from a NULL item (" << item.row() << ")";
      return;
   }

   Account* account = AccountListModel::instance()->getAccountByModelIndex(srcItem);
   if(! account ) {
      kDebug() << "Attempting to load details of an unexisting account";
      return;
   }
   m_IsLoading++;

   edit1_alias->setText( ACC alias());

   const int protocolIndex = static_cast<int>( ACC protocol());

   if ( ACC protocol() == Account::Protocol::SIP) {
      const QModelIndex idx = ACC credentialsModel()->index(0,0);
      disconnect(edit5_password, SIGNAL(textEdited(QString)), this , SLOT(main_password_field_changed()));
      if (idx.isValid() && !account->id().isEmpty()) {
         edit5_password->setText( ACC credentialsModel()->data(idx,CredentialModel::Role::PASSWORD).toString());
      }
      else
         edit5_password->setText(QString());
      connect(edit5_password, SIGNAL(textEdited(QString)), this , SLOT(main_password_field_changed()));
   }
   else {
      edit5_password->setText( ACC password());
   }

   disconnect(this,SLOT(aliasChanged(QString)));
   connect(account,SIGNAL(aliasChanged(QString)),this,SLOT(aliasChanged(QString)));

   //         WIDGET VALUE                                          VALUE                     /
   /**/edit2_protocol->setCurrentIndex          ( (protocolIndex < 0) ? 0 : protocolIndex    );
   /**/edit3_server->setText                    (  ACC hostname                        ());
   /**/edit4_user->setText                      (  ACC username                        ());
   /**/edit6_mailbox->setText                   (  ACC mailbox                         ());
   /**/m_pProxyLE->setText                      (  ACC proxy                           ());
   /**/m_pPresenceCK->setChecked                (  ACC presenceEnabled                 ());
   /**/m_pPresenceCK->setEnabled                (  ACC supportPresencePublish() || ACC supportPresenceSubscribe());
   /**/checkbox_ZRTP_Ask_user->setChecked       (  ACC isDisplaySasOnce                ());
   /**/checkbox_SDES_fallback_rtp->setChecked   (  ACC isSrtpRtpFallback               ());
   /**/checkbox_ZRTP_display_SAS->setChecked    (  ACC isZrtpDisplaySas                ());
   /**/checkbox_ZRTP_warn_supported->setChecked (  ACC isZrtpNotSuppWarning            ());
   /**/checkbox_ZTRP_send_hello->setChecked     (  ACC isZrtpHelloHash                 ());
   /**/checkbox_stun->setChecked                (  ACC isSipStunEnabled                ());
   /**/m_pEnableVideo->setChecked               (  ACC isVideoEnabled                  ());
   /**/line_stun->setText                       (  ACC sipStunServer                   ());
   /**/spinbox_regExpire->setValue              (  ACC registrationExpire              ());
   /**/radioButton_pa_same_as_local->setChecked (  ACC isPublishedSameAsLocal          ());
   /**/radioButton_pa_custom->setChecked        ( !account->isPublishedSameAsLocal     ());
   /**/lineEdit_pa_published_address->setText   (  ACC publishedAddress                ());
   /**/spinBox_pa_published_port->setValue      (  ACC publishedPort                   ());
   /*                                                  Security                             **/
   /**/edit_tls_private_key_password->setText   (  ACC tlsPassword                     ());
   /**/spinbox_tls_listener->setValue           (  ACC tlsListenerPort                 ());
   /**/file_tls_authority->setText              (  ACC tlsCaListCertificate    ()->path().toLocalFile());
   /**/file_tls_endpoint->setText               (  ACC tlsCertificate          ()->path().toLocalFile());
   /**/file_tls_private_key->setText            (  ACC tlsPrivateKeyCertificate()->path().toLocalFile());
   /**/edit_tls_cipher->setText                 (  ACC tlsCiphers                      ());
   /**/edit_tls_outgoing->setText               (  ACC tlsServerName                   ());
   /**/spinbox_tls_timeout_sec->setValue        (  ACC tlsNegotiationTimeoutSec        ());
   /**/spinbox_tls_timeout_msec->setValue       (  ACC tlsNegotiationTimeoutMsec       ());
   /**/check_tls_incoming->setChecked           (  ACC isTlsVerifyServer               ());
   /**/check_tls_answer->setChecked             (  ACC isTlsVerifyClient               ());
   /**/check_tls_requier_cert->setChecked       (  ACC isTlsRequireClientCertificate   ());
   /**/group_security_tls->setChecked           (  ACC isTlsEnabled                    ());
   /**/m_pAutoAnswer->setChecked                (  ACC isAutoAnswer                    ());
   /**/m_pMinVideoPort->setValue                (  ACC videoPortMin                    ());
   /**/m_pMaxVideoPort->setValue                (  ACC videoPortMax                    ());
   /**/m_pMaxAudioPort->setValue                (  ACC audioPortMax                    ());
   /**/m_pMinAudioPort->setValue                (  ACC audioPortMin                    ());
   /**/m_pUserAgent->setText                    (  ACC userAgent                       ());
   /*                                                                                        */

   combo_security_STRP->setModel(ACC keyExchangeModel());
   groupbox_STRP_keyexchange->setChecked(ACC isSrtpEnabled());
   if (!(ACC keyExchange() == KeyExchangeModel::Type::NONE)) {
      /**/combo_security_STRP->setCurrentIndex     (  ACC keyExchangeModel()->toIndex( ACC keyExchange()).row());
   }

   //BUG the daemon doesn't support changing account type after
   edit2_protocol->setEnabled(ACC state() == Account::AccountEditState::NEW || ACC id().isEmpty());

   m_pCodecsLW->setEnabled(ACC isVideoEnabled ());

   updateCombo(0);

   m_pDTMFOverRTP->setChecked( ACC DTMFType()==DtmfType::OverRtp);
   m_pDTMFOverSIP->setChecked( ACC DTMFType()==DtmfType::OverSip);

   edit_credential_realm    -> setText(QString());
   edit_credential_auth     -> setText(QString());
   edit_credential_password -> setText(QString());
   edit_credential_realm    -> setEnabled(false);
   edit_credential_auth     -> setEnabled(false);
   edit_credential_password -> setEnabled(false);
   m_pProxyCK               -> setChecked(m_pProxyLE->text().size());
   m_pProxyLE               -> setEnabled(m_pProxyCK->isChecked());


   disconnect(m_pDefaultAccount, SIGNAL(clicked(bool)) , this , SLOT(changedAccountList()) );
   m_pDefaultAccount->setChecked(account == AccountListModel::instance()->getDefaultAccount());
   connect(m_pDefaultAccount,    SIGNAL(clicked(bool)) , this , SLOT(changedAccountList()) );


   disconnect(list_credential->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(selectCredential(QModelIndex,QModelIndex))     );
   list_credential->setModel( ACC credentialsModel());
   connect(list_credential->selectionModel()   ,SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(selectCredential(QModelIndex,QModelIndex))     );

   disconnect(list_audiocodec->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(selectedCodecChanged(QModelIndex,QModelIndex)) );
   disconnect(list_audiocodec->model()         ,SIGNAL(dataChanged(QModelIndex,QModelIndex)),    this, SLOT(changedAccountList())                          );
   list_audiocodec->setModel( ACC audioCodecModel());
   connect(list_audiocodec->model()            ,SIGNAL(dataChanged(QModelIndex,QModelIndex)),    this, SLOT(changedAccountList())                          );
   connect(list_audiocodec->selectionModel()   ,SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(selectedCodecChanged(QModelIndex,QModelIndex)) );

   #ifdef ENABLE_VIDEO
   disconnect(m_pCodecsLW->selectionModel()    ,SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(loadVidCodecDetails(QModelIndex,QModelIndex))  );
   disconnect(m_pCodecsLW->model()             ,SIGNAL(dataChanged(QModelIndex,QModelIndex)),    this, SLOT(changedAccountList())                          );
   ACC videoCodecModel()->reload();
   m_pCodecsLW->setModel( ACC videoCodecModel());
   connect(m_pCodecsLW->model()                ,SIGNAL(dataChanged(QModelIndex,QModelIndex)),    this, SLOT(changedAccountList())                          );
   connect(m_pCodecsLW->selectionModel()       ,SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(loadVidCodecDetails(QModelIndex,QModelIndex))  );
   #endif

   ACC securityValidationModel()->update();
   m_pSecurityIssues->setModel(ACC securityValidationModel());


   //Enable tabs
   bool isntIP2IP = ! ( ACC alias() == Account::ProtocolName::IP2IP );
   bool isIAX     = ACC protocol()  == Account::Protocol::IAX;
   bool enableTab[7] = {isntIP2IP,isntIP2IP,isntIP2IP,true,isntIP2IP && !isIAX,isntIP2IP,true};
   for (int i=0;i<7;i++)
      frame2_editAccounts->setTabEnabled( i, enableTab[i] );

   //Setup ringtone
   m_pEnableRingtoneGB->setChecked( ACC isRingtoneEnabled());
   const QString ringtonePath = KStandardDirs::realFilePath( ACC ringtonePath());
   m_pRingTonePath->setUrl( ringtonePath );

   combo_tls_method->setCurrentIndex( TlsMethodModel::instance()->toIndex( ACC tlsMethod()).row() );
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();

   m_pRingtoneListLW->setModel( ACC ringToneModel());
   const QModelIndex& rtIdx = ACC ringToneModel()->currentIndex();
   if (rtIdx.isValid()) {
      m_pRingtoneListLW->setEnabled(true);
      m_pRingTonePath->setEnabled(false);
      m_pUseCustomFileCK->setChecked(false);
      disconnect(m_pRingtoneListLW->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(changedAccountList()));
      m_pRingtoneListLW->setCurrentIndex(rtIdx);
      connect(m_pRingtoneListLW->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(changedAccountList()));
   }
   else {
      m_pRingTonePath->setEnabled(true);
      m_pUseCustomFileCK->setChecked(true);
      m_pRingtoneListLW->setEnabled(false);
   }

   #ifndef ENABLE_VIDEO
   m_pVideoCodecGB->setVisible(false);
   #endif

   comboBox_ni_local_address->clear();
   QStringList interfaceList = configurationManager.getAllIpInterfaceByName();
   comboBox_ni_local_address->addItems(interfaceList);

   spinBox_ni_local_port->setValue( ACC localPort());
   if (comboBox_ni_local_address->findText( ACC localInterface()) >=0)
      comboBox_ni_local_address->setCurrentIndex(comboBox_ni_local_address->findText( ACC localInterface()));
   else //Just to be sure
      comboBox_ni_local_address->setCurrentIndex(0);

   if(protocolIndex == 0 || ACC isNew()) { // if sip selected
      checkbox_stun->setChecked( ACC isSipStunEnabled());
      line_stun->setText( ACC sipStunServer() );

      tab_advanced->                setEnabled ( true                       );
      line_stun->                   setEnabled ( checkbox_stun->isChecked() );
      radioButton_pa_same_as_local->setDisabled( checkbox_stun->isChecked() );
      radioButton_pa_custom->       setDisabled( checkbox_stun->isChecked() );
   }
   else {
      checkbox_stun->setChecked(false);
      tab_advanced->setEnabled (false);
      line_stun->setText( ACC sipStunServer() );
   }

   updateSecurityValidation();

   updateStatusLabel(account);
   enablePublished();
   frame2_editAccounts->setEnabled(true);
   m_IsLoading--;
   ACC performAction(Account::AccountEditAction::EDIT);
   emit updateButtons();
} //loadAccount

///Load an account
void DlgAccounts::loadAccountList()
{
//    AccountListModel::instance()->updateAccounts();
   if (treeView_accountList->model()->rowCount() > 0 && !treeView_accountList->currentIndex().isValid()) {
      if (CategorizedAccountModel::instance()->index(0,0,CategorizedAccountModel::instance()->index(0,0)).isValid())
         treeView_accountList->setCurrentIndex(CategorizedAccountModel::instance()->index(0,0,CategorizedAccountModel::instance()->index(0,0)));
      else //IP2IP
         treeView_accountList->setCurrentIndex(CategorizedAccountModel::instance()->index(0,0,CategorizedAccountModel::instance()->index(1,0)));
      loadAccount(treeView_accountList->currentIndex());
   }
   else
      frame2_editAccounts->setEnabled(treeView_accountList->currentIndex().isValid());
   updateAccountListCommands();
}

///Called when one of the child widget is modified
void DlgAccounts::changedAccountList()
{
   if (!m_IsLoading) {
      Account* acc = currentAccount();
      if (acc)
         acc->performAction(Account::AccountEditAction::MODIFY);
      accountListHasChanged = true;
      emit updateButtons();
   }
}

void DlgAccounts::otherAccountChanged()
{
   accountListHasChanged = true;
   if (!m_IsLoading) {
      emit updateButtons();
   }
   updateStatusLabel(treeView_accountList->currentIndex());
}

///Callback when the account change
void DlgAccounts::accountListChanged(const QModelIndex& current, const QModelIndex& previous)
{
   saveAccount(previous);
   const QModelIndex srcPrevious = CategorizedAccountModel::instance()->mapToSource(previous);
   if (srcPrevious.isValid()) {
      Account* acc = AccountListModel::instance()->getAccountByModelIndex(srcPrevious);
      if (acc && (acc->state() == Account::AccountEditState::EDITING || acc->state() == Account::AccountEditState::OUTDATED))
         acc->performAction(Account::AccountEditAction::CANCEL);
   }
   loadAccount(current);
}

///Move account up
void DlgAccounts::on_button_accountUp_clicked()
{
   const QModelIndex cur = treeView_accountList->currentIndex();
   Account* acc = currentAccount();
   AccountListModel::instance()->accountUp(acc->index().row());
   treeView_accountList->setCurrentIndex(CategorizedAccountModel::instance()->index(cur.row()-1,0,cur.parent()));
}

///Move account down
void DlgAccounts::on_button_accountDown_clicked()
{
   const QModelIndex cur = treeView_accountList->currentIndex();
   Account* acc = currentAccount();
   AccountListModel::instance()->accountDown(acc->index().row());
   treeView_accountList->setCurrentIndex(CategorizedAccountModel::instance()->index(cur.row()+1,0,cur.parent()));
}

///Add new account
void DlgAccounts::on_button_accountAdd_clicked()
{
   const QString newAlias = i18n("New account%1",AccountListModel::getSimilarAliasIndex(i18n("New account")));
   Account* newAcc = AccountListModel::instance()->addAccount(newAlias);
   const QModelIndex& newIndex = CategorizedAccountModel::instance()->mapFromSource(newAcc->index());
   treeView_accountList->setCurrentIndex(newIndex);

   frame2_editAccounts->setEnabled(true);
   frame2_editAccounts->setCurrentIndex(0);
   edit1_alias->setSelection(0,edit1_alias->text().size());
   edit1_alias->setFocus(Qt::OtherFocusReason);
} //on_button_accountAdd_clicked

///Remove selected account
void DlgAccounts::on_button_accountRemove_clicked()
{
   Account* acc = currentAccount();
   const int ret = KMessageBox::questionYesNo(this, i18n("Are you sure you want to remove %1?",acc->alias()), i18n("Remove account"));
   if (ret == KMessageBox::Yes) {
      AccountListModel::instance()->removeAccount(CategorizedAccountModel::instance()->mapToSource(treeView_accountList->currentIndex()));
      if (CategorizedAccountModel::instance()->index(0,0,CategorizedAccountModel::instance()->index(0,0)).isValid())
         treeView_accountList->setCurrentIndex(CategorizedAccountModel::instance()->index(0,0,CategorizedAccountModel::instance()->index(0,0)));
      else //IP2IP
         treeView_accountList->setCurrentIndex(CategorizedAccountModel::instance()->index(0,0,CategorizedAccountModel::instance()->index(1,0)));
      loadAccount(treeView_accountList->currentIndex());
   }
}

///Update account list
void DlgAccounts::updateAccountListCommands()
{
   bool isCategory        = ((!treeView_accountList->currentIndex().isValid()) || !treeView_accountList->currentIndex().parent().isValid());
   bool isIP2IP           = treeView_accountList->currentIndex().parent().row()==1;
   bool buttonsEnabled[4] = {!isCategory && !isIP2IP,!isCategory && !isIP2IP,!isCategory,!isCategory && !isIP2IP};
   buttonsEnabled[0]     &= (treeView_accountList->currentIndex().row() != 0);
   const QModelIndex cur  = treeView_accountList->currentIndex();
   buttonsEnabled[1]     &= CategorizedAccountModel::instance()->index(cur.row()+1,0,cur.parent()).isValid();

   button_accountUp->setEnabled     ( buttonsEnabled[0] );
   button_accountDown->setEnabled   ( buttonsEnabled[1] );
   button_accountAdd->setEnabled    ( buttonsEnabled[2] );
   button_accountRemove->setEnabled ( buttonsEnabled[3] );
}

///Password changed
void DlgAccounts::main_password_field_changed()
{
   list_credential->model()->setData(list_credential->model()->index(0,0),edit5_password->text(),CredentialModel::Role::PASSWORD);
#ifdef Q_WS_WIN // MS Windows version
      if (GetKeyState(VK_CAPITAL) == 1) {
#endif
#ifdef Q_WS_X11 // X11 version
      uint n = 0;
      Display *d = QX11Info::display();
      XkbGetIndicatorState(d, XkbUseCoreKbd, &n);
      if ((n & 0x01) == 1) {
#else
      //# error Platform not supported
      if (false) {
#endif
      label_message->setText("<b>"+i18n("Warning, caps lock is turned on!")+"</b>");
      label_message_icon->setPixmap(KIcon("dialog-warning").pixmap(QSize(32,32)));
   }
   else {
      label_message->setText(i18n("Fields marked with \"*\" are compulsory and required"));
      label_message_icon->setPixmap(KIcon("dialog-information").pixmap(QSize(24,24)));
   }
}

///Credential changed
void DlgAccounts::main_credential_password_changed()
{
   if (list_credential->currentIndex().row() == 0) {
      edit5_password->setText(edit_credential_password->text());
   }
}

///Update the first credential
void DlgAccounts::updateFirstCredential(QString text)
{
   if (!m_IsLoading) {
      Account* acc = currentAccount();
      acc->credentialsModel()->setData(acc->credentialsModel()->index(0,0),text, CredentialModel::Role::NAME);
      if (acc->credentialsModel()->index(0,0) == list_credential->currentIndex()) {
         edit_credential_auth->setText(text);
      }
   }
}

///Move codec up
void DlgAccounts::moveAudioCodecUp()
{
   if (static_cast<AudioCodecModel*>(list_audiocodec->model())->moveUp(list_audiocodec->currentIndex()))
      list_audiocodec->setCurrentIndex(list_audiocodec->model()->index(list_audiocodec->currentIndex().row()-1,0));
}

///Move codec down
void DlgAccounts::moveAudioCodecDown()
{
   if (static_cast<AudioCodecModel*>(list_audiocodec->model())->moveDown(list_audiocodec->currentIndex()))
      list_audiocodec->setCurrentIndex(list_audiocodec->model()->index(list_audiocodec->currentIndex().row()+1,0));
}

///Move video codec up
void DlgAccounts::moveVideoCodecUp()
{
   if (static_cast<VideoCodecModel*>(m_pCodecsLW->model())->moveUp(m_pCodecsLW->currentIndex()))
      m_pCodecsLW->setCurrentIndex(m_pCodecsLW->model()->index(m_pCodecsLW->currentIndex().row()-1,0));
}

///Move video codec down
void DlgAccounts::moveVideoCodecDown()
{
   if (static_cast<VideoCodecModel*>(m_pCodecsLW->model())->moveDown(m_pCodecsLW->currentIndex()))
      m_pCodecsLW->setCurrentIndex(m_pCodecsLW->model()->index(m_pCodecsLW->currentIndex().row()+1,0));
}

///Load the video codec list
void DlgAccounts::loadVidCodecDetails(const QModelIndex& current,const QModelIndex& previous)
{
   if (previous != current && previous.isValid()) {
      m_pCodecsLW->model()->setData(previous,m_pBitrateSB->value(),VideoCodecModel::BITRATE_ROLE);
   }

   const int bitrate = m_pCodecsLW->model()->data(current,VideoCodecModel::BITRATE_ROLE).toInt();
   m_pBitrateSB->setValue(bitrate);
}

///Update account state
// void DlgAccounts::updateAccountStates()
// {
//    kDebug() << "updateAccountStates";
//    for (int i = 0; i < AccountListModel::instance()->size(); i++) {
//       Account* current = AccountListModel::instance()->getAccountAt(i);
//       current->updateState();
//    }
//    updateStatusLabel(treeView_accountList->currentIndex());
// }

///Update the status label to current account state
void DlgAccounts::updateStatusLabel(const QModelIndex& item)
{
   const QModelIndex srcItem = CategorizedAccountModel::instance()->mapToSource(item);
   if(!srcItem.isValid())
      return;
   Account* account = AccountListModel::instance()->getAccountByModelIndex(srcItem);
   if (account)
      updateStatusLabel(account);
}

///Update the status label to current account state
void DlgAccounts::updateStatusLabel(Account* account)
{
   if(!account || currentAccount() != account)
      return;
   const QString status = ACC toHumanStateName();
   const QString extra = ACC protocol() == Account::Protocol::SIP && ACC lastErrorCode()>=0
      ? (QString(" (%1)").arg(ACC lastErrorCode())) :QString();
   edit7_state->setText( "<FONT COLOR=\"" + ACC stateColorName() + "\">" + status + "</FONT>"+extra );
}

void DlgAccounts::updateStatusLabel()
{
   updateStatusLabel(treeView_accountList->currentIndex());
}

///Have the account changed
bool DlgAccounts::hasChanged()
{
   return accountListHasChanged;
}

///Have all required fields completed?
bool DlgAccounts::hasIncompleteRequiredFields()
{
   Account* acc = currentAccount();
   if (!acc) return false;
   QList<QLabel*> requiredFieldsLabels;
   if (!requiredFieldsLabels.size()) {
      requiredFieldsLabels << label1_alias << label3_server << label4_user << label5_password;
   }

   bool fields[4] = {edit1_alias->text().isEmpty(),edit3_server->text().isEmpty(),edit4_user->text().isEmpty(),edit5_password->text().isEmpty()};
   bool isIncomplete = acc && (acc->alias() != Account::ProtocolName::IP2IP) && (fields[0]|fields[1]|fields[2]|fields[3]);
   //Add visual feedback for missing fields
   for (int i=0;i<requiredFieldsLabels.size();i++) {
      static KStatefulBrush errorBrush( KColorScheme::Window, KColorScheme::NegativeText );
      QPalette pal = QApplication::palette();
      pal.setBrush(QPalette::WindowText,fields[i]?errorBrush.brush(QPalette::Normal):pal.windowText());
      requiredFieldsLabels[i]->setPalette(pal);
   }
   return isIncomplete;
}

///Save settings
void DlgAccounts::updateSettings()
{
   if(accountListHasChanged) {
      if(treeView_accountList->currentIndex().isValid()) {
         Account* acc = currentAccount();
         saveAccount(treeView_accountList->currentIndex());
         if (acc && (acc->state() == Account::AccountEditState::EDITING || acc->state() == Account::AccountEditState::OUTDATED))
            acc->performAction(Account::AccountEditAction::CANCEL);
      }

      AccountListModel::instance()->save();
      accountListHasChanged = false;
   }
}

///Reload
void DlgAccounts::updateWidgets()
{
   loadAccountList();
   updateStatusLabel(treeView_accountList->currentIndex());
   accountListHasChanged = false;
}

///The audio codec changed
void DlgAccounts::selectedCodecChanged(const QModelIndex& current,const QModelIndex& previous)
{
   Q_UNUSED(previous)
   label_bitrate_value->setText   ( list_audiocodec->model()->data(current,AudioCodecModel::Role::BITRATE)   .toString());
   label_frequency_value->setText ( list_audiocodec->model()->data(current,AudioCodecModel::Role::SAMPLERATE).toString());
}

///Select available security options for various methods
void DlgAccounts::updateCombo(int value)
{
   Q_UNUSED(value)
   static const bool enabledCombo[3][5] = {
      /*KeyExchangeModel::Type::ZRTP*/ {false,true ,true ,true ,true },
      /*KeyExchangeModel::Type::SDES*/ {true ,false,false,false,false},
      /*KeyExchangeModel::Type::NONE*/ {false,false,false,false,false},
   };
   const int idx = combo_security_STRP->currentIndex();
   if (idx >= 3) return;
   checkbox_SDES_fallback_rtp->setVisible   ( enabledCombo[idx][0] );
   checkbox_ZRTP_Ask_user->setVisible       ( enabledCombo[idx][1] );
   checkbox_ZRTP_display_SAS->setVisible    ( enabledCombo[idx][2] );
   checkbox_ZRTP_warn_supported->setVisible ( enabledCombo[idx][3] );
   checkbox_ZTRP_send_hello->setVisible     ( enabledCombo[idx][4] );
} //updateCombo

///Save the current credential
void DlgAccounts::saveCredential()
{
   Account* acc = currentAccount();
   if (!acc) return;
   const QModelIndex currentCredential = list_credential->currentIndex();
   if (currentCredential.isValid()) {
      acc->credentialsModel()->setData(currentCredential,edit_credential_auth->text()    , CredentialModel::Role::NAME     );
      acc->credentialsModel()->setData(currentCredential,edit_credential_password->text(), CredentialModel::Role::PASSWORD );
      acc->credentialsModel()->setData(currentCredential,edit_credential_realm->text()   , CredentialModel::Role::REALM    );
   }
   acc->saveCredentials();
} //saveCredential

///Add a new credential
void DlgAccounts::addCredential()
{
   list_credential->setCurrentIndex(currentAccount()->credentialsModel()->addCredentials());
} //addCredential

///Save and load a credential
void DlgAccounts::selectCredential(const QModelIndex& item, const QModelIndex& previous)
{
   list_credential->model()->setData(previous,edit_credential_auth->text()    , CredentialModel::Role::NAME     );
   list_credential->model()->setData(previous,edit_credential_password->text(), CredentialModel::Role::PASSWORD );
   list_credential->model()->setData(previous,edit_credential_realm->text()   , CredentialModel::Role::REALM    );

   edit_credential_realm->setText       ( list_credential->model()->data(item,CredentialModel::Role::REALM)    .toString());
   edit_credential_auth->setText        ( list_credential->model()->data(item,CredentialModel::Role::NAME)     .toString());
   edit_credential_password->setText    ( list_credential->model()->data(item,CredentialModel::Role::PASSWORD) .toString());

   edit_credential_realm->setEnabled    ( true );
   edit_credential_auth->setEnabled     ( true );
   edit_credential_password->setEnabled ( true );
//TODO
} //selectCredential

///Remove a credential
void DlgAccounts::removeCredential() {
   Account* acc = currentAccount();
   acc->credentialsModel()->removeCredentials(list_credential->currentIndex());
   list_credential->setCurrentIndex(acc->credentialsModel()->index(0,0));
}

///Enable published
void DlgAccounts::enablePublished()
{
   lineEdit_pa_published_address->setDisabled(radioButton_pa_same_as_local->isChecked());
   spinBox_pa_published_port->setDisabled(radioButton_pa_same_as_local->isChecked());
}

///Force a new alias for the account
void DlgAccounts::aliasChanged(QString newAlias)
{
   if (newAlias != edit1_alias->text())
      edit1_alias->setText(newAlias);
}

///Force a new alias for the account
void DlgAccounts::changeAlias(QString newAlias)
{
   Account* acc = currentAccount();
   if (acc && newAlias != acc->alias()) {
      const QModelIndex src = CategorizedAccountModel::instance()->mapToSource(treeView_accountList->currentIndex());
      AccountListModel::instance()->setData(src,newAlias,Qt::EditRole);
   }
}

Account* DlgAccounts::currentAccount() const
{
   return AccountListModel::instance()->getAccountByModelIndex(CategorizedAccountModel::instance()->mapToSource(treeView_accountList->currentIndex()));
}


void DlgAccounts::addFlawToCertificateField(const Flaw* flaw)
{
Q_UNUSED(flaw)
//BEGIN Temporarely disable advanced security widgets
#if false
   switch(flaw->type()) {
      case Certificate::Type::AUTHORITY:
         m_pAuthorityII->addFlaw(flaw);
         break;
      case Certificate::Type::USER:
         m_pEndCertII->addFlaw(flaw);
         break;
      case Certificate::Type::PRIVATE_KEY:
         m_pPKII->addFlaw(flaw);
         break;
      case Certificate::Type::NONE:
         qDebug() << "Invalid certificate type";
         break;
   };
#endif
//END
}


void DlgAccounts::updateSecurityValidation()
{
#if false
   Account* a = currentAccount();
   //Create the widgets
   m_pVerifyServer->setModel(a->securityValidationModel());
   m_pVerifyClient->setModel(a->securityValidationModel());
   m_pReqTLS->setModel(a->securityValidationModel());
   m_pAuthorityII->setModel(a->securityValidationModel());
   m_pEndCertII->setModel(a->securityValidationModel());
   m_pPKII->setModel(a->securityValidationModel());

//    IssuesIcon*

//    newWidget->addFlaw(new Flaw(SecurityValidationModel::SecurityFlaw::TLS_DISABLED,Certificate::Type::NONE));

   //Add the flaws
   foreach(const Flaw* flaw, currentAccount()->securityValidationModel()->currentFlaws()) {
      switch (flaw->flaw()) {
         case SecurityValidationModel::SecurityFlaw::SRTP_DISABLED:

            break;
         case SecurityValidationModel::SecurityFlaw::TLS_DISABLED:

            break;
         case SecurityValidationModel::SecurityFlaw::CERTIFICATE_EXPIRED:
            addFlawToCertificateField(flaw);
            break;
         case SecurityValidationModel::SecurityFlaw::CERTIFICATE_SELF_SIGNED:
            addFlawToCertificateField(flaw);
            break;
         case SecurityValidationModel::SecurityFlaw::CA_CERTIFICATE_MISSING:
            addFlawToCertificateField(flaw);
            break;
         case SecurityValidationModel::SecurityFlaw::END_CERTIFICATE_MISSING:
            addFlawToCertificateField(flaw);
            break;
         case SecurityValidationModel::SecurityFlaw::PRIVATE_KEY_MISSING:
            addFlawToCertificateField(flaw);
            break;
         case SecurityValidationModel::SecurityFlaw::CERTIFICATE_MISMATCH:

            break;
         case SecurityValidationModel::SecurityFlaw::CERTIFICATE_STORAGE_PERMISSION:

            break;
         case SecurityValidationModel::SecurityFlaw::CERTIFICATE_STORAGE_FOLDER:

            break;
         case SecurityValidationModel::SecurityFlaw::CERTIFICATE_STORAGE_LOCATION:

            break;
         case SecurityValidationModel::SecurityFlaw::OUTGOING_SERVER_MISMATCH:

            break;
         case SecurityValidationModel::SecurityFlaw::VERIFY_INCOMING_DISABLED:
            m_pVerifyServer->addFlaw(flaw);
            break;
         case SecurityValidationModel::SecurityFlaw::VERIFY_ANSWER_DISABLED:
            m_pVerifyClient->addFlaw(flaw);
            break;
         case SecurityValidationModel::SecurityFlaw::REQUIRE_CERTIFICATE_DISABLED:

            break;
         case SecurityValidationModel::SecurityFlaw::MISSING_CERTIFICATE:
            addFlawToCertificateField(flaw);
            break;
         case SecurityValidationModel::SecurityFlaw::MISSING_AUTHORITY:
            addFlawToCertificateField(flaw);
            break;
         case SecurityValidationModel::SecurityFlaw::__COUNT:
         default:
            qDebug() << "Invalid flaw";
      }
   }
#endif
}

#undef ACC_
