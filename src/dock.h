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

#include <QtCore/QObject>

class QMainWindow;

class DockBase;
class CategorizedDelegate;

/**
 * This single instance class manage the Docks
 */
class Dock : public QObject
{
   Q_OBJECT
public:
   explicit Dock(QMainWindow* w);
   virtual ~Dock();

   DockBase*  contactDock ();
   DockBase*  historyDock ();
   DockBase*  bookmarkDock();

private:
   DockBase*   m_pContactCD  {nullptr};
   DockBase*   m_pHistoryDW  {nullptr};
   DockBase*   m_pBookmarkDW {nullptr};

   CategorizedDelegate* m_pCategoryDelegate {nullptr};
   CategorizedDelegate* m_pHCategoryDelegate {nullptr};

public Q_SLOTS:
   void updateTabIcons();
   void focusHistory ();
   void focusContact ();
   void focusCall    ();
   void focusBookmark();
};

#endif
