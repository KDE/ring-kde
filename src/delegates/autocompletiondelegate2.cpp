/****************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                          *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                    *
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
#include "autocompletiondelegate2.h"

#include <QtGui/QPainter>
#include <QtGui/QFontMetrics>
#include <QtWidgets/QApplication>

#include <numbercompletionmodel.h>

static const int RIGHT_MARGIN = 2;

AutoCompletionDelegate2::AutoCompletionDelegate2() : QStyledItemDelegate()
{

}

void AutoCompletionDelegate2::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   //Paint the background
   QStyledItemDelegate::paint(painter,option,QModelIndex());

   //Draw the icon
   const QRect pixmapRect(option.rect.x()+2,option.rect.y()+2,option.rect.height()/2-4,option.rect.height()/2-4);
   if (index.data(Qt::DecorationRole).canConvert<QIcon>()) {
      const QIcon icn = index.data(Qt::DecorationRole).value<QIcon>();
      const QList<QSize> sizes = icn.availableSizes();
      if (sizes.size()) {
         const QRect r = (sizes[0].height() > pixmapRect.size().height()) ? pixmapRect : QRect(0,0,sizes[0].width(),sizes[0].height());
         painter->drawPixmap(r, icn.pixmap(r.size()));
      }
   }
   else if(index.data(Qt::DecorationRole).canConvert<QPixmap>()) {
      const QPixmap pm = index.data(Qt::DecorationRole).value<QPixmap>();
      painter->drawPixmap(pm.rect().height() > pixmapRect.height() ?
         pixmapRect : QRect(option.rect.x()+2, option.rect.y()+2, pm.rect().width(), pm.rect().height()), pm
      );
   }

   //Draw the contact method number
   painter->drawText(
      QRect(pixmapRect.x()+pixmapRect.width()+2,option.rect.y()+2,option.rect.width()-pixmapRect.x()+pixmapRect.width()+2,option.rect.height()/2),
      index.data(Qt::DisplayRole).toString()
   );

   //Draw the peer name
   painter->save();
   painter->setPen((option.state & QStyle::State_Selected)?Qt::white:QApplication::palette().color(QPalette::Disabled,QPalette::Text));
   painter->drawText(
      QRect(pixmapRect.x()+pixmapRect.width()+2,option.rect.y()+2+option.rect.height()/2,option.rect.width()-pixmapRect.x()+pixmapRect.width()+2,option.rect.height()/2),
      index.data(NumberCompletionModel::Role::PEER_NAME).toString()
   );
   painter->restore();

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
      int rectHeight = option.rect.height();
      int leftOver = (rectHeight-height);
      int padding   = leftOver/2 >= 4 ? 2: 0;
      int topMargin = (leftOver-2*padding)/2;
      painter->setFont(f);
      const int width = m.width(tag);
      const int x = option.rect.x()+option.rect.width()-width-RIGHT_MARGIN-rectHeight/2;
      painter->setBrush(g);
      painter->setPen(Qt::NoPen);
      QRect rect(x,
         option.rect.y()+topMargin,
         width,
         height+2*padding);
      painter->drawRect(rect);
      painter->setClipRect(x-rectHeight/2,option.rect.y(),rectHeight/2,rectHeight);
      painter->drawEllipse(QRect(
         x-(height+2*padding)/2,
         option.rect.y()+topMargin,
         (height+2*padding),
         (height+2*padding)));
      painter->setClipRect(x+width,option.rect.y(),option.rect.height(),option.rect.height());
      painter->drawEllipse(QRect(
         x+width-(height+2*padding)/2,
         option.rect.y()+topMargin,
         (height+2*padding),
         (height+2*padding)));
      painter->setPen(QApplication::palette().base().color());
      painter->setClipping(false);
      painter->drawText(rect,Qt::AlignHCenter|Qt::AlignVCenter,tag);
      painter->restore();
   }
}

QSize AutoCompletionDelegate2::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   const QSize s = QStyledItemDelegate::sizeHint(option, index);
   return QSize(s.width(),s.height()*2);
}

// kate: space-indent on; indent-width 3; replace-tabs on;
