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
#ifndef SORTEDTREEDELEGATE_H
#define SORTEDTREEDELEGATE_H

#include <QtGui/QStyledItemDelegate>

class QTreeView;

class SortedTreeDelegate : public QStyledItemDelegate
{
  public:
   SortedTreeDelegate(QTreeView* widget);

   QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
   QRect fullCategoryRect(const QStyleOptionViewItem& option, const QModelIndex& index) const;
   virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
   void setChildDelegate(QStyledItemDelegate* childDelegate);
   void setChildChildDelegate(QStyledItemDelegate* childDelegate);

  private:
    void drawCategory(const QModelIndex &index, int sortRole, const QStyleOption &option, QPainter *painter) const;
    int categoryHeight(const QModelIndex &index, const QStyleOption &option) const;
   QTreeView* m_tree;
   QSize m_SH;
   int m_LeftMargin;
   int m_RightMargin;
   QStyledItemDelegate* m_pChildDelegate;
   QStyledItemDelegate* m_pChildChildDelegate;
};

#endif