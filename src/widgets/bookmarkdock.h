/***************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                         *
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
#include "ui_dockbase.h"

//Qt
class QCheckBox;

//SFLPhone
#include "klib/bookmarkmodel.h"

///BookmarkDock: Dock for managing favorite contacts
class BookmarkDock : public QDockWidget, public Ui_DockBase {
   Q_OBJECT
public:
   //Constructors
   explicit BookmarkDock(QWidget* parent = nullptr);
   virtual ~BookmarkDock();

   //Mutators
   void addBookmark(const QString& phone);
   void removeBookmark(const QString& phone);
private:
   //Attributes
   QCheckBox*              m_pMostUsedCK;

private Q_SLOTS:
   void filter(QString text);
   void reload();
   void expandTree();
};

class BookmarkSortFilterProxyModel : public QSortFilterProxyModel
{
   Q_OBJECT
public:
   BookmarkSortFilterProxyModel(QObject* parent) : QSortFilterProxyModel(parent) {}
protected:
   virtual bool filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const
   {
      if (!source_parent.isValid() ) { //Is a category
         for (int i=0;i<BookmarkModel::getInstance()->rowCount(BookmarkModel::getInstance()->index(source_row,0,source_parent));i++) {
            if (filterAcceptsRow(i, BookmarkModel::getInstance()->index(source_row,0,source_parent)))
               return true;
         }
      }

      return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
   }
};

#endif
