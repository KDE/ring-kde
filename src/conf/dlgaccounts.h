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
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#ifndef DLGACCOUNTS_H
#define DLGACCOUNTS_H

#include "ui_dlgaccountsbase.h"
#include "../lib/account.h"
#include "../lib/dbus/callmanager.h"
#include "../lib/ringtonemodel.h"
#include <QtGui/QStyledItemDelegate>

//Qt
class QWidget;

//KDE
class KConfigDialog;

//SFLPhone
class TipManager;
class Tip;

/**
 *  @author Jérémy Quentin <jeremy.quentin@gmail.com>
 *
 *  \note see ticket #1309 for advices about how to improve this class.
 */
class DlgAccounts : public QWidget, public Ui_DlgAccountsBase
{
Q_OBJECT
public:
   explicit DlgAccounts(KConfigDialog *parent = nullptr);
   ~DlgAccounts();

   void saveAccount(const QModelIndex& item);

   void cancel();

   /**
    *   Fills the settings form in the right side with the
    *   settings of @p item.
    *
    *   \note When the user creates a new account, its accountDetails
    *   map is empty, so the form is filled with blank strings,
    *   zeros... And when the user clicks \e Apply , these settings are
    *   saved just after the account is created. So be careful the form
    *   is filled with the right default settings if blank (as 600 for
    *   registration expire).
    *
    * @param item the item with which to fill the settings form
    *
    */
   void loadAccount(QModelIndex item);

private:
   ///Attributes
   QList< QHash<QString, QString> >  codecList ;
   bool                  accountListHasChanged ;
   QMap<QString,QString> m_hRingtonePath       ;
   int                   m_IsLoading           ;
   TipManager*           m_pTipManager         ;
   Tip*                  m_pTip                ;
   IssuesIcon*           m_pAuthorityII        ;
   IssuesIcon*           m_pEndCertII          ;
   IssuesIcon*           m_pPKII               ;

   //Methods
   Account* currentAccount() const;
   void updateSecurityValidation();
   void addFlawToCertificateField(const Flaw* flaw);

public Q_SLOTS:
   void loadAccountList                  ();
   bool hasChanged                       ();
   void updateSettings                   ();
   void updateWidgets                    ();
   bool hasIncompleteRequiredFields      ();

private Q_SLOTS:
   void changedAccountList               ();
   void otherAccountChanged              ();
   void on_button_accountUp_clicked      ();
   void on_button_accountDown_clicked    ();
   void on_button_accountAdd_clicked     ();
   void on_button_accountRemove_clicked  ();
   void addCredential                    ();
   void removeCredential                 ();
   void enablePublished                  ();
//    void updateAccountStates              ();
   void updateAccountListCommands        ();
   void main_password_field_changed      ();
   void main_credential_password_changed ();
   void updateFirstCredential(QString text);

   void moveAudioCodecUp                 ();
   void moveAudioCodecDown               ();
   void moveVideoCodecUp                 ();
   void moveVideoCodecDown               ();

   void loadVidCodecDetails        ( const QModelIndex& current, const QModelIndex& previous );
   void selectedCodecChanged       ( const QModelIndex& current, const QModelIndex& previous );
   void updateStatusLabel          ( const QModelIndex& item                                 );
   void updateStatusLabel          (                                                         );
   void accountListChanged         ( const QModelIndex& current, const QModelIndex& previous );
   void selectCredential           ( const QModelIndex& item   , const QModelIndex& previous );
   void updateStatusLabel          ( Account*   account                                      );
   void updateCombo                ( int            value                                    );
   void saveCredential             (                                                         );
   void aliasChanged               (QString newAlias                                         );
   void changeAlias                (QString newAlias                                         );

Q_SIGNALS:
   ///Update the Ok and Apply button style
   void updateButtons();
};

#endif
