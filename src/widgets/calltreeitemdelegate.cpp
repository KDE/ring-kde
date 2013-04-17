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
#include <QtGui/QApplication>

//KDE
#include <KStandardDirs>

//SFLPhone
#include "categorizedtreeview.h"
#include "calltreeitem.h"
#include "sflphone.h"
#include "../lib/call.h"

///Constructor
CallTreeItemDelegate::CallTreeItemDelegate(CategorizedTreeView* widget,QPalette pal)
      : QStyledItemDelegate(widget) , m_tree(widget) , m_ConferenceDrawer() , m_Pal(pal),
      m_LeftMargin(0),m_RightMargin(0)
{
}

///Guess the size of the item
QSize CallTreeItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const 
{
   QSize sh = QStyledItemDelegate::sizeHint(option, index);
   if (index.parent().isValid()) {
      sh = m_pCallDelegate->sizeHint(option,index);
   }
   else if (!index.child(0,0).isValid()) {
      sh = m_pCallDelegate->sizeHint(option,index);
   }
   else {
      sh.setHeight(m_ConferenceDrawer.categoryHeight(index,option,&m_Pal));
   }

   //Bottom margin
   if (index.parent().isValid() && !index.parent().child(index.row()+1,0).isValid())
      sh.rheight() += 15;
   return sh;
}

///Generate the rectangle
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

   QRect r = m_tree->visualRect(i);

   // adapt width
   r.setLeft(m_ConferenceDrawer.leftMargin());
   r.setWidth(m_tree->viewport()->width() - m_ConferenceDrawer.leftMargin() - m_ConferenceDrawer.rightMargin());

   // adapt height
   const int childCount = m_tree->model()->rowCount(i);
   if (m_tree->isExpanded(i) && childCount > 0) {
      //There is a massive implact on CPU usage to have massive rect
      r.setHeight(r.height() + sizeHint(option,i.child(0,0)).height()*childCount+2*15); //2*15 = margins
   }
   r.setTop(r.top() + m_ConferenceDrawer.leftMargin());

   return r;
} //fullCategoryRect

///Paint the delegate
void CallTreeItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   Q_ASSERT(index.isValid());

   QStyleOptionViewItem opt(option);
//    QTreeWidgetItem* item = m_tree->itemFromIndex(index);
//    CallTreeItem* itemWidget = nullptr;
//    if (item) {
//       itemWidget = qobject_cast<CallTreeItem*>(m_tree->itemWidget(item,0));
//    }
   //BEGIN: draw toplevel items
   if (!index.parent().isValid() && index.child(0,0).isValid()) {
      const QRegion cl = painter->clipRegion();
      painter->setClipRect(opt.rect);
      opt.rect = fullCategoryRect(option, index);
      m_ConferenceDrawer.drawCategory(index, 0, opt, painter,&m_Pal);

      //Drag bubble
//       if (itemWidget->isDragged()) {
//          QSize size  = itemWidget->size();
//          int i = 0;
//          while (index.child(i,0).isValid()) i++;
//          if (i) {
// //                QTreeWidgetItem* firstChild = m_tree->itemFromIndex(index);
//             QWidget* childWidget = qobject_cast<CallTreeItem*>(m_tree->itemWidget(item,0));
//             if (childWidget) {
//                size.setHeight(itemWidget->height()+(i*childWidget->height())+10);
//                QPixmap pixmap(size);
//                childWidget->render(&pixmap);
//                painter->drawPixmap(10,2,pixmap);
//             }
//          }
//       }

      //Draw the conference icon and infos
      static const QPixmap* pxm = nullptr;
      if (!pxm) //Static
         pxm = new QPixmap(KStandardDirs::locate("data","sflphone-client-kde/conf-small.png"));
      painter->drawPixmap ( opt.rect.x()+5, opt.rect.y()+2, 24, 24, *pxm);
      QFont font = painter->font();
      font.setBold(true);

      static QColor baseColor = Qt::red;
      /*static block*/if (baseColor == Qt::red) {
         QColor textColor = QApplication::palette().text().color();
         baseColor = QApplication::palette().base().color().name();
         baseColor.setBlue (baseColor.blue() + (textColor.blue() -baseColor.blue()) *0.6);
         baseColor.setRed  (baseColor.red()  + (textColor.red()  -baseColor.red())  *0.6);
         baseColor.setGreen(baseColor.green()+ (textColor.green()-baseColor.green())*0.6);
      }
      painter->setPen(baseColor);
      painter->setFont(font);
      painter->drawText(opt.rect.x()+33,opt.rect.y()+font.pointSize()+8,"Conference");
      font.setBold(false);
      painter->setFont(font);
      baseColor.setAlpha(150);
      painter->setPen(baseColor);
      baseColor.setAlpha(255);
      painter->drawText(opt.rect.x()+opt.rect.width()-40,opt.rect.y()+font.pointSize()+8,index.data(Call::Role::Length).toString());
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

      //Box background
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
      /*if (itemWidget && itemWidget->isDragged()) {
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
      }*/
   }

   //Check if it is the last item
   if (index.parent().isValid() && !index.parent().child(index.row()+1,0).isValid()) {
      opt2.rect.setHeight(opt2.rect.height()-15);
      m_pCallDelegate->paint(painter,opt2,index);
   }
   else if (index.parent().isValid() || (!index.parent().isValid() && !index.child(0,0).isValid())) {
      m_pCallDelegate->paint(painter,opt2,index);
   }

   if (index.parent().isValid() && !index.parent().child(index.row()+1,0).isValid()) {
//       QStyleOptionViewItem opt5(option);
//       opt5.rect.setLeft(m_ConferenceDrawer.leftMargin());
//       opt5.rect.setWidth(m_tree->viewport()->width() - m_ConferenceDrawer.leftMargin() - m_ConferenceDrawer.rightMargin());
      m_ConferenceDrawer.drawBoxBottom(index, 0, option, painter);
   }
   
//       m_pCallDelegate->paint(painter,option,index);
} //paint
