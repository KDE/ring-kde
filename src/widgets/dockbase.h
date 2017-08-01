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
#ifndef CONTACT_DOCK_H
#define CONTACT_DOCK_H

#include "ui_contactdock.h"

//Qt
#include <QtWidgets/QDockWidget>

//Ring
#include "categorizedtreeview.h"

//Qt
class QMenu;
class QSortFilterProxyModel;
class QStyledItemDelegate;
class QAbstractItemModel;
class QItemSelectionModel;

///Ring
class KeyPressEaterC;
class CategorizedTreeView;
class UserActionModel;

///DockBase: Dock to access contacts
class DockBase : public QDockWidget, public Ui_ContactDock
{
   Q_OBJECT
   friend class KeyPressEaterC;
   friend class ArrowGrabber;
public:
   //Constructor
   explicit DockBase(QWidget* parent = nullptr);
   virtual ~DockBase();

   //Mutator
   void setProxyModel(QAbstractItemModel* model, QSortFilterProxyModel* filterProxy);
   void setDelegate(QStyledItemDelegate* delegate);
   void setSortingModel(QAbstractItemModel* m, QItemSelectionModel* s);

   //Getter
   CategorizedTreeView* view() const;

private:
   //Attributes
   KeyPressEaterC*          m_pKeyPressEater ;
   UserActionModel*         m_pUserActionModel {nullptr};

   //Helper
   void initUAM();

private Q_SLOTS:
   void slotContextMenu       (const QModelIndex& index );
   void slotContextMenuClicked(const QModelIndex& index );

private Q_SLOTS:
   void transferEvent( QMimeData* data   );
   void expandTree  ();
   void expandTreeRows(const QModelIndex& idx);
   void slotDoubleClick(const QModelIndex& index);
};

#endif //CONTACT_DOCK_H

// kate: space-indent on; indent-width 3; replace-tabs on;
