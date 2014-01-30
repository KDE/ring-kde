/****************************************************************************
 *   Copyright (C) 2009-2014 by Savoir-Faire Linux                          *
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

//Qt
#include <QtGui/QTreeView>
#include <QtGui/QPainter>
#include <QtGui/QApplication>
#include <QtCore/QDebug>

namespace {
   static const int TOP_MARGIN       = 20;
   static const int BOTTOM_MARGIN    = 7 ;
}

///Construnctor
CategorizedDelegate::CategorizedDelegate(QTreeView* widget)
   : QStyledItemDelegate(widget)
   , m_LeftMargin(7),m_pChildDelegate(nullptr),m_pChildChildDelegate(nullptr)
{
}

///Destructor
CategorizedDelegate::~CategorizedDelegate()
{
   if ( m_pChildDelegate      ) delete m_pChildDelegate     ;
   if ( m_pChildChildDelegate ) delete m_pChildChildDelegate;
}

///Report category height
QSize CategorizedDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
   //Only do it for categories and objects deeper than 1 level, use precalculated values for others
   if (index.parent().isValid() && !index.parent().parent().isValid() && m_pChildDelegate) {
      return m_pChildDelegate->sizeHint(option,index);
   }
   if (index.parent().parent().isValid() && m_pChildChildDelegate) {
      return m_pChildChildDelegate->sizeHint(option,index);
   }
   if (!index.parent().isValid()) {
      //This allow deleted contacts and filtered out categories not to be displayed
      const bool hasChildren = index.child(0,0).isValid();
      if (!hasChildren)
         return QSize(0,0);

      //If the category has children, then return the real size
      static const int metric = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameVMargin)*2;
      QSize sh = QStyledItemDelegate::sizeHint(option, index);
      sh.rheight() += BOTTOM_MARGIN + (index.row()==0?metric:TOP_MARGIN);
      sh.rwidth() += m_LeftMargin;
      return sh;
   }
   return QStyledItemDelegate::sizeHint(option,index);
} //sizeHint

///Draw the category (and the first child)
void CategorizedDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   Q_ASSERT(index.isValid());

   if (!option.rect.height())
      return;
   if (!index.parent().isValid()) {
      QStyleOptionViewItem opt(option);
      const QRegion cl = painter->clipRegion();
      painter->setClipRect(opt.rect);
      drawSimpleCategory(index, 0, opt, painter);
      painter->setClipRegion(cl);
      return;
   }

   painter->setClipRect(option.rect);

   if (m_pChildDelegate && !index.parent().parent().isValid())
      m_pChildDelegate->paint(painter,option,index);
   else if (m_pChildChildDelegate && index.parent().parent().isValid())
      m_pChildChildDelegate->paint(painter,option,index);
} //paint

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

void CategorizedDelegate::drawSimpleCategory(const QModelIndex &index, int sortRole, const QStyleOption &option, QPainter *painter) const
{
   Q_UNUSED(sortRole)
   //Some theme have internal glow (such as Oxygen)
   static const int metric = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameVMargin)*2;

   const QString category = index.model()->data(index, Qt::DisplayRole).toString();
   painter->setClipping(false);
   static QPen pen(QApplication::palette().color(QPalette::Disabled,QPalette::Text));
   pen.setWidth(1);
   painter->setPen(pen);
   painter->setOpacity(0.3);
   const int topMargin2 = index.row()==0?metric:TOP_MARGIN;
   painter->drawLine(0,option.rect.y()+(topMargin2),option.rect.x()+option.rect.width()+option.rect.x(),option.rect.y()+(topMargin2));
   painter->setOpacity(1);
   painter->drawText(QRect(metric?1.5*metric:4,option.rect.y()+1+(topMargin2),option.rect.width(),option.rect.height()-1),Qt::AlignLeft | Qt::AlignTop,category);
}
