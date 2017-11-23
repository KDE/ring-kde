/***************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                         *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>         *
 *            Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
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
#ifndef RING_H
#define RING_H

#include <KXmlGuiWindow>

//Qt
class QString;
class QLabel;
class QToolButton;

//KDE
class QComboBox;

//Ring
class Call;
class ContactDock;
class BookmarkDock;
class HistoryDock;
class AvailableAccountModel;
class Account;
class View;
class Dock;
class AutoComboBox;
class SysTray;

/**
 * This class represents the Ring main window
 * It implements the methods relative to windowing
 * (status, menus, toolbars, notifications...).
 * It uses a view which implements the real functionning
 * and features of the phone.
 * The display of the window is according to the state of the view,
 * so the view sends some signals to ask for changes on the window
 * that the window has to take into account.
 *
 * @short Main window
 * @author Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>
 * @author Emmanuel Lepage <elv1313@gmail.com>
 * @version 1.4.1
**/
class PhoneWindow final : public KXmlGuiWindow
{
Q_OBJECT

private:
   //Attributes
   View*          m_pView            ;
   bool           m_pIconChanged     ;
   SysTray*       m_pTrayIcon        {nullptr};
   QLabel*        m_pStatusBarWidget ;
   QDockWidget*   m_pCentralDW       ;
   AutoComboBox*  m_pAccountStatus   ;
   QToolButton*   m_pPresent         ;
   QDockWidget*   m_pPresenceDock    ;
   QLabel*        m_pCurAccL         ;
   Dock*          m_pDock            ;

   static PhoneWindow*            m_sApp;

protected:
   virtual bool  queryClose ()       override;
   virtual QSize sizeHint   () const override;

public:
   explicit PhoneWindow(QWidget* parent = nullptr);
   virtual ~PhoneWindow();
   friend class View;

   static PhoneWindow* app  ();
   View* view ();

   bool isAutoStart() const;
   void setAutoStart(bool value);

   QDockWidget* callDock() const;

private Q_SLOTS:
   void quitButton                               (                               );
   void updatePresence                           ( const QString& status         );
   void hidePresenceDock                         (                               );
   void slotPresenceEnabled                      ( bool state                    );
public Q_SLOTS:
   void displayAccountCbb                        ( bool checked = true           );
   void selectCallTab                            (                               );
};

#endif

// kate: space-indent on; indent-width 3; replace-tabs on;
