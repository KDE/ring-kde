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
#include "calltreeitemdelegate.h"

//Qt
#include <QtCore/QSize>
#include <QtGui/QPainter>
#include <QtGui/QPalette>
#include <QtGui/QTreeWidget>
#include <QtGui/QGraphicsEffect>
#include <QtGui/QGraphicsOpacityEffect>

//SFLPhone
#include "../callview.h"
#include "calltreeitem.h"

///Constructor
CallTreeItemDelegate::CallTreeItemDelegate(CallView* widget,QPalette pal)
      : QStyledItemDelegate(widget) , m_tree(widget) , m_ConferenceDrawer() , m_Pal(pal),
      m_LeftMargin(0),m_RightMargin(0)
{
}

///Guess the size of the item
QSize CallTreeItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const 
{
   QSize sh = QStyledItemDelegate::sizeHint(option, index);
   QTreeWidgetItem* item = (m_tree)->itemFromIndex(index);
   if (item) {
      CallTreeItem* widget = (CallTreeItem*)m_tree->itemWidget(item,0);
      if (widget)
         sh.rheight() = widget->sizeHint().height()+11; //Equal top and bottom padding

      if (index.parent().isValid() && !index.parent().child(index.row()+1,0).isValid())
         sh.rheight() += 15;
   }
   return sh;
}

///Draw the rectangle
QRect CallTreeItemDelegate::fullCategoryRect(const QStyleOptionViewItem& option, const QModelIndex& index) const 
{
   QModelIndex i(index),old(index);
   //BEGIN real sizeHint()
   //Otherwise it would be called too often (thanks to valgrind)
   ((CallTreeItemDelegate*)this)->m_SH          = QStyledItemDelegate::sizeHint(option, index);
   ((CallTreeItemDelegate*)this)->m_LeftMargin  = m_ConferenceDrawer.leftMargin();
   ((CallTreeItemDelegate*)this)->m_RightMargin = m_ConferenceDrawer.rightMargin();
   if (!index.parent().isValid() && index.child(0,0).isValid()) {
      ((QSize)m_SH).rheight() += 2 * m_ConferenceDrawer.leftMargin();
   } else {
      ((QSize)m_SH).rheight() += m_ConferenceDrawer.leftMargin();
   }
   ((QSize)m_SH).rwidth() += m_ConferenceDrawer.leftMargin();
   //END real sizeHint()

   if (i.parent().isValid()) {
      i = i.parent();
   }

   //Avoid repainting the category over and over (optimization)
   //note: 0,0,0,0 is actually wrong, but it wont change anything for this use case
   if (i != old && old.row()>2)
      return QRect(0,0,0,0);

   QTreeWidgetItem* item = m_tree->itemFromIndex(i);
   QRect r = m_tree->visualItemRect(item);

   // adapt width
   r.setLeft(m_ConferenceDrawer.leftMargin());
   r.setWidth(m_tree->viewport()->width() - m_ConferenceDrawer.leftMargin() - m_ConferenceDrawer.rightMargin());

   // adapt height
   if (item->isExpanded() && item->childCount() > 0) {
      const int childCount = item->childCount();
      //There is a massive implact on CPU usage to have massive rect
      for (int i =0;i < childCount;i++) {
         r.setHeight(r.height() + sizeHint(option,index).height());
      }
   }

   r.setTop(r.top() + m_ConferenceDrawer.leftMargin());

   return r;
} //fullCategoryRect

///Paint the delegate
void CallTreeItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   Q_ASSERT(index.isValid());

   QStyleOptionViewItem opt(option);
   QTreeWidgetItem* item = m_tree->itemFromIndex(index);
   CallTreeItem* itemWidget = nullptr;
   if (item) {
      itemWidget = qobject_cast<CallTreeItem*>(m_tree->itemWidget(item,0));
   }
   //BEGIN: draw toplevel items
   if (!index.parent().isValid() && index.child(0,0).isValid()) {
      const QRegion cl = painter->clipRegion();
      painter->setClipRect(opt.rect);
      opt.rect = fullCategoryRect(option, index);
      m_ConferenceDrawer.drawCategory(index, 0, opt, painter,&m_Pal);

      //Drag bubble
      if (itemWidget->isDragged()) {
         QSize size  = itemWidget->size();
         int i = 0;
         while (index.child(i,0).isValid()) i++;
         if (i) {
//                QTreeWidgetItem* firstChild = m_tree->itemFromIndex(index);
            QWidget* childWidget = qobject_cast<CallTreeItem*>(m_tree->itemWidget(item,0));
            if (childWidget) {
               size.setHeight(itemWidget->height()+(i*childWidget->height())+10);
               QPixmap pixmap(size);
               childWidget->render(&pixmap);
               painter->drawPixmap(10,2,pixmap);
            }
         }
      }
      painter->setClipRegion(cl);
      return;
   }

   if (!index.parent().parent().isValid()) {
      opt.rect = fullCategoryRect(option, index);
      const QRegion cl = painter->clipRegion();
      QRect cr = option.rect;
      if (index.column() == 0) {
         if (m_tree->layoutDirection() == Qt::LeftToRight) {
            cr.setLeft(5);
         } else {
            cr.setRight(opt.rect.right());
         }
      }
      painter->setClipRect(cr);
      if (index.parent().isValid())
         m_ConferenceDrawer.drawCategory(index, 0, opt, painter,&m_Pal);
      painter->setClipRegion(cl);
      painter->setRenderHint(QPainter::Antialiasing, false);
   }

   //END: draw background of category for all other items

   QStyleOptionViewItem opt2(option);
   if (index.parent().isValid())
      opt2.rect.setWidth(opt2.rect.width()-15);
   painter->setClipRect(option.rect);
   if (option.state & (QStyle::State_Selected | QStyle::State_MouseOver)) {
      //Draw a copy of the widget when in drag and drop
      if (itemWidget && itemWidget->isDragged()) {
         itemWidget->setTextColor(option.state);

         //Check if it is the last item
         if (index.parent().isValid() && !index.parent().child(index.row()+1,0).isValid()) {
            opt2.rect.setHeight(opt2.rect.height()-15);
            QStyledItemDelegate::paint(painter,opt2,index);
         }

         //Necessary to render conversation participants
         if (opt2.rect != option.rect) {
            QPainter::CompositionMode mode = painter->compositionMode();
            painter->setCompositionMode(QPainter::CompositionMode_Clear);
            painter->fillRect(option.rect,Qt::transparent);
            painter->setCompositionMode(mode);
         }

         //Remove opacity effect to prevent artefacts when there is no compositor
         QGraphicsEffect* opacityEffect = itemWidget->graphicsEffect();
         if (opacityEffect)
            itemWidget->setGraphicsEffect(nullptr);
         QStyledItemDelegate::paint(painter,index.parent().isValid()?opt2:option,index);
         QPixmap pixmap(itemWidget->size());
         itemWidget->render(&pixmap);
         painter->drawPixmap(0,0,pixmap);
         if (opacityEffect) {
            QGraphicsOpacityEffect* opacityEffect2 = new QGraphicsOpacityEffect;
            itemWidget->setGraphicsEffect(opacityEffect2);
         }
         return;
      }
      //Check if it is not the last item
      else if (!(index.parent().isValid() && !index.parent().child(index.row()+1,0).isValid())) {
         QStyledItemDelegate::paint(painter,index.parent().isValid()?opt2:option,index);
      }
   }

   //Check if it is the last item
   if (index.parent().isValid() && !index.parent().child(index.row()+1,0).isValid()) {
      opt2.rect.setHeight(opt2.rect.height()-15);
      QStyledItemDelegate::paint(painter,opt2,index);
   }

   if (item && itemWidget) {
      itemWidget->setTextColor(option.state);
      itemWidget->setMinimumSize(opt2.rect.width(),10);
      itemWidget->setMaximumSize(opt2.rect.width(),opt2.rect.height());
      itemWidget->resize(opt2.rect.width(),option.rect.height());
   }

   if (index.parent().isValid() && !index.parent().child(index.row()+1,0).isValid()) {
      m_ConferenceDrawer.drawBoxBottom(index, 0, option, painter);
   }
} //paint