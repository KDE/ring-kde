/****************************************************************************
 *   Copyright (C) 2013 by Savoir-Faire Linux                               *
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
#include "autocompletiondelegate.h"

#include <QtGui/QPainter>
#include <QtGui/QFontMetrics>
#include <QtGui/QApplication>

#include <lib/numbercompletionmodel.h>

static const int RIGHT_MARGIN = 2;

AutoCompletionDelegate::AutoCompletionDelegate() : QStyledItemDelegate()
{
   
}

void AutoCompletionDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   QStyledItemDelegate::paint(painter,option,index);
   const QString tag = index.data(Qt::UserRole).toString();
   if (!tag.isEmpty()) {
      painter->save();
      painter->setRenderHint(QPainter::Antialiasing, true);
      painter->setClipRect(option.rect);
      static bool init = false;
      static QFont f;
      static QLinearGradient g;
      if (!init) {
         f = painter->font();
         f.setBold(true);
         f.setPointSize(f.pointSize()-1);
         g = QLinearGradient(QPointF(0, 0), QPointF(0, option.rect.height()-4));
         QColor col = QApplication::palette().color(QPalette::Highlight);
         col.setAlpha(125);
         g.setColorAt(0, col);
         col.setAlpha(175);
         g.setColorAt(1, col);
         init = true;
      }
      static QFontMetrics m(f);
      static int height    = m.height();
      static int topMargin = 2;
      painter->setFont(f);
      const int width = m.width(tag);
      const int x = option.rect.x()+option.rect.width()-width-RIGHT_MARGIN-option.rect.height()/2;
      painter->setBrush(g);
      painter->setPen(Qt::NoPen);
      painter->drawRect(QRect(x-option.rect.height()/2+option.rect.height()/2,option.rect.y()+2,width,option.rect.height()-4));
      painter->setClipRect(x-option.rect.height()/2,option.rect.y()+2,option.rect.height()/2,option.rect.height()-4);
      painter->drawEllipse(QRect(x-option.rect.height()/2,option.rect.y()+2,option.rect.height()-4,option.rect.height()-4));
      painter->setClipRect(x+width,option.rect.y()+2,option.rect.height()-4,option.rect.height()-4);
      painter->drawEllipse(QRect(x+width-option.rect.height()/2,option.rect.y()+2,option.rect.height()-4,option.rect.height()-4));
      painter->setPen(QApplication::palette().base().color());
      painter->setClipping(false);
      painter->drawText(x,option.rect.y()+height-topMargin,tag);
      painter->restore();
   }
}
