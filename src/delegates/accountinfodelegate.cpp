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

// Qt
#include <QtGui/QPainter>
#include <QtGui/QFontMetrics>
#include <QtCore/QDebug>
#include <QtWidgets/QApplication>

// Ring
#include <account.h>

// STD
#include <time.h>

AccountInfoDelegate::AccountInfoDelegate() : QStyledItemDelegate()
{}

void AccountInfoDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   // Use the right background color
   auto opt = option;
   opt.backgroundBrush = qvariant_cast<QBrush>(index.data(Qt::BackgroundRole));

   //Paint the background
   static QStyle* style = QApplication::style();
   style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, nullptr);

   // Draw the main text
   const QString display = index.data(Qt::DisplayRole).toString();

   // Usually, this will be equal to option.rect, but some themes may change it
   auto area = style->subElementRect(QStyle::SE_ItemViewItemText, &opt);

   static QFontMetrics m(painter->font());

   const int originalHeight = area.height()/2;
   const int fontHeight     = m.height();
   const int pad            = (originalHeight-fontHeight)/2;

   // Draw the first row
   const QRect firstRow(area.x(), area.y()+pad, area.width(), originalHeight);
   painter->drawText( firstRow, display );

   // Draw the second row
   const  QPen   currentPen  = painter->pen();
   static QColor secondColor = QApplication::palette().color(QPalette::Disabled,QPalette::Text);
   painter->setPen(secondColor);

   const QRect secondRow(area.x(), area.y()+pad+originalHeight, area.width(), originalHeight);
   painter->drawText( secondRow, index.data(9999).toString() );
   painter->setPen(currentPen);
}

QSize AccountInfoDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   const QSize s = QStyledItemDelegate::sizeHint(option, index);
   return QSize(s.width(),s.height()*2);
}

QVariant AccountIndoProxy::data( const QModelIndex& index, int role ) const
{
   if (index.isValid() && role == 9999) {
      const time_t t = qvariant_cast<time_t>(index.data((int)Account::Role::LastStatusChangeTimeStamp));

      time_t curTime;
      ::time(&curTime);
      const time_t nsec = curTime - t;

      if (nsec/3600)
         return QString("%1:%2:%3 ").arg((nsec%(3600*24))/3600).arg(((nsec%(3600*24))%3600)/60,2,10,QChar('0')).arg(((nsec%(3600*24))%3600)%60,2,10,QChar('0'));
      else
         return QString("%1:%2 ").arg(nsec/60,2,10,QChar('0')).arg(nsec%60,2,10,QChar('0'));
   }

   return PollingProxy::data(index, role);
}

//kate: space-indent on; indent-width 3; replace-tabs on;
