/***************************************************************************
 *   Copyright (C) 2009-2014 by Savoir-Faire Linux                         *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>         *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/

#ifndef ACCOUNTWIZARD_H
#define ACCOUNTWIZARD_H

#include <QWizard>

//Qt
class QLabel;
class QRadioButton;
class KLineEdit;
class QCheckBox;

typedef struct {
   bool    success ;
   QString reason  ;
   QString user    ;
   QString passwd  ;
} rest_account;

/**
   @author Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>
*/
class AccountWizard : public QWizard
{
Q_OBJECT

public:
   enum { Page_Intro, Page_Type, Page_SIPForm, Page_IAXForm, Page_Stun, Page_Conclusion };
   explicit AccountWizard(QWidget * parent = nullptr);
   ~AccountWizard();
   void accept();
private:
   //Helpers
   rest_account get_rest_account(const QString& host, const QString& email);
   int sendRequest(const QString& host, int port, const QString& req, QString& ret);
};

/***************************************************************************
 *   Class WizardIntroPage                                                 *
 *   Widget of the introduction page of the wizard                         *
 ***************************************************************************/

class WizardIntroPage : public QWizardPage
{
   Q_OBJECT

public:
   explicit WizardIntroPage(QWidget *parent = nullptr);
   ~WizardIntroPage();
   int nextId() const;

private:
   QLabel * introLabel;
   QCheckBox * doNotshowAgain;
};

/***************************************************************************
 *   Class WizardAccountTypePage                                           *
 *   Page in which user choses between SIP and IAX account.                *
 ***************************************************************************/

class WizardAccountTypePage : public QWizardPage
{
   Q_OBJECT

public:
   explicit WizardAccountTypePage(QWidget *parent = nullptr);
   ~WizardAccountTypePage();
   int nextId() const;

private:
   QRadioButton* radioButton_SIP;
   QRadioButton* radioButton_IAX;
};

/***************************************************************************
 *   Class WizardAccountSIPFormPage                                        *
 *   Page of account settings.                                             *
 ***************************************************************************/

class WizardAccountSIPFormPage : public QWizardPage
{
   Q_OBJECT

public:

   explicit WizardAccountSIPFormPage(QWidget *parent = nullptr);
   ~WizardAccountSIPFormPage();
   int nextId() const;

private:
   QLabel* label_alias           ;
   QLabel* label_server          ;
   QLabel* label_user            ;
   QLabel* label_password        ;
   QLabel* label_voicemail       ;
   QLabel* label_enableZrtp      ;

   KLineEdit* lineEdit_alias     ;
   KLineEdit* lineEdit_server    ;
   KLineEdit* lineEdit_user      ;
   KLineEdit* lineEdit_password  ;
   KLineEdit* lineEdit_voicemail ;
   QCheckBox* checkBox_enableZrtp;
};

/***************************************************************************
 *   Class WizardAccountIAXFormPage                                        *
 *   Page of account settings.                                             *
 ***************************************************************************/

class WizardAccountIAXFormPage : public QWizardPage
{
   Q_OBJECT

public:

   explicit WizardAccountIAXFormPage(QWidget *parent = nullptr);
   ~WizardAccountIAXFormPage();
   int nextId() const;

private:
   QLabel* label_alias          ;
   QLabel* label_server         ;
   QLabel* label_user           ;
   QLabel* label_password       ;
   QLabel* label_voicemail      ;

   KLineEdit* lineEdit_alias    ;
   KLineEdit* lineEdit_server   ;
   KLineEdit* lineEdit_user     ;
   KLineEdit* lineEdit_password ;
   KLineEdit* lineEdit_voicemail;
};

/***************************************************************************
 *   Class WizardAccountStunPage                                           *
 *   Page of Stun settings.                                                *
 ***************************************************************************/

class WizardAccountStunPage : public QWizardPage
{
   Q_OBJECT

public:
   explicit WizardAccountStunPage(QWidget *parent = nullptr);
   ~WizardAccountStunPage();
   int nextId() const;

private:
   QCheckBox* checkBox_enableStun;
   QLabel*    label_StunServer   ;
   KLineEdit* lineEdit_StunServer;
};

/***************************************************************************
 *   Class WizardAccountConclusionPage                                     *
 *   Conclusion page.                                                      *
 ***************************************************************************/

class WizardAccountConclusionPage : public QWizardPage
{
   Q_OBJECT

public:
   explicit WizardAccountConclusionPage(QWidget *parent = nullptr);
   ~WizardAccountConclusionPage();
   int nextId() const;

private:
};

#endif
