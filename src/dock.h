/***************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                         *
 *   Copyright (C) 2015 by Emmanuel Lepage Vallee                          *
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
#ifndef DOCK_H
#define DOCK_H

// Qt
#include <QtCore/QObject>
#include <QtCore/QHash>
class QTabBar;
class QMainWindow;
class QToolButton;

// Ring
class PhoneWindow;
class DockBase;
class CategorizedDelegate;
class RecentDock;
class ViewContactDock;
class ContactMethod;

/**
 * This single instance class manage the Docks
 */
class Dock : public QObject
{
   Q_OBJECT
public:
   explicit Dock(PhoneWindow* w);
   virtual ~Dock();

   DockBase*   contactDock ();
   DockBase*   historyDock ();
   DockBase*   bookmarkDock();
   RecentDock* recentDock  ();

private:
   DockBase*   m_pContactCD  {nullptr};
   DockBase*   m_pHistoryDW  {nullptr};
   DockBase*   m_pBookmarkDW {nullptr};
   RecentDock* m_pRecentDock {nullptr};

   ViewContactDock* m_pViewContact {nullptr};

   CategorizedDelegate* m_pCategoryDelegate {nullptr};
   CategorizedDelegate* m_pHCategoryDelegate {nullptr};

public Q_SLOTS:
   void focusHistory ();
   void focusContact ();
   void focusCall    ();
   void focusBookmark();
   void focusRecent  ();

   void viewContact(ContactMethod* cm);
};

#endif

// kate: space-indent on; indent-width 3; replace-tabs on;
