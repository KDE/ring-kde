/***************************************************************************
 *   Copyright (C) 2014-2015 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
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
#ifndef DLGACCOUNT_H
#define DLGACCOUNT_H

#include "ui_accountdlg.h"
#include "typedefs.h"

class Account;
class ExtendedProtocolModel;

namespace Pages {
    class Account;
}

class LIB_EXPORT DlgAccount : public QWidget, public Ui::AccountDlg
{
   Q_OBJECT
public:
   explicit DlgAccount(QWidget* parent);
   virtual ~DlgAccount();

   //Getters
   bool hasChanged();

private:
   Pages::Account* m_pCurrentAccount;
   ExtendedProtocolModel* m_pProtocolModel;
   bool m_HasChanged;

public Q_SLOTS:
   //Housekeeping
   void updateSettings();
   void updateWidgets();
   void cancel();

   //Core logic
   void slotNewAddAccount();

private Q_SLOTS:
   void slotUpdateButtons();
   void slotRemoveAccount();
   void slotExpand();
   void slotSetAccount(const QModelIndex& idx);

Q_SIGNALS:
   void updateButtons();
};

#endif // _DLGACCOUNT_H_
