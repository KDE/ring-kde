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
#ifndef CALL_MENU_H
#define CALL_MENU_H

#include <QtWidgets/QMenu>

class ContactMethod;
class Call;
class CollectionInterface;

namespace Menu {

class Call : public QMenu
{
   Q_OBJECT

public:
   explicit Call(QWidget* parent = nullptr);

   void setCall(::Call* p);

private:

   //Actions
   QAction* m_pCallAgain   {nullptr};
   QAction* m_pAddPerson   {nullptr};
   QAction* m_pAddToPerson {nullptr};
   QAction* m_pCopy        {nullptr};
   QAction* m_pEmail       {nullptr};
   QAction* m_pRemove      {nullptr};
   QAction* m_pBookmark    {nullptr};

   //Attributes
   ::Call* m_pCurrentCall {nullptr};

public Q_SLOTS:
   void slotCallAgain ();

private Q_SLOTS:
   ///Menu actions
   void slotSendEmail ();
   void slotRemove    ();
   void slotCopy      ();
   void slotAddPerson (CollectionInterface* col);
   void slotBookmark  ();
};

}

#endif