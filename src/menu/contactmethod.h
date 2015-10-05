/***************************************************************************
 *   Copyright (C) 2013-2015 Savoir-Faire Linux                            *
 *   Copyright (C) 2015 Emmanuel Lepage Vallee                             *
 *   Author: Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
 *                                                                         *
 * This library is free software; you can redistribute it and/or           *
 * modify it under the terms of the GNU Library General Public             *
 * License version 2 as published by the Free Software Foundation.         *
 *                                                                         *
 * This library is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * Library General Public License for more details.                        *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/
#ifndef CONTACTMETHOD_MENU_H
#define CONTACTMETHOD_MENU_H

#include <QtWidgets/QMenu>

class ContactMethod;
class Call;
class CollectionInterface;

namespace Menu {

class ContactMethod : public QMenu
{
   Q_OBJECT

public:
   explicit ContactMethod(QWidget* parent = nullptr);
   virtual ~ContactMethod() {}

   void setContactMethod(::ContactMethod* p);

private:

   //Actions
   QAction * m_pBookmark   {nullptr};
   QAction * m_pCallAgain  {nullptr};
   QAction * m_pEditPerson {nullptr};
   QAction * m_pCopy       {nullptr};
   QAction * m_pEmail      {nullptr};
   QAction * m_pAddPhone   {nullptr};

   //Attributes
   ::ContactMethod* m_pCurrentCM {nullptr};

public Q_SLOTS:
private Q_SLOTS:
   void removeBookmark();
   void callAgain   ();
   void addPhone    ();
   void sendEmail   ();
   void copy        ();
   void editPerson ();

};

}

#endif