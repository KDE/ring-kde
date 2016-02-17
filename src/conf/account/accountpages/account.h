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
#ifndef ACCOUNT_PAGE_H
#define ACCOUNT_PAGE_H

#include <QtWidgets/QWidget>
#include "pagebase.h"
#include "ui_tabs.h"

class Account;
class Person;

namespace Pages {

class Account : public PageBase, public Ui::Tabs
{
Q_OBJECT
public:
   //Constructor
   explicit Account(QWidget *parent = nullptr);

public Q_SLOTS:
   virtual void updateWidgets() override;
   virtual void updateSettings() override;

   void selectAlias();

public Q_SLOTS:
   virtual void setAccount(::Account* a) override;
   void setProfile(Person* p);
   void setAccount(const QModelIndex& idx);

private Q_SLOTS:
   void slotUpdateButtons();
   void displayProfile(bool display);

Q_SIGNALS:
   void changed();

private:
   //Attributes
   Account* m_pAccount;
   QList<Person*> m_lToSave;
};

}

#endif
