/***************************************************************************
 *   Copyright (C) 2014-2015 by Savoir-Faire Linux                         *
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
#ifndef CREDENTIALS_PAGE_H
#define CREDENTIALS_PAGE_H

#include <QWidget>
#include "pagebase.h"
#include "ui_credentials.h"

class CategorizedDelegate;
class QStyledItemDelegate;

namespace Pages {

class Credentials : public PageBase, public Ui_CredentialsPage
{
Q_OBJECT
public:
   //Constructor
   explicit Credentials(QWidget *parent = nullptr);
   virtual ~Credentials();

private:
   CategorizedDelegate* m_pDelegate;
   QStyledItemDelegate* m_pChildDelegate;

private Q_SLOTS:
   void loadInfo();
   void slotSetAccount();
   void slotAddCredential();
   void slotRemoveCredential();
   void slotRealmChanged(const QString& text);
   void slotUserChanged(const QString& text);
   void slotPasswdChanged(const QString& text);
};

}

#endif
