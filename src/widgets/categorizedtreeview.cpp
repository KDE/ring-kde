/***************************************************************************
 *   Copyright (C) 2009 by Rafael Fernández López <ereslibre@kde.org>      *
 *   @author: Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
 *                                                                         *
 * This library is free software; you can redistribute it and/or           *
 * modify it under the terms of the GNU Library General Public             *
 * License version 2 as published by the Free Software Foundation.         *
 *                                                                         *
 * This library is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * Library General Public License for more details.                        *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/

#include "categorizedtreeview.h"

#include "categorydrawer.h"

//Qt
#include <QtGui/QStyledItemDelegate>
#include <QtGui/QPainter>
#include <QtGui/QHeaderView>
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>

//KDE
#include <klocale.h>
#include <kdebug.h>

//BEGIN SortedTreeDelegate
///Code taken from Kate and adapted to render categorized lists
class SortedTreeDelegate : public QStyledItemDelegate
{
  public:
   SortedTreeDelegate(CategorizedTreeView* widget)
   : QStyledItemDelegate(widget)
   , m_tree(widget),m_LeftMargin(0),m_RightMargin(0)
   {
   }

   QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
      //Only do it for categories and objects deeper than 1 level, use precalculated values for others
      if (!index.parent().isValid() || index.parent().parent().isValid()) {
         QSize sh = QStyledItemDelegate::sizeHint(option, index);
         sh.rheight() += 2 * m_categoryDrawer.leftMargin();
         sh.rwidth() += m_categoryDrawer.leftMargin();
         return sh;
      }
      return m_SH;
   }

   QRect fullCategoryRect(const QStyleOptionViewItem& option, const QModelIndex& index) const {
      QModelIndex i(index),old(index);

      //BEGIN real sizeHint()
      //Otherwise it would be called too often (thanks to valgrind)
      ((SortedTreeDelegate*)this)->m_SH          = QStyledItemDelegate::sizeHint(option, index);
      ((SortedTreeDelegate*)this)->m_LeftMargin  = m_categoryDrawer.leftMargin();
      ((SortedTreeDelegate*)this)->m_RightMargin = m_categoryDrawer.rightMargin();
      if (!index.parent().isValid()) {
         ((QSize)m_SH).rheight() += 2 * m_categoryDrawer.leftMargin();
      } else {
         ((QSize)m_SH).rheight() += m_categoryDrawer.leftMargin();
      }
      ((QSize)m_SH).rwidth() += m_categoryDrawer.leftMargin();
      //END real sizeHint()
      
      if (i.parent().isValid()) {
         i = i.parent();
      }

      //Avoid repainting the category over and over (optimization)
      //note: 0,0,0,0 is actually wrong, but it wont change anything for this use case
      if (i != old && old.row()>2)
         return QRect(0,0,0,0);

//       QModelIndex* item = m_tree->itemFromIndex(i);
      QRect r = m_tree->visualRect(i);

      // adapt width
      r.setLeft(m_categoryDrawer.leftMargin());
      r.setWidth(m_tree->viewport()->width() - m_categoryDrawer.leftMargin() - m_categoryDrawer.rightMargin());

      // adapt height
      if (m_tree->isExpanded(i) && m_tree->model()->rowCount(i) > 0) {
         const int childCount = m_tree->model()->rowCount(i);
         const int h = sizeHint(option, index.child(0, 0)).height();
         //There is a massive implact on CPU usage to have massive rect
         r.setHeight((r.height() + childCount * h > 100)?100:(r.height() + childCount * h));
      }

      r.setTop(r.top() + m_categoryDrawer.leftMargin());

      return r;
   } //fullCategoryRect

   virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
   {
      Q_ASSERT(index.isValid());
      
      //BEGIN: draw toplevel items
      if (!index.parent().isValid()) {
         QStyleOptionViewItem opt(option);
         const QRegion cl = painter->clipRegion();
         painter->setClipRect(opt.rect);
         opt.rect = fullCategoryRect(option, index);
         m_categoryDrawer.drawCategory(index, 0, opt, painter);
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
         m_categoryDrawer.drawCategory(index, 0, opt, painter);
         painter->setClipRegion(cl);
         painter->setRenderHint(QPainter::Antialiasing, false);
      }
      //END: draw background of category for all other items

      painter->setClipRect(option.rect);
      if (option.state & QStyle::State_Selected) {
         QStyledItemDelegate::paint(painter,option,index);
      }

      if (index.isValid()) {
         QWidget* widget = m_tree->indexWidget(index);
         if (widget) {
            widget->setMinimumSize((m_tree->viewport()->width() - m_categoryDrawer.leftMargin() - m_categoryDrawer.rightMargin())-m_categoryDrawer.leftMargin(),10);
            widget->setMaximumSize((m_tree->viewport()->width() - m_categoryDrawer.leftMargin() - m_categoryDrawer.rightMargin())-m_categoryDrawer.leftMargin(),99999);
         }
      }
      
         QStyledItemDelegate::paint(painter,option,index);
   } //paint

  private:
   CategorizedTreeView* m_tree;
   CategoryDrawer m_categoryDrawer;
   QSize m_SH;
   int m_LeftMargin;
   int m_RightMargin;
};
//END SortedTreeDelegate

///Constructor
CategorizedTreeView::CategorizedTreeView(QWidget *parent)
  : QTreeView(parent)
{
  setItemDelegate(new SortedTreeDelegate(this));
  setHeaderHidden(true);
  setRootIsDecorated(false);
  //setUniformRowHeights(false);
  setIndentation(25);
  setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  setHorizontalScrollMode(QAbstractItemView::ScrollPerItem);
}

///Do not draw branches
void CategorizedTreeView::drawBranches(QPainter* painter, const QRect& rect, const QModelIndex& index) const
{
  Q_UNUSED( painter )
  Q_UNUSED( rect    )
  Q_UNUSED( index   )
}
