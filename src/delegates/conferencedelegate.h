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
#ifndef CALL_TREE_ITEM_DELEGATE_H
#define CALL_TREE_ITEM_DELEGATE_H

#include <QtGui/QStyledItemDelegate>

//SFLPhone
class CategorizedTreeView;

///ConferenceDelegate: Delegates for CallTreeItem
class ConferenceDelegate : public QStyledItemDelegate
{
public:
   ConferenceDelegate(CategorizedTreeView* widget,QPalette pal);
   QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
   QRect fullCategoryRect(const QStyleOptionViewItem& option, const QModelIndex& index) const;
   virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
   void setCallDelegate(QStyledItemDelegate* delegate) {
      m_pCallDelegate = delegate;
   }
   
   static QPixmap getDragPixmap(CategorizedTreeView* parent, const QModelIndex& index);

private:
   CategorizedTreeView* m_tree      ;
   QSize          m_SH              ;
   int            m_LeftMargin      ;
   int            m_RightMargin     ;
   QPalette       m_Pal             ;
   QStyledItemDelegate* m_pCallDelegate;

   void drawCategory(const QModelIndex &index, int sortRole, const QStyleOption &option, QPainter *painter,const QPalette* pal = nullptr) const;
   int categoryHeight(const QModelIndex &index, const QStyleOption &option,const QPalette* pal = nullptr) const;
   void drawBoxBottom(const QModelIndex &index, int sortRole, const QStyleOption &option, QPainter *painter,const QPalette* pal = nullptr) const;
};

#endif
