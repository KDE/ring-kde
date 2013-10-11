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
#include "lib/accountlistmodel.h"
#include "lib/keyexchangemodel.h"
#include "lib/tlsmethodmodel.h"
#include "lib/categorizedaccountmodel.h"
#include "../delegates/categorizeddelegate.h"

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
 : QWidget(parent),m_IsLoading(false)
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

   m_pInfoIconL->setPixmap(KIcon("dialog-information").pixmap(QSize(32,32)));
   label_message_icon->setPixmap(KIcon("dialog-information").pixmap(QSize(24,24)));

   //Add an info tip in the account list
   m_pTipManager = new TipManager(treeView_accountList);
   m_pTip = new Tip(i18n("To add an account, press the \"add\" button bellow. Use the\
   ⬆ up and ⬇ down button to change the default account priority."),this);
   m_pTipManager->setCurrentTip(m_pTip);

   m_pRingTonePath->setMode(KFile::File | KFile::ExistingOnly);
   m_pRingTonePath->lineEdit()->setObjectName("m_pRingTonePath");
   m_pRingTonePath->lineEdit()->setReadOnly(true);

   loadAccountList();
   accountListHasChanged = false;

   combo_security_STRP->setModel(KeyExchangeModel::instance());
   combo_tls_method->setModel(TlsMethodModel::instance());

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
   /**/connect(m_pDTMFOverRTP,                    SIGNAL(clicked(bool))                  , this   , SLOT(changedAccountList())              );
   /**/connect(m_pDTMFOverSIP,                    SIGNAL(clicked(bool))                  , this   , SLOT(changedAccountList())              );
   /**/connect(m_pAutoAnswer,                     SIGNAL(clicked(bool))                  , this   , SLOT(changedAccountList())              );
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
   /**/connect(radioButton_pa_same_as_local,      SIGNAL(clicked(bool))                  , this   , SLOT(changedAccountList())              );
   /**/connect(radioButton_pa_custom,             SIGNAL(clicked(bool))                  , this   , SLOT(changedAccountList())              );
   /**/connect(m_pRingtoneListLW,                 SIGNAL(currentRowChanged(int))         , this   , SLOT(changedAccountList())              );
   /**/connect(m_pUseCustomFileCK,                SIGNAL(clicked(bool))                  , this   , SLOT(changedAccountList())              );
   /**/connect(edit_credential_realm,             SIGNAL(textEdited(QString))            , this   , SLOT(changedAccountList())              );
   /**/connect(edit_credential_auth,              SIGNAL(textEdited(QString))            , this   , SLOT(changedAccountList())              );
   /**/connect(edit_credential_password,          SIGNAL(textEdited(QString))            , this   , SLOT(changedAccountList())              );
//    /**/connect(m_pCodecsLW,                       SIGNAL(currentTextChanged(QString))    , this   , SLOT(loadVidCodecDetails(QString))      );
   ///**/connect(&configurationManager,             SIGNAL(accountsChanged())            , this   , SLOT(updateAccountStates()             ));
   /**/connect(edit_tls_private_key_password,     SIGNAL(textEdited(QString))            , this   , SLOT(changedAccountList())              );
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
   /**/connect(AccountListModel::instance(),        SIGNAL(accountEnabledChanged(Account*)), this   , SLOT(otherAccountChanged())             );
   /**/connect(AccountListModel::instance(),        SIGNAL(accountStateChanged(Account*,QString)), this   , SLOT(updateStatusLabel(Account*)) );
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
void DlgAccounts::saveAccount(QModelIndex item)
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
   if (account->state() != Account::AccountEditState::NEW and account->state() != Account::AccountEditState::MODIFIED) {
      kDebug() << "Nothing to be saved";
      return;
   }
   m_IsLoading++;

   if (!m_pProxyCK->isChecked()) {
      m_pProxyLE->setText("");
   }

   //ACCOUNT DETAILS
   //                                                                     WIDGET VALUE                                     /
   /**/account->setProtocol                    ( static_cast<Account::Protocol>(edit2_protocol->currentIndex())           );
   /**/account->setAlias                       ( edit1_alias->text()                                                      );
   /**/account->setHostname                    ( edit3_server->text()                                                     );
   /**/account->setUsername                    ( edit4_user->text()                                                       );
   /**/account->setPassword                    ( edit5_password->text()                                                   );
   /**/account->setMailbox                     ( edit6_mailbox->text()                                                    );
   /**/account->setProxy                       ( m_pProxyLE->text()                                                       );
   /**/account->setEnabled                     ( item.data(Qt::CheckStateRole).toBool()                                   );
   /**/account->setRegistrationExpire          ( spinbox_regExpire->value()                                               );
   /**/                                                                                                                 /**/
   /*                                            Security                                                                 */
   /**/account->setTlsPassword                 ( edit_tls_private_key_password->text()                                    );
   /**/account->setTlsListenerPort             ( spinbox_tls_listener->value()                                            );
   /**/account->setTlsCaListFile               ( file_tls_authority->text()                                               );
   /**/account->setTlsCertificateFile          ( file_tls_endpoint->text()                                                );
   /**/account->setTlsPrivateKeyFile           ( file_tls_private_key->text()                                             );
   /**/account->setTlsMethod                   ( static_cast<TlsMethodModel::Type>(combo_tls_method->currentIndex())      );
   /**/account->setTlsCiphers                  ( edit_tls_cipher->text()                                                  );
   /**/account->setTlsServerName               ( edit_tls_outgoing->text()                                                );
   /**/account->setTlsNegotiationTimeoutSec    ( spinbox_tls_timeout_sec->value()                                         );
   /**/account->setTlsNegotiationTimeoutMsec   ( spinbox_tls_timeout_msec->value()                                        );
   /**/account->setKeyExchange                 ( static_cast<KeyExchangeModel::Type>(combo_security_STRP->currentIndex()) );
   /**/account->setTlsVerifyServer             ( check_tls_incoming->isChecked()                                          );
   /**/account->setTlsVerifyClient             ( check_tls_answer->isChecked()                                            );
   /**/account->setTlsRequireClientCertificate ( check_tls_requier_cert->isChecked()                                      );
   /**/account->setTlsEnable                   ( group_security_tls->isChecked()                                          );
   /**/account->setDisplaySasOnce              ( checkbox_ZRTP_Ask_user->isChecked()                                      );
   /**/account->setSrtpRtpFallback             ( checkbox_SDES_fallback_rtp->isChecked()                                  );
   /**/account->setZrtpDisplaySas              ( checkbox_ZRTP_display_SAS->isChecked()                                   );
   /**/account->setZrtpNotSuppWarning          ( checkbox_ZRTP_warn_supported->isChecked()                                );
   /**/account->setZrtpHelloHash               ( checkbox_ZTRP_send_hello->isChecked()                                    );
   /**/account->setSipStunEnabled              ( checkbox_stun->isChecked()                                               );
   /**/account->setPublishedSameAsLocal        ( radioButton_pa_same_as_local->isChecked()                                );
   /**/account->setSipStunServer               ( line_stun->text()                                                        );
   /**/account->setPublishedPort               ( spinBox_pa_published_port->value()                                       );
   /**/account->setPublishedAddress            ( lineEdit_pa_published_address ->text()                                   );
   /**/account->setLocalPort                   ( spinBox_pa_published_port->value()                                       );
   /**/account->setLocalInterface              ( comboBox_ni_local_address->currentText()                                 );
   /**/account->setRingtoneEnabled             ( m_pEnableRingtoneGB->isChecked()                                         );
   /**/account->setRingtonePath                ( m_pRingTonePath->url().path()                                            );
   /**/account->setDTMFType                    ( m_pDTMFOverRTP->isChecked()?DtmfType::OverRtp:DtmfType::OverSip          );
   /**/account->setAutoAnswer                  ( m_pAutoAnswer->isChecked()                                               );
   //                                                                                                                      /


   if (m_pDefaultAccount->isChecked()) {
      ConfigurationSkeleton::setDefaultAccountId(account->id());
      AccountListModel::instance()->setDefaultAccount(account);
   }

   if (m_pRingtoneListLW->selectedItems().size() == 1 && m_pRingtoneListLW->currentIndex().isValid() ) {
      QListWidgetItem* selectedRingtone = m_pRingtoneListLW->currentItem();
      RingToneListItem* ringtoneWidget = qobject_cast<RingToneListItem*>(m_pRingtoneListLW->itemWidget(selectedRingtone));
      if (ringtoneWidget) {
         account->setRingtonePath(ringtoneWidget->m_Path);
      }
   }

   if (m_pCodecsLW->currentIndex().isValid())
      m_pCodecsLW->model()->setData(m_pCodecsLW->currentIndex(),m_pBitrateSB->value(),VideoCodecModel::BITRATE_ROLE);
   saveCredential();
   m_IsLoading--;
} //saveAccount

void DlgAccounts::cancel()
{
//    Account* account = AccountListModel::instance()->getAccountByModelIndex(treeView_accountList->currentIndex());
//    if (account) {
//       account->performAction(CANCEL);
//    }
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

   edit1_alias->setText( account->alias());

   const int protocolIndex = static_cast<int>(account->protocol());

   if (account->protocol() == Account::Protocol::SIP) {
      const QModelIndex idx = account->credentialsModel()->index(0,0);
      disconnect(edit5_password, SIGNAL(textEdited(QString)), this , SLOT(main_password_field_changed()));
      if (idx.isValid() && !account->id().isEmpty()) {
         edit5_password->setText(account->credentialsModel()->data(idx,CredentialModel::Role::PASSWORD).toString());
      }
      else
         edit5_password->setText("");
      connect(edit5_password, SIGNAL(textEdited(QString)), this , SLOT(main_password_field_changed()));
   }
   else {
      edit5_password->setText(account->password());
   }

   disconnect(this,SLOT(aliasChanged(QString)));
   connect(account,SIGNAL(aliasChanged(QString)),this,SLOT(aliasChanged(QString)));

   switch (account->keyExchange()) {
      case KeyExchangeModel::Type::NONE:
         checkbox_SDES_fallback_rtp->setVisible   ( false );
         checkbox_ZRTP_Ask_user->setVisible       ( false );
         checkbox_ZRTP_display_SAS->setVisible    ( false );
         checkbox_ZRTP_warn_supported->setVisible ( false );
         checkbox_ZTRP_send_hello->setVisible     ( false );
         break;
      case KeyExchangeModel::Type::ZRTP:
         checkbox_SDES_fallback_rtp->setVisible   ( false );
         checkbox_ZRTP_Ask_user->setVisible       ( true  );
         checkbox_ZRTP_display_SAS->setVisible    ( true  );
         checkbox_ZRTP_warn_supported->setVisible ( true  );
         checkbox_ZTRP_send_hello->setVisible     ( true  );
         break;
      case KeyExchangeModel::Type::SDES:
         checkbox_SDES_fallback_rtp->setVisible   ( true  );
         checkbox_ZRTP_Ask_user->setVisible       ( false );
         checkbox_ZRTP_display_SAS->setVisible    ( false );
         checkbox_ZRTP_warn_supported->setVisible ( false );
         checkbox_ZTRP_send_hello->setVisible     ( false );
         break;
   }

   //         WIDGET VALUE                                          VALUE                     /
   /**/edit2_protocol->setCurrentIndex          ( (protocolIndex < 0) ? 0 : protocolIndex    );
   /**/edit3_server->setText                    (  account->hostname                       ());
   /**/edit4_user->setText                      (  account->username                       ());
   /**/edit6_mailbox->setText                   (  account->mailbox                        ());
   /**/m_pProxyLE->setText                      (  account->proxy                          ());
   /**/checkbox_ZRTP_Ask_user->setChecked       (  account->isDisplaySasOnce               ());
   /**/checkbox_SDES_fallback_rtp->setChecked   (  account->isSrtpRtpFallback              ());
   /**/checkbox_ZRTP_display_SAS->setChecked    (  account->isZrtpDisplaySas               ());
   /**/checkbox_ZRTP_warn_supported->setChecked (  account->isZrtpNotSuppWarning           ());
   /**/checkbox_ZTRP_send_hello->setChecked     (  account->isZrtpHelloHash                ());
   /**/checkbox_stun->setChecked                (  account->isSipStunEnabled               ());
   /**/line_stun->setText                       (  account->sipStunServer                  ());
   /**/spinbox_regExpire->setValue              (  account->registrationExpire             ());
   /**/radioButton_pa_same_as_local->setChecked (  account->isPublishedSameAsLocal         ());
   /**/radioButton_pa_custom->setChecked        ( !account->isPublishedSameAsLocal         ());
   /**/lineEdit_pa_published_address->setText   (  account->publishedAddress               ());
   /**/spinBox_pa_published_port->setValue      (  account->publishedPort                  ());
   /*                                                  Security                             **/
   /**/edit_tls_private_key_password->setText   (  account->tlsPassword                    ());
   /**/spinbox_tls_listener->setValue           (  account->tlsListenerPort                ());
   /**/file_tls_authority->setText              (  account->tlsCaListFile                  ());
   /**/file_tls_endpoint->setText               (  account->tlsCertificateFile             ());
   /**/file_tls_private_key->setText            (  account->tlsPrivateKeyFile              ());
   /**/edit_tls_cipher->setText                 (  account->tlsCiphers                     ());
   /**/edit_tls_outgoing->setText               (  account->tlsServerName                  ());
   /**/spinbox_tls_timeout_sec->setValue        (  account->tlsNegotiationTimeoutSec       ());
   /**/spinbox_tls_timeout_msec->setValue       (  account->tlsNegotiationTimeoutMsec      ());
   /**/check_tls_incoming->setChecked           (  account->isTlsVerifyServer              ());
   /**/check_tls_answer->setChecked             (  account->isTlsVerifyClient              ());
   /**/check_tls_requier_cert->setChecked       (  account->isTlsRequireClientCertificate  ());
   /**/group_security_tls->setChecked           (  account->isTlsEnable                    ());
   /**/m_pAutoAnswer->setChecked                (  account->isAutoAnswer                   ());
   /**/combo_security_STRP->setCurrentIndex     (  KeyExchangeModel::instance()->toIndex(account->keyExchange()).row());
   /*                                                                                        */

   m_pDTMFOverRTP->setChecked(account->DTMFType()==DtmfType::OverRtp);
   m_pDTMFOverSIP->setChecked(account->DTMFType()==DtmfType::OverSip);

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
   list_credential->setModel(account->credentialsModel());
   connect(list_credential->selectionModel()   ,SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(selectCredential(QModelIndex,QModelIndex))     );

   disconnect(list_audiocodec->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(selectedCodecChanged(QModelIndex,QModelIndex)) );
   disconnect(list_audiocodec->model()         ,SIGNAL(dataChanged(QModelIndex,QModelIndex)),    this, SLOT(changedAccountList())                          );
   list_audiocodec->setModel(account->audioCodecModel());
   connect(list_audiocodec->model()            ,SIGNAL(dataChanged(QModelIndex,QModelIndex)),    this, SLOT(changedAccountList())                          );
   connect(list_audiocodec->selectionModel()   ,SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(selectedCodecChanged(QModelIndex,QModelIndex)) );

   #ifdef ENABLE_VIDEO
   disconnect(m_pCodecsLW->selectionModel()    ,SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(loadVidCodecDetails(QModelIndex,QModelIndex))  );
   disconnect(m_pCodecsLW->model()             ,SIGNAL(dataChanged(QModelIndex,QModelIndex)),    this, SLOT(changedAccountList())                          );
   account->videoCodecModel()->reload();
   m_pCodecsLW->setModel(account->videoCodecModel());
   connect(m_pCodecsLW->model()                ,SIGNAL(dataChanged(QModelIndex,QModelIndex)),    this, SLOT(changedAccountList())                          );
   connect(m_pCodecsLW->selectionModel()       ,SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(loadVidCodecDetails(QModelIndex,QModelIndex))  );
   #endif


   if (account->alias() == "IP2IP") {
      frame2_editAccounts->setTabEnabled( 0, false );
      frame2_editAccounts->setTabEnabled( 1, false );
      frame2_editAccounts->setTabEnabled( 2, true  );
      frame2_editAccounts->setTabEnabled( 3, false );
      frame2_editAccounts->setTabEnabled( 4, false );
      frame2_editAccounts->setTabEnabled( 5, true  );
   }
   else {
      frame2_editAccounts->setTabEnabled( 0, true );
      frame2_editAccounts->setTabEnabled( 1, true );
      frame2_editAccounts->setTabEnabled( 3, true );
      frame2_editAccounts->setTabEnabled( 4, true );
      frame2_editAccounts->setCurrentIndex(0);
   }

   m_pEnableRingtoneGB->setChecked(account->isRingtoneEnabled());
   const QString ringtonePath = KStandardDirs::realFilePath(account->ringtonePath());
   m_pRingTonePath->setUrl( ringtonePath );

   combo_tls_method->setCurrentIndex( TlsMethodModel::instance()->toIndex(account->tlsMethod()).row() );
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();

   m_pRingtoneListLW->clear();
   m_hRingtonePath = configurationManager.getRingtoneList();
   QMutableMapIterator<QString, QString> iter(m_hRingtonePath);
   bool found = false;
   while (iter.hasNext()) {
      iter.next();
      QListWidgetItem*  item        = new QListWidgetItem();
      RingToneListItem* item_widget = new RingToneListItem(iter.key(),iter.value());
      m_pRingtoneListLW->addItem      ( item              );
      m_pRingtoneListLW->setItemWidget( item, item_widget );

      if (KStandardDirs::realFilePath(iter.key()) == ringtonePath) {
         m_pUseCustomFileCK->setChecked( false );
         m_pRingTonePath->setDisabled  ( true  );
         item->setSelected             ( true  );
         found = true;
      }
   }
   m_pRingtoneListLW->setEnabled(!m_pUseCustomFileCK->isChecked());
   if (!found && !ringtonePath.isEmpty()) {
      m_pRingtoneListLW->setDisabled(true);
      m_pUseCustomFileCK->setChecked(true);
      m_pRingTonePath->setEnabled(true);
      
      m_pRingTonePath->setUrl( ringtonePath );
   }

   #ifndef ENABLE_VIDEO
   m_pVideoCodecGB->setVisible(false);
   #endif

   comboBox_ni_local_address->clear();
   QStringList interfaceList = configurationManager.getAllIpInterfaceByName();
   comboBox_ni_local_address->addItems(interfaceList);

   spinBox_ni_local_port->setValue(account->localPort());
   if (comboBox_ni_local_address->findText(account->localInterface()) >=0)
      comboBox_ni_local_address->setCurrentIndex(comboBox_ni_local_address->findText(account->localInterface()));
   else //Just to be sure
      comboBox_ni_local_address->setCurrentIndex(0);

   if(protocolIndex == 0 || account->isNew()) { // if sip selected
      checkbox_stun->setChecked(account->isSipStunEnabled());
      line_stun->setText( account->sipStunServer() );
      //checkbox_zrtp->setChecked(account->accountDetail(ACCOUNT_SRTP_ENABLED) == REGISTRATION_ENABLED_TRUE);

      tab_advanced->                setEnabled ( true                       );
      line_stun->                   setEnabled ( checkbox_stun->isChecked() );
      radioButton_pa_same_as_local->setDisabled( checkbox_stun->isChecked() );
      radioButton_pa_custom->       setDisabled( checkbox_stun->isChecked() );
   }
   else {
      checkbox_stun->setChecked(false);
      tab_advanced->setEnabled (false);
      line_stun->setText( account->sipStunServer() );
      //checkbox_zrtp->setChecked(false);
   }

   updateStatusLabel(account);
   enablePublished();
   frame2_editAccounts->setEnabled(true);
   m_IsLoading--;
   account->performAction(Account::AccountEditAction::EDIT);
   emit updateButtons();
} //loadAccount

///Load an account
void DlgAccounts::loadAccountList()
{
   AccountListModel::instance()->updateAccounts();
   if (treeView_accountList->model()->rowCount() > 0 && !treeView_accountList->currentIndex().isValid()) {
      if (CategorizedAccountModel::instance()->index(0,0,CategorizedAccountModel::instance()->index(0,0)).isValid())
         treeView_accountList->setCurrentIndex(CategorizedAccountModel::instance()->index(0,0,CategorizedAccountModel::instance()->index(0,0)));
      else //IP2IP
         treeView_accountList->setCurrentIndex(CategorizedAccountModel::instance()->index(0,0,CategorizedAccountModel::instance()->index(1,0)));
      loadAccount(treeView_accountList->currentIndex());
   }
   else
      frame2_editAccounts->setEnabled(treeView_accountList->currentIndex().isValid());
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
void DlgAccounts::accountListChanged(QModelIndex current, QModelIndex previous)
{
   saveAccount(previous);
   const QModelIndex srcPrevious = CategorizedAccountModel::instance()->mapToSource(previous);
   if (srcPrevious.isValid()) {
      Account* acc = AccountListModel::instance()->getAccountByModelIndex(srcPrevious);
      if (acc && (acc->state() == Account::AccountEditState::EDITING || acc->state() == Account::AccountEditState::OUTDATED))
         acc->performAction(Account::AccountEditAction::CANCEL);
   }
   loadAccount(current);
   //updateAccountListCommands();
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
   const QString newAlias = i18n("New account%1",AccountListModel::getSimilarAliasIndex("New account"));
   AccountListModel::instance()->addAccount(newAlias);
   int r = treeView_accountList->model()->rowCount(treeView_accountList->model()->index(0,0)) - 1;
   const QModelIndex index = treeView_accountList->model()->index(r,0,treeView_accountList->model()->index(0,0));
   treeView_accountList->setCurrentIndex(index);

   frame2_editAccounts->setEnabled(true);
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
   bool buttonsEnabled[4] = {true,true,true,true};
   if(! treeView_accountList->currentIndex().isValid() || !treeView_accountList->currentIndex().parent().isValid()) {
      buttonsEnabled[0]   = false;
      buttonsEnabled[1]   = false;
      buttonsEnabled[3]   = false;
   }
   else if(treeView_accountList->currentIndex().row() == 0) {
      buttonsEnabled[0]   = false;
   }
   const QModelIndex cur = treeView_accountList->currentIndex();
   if(!CategorizedAccountModel::instance()->index(cur.row()+1,0,cur.parent()).isValid()) {
      buttonsEnabled[1]   = false;
   }

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
void DlgAccounts::updateAccountStates()
{
   kDebug() << "updateAccountStates";
   for (int i = 0; i < AccountListModel::instance()->size(); i++) {
      Account* current = AccountListModel::instance()->getAccountAt(i);
      current->updateState();
   }
   updateStatusLabel(treeView_accountList->currentIndex());
}

///Update the status label to current account state
void DlgAccounts::updateStatusLabel(QModelIndex item)
{
   kDebug() << "MODEL index is" << item.row();
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
   const QString status = account->registrationStatus();
   edit7_state->setText( "<FONT COLOR=\"" + account->stateColorName() + "\">" + status + "</FONT>" );
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
   bool isIncomplete = acc && (acc->alias() != "IP2IP") && (fields[0]|fields[1]|fields[2]|fields[3]);
   //Add visual feedback for missing fields
   for (int i=0;i<requiredFieldsLabels.size();i++) {
      requiredFieldsLabels[i]->setStyleSheet(fields[i]?"color:darkred;":QString());
   }
   return isIncomplete;
}

///Save settings
void DlgAccounts::updateSettings()
{
   if(accountListHasChanged) {
      if(treeView_accountList->currentIndex().isValid()) {
         Account* acc = currentAccount();
//          if (acc && acc->isNew()) { //Trying to save an account without ID work, but codecs and credential will be corrupted
//             acc->performAction(AccountEditAction::SAVE);
//          }
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
   //toolButton_accountsApply->setEnabled(false);
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
   switch (static_cast<KeyExchangeModel::Type>(combo_security_STRP->currentIndex())) {
      case KeyExchangeModel::Type::NONE:
         checkbox_SDES_fallback_rtp->setVisible   ( false );
         checkbox_ZRTP_Ask_user->setVisible       ( false );
         checkbox_ZRTP_display_SAS->setVisible    ( false );
         checkbox_ZRTP_warn_supported->setVisible ( false );
         checkbox_ZTRP_send_hello->setVisible     ( false );
         break;
      case KeyExchangeModel::Type::ZRTP:
         checkbox_SDES_fallback_rtp->setVisible   ( false );
         checkbox_ZRTP_Ask_user->setVisible       ( true  );
         checkbox_ZRTP_display_SAS->setVisible    ( true  );
         checkbox_ZRTP_warn_supported->setVisible ( true  );
         checkbox_ZTRP_send_hello->setVisible     ( true  );
         break;
      case KeyExchangeModel::Type::SDES:
         checkbox_SDES_fallback_rtp->setVisible   ( true  );
         checkbox_ZRTP_Ask_user->setVisible       ( false );
         checkbox_ZRTP_display_SAS->setVisible    ( false );
         checkbox_ZRTP_warn_supported->setVisible ( false );
         checkbox_ZTRP_send_hello->setVisible     ( false );
         break;
   }
} //updateCombo

///Save the current credential
void DlgAccounts::saveCredential()
{
   Account* acc = currentAccount();
   const QModelIndex currentCredential = list_credential->currentIndex();
   if (currentCredential.isValid()) {
      acc->credentialsModel()->setData(currentCredential,edit_credential_auth->text()    , CredentialModel::Role::NAME     );
      acc->credentialsModel()->setData(currentCredential,edit_credential_password->text(), CredentialModel::Role::PASSWORD );
      acc->credentialsModel()->setData(currentCredential,edit_credential_realm->text()   , CredentialModel::Role::REALM    );
   }

   if (acc)
      acc->saveCredentials();
} //saveCredential

///Add a new credential
void DlgAccounts::addCredential()
{
   Account* acc = currentAccount();

   QModelIndex idx = acc->credentialsModel()->addCredentials();
   list_credential->setCurrentIndex(idx);
} //addCredential

///Save and load a credential
void DlgAccounts::selectCredential(QModelIndex item, QModelIndex previous)
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
//       acc->setAccountAlias(newAlias);
}

Account* DlgAccounts::currentAccount() const
{
   return AccountListModel::instance()->getAccountByModelIndex(CategorizedAccountModel::instance()->mapToSource(treeView_accountList->currentIndex()));
}

//#include <dlgaccount.moc>
