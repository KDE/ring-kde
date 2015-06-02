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
#ifndef BASE_PAGE_H
#define BASE_PAGE_H

#include <QtWidgets/QWidget>
#include "ui_basic.h"

class Account;

namespace Pages {

class PageBase : public QWidget
{
Q_OBJECT
public:
   //Constructor
   explicit PageBase(QWidget *parent = nullptr);

   //Destructor
   virtual ~PageBase();

   virtual bool hasChanged();

   ::Account* account() const;
   void setAccount(::Account* a);

protected:
   bool m_Changed;

private:
   ::Account* m_pAccount;

public Q_SLOTS:
   virtual void updateWidgets();
   virtual void updateSettings();

protected Q_SLOTS:
   virtual void setChanged();

Q_SIGNALS:
   void updateButtons();
   void accountSet(::Account* a);

};

};

#endif