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
#include "conferencedelegate.h"

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
#include "../widgets/categorizedtreeview.h"
#include "sflphone.h"
#include "../lib/call.h"

///Constructor
ConferenceDelegate::ConferenceDelegate(CategorizedTreeView* widget,QPalette pal)
      : QStyledItemDelegate(widget) , m_tree(widget) , m_Pal(pal),
      m_LeftMargin(7),m_RightMargin(7)
{
}

///Guess the size of the item
QSize ConferenceDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const 
{
   QSize sh = QStyledItemDelegate::sizeHint(option, index);
   if (index.parent().isValid()) {
      sh = m_pCallDelegate->sizeHint(option,index);
   }
   else if (!index.child(0,0).isValid()) {
      sh = m_pCallDelegate->sizeHint(option,index);
   }
   else {
      sh.setHeight(categoryHeight(index,option,&m_Pal));
   }

   //Bottom margin
   if (index.parent().isValid() && !index.parent().child(index.row()+1,0).isValid())
      sh.rheight() += 15;
   return sh;
}

///Generate the rectangle
QRect ConferenceDelegate::fullCategoryRect(const QStyleOptionViewItem& option, const QModelIndex& index) const 
{
   QModelIndex i(index),old(index);
   //BEGIN real sizeHint()
   //Otherwise it would be called too often (thanks to valgrind)
   ((ConferenceDelegate*)this)->m_SH          = QStyledItemDelegate::sizeHint(option, index);
   if (!index.parent().isValid() && index.child(0,0).isValid()) {
      ((QSize)m_SH).rheight() += 2 * m_LeftMargin;
   } else {
      ((QSize)m_SH).rheight() += m_LeftMargin;
   }
   ((QSize)m_SH).rwidth() += m_LeftMargin;
   //END real sizeHint()

   if (i.parent().isValid()) {
      i = i.parent();
   }

   //Avoid repainting the category over and over (optimization)
   //note: 0,0,0,0 is actually wrong, but it wont change anything for this use case
   if (i != old && old.row()>2)
      return QRect(0,0,0,0);

   QRect r;
   if (option.state & QStyle::State_Small) //Hack to know when this is a drag popup
      r = option.rect;
   else
      r = m_tree->visualRect(i);
   // adapt width
   r.setLeft(m_LeftMargin);
   r.setWidth(m_tree->viewport()->width() - m_LeftMargin - m_RightMargin);

   // adapt height
   const int childCount = m_tree->model()->rowCount(i);
   if (m_tree->isExpanded(i) && childCount > 0) {
      //There is a massive implact on CPU usage to have massive rect
      r.setHeight(r.height() + sizeHint(option,i.child(0,0)).height()*childCount+2*15); //2*15 = margins
   }
   r.setTop(r.top() + m_LeftMargin);

   return r;
} //fullCategoryRect

///Paint the delegate
void ConferenceDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
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
      drawCategory(index, 0, opt, painter,&m_Pal);

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
         drawCategory(index, 0, opt, painter,&m_Pal);

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
//       opt5.rect.setLeft(leftMargin());
//       opt5.rect.setWidth(m_tree->viewport()->width() - leftMargin() - rightMargin());
      painter->setClipRect(opt.rect);
      drawBoxBottom(index, 0, option, painter);
   }
   
//       m_pCallDelegate->paint(painter,option,index);
} //paint

void ConferenceDelegate::drawCategory(const QModelIndex&  index   ,
                                 int                 sortRole,
                                 const QStyleOption& option  ,
                                 QPainter*           painter ,
                                 const QPalette* pal) const
{
   Q_UNUSED( sortRole )
   Q_UNUSED( index    )
   const QPalette* palette   = (pal)?pal:&option.palette  ;
   painter->setRenderHint(QPainter::Antialiasing);

   const QRect optRect = option.rect;

   //BEGIN: decoration gradient
   {
      QPainterPath path(optRect.bottomLeft());

      path.lineTo(QPoint(optRect.topLeft().x(), optRect.topLeft().y() - 3));
      const QPointF topLeft(optRect.topLeft());
      QRectF arc(topLeft, QSizeF(4, 4));
      path.arcTo(arc, 180, -90);
      path.lineTo(optRect.topRight());
      path.lineTo(optRect.bottomRight());
      path.lineTo(optRect.bottomLeft());

      QColor window(palette->window().color());
      const QColor base(palette->base().color());

      window.setAlphaF(option.state & QStyle::State_Selected?0.9:0.9);

      QColor window2(window);
      window2.setAlphaF(option.state & QStyle::State_Selected?0.4:0.4);

      QLinearGradient decoGradient1;
      decoGradient1.setStart(optRect.topLeft());
      decoGradient1.setFinalStop(optRect.bottomLeft());
      decoGradient1.setColorAt(0, window);
      decoGradient1.setColorAt(1, Qt::transparent);

      QLinearGradient decoGradient2;
      decoGradient2.setStart(optRect.topLeft());
      decoGradient2.setFinalStop(optRect.topRight());
      decoGradient2.setColorAt(0, window2);
      decoGradient2.setColorAt(1, Qt::transparent);

      painter->fillPath(path, decoGradient1);
      painter->fillRect(optRect, decoGradient2);
   }
   //END: decoration gradient

   {
      QRect newOptRect(optRect);

      newOptRect.translate(1, 1);

      //BEGIN: inner top left corner
      {
         painter->save();
         painter->setPen(palette->base().color());
         const QPointF topLeft(newOptRect.topLeft());
         QRectF arc = QRectF(topLeft, QSizeF(4, 4));
         arc.translate(0.5, 0.5);
         painter->drawArc(arc, 1440, 1440);
         painter->restore();
      }
      //END: inner top left corner

      //BEGIN: inner left vertical line
      {
         QPoint start = newOptRect.topLeft();
         QPoint verticalGradBottom = newOptRect.topLeft();
         start.ry() += 3;
         verticalGradBottom.ry() += newOptRect.height() - 3;
         QLinearGradient gradient(start, verticalGradBottom);
         gradient.setColorAt(0, palette->base().color());
         gradient.setColorAt(1, Qt::transparent);
         painter->fillRect(QRect(start, QSize(1, newOptRect.height() - 3)), gradient);
      }
      //END: inner left vertical line

      //BEGIN: top inner horizontal line
      {
         QPoint start = newOptRect.topLeft();
         QPoint horizontalGradTop = newOptRect.topLeft();
         start.rx() += 3;
         horizontalGradTop.rx() += newOptRect.width() - 3;
         QLinearGradient gradient(start, horizontalGradTop);
         gradient.setColorAt(0, palette->base().color());
         gradient.setColorAt(1, Qt::transparent);
         QSize rectSize = QSize(newOptRect.width() - 30, 1);
         painter->fillRect(QRect(start, rectSize), gradient);
      }
      //END: top inner horizontal line
   }

   QColor outlineColor = palette->text().color();
   outlineColor.setAlphaF(0.35);

   //BEGIN: top left corner
   {
      painter->save();
      painter->setPen(outlineColor);
      QRectF arc;
      const QPointF topLeft(optRect.topLeft());
      arc = QRectF(topLeft, QSizeF(4, 4));
      arc.translate(0.5, 0.5);
      painter->drawArc(arc, 1440, 1440);
      painter->restore();
   }
   //END: top left corner

   //BEGIN: top right corner
   {
      painter->save();
      painter->setPen(outlineColor);
      QPointF topRight(optRect.topRight());
      topRight.rx() -= 3;
      QRectF arc = QRectF(topRight, QSizeF(4, 4));
      arc.translate(-0.5, 0.5);
      painter->drawArc(arc, 0, 1440);
      painter->restore();
   }
   //END: top right corner

   //BEGIN: left vertical line
   {
      QPoint start = optRect.topLeft();
      QPoint verticalGradBottom = optRect.topLeft();
      start.ry() += 3;
      verticalGradBottom.ry() += optRect.height() - 3 + 200;
      painter->fillRect(QRect(start, QSize(1, optRect.height() - 21)), outlineColor);
   }
   //END: left vertical line

   //BEGIN: right vertical line
   {
      QPoint start = optRect.topRight();
      QPoint verticalGradBottom = optRect.topRight();
      start.ry() += 3;
      verticalGradBottom.ry() += optRect.height() - 3 + 200;
      painter->fillRect(QRect(start, QSize(1, optRect.height() - 21)), outlineColor);
   }
   //END: right vertical line

   //BEGIN: horizontal line
   {
      QPoint start = optRect.topLeft();
      QPoint horizontalGradTop = optRect.topLeft();
      start.rx() += 3;
      horizontalGradTop.rx() += optRect.width() - 3;
      QLinearGradient gradient(start, horizontalGradTop);
      gradient.setColorAt(0, outlineColor);
      gradient.setColorAt(1, outlineColor);
      QSize rectSize = QSize(optRect.width() - 6, 1);
      painter->fillRect(QRect(start, rectSize), gradient);
   }
   //END: horizontal line
} //drawCategory

///Draw the bottom border of the box
void ConferenceDelegate::drawBoxBottom(const QModelIndex &index, int sortRole, const QStyleOption &option, QPainter *painter,const QPalette* pal) const {
   Q_UNUSED(index)
   Q_UNUSED(sortRole)
   const QPalette* palette = (pal)?pal:&option.palette  ;
   painter->setRenderHint(QPainter::Antialiasing);
   QColor outlineColor = palette->text().color();
   outlineColor.setAlphaF(0.35);
   painter->setPen(outlineColor);

   //BEGIN: bottom horizontal line
   {
   QPoint bl = option.rect.bottomLeft();
   bl.setY(bl.y());
   bl.setX(m_LeftMargin+3);

   painter->fillRect(QRect(bl, QSize(option.rect.width()+4,1)), outlineColor);
   }
   //END: bottom horizontal line

   //BEGIN: bottom right corner
   {
      QRectF arc;
      QPointF br(option.rect.bottomRight());
      br.setY(br.y()-4);
      br.setX(br.x()-12);
      arc = QRectF(br, QSizeF(4, 4));
      arc.translate(0.5, 0.5);
      painter->drawArc(arc, 4320, 1440);
   }
   //END: bottom right corner

   //BEGIN: bottom left corner
   {
      QRectF arc;
      QPointF br(option.rect.bottomRight());
      br.setY(br.y()-4);
      br.setX(m_LeftMargin);
      arc = QRectF(br, QSizeF(4, 4));
      arc.translate(0.5, 0.5);
      painter->drawArc(arc, 1440*2, 1440);
   }
   //END: bottom left corner
} //drawBoxBottom

///Return the height of the conference box
int ConferenceDelegate::categoryHeight(const QModelIndex &index, const QStyleOption &option,const QPalette* pal) const
{
   Q_UNUSED( index  );
   Q_UNUSED( option );
   Q_UNUSED( pal    );
   QFont font(QApplication::font());
   font.setBold(true);
   const QFontMetrics fontMetrics = QFontMetrics(font);

   return fontMetrics.height() + 2 + 16 /* vertical spacing */;
}

QPixmap ConferenceDelegate::getDragPixmap(CategorizedTreeView* parent, const QModelIndex& index)
{
   QStyleOptionViewItemV4 option;
   option.locale = parent->locale();
   option.widget = parent;
   option.state = QStyle::State_Selected | QStyle::State_Enabled | QStyle::State_Active | QStyle::State_Small;
   option.rect = QRect(0,0,parent->width(),parent->height());
   QSize size = parent->itemDelegate()->sizeHint(option,index);
   QSize itemSize = size;
   for (int i=0;i<parent->model()->rowCount(index);i++) {
      size.setHeight(size.height()+parent->itemDelegate()->sizeHint(option,index.child(i,0)).height());
   }

   //Setup the painter
   QPixmap pixmap(parent->width(),size.height());
   QPainter customPainter(&pixmap);
   customPainter.eraseRect(option.rect);
   customPainter.setCompositionMode(QPainter::CompositionMode_Clear);
   customPainter.fillRect(option.rect,QBrush(Qt::red));
   customPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);

   //Draw the parent
   option.rect = QRect(0,0,parent->width(),itemSize.height());
   parent->itemDelegate()->paint(&customPainter, option, index);

   //Draw the children
   for (int i=0;i<parent->model()->rowCount(index);i++) {
      itemSize.setHeight(parent->itemDelegate()->sizeHint(option,index.child(i,0)).height());
      option.rect = QRect(10,option.rect.y()+option.rect.height(),parent->width()-20,itemSize.height());
      option.state = QStyle::State_Enabled | QStyle::State_Active | QStyle::State_Small;
      parent->itemDelegate()->paint(&customPainter, option, index.child(i,0));
   }
   return pixmap;
}