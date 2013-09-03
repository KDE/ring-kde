/****************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                          *
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
#include "categorizeddelegate.h"

#include <QtGui/QTreeView>
#include <QtGui/QPainter>
#include <QtGui/QApplication>

#include <QtCore/QDebug>

///Construnctor
CategorizedDelegate::CategorizedDelegate(QTreeView* widget)
   : QStyledItemDelegate(widget)
   , m_tree(widget),m_LeftMargin(7),m_RightMargin(7),m_pChildDelegate(nullptr),m_pChildChildDelegate(nullptr)
{
}

///Destructor
CategorizedDelegate::~CategorizedDelegate()
{
   if (m_pChildDelegate) delete m_pChildDelegate;
   if (m_pChildChildDelegate) delete m_pChildChildDelegate;
}

///Report category height
QSize CategorizedDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
   //Only do it for categories and objects deeper than 1 level, use precalculated values for others
   if (index.parent().isValid() && !index.parent().parent().isValid() && m_pChildDelegate) {
      return m_pChildDelegate->sizeHint(option,index);
   }
   if (!index.parent().isValid() || index.parent().parent().isValid()) {
      QSize sh = QStyledItemDelegate::sizeHint(option, index);
      sh.rheight() += 2 * m_LeftMargin;
      sh.rwidth() += m_LeftMargin;
      return sh;
   }
   return m_SH;
} //sizeHint

///Generate a gradient rectangle for the categories and the first child
QRect CategorizedDelegate::fullCategoryRect(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   QModelIndex i(index),old(index);

   //BEGIN real sizeHint()
   //Otherwise it would be called too often (thanks to valgrind)
   const_cast<CategorizedDelegate*>(this)->m_SH = QStyledItemDelegate::sizeHint(option, index);
   if (!index.parent().isValid()) {
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

   QRect r = m_tree->visualRect(i);

   // adapt width
   r.setLeft(m_LeftMargin);
   r.setWidth(m_tree->viewport()->width() - m_LeftMargin - m_RightMargin);

   // adapt height
   if (m_tree->isExpanded(i) && m_tree->model()->rowCount(i) > 0) {
      const int childCount = m_tree->model()->rowCount(i);
      const int h = sizeHint(option, index.child(0, 0)).height();
      //There is a massive implact on CPU usage to have massive rect
      r.setHeight((r.height() + childCount * h > 100)?100:(r.height() + childCount * h));
   }

   r.setTop(r.top() + m_LeftMargin);

   return r;
} //fullCategoryRect

///Draw the category (and the first child)
void CategorizedDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   Q_ASSERT(index.isValid());
   
   //BEGIN: draw toplevel items
   if (!index.parent().isValid()) {
      QStyleOptionViewItem opt(option);
      const QRegion cl = painter->clipRegion();
      painter->setClipRect(opt.rect);
      opt.rect = fullCategoryRect(option, index);
      drawCategory(index, 0, opt, painter);
      painter->setClipRegion(cl);
      return;
   }
   
   if (!index.parent().parent().isValid()) {
      QStyleOptionViewItem opt(option);
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
      drawCategory(index, 0, opt, painter);
      painter->setClipRegion(cl);
      painter->setRenderHint(QPainter::Antialiasing, false);
   }
   //END: draw background of category for all other items

   painter->setClipRect(option.rect);
   

   if (index.isValid()) {
      QWidget* widget = m_tree->indexWidget(index);
      if (widget) {
         widget->setMinimumSize((m_tree->viewport()->width() - m_LeftMargin - m_RightMargin)-m_LeftMargin,10);
         widget->setMaximumSize((m_tree->viewport()->width() - m_LeftMargin - m_RightMargin)-m_LeftMargin,99999);
      }
   }
   
   if (m_pChildDelegate && !index.parent().parent().isValid()) {
      m_pChildDelegate->paint(painter,option,index);
   }
   else if (m_pChildChildDelegate && index.parent().parent().isValid()) {
      m_pChildChildDelegate->paint(painter,option,index);
   }
} //paint

///Draw a category style
void CategorizedDelegate::drawCategory(const QModelIndex& index,
                                 int                 sortRole,
                                 const QStyleOption& option  ,
                                 QPainter*           painter ) const
{
   Q_UNUSED( sortRole )
   
   painter->setRenderHint(QPainter::Antialiasing);

   const QRect optRect = option.rect;
   QFont font(QApplication::font());
   font.setBold(true);
   const QFontMetrics fontMetrics = QFontMetrics(font);
   const int height = categoryHeight(index, option);
   const bool leftToRight = painter->layoutDirection() == Qt::LeftToRight;

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

      QColor window(option.palette.window().color());
      const QColor base(option.palette.base().color());

      window.setAlphaF(0.4);

      QLinearGradient decoGradient1;
      if (leftToRight) {
         decoGradient1.setStart(optRect.topLeft());
         decoGradient1.setFinalStop(optRect.bottomLeft());
      } else {
         decoGradient1.setStart(optRect.topRight());
         decoGradient1.setFinalStop(optRect.bottomRight());
      }
      decoGradient1.setColorAt(0, window);
      decoGradient1.setColorAt(1, Qt::transparent);

      QLinearGradient decoGradient2;
      if (leftToRight) {
         decoGradient2.setStart(optRect.topLeft());
         decoGradient2.setFinalStop(optRect.topRight());
      } else {
         decoGradient2.setStart(optRect.topRight());
         decoGradient2.setFinalStop(optRect.topLeft());
      }
      decoGradient2.setColorAt(0, Qt::transparent);
      decoGradient2.setColorAt(1, base);

      painter->fillPath(path, decoGradient1);
      painter->fillPath(path, decoGradient2);
   }
   //END: decoration gradient

   {
      QRect newOptRect(optRect);

      if (leftToRight) {
         newOptRect.translate(1, 1);
      } else {
         newOptRect.translate(-1, 1);
      }

      //BEGIN: inner top left corner
      {
         painter->save();
         painter->setPen(option.palette.base().color());
         QRectF arc;
         if (leftToRight) {
               const QPointF topLeft(newOptRect.topLeft());
               arc = QRectF(topLeft, QSizeF(4, 4));
               arc.translate(0.5, 0.5);
               painter->drawArc(arc, 1440, 1440);
         } else {
               QPointF topRight(newOptRect.topRight());
               topRight.rx() -= 4;
               arc = QRectF(topRight, QSizeF(4, 4));
               arc.translate(-0.5, 0.5);
               painter->drawArc(arc, 0, 1440);
         }
         painter->restore();
      }
      //END: inner top left corner

      //BEGIN: inner left vertical line
      {
         QPoint start;
         QPoint verticalGradBottom;
         if (leftToRight) {
               start = newOptRect.topLeft();
               verticalGradBottom = newOptRect.topLeft();
         } else {
               start = newOptRect.topRight();
               verticalGradBottom = newOptRect.topRight();
         }
         start.ry() += 3;
         verticalGradBottom.ry() += newOptRect.height() - 3;
         QLinearGradient gradient(start, verticalGradBottom);
         gradient.setColorAt(0, option.palette.base().color());
         gradient.setColorAt(1, Qt::transparent);
         painter->fillRect(QRect(start, QSize(1, newOptRect.height() - 3)), gradient);
      }
      //END: inner left vertical line

      //BEGIN: inner horizontal line
      {
         QPoint start;
         QPoint horizontalGradTop;
         if (leftToRight) {
               start = newOptRect.topLeft();
               horizontalGradTop = newOptRect.topLeft();
               start.rx() += 3;
               horizontalGradTop.rx() += newOptRect.width() - 3;
         } else {
               start = newOptRect.topRight();
               horizontalGradTop = newOptRect.topRight();
               start.rx() -= 3;
               horizontalGradTop.rx() -= newOptRect.width() - 3;
         }
         QLinearGradient gradient(start, horizontalGradTop);
         gradient.setColorAt(0, option.palette.base().color());
         gradient.setColorAt(1, Qt::transparent);
         QSize rectSize;
         if (leftToRight) {
               rectSize = QSize(newOptRect.width() - 3, 1);
         } else {
               rectSize = QSize(-newOptRect.width() + 3, 1);
         }
         painter->fillRect(QRect(start, rectSize), gradient);
      }
      //END: inner horizontal line
   }

    QColor outlineColor = option.palette.text().color();
    outlineColor.setAlphaF(0.35);

   //BEGIN: top left corner
   {
      painter->save();
      painter->setPen(outlineColor);
      QRectF arc;
      if (leftToRight) {
         const QPointF topLeft(optRect.topLeft());
         arc = QRectF(topLeft, QSizeF(4, 4));
         arc.translate(0.5, 0.5);
         painter->drawArc(arc, 1440, 1440);
      } else {
         QPointF topRight(optRect.topRight());
         topRight.rx() -= 4;
         arc = QRectF(topRight, QSizeF(4, 4));
         arc.translate(-0.5, 0.5);
         painter->drawArc(arc, 0, 1440);
      }
      painter->restore();
   }
   //END: top left corner

   //BEGIN: left vertical line
   {
      QPoint start;
      QPoint verticalGradBottom;
      if (leftToRight) {
         start = optRect.topLeft();
         verticalGradBottom = optRect.topLeft();
      } else {
         start = optRect.topRight();
         verticalGradBottom = optRect.topRight();
      }
      start.ry() += 3;
      verticalGradBottom.ry() += optRect.height() - 3;
      QLinearGradient gradient(start, verticalGradBottom);
      gradient.setColorAt(0, outlineColor);
      gradient.setColorAt(1, option.palette.base().color());
      painter->fillRect(QRect(start, QSize(1, optRect.height() - 3)), gradient);
   }
   //END: left vertical line

   //BEGIN: horizontal line
   {
      QPoint start;
      QPoint horizontalGradTop;
      if (leftToRight) {
         start = optRect.topLeft();
         horizontalGradTop = optRect.topLeft();
         start.rx() += 3;
         horizontalGradTop.rx() += optRect.width() - 3;
      } else {
         start = optRect.topRight();
         horizontalGradTop = optRect.topRight();
         start.rx() -= 3;
         horizontalGradTop.rx() -= optRect.width() - 3;
      }
      QLinearGradient gradient(start, horizontalGradTop);
      gradient.setColorAt(0, outlineColor);
      gradient.setColorAt(1, option.palette.base().color());
      QSize rectSize;
      if (leftToRight) {
         rectSize = QSize(optRect.width() - 3, 1);
      } else {
         rectSize = QSize(-optRect.width() + 3, 1);
      }
      painter->fillRect(QRect(start, rectSize), gradient);
   }
   //END: horizontal line

   //BEGIN: draw text
   {
      const QString category = index.model()->data(index, Qt::DisplayRole).toString();
      QRect textRect = QRect(option.rect.topLeft(), QSize(option.rect.width() - 2 - 3 - 3, height));
      textRect.setTop(textRect.top() + 2 + 3 /* corner */);
      textRect.setLeft(textRect.left() + 2 + 3 /* corner */ + 3 /* a bit of margin */);
      painter->save();
      painter->setFont(font);
      QColor penColor(option.palette.text().color());
      penColor.setAlphaF(0.6);
      painter->setPen(penColor);
      painter->drawText(textRect, Qt::AlignLeft | Qt::AlignTop, category);
      painter->restore();
   }
   //END: draw text
} //drawCategory

///Return category height
int CategorizedDelegate::categoryHeight(const QModelIndex &index, const QStyleOption &option) const
{
   Q_UNUSED( index );
   Q_UNUSED( option );

   QFont font(QApplication::font());
   font.setBold(true);
   const QFontMetrics fontMetrics = QFontMetrics(font);

   return fontMetrics.height() + 2 + 12 /* vertical spacing */;
} //categoryHeight

///Set the delagate that are categorized
void CategorizedDelegate::setChildDelegate(QStyledItemDelegate* childDelegate)
{
   m_pChildDelegate = childDelegate;
}

///Set the categorized delegate own children
void CategorizedDelegate::setChildChildDelegate(QStyledItemDelegate* childDelegate)
{
   m_pChildChildDelegate = childDelegate;
}
