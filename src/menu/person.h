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
#ifndef PERSON_MENU_H
#define PERSON_MENU_H

#include <QtWidgets/QMenu>

class ContactMethod;
class Person;

namespace Menu {

class Person : public QMenu
{
   Q_OBJECT

public:
   explicit Person(QWidget* parent = nullptr);

   void setPerson(::Person* p);

   //Helper
   ::ContactMethod* showNumberSelector(bool& ok);
   void callAgain(const ::ContactMethod* n = nullptr);

private:

   //Actions
   QAction * m_pCallAgain   {nullptr};
   QAction * m_pEditPerson  {nullptr};
   QAction * m_pCopy        {nullptr};
   QAction * m_pEmail       {nullptr};
   QAction * m_pAddPhone    {nullptr};
   QAction * m_pBookmark    {nullptr};
   QAction * m_pRemove      {nullptr};

   //Attributes
   ::Person* m_pCurrentPerson {nullptr};


private Q_SLOTS:
   ///Menu actions
   void sendEmail   ();
   void copy        ();
   void editPerson  ();
   void addPhone    ();
   void bookmark    ();
   void slotDelete  ();
};

}

#endif