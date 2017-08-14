/****************************************************************************
 *   Copyright (C) 2015 by Savoir-Faire Linux                               *
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
#include "toolbardelegate.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QApplication>
#include <QtGui/QPainter>
#include <QtWidgets/QStylePainter>
#include <QtCore/QDebug>

#include <cmath>

ToolbarDelegate::ToolbarDelegate(QWidget* parent) : QStyledItemDelegate(parent),m_pParent(parent)
{

}

void ToolbarDelegate::paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
   Q_ASSERT(index.isValid());
   Q_UNUSED(painter)

   //Init
   static const QColor hoverBg = QApplication::palette().highlight().color();
   static const QColor textCol = QColor(0xF2, 0xF2, 0xF2); //Same as the icons
   static const int    fmh     = option.fontMetrics.height();
   static       QPen   checked ( hoverBg );
   static bool init = false;

   if (!init) {
      checked.setWidth(3);
      init = true;
   }

   painter->setRenderHint(QPainter::Antialiasing,true);

   //Background
   if (option.state & QStyle::State_MouseOver) {
      painter->setPen(Qt::transparent);
      painter->setBrush(hoverBg);
      painter->drawRoundedRect(option.rect,7,7);
   }

   if (option.checkState == Qt::Checked) {
      painter->setBrush(Qt::transparent);
      painter->setPen(hoverBg);
      painter->drawRoundedRect(option.rect,7,7);
   }

   //Text
   const QString text = index.data(Qt::DisplayRole).toString();
   const float w = option.fontMetrics.width(text)+1;
   int x = std::ceil(option.rect.x()+option.rect.width()/2-w/2);
   int y = option.rect.y()+option.rect.height()-5-fmh;
   painter->setPen(textCol);
   painter->drawText(QRect(x,y,w,fmh),text);

   //Icon
   const QPixmap pxm = index.data(Qt::DecorationRole).value<QIcon>().pixmap(24,24);
   x = option.rect.x()+option.rect.width()/2- 24/2;
   y = option.rect.y()+5;
   painter->drawPixmap(QRect(x,y,24,24),pxm);
}

ToolbarDelegate::~ToolbarDelegate()
{
}

// kate: space-indent on; indent-width 3; replace-tabs on;
