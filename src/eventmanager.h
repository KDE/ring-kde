/***************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                         *
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
#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <macromodel.h>

//Qt
class QKeyEvent      ;
class QDragEnterEvent;
class QDropEvent     ;
class QDragMoveEvent ;
class QDragMoveEvent ;

//Ring
#include <call.h>
class View;
class CanvasObjectManager;
class MainWindowEvent;
class Account;

class EventManager : public QObject, public MacroModel::MacroListener {
   Q_OBJECT

public:
   friend class MainWindowEvent;
   explicit EventManager(View* parent = nullptr);
   virtual ~EventManager();
   /**
    *   Handles the behaviour when the user types a backspace
    *   according to the current state (window, item selected...)
    */
   void backspace();

   /**
    *   Handles the behaviour when the user types escape
    *   according to the current state (window, item selected...)
    */
   void escape();

   //Implement macro key listener
   virtual void addDTMF(const QString& sequence) override;

   /**
    * An unreliable way to track the application focus
    *
    * It is better than nothing
    */
   static bool mayHaveFocus();

protected:
   virtual bool eventFilter(QObject *obj, QEvent *event) override;

private:
   //Attributes
   View*            m_pParent        ;
   MainWindowEvent* m_pMainWindowEv  ;
   static bool      m_HasFocus       ;

   //Methods
   bool viewKeyEvent      ( QKeyEvent*       e);
   bool viewDragEnterEvent( const QDragEnterEvent* e);
   bool viewDropEvent     ( QDropEvent*      e);
   bool viewDragMoveEvent ( const QDragMoveEvent*  e);
   bool viewDragLeaveEvent( const QDragMoveEvent*  e);


public Q_SLOTS:
   /**
    *   Handles the behaviour when the user types enter
    *   according to the current state (window, item selected...)
    */
   void enter();
   /**
    *   Handles the behaviour when the user types something with
    *   the dialpad widget or his keyboard (normally it's a one char
    *   string but we use a string just in case).
    *   Adds str to the selected item if in the main window
    *   and creates a new item if no item is selected.
    *   Send DTMF if appropriate according to current item's state.
    *   Adds str to the search bar if in history or address book.
    * @param str the string sent by the user
    */
   void typeString(const QString& str);

private Q_SLOTS:
   void slotCallStateChanged(Call* call, Call::State previousState);
   void slotIncomingCall(Call* call);
   void slotregistrationChanged(Account* a,bool reg);
   void slotNetworkDown();
};

#endif //EVENTMANAGER_H
