/***************************************************************************
 *   Copyright (C) 2009-2014 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Valle <emmanuel.lepage@savoirfairelinux.com >*
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
#ifndef BOOKMARK_DOCK_H
#define BOOKMARK_DOCK_H

#include <QtGui/QDockWidget>
#include <QtGui/QSortFilterProxyModel>
#include "ui_dockbase.h"

//Qt
class QCheckBox;

//KDE
class KAction;
class BookmarkSortFilterProxyModel;

//SFLPhone
#include "klib/bookmarkmodel.h"

///BookmarkDock: Dock for managing favorite contacts
class BookmarkDock : public QDockWidget, public Ui_DockBase {
   Q_OBJECT
public:
   //Constructors
   explicit BookmarkDock(QWidget* parent = nullptr);
   virtual ~BookmarkDock();
private:
   //Attributes
   QCheckBox*              m_pMostUsedCK ;
   QMenu*                  m_pMenu       ;
   QModelIndex             m_CurrentIndex;
   BookmarkSortFilterProxyModel* m_pProxyModel;

   //Actions
   KAction* m_pCallAgain   ;
   KAction* m_pEditContact ;
   KAction* m_pCopy        ;
   KAction* m_pEmail       ;
   KAction* m_pAddPhone    ;
   KAction* m_pBookmark    ;

private Q_SLOTS:
   void reload();
   void expandTree();
   void slotDoubleClick(const QModelIndex& index);
   void slotContextMenu( const QModelIndex& index );

   //Menu
   void removeBookmark();
   void callAgain   ();
   void addPhone    ();
   void sendEmail   ();
   void copy        ();
   void editContact ();
};

class BookmarkSortFilterProxyModel : public QSortFilterProxyModel
{
   Q_OBJECT
public:
   explicit BookmarkSortFilterProxyModel(QObject* parent) : QSortFilterProxyModel(parent) {}
protected:
   virtual bool filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const
   {
      if (!source_parent.isValid() ) { //Is a category
         for (int i=0;i<BookmarkModel::instance()->rowCount(BookmarkModel::instance()->index(source_row,0,source_parent));i++) {
            if (filterAcceptsRow(i, BookmarkModel::instance()->index(source_row,0,source_parent)))
               return true;
         }
      }

      return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
   }
};

#endif
