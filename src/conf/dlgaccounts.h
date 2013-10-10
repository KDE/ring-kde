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
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#ifndef DLGACCOUNTS_H
#define DLGACCOUNTS_H

#include "ui_dlgaccountsbase.h"
#include "../lib/account.h"
#include "../lib/dbus/callmanager.h"

//Qt
class QWidget;

//KDE
class KConfigDialog;

//SFLPhone
class TipManager;
class Tip;

//Typedef
typedef QHash<QString, QString> StringHash;                          //Needed to fix a Qt foreach macro argument parsing bug

///RingToneListItem: Ringtone list widget
class RingToneListItem : public QWidget
{
   Q_OBJECT
   friend class DlgAccounts;
   ///Constructor
   RingToneListItem(QString path, QString name) : QWidget(0),m_Path(path) {
      QHBoxLayout* l  = new QHBoxLayout(this);
      m_pPlayPB       = new QPushButton(this);
      QLabel* lblName = new QLabel(name,this);
      l->setContentsMargins(0,0,0,0);
      m_pPlayPB->setIcon(KIcon("media-playback-start"));
      m_pPlayPB->setVisible(false);
      lblName->setStyleSheet("padding-left:5px;");
      lblName->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
      l->addWidget(lblName);
      l->addWidget(m_pPlayPB);
      connect(m_pPlayPB,SIGNAL(clicked()),this,SLOT(playRingtone()));
   }
protected:
   ///Show the button when the cursor is over the item
   virtual void enterEvent ( QEvent * event ) {
      Q_UNUSED(event)
      m_pPlayPB->setVisible(true);
   }
   ///Hide the button when the mouse leave the button
   virtual void leaveEvent ( QEvent * event ) {
      Q_UNUSED(event)
      m_pPlayPB->setVisible(false);
   }
private Q_SLOTS:
   ///Play the ringtone file when the button is clicked
   void playRingtone() {
      CallManagerInterface& callManager = DBus::CallManager::instance();
      Q_NOREPLY callManager.startRecordedFilePlayback(m_Path);
   }
private:
   QString m_Path;
   QPushButton* m_pPlayPB;
};

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

   void saveAccount(QModelIndex item);

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
   QList<StringHash>     codecList             ;
   bool                  accountListHasChanged ;
   QMap<QString,QString> m_hRingtonePath       ;
   int                   m_IsLoading           ;
   TipManager*           m_pTipManager         ;
   Tip*                  m_pTip                ;

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
   void updateAccountStates              ();
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
   void updateStatusLabel          ( QModelIndex item                                        );
   void updateStatusLabel          (                                                         );
   void accountListChanged         ( QModelIndex current, QModelIndex previous               );
   void selectCredential           ( QModelIndex item, QModelIndex previous                  );
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
