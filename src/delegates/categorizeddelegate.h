/****************************************************************************
 *   Copyright (C) 2012-2014 by Savoir-Faire Linux                          *
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
#ifndef CATEGORIZEDDELEGATE_H
#define CATEGORIZEDDELEGATE_H

#include <QtGui/QStyledItemDelegate>

class QTreeView;

///CategorizedDelegate: A categorized tree delegate
class CategorizedDelegate : public QStyledItemDelegate
{
public:
   //Constructor
   explicit CategorizedDelegate(QTreeView* widget);
   virtual ~CategorizedDelegate();

   //Reimplemented methods
   virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
   virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

   //Setters
   void setChildDelegate(QStyledItemDelegate* childDelegate);
   void setChildChildDelegate(QStyledItemDelegate* childDelegate);

private:
   //Helpers
   void drawSimpleCategory(const QModelIndex &index, int sortRole, const QStyleOption &option, QPainter *painter) const;

   //Attributes
   const int  m_LeftMargin ;
   QStyledItemDelegate* m_pChildDelegate     ;
   QStyledItemDelegate* m_pChildChildDelegate;
};

#endif
