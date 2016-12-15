/***************************************************************************
 *   Copyright (C) 2015 by Savoir-Faire Linux                              *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
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
#ifndef WIZARD_H
#define WIZARD_H

#include <QtWidgets/QWidget>

#include <account.h>
#include <namedirectory.h>

class QHBoxLayout;
class QLineEdit;

class Wizard : public QWidget
{
   Q_OBJECT
public:
   explicit Wizard(QWidget* parent = nullptr);
   virtual ~Wizard();

signals:
   void accountRegistrationEnded();

protected:
   virtual void paintEvent  (QPaintEvent*  event ) override;

private:
   QWidget*     m_pCurrentPage;
   QHBoxLayout* m_pLayout     ;
   QLineEdit*   m_pDisplayNameEdit;
   Account*     m_pAccount    {nullptr};

   bool validUsername;
   bool validPassword;
   bool validConfirmPassword;

   void validateAccountForm();

   //Event filter
   bool eventFilter( QObject *obj, QEvent *event) override;

private slots:
   void showAccountCreatorWidget();
   void validateUsername(const QString & text);
   void validatePassword(const QString & text);
   void handleSignUpCheckBoxStateChange(int state);
   void validatePublicUsername(const Account * account,
                               NameDirectory::LookupStatus status,
                               const QString & address,
                               const QString & name);
   void createAccount();
   void registerAccount(Account::RegistrationState state);
   void handleNameRegistrationEnd(NameDirectory::RegisterNameStatus status,
                                  const QString & name);
   void showShareWidget();
   void slotEmail();
   void slotPrint();
   void slotCopy();
   void slotConfigure();
   void slotComplete();
};

#endif
