/****************************************************************************
 *   Copyright (C) 2016 by Emmanuel Lepage Vallee                           *
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
#include "accountinfodelegate.h"

#include <QtGui/QPainter>
#include <QtGui/QFontMetrics>

AccountInfoDelegate::AccountInfoDelegate() : QStyledItemDelegate()
{

}

void AccountInfoDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    painter->setPen(Qt::transparent);

   //Paint the background
   QStyledItemDelegate::paint(painter, option, index);

   const QRect r = QRect(
       option.rect.x(), option.rect.y(),
                         option.rect.width(), option.rect.height()
   );

   painter->drawText(
      r,""
   );
}

QSize AccountInfoDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   const QSize s = QStyledItemDelegate::sizeHint(option, index);
   return QSize(s.width(),s.height()*2);
}
