/****************************************************************************
 *   Copyright (C) 2012-2013 by Savoir-Faire Linux                          *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Lesser General Public             *
 *   License as published by the Free Software Foundation; either           *
 *   version 2.1 of the License, or (at your option) any later version.     *
 *                                                                          *
 *   This library is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#ifndef SORTED_TREE_DELEGATE_H
#define SORTED_TREE_DELEGATE_H

#include <QtGui/QStyledItemDelegate>

class QTreeView;

///SortedTreeDelegate: A categorized tree delegate
class SortedTreeDelegate : public QStyledItemDelegate
{
public:
   //Constructor
   explicit SortedTreeDelegate(QTreeView* widget);
   virtual ~SortedTreeDelegate();

   //Reimplemented methods
   virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
   virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

   //Setters
   void setChildDelegate(QStyledItemDelegate* childDelegate);
   void setChildChildDelegate(QStyledItemDelegate* childDelegate);

private:
   //Helpers
   QRect fullCategoryRect(const QStyleOptionViewItem& option, const QModelIndex& index) const;
   void drawCategory(const QModelIndex &index, int sortRole, const QStyleOption &option, QPainter *painter) const;
   int categoryHeight(const QModelIndex &index, const QStyleOption &option) const;

   //Attributes
   QTreeView* m_tree       ;
   QSize      m_SH         ;
   const int  m_LeftMargin ;
   const int  m_RightMargin;
   QStyledItemDelegate* m_pChildDelegate     ;
   QStyledItemDelegate* m_pChildChildDelegate;
};

#endif
