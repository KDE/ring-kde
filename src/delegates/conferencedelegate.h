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
#ifndef CONFERENCEDELEGATE_H
#define CONFERENCEDELEGATE_H

#include <QtGui/QStyledItemDelegate>

//SFLPhone
class CategorizedTreeView;

///ConferenceDelegate: Delegates for CallTreeItem
class ConferenceDelegate : public QStyledItemDelegate
{
   Q_OBJECT
public:
   explicit ConferenceDelegate(CategorizedTreeView* widget,QPalette pal);

   //Setters
   void setCallDelegate(QStyledItemDelegate* delegate) {
      m_pCallDelegate = delegate;
   }

   static QPixmap getDragPixmap(CategorizedTreeView* parent, const QModelIndex& index);

   //Reimplemented methods
   virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
   virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
   virtual QWidget * createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
   virtual void setEditorData ( QWidget * editor, const QModelIndex & index ) const;
   virtual void setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const;

protected:
   virtual bool eventFilter(QObject *obj, QEvent *event);

private:
   CategorizedTreeView* m_tree      ;
   QSize          m_SH              ;
   int            m_LeftMargin      ;
   int            m_RightMargin     ;
   QPalette       m_Pal             ;
   QStyledItemDelegate* m_pCallDelegate;

   //Helpers
   void drawCategory(const QModelIndex &index, int sortRole, const QStyleOption &option, QPainter *painter,const QPalette* pal = nullptr) const;
   int categoryHeight(const QModelIndex &index, const QStyleOption &option,const QPalette* pal = nullptr) const;
   void drawBoxBottom(const QModelIndex &index, int sortRole, const QStyleOption &option, QPainter *painter,const QPalette* pal = nullptr) const;
   QRect fullCategoryRect(const QStyleOptionViewItem& option, const QModelIndex& index) const;

private Q_SLOTS:
   void slotTextChanged(const QString& text);
   void slotReturnPressed();
};

#endif
