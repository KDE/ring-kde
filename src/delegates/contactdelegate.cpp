/****************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                          *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>          *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
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
#include "contactdelegate.h"

#include <QtGui/QPainter>
#include <QtGui/QApplication>
#include <QtGui/QBitmap>

#include <QtCore/QDebug>

#include <KLocale>
#include <KIcon>

#include <lib/contact.h>

ContactDelegate::ContactDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
}

QSize ContactDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
   QSize sh = QStyledItemDelegate::sizeHint(option, index);
   QFontMetrics fm(QApplication::font());
   Contact* ct = (Contact*)((ContactTreeBackend*)(index.internalPointer()))->getSelf();
   int lineHeight = fm.height()+2;
   int lines = ((!ct->getOrganization().isEmpty()) + (!ct->getPreferredEmail().isEmpty()))*lineHeight + 2*lineHeight;
   return QSize(sh.rwidth(),lines<52?52:lines);
}

void ContactDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   Q_ASSERT(index.isValid());
   
   if (option.state & QStyle::State_Selected || option.state & QStyle::State_MouseOver) {
      QStyleOptionViewItem opt2 = option;
      QPalette pal = option.palette;
      pal.setBrush(QPalette::Text,QColor(0,0,0,0));
      pal.setBrush(QPalette::HighlightedText,QColor(0,0,0,0));
      opt2.palette = pal;
      QStyledItemDelegate::paint(painter,opt2,index);
   }

   painter->setPen(QApplication::palette().color(QPalette::Active,(option.state & QStyle::State_Selected)?QPalette::HighlightedText:QPalette::Text));
   Contact* ct = (Contact*)((ContactTreeBackend*)(index.internalPointer()))->getSelf();
   if (ct->getPhoto()) {
      QPixmap pxm = *ct->getPhoto();
      QRect pxRect = pxm.rect();
      QBitmap mask(pxRect.size());
      QPainter customPainter(&mask);
      customPainter.setRenderHint  (QPainter::Antialiasing, true   );
      customPainter.fillRect       (pxRect                ,"white" );
      customPainter.setBackground  (QColor("black")                );
      customPainter.setBrush       (QColor("black")                );
      customPainter.drawRoundedRect(pxRect,5,5);
      pxm.setMask(mask);
      painter->drawPixmap(option.rect.x()+4,option.rect.y()+(option.rect.height()-48)/2,pxm);
   }
   else {
      painter->drawPixmap(option.rect.x()+4,option.rect.y()+(option.rect.height()-48)/2,QPixmap(KIcon("user-identity").pixmap(QSize(48,48))));
   }

   QFont font = painter->font();
   QFontMetrics fm(font);
   int currentHeight = option.rect.y()+fm.height()+2;
   font.setBold(true);
   painter->setFont(font);
   painter->drawText(option.rect.x()+15+48,currentHeight,index.data(Qt::DisplayRole).toString());
   currentHeight +=fm.height();
   font.setBold(false);
   painter->setFont(font);
   if (!ct->getOrganization().isEmpty()) {
      painter->drawText(option.rect.x()+15+48,currentHeight,ct->getOrganization());
      currentHeight +=fm.height();
   }
   if (!ct->getPreferredEmail().isEmpty()) {
      painter->drawText(option.rect.x()+15+48,currentHeight,ct->getPreferredEmail());
      currentHeight +=fm.height();
   }
   if (ct->getPhoneNumbers().size() == 1) {
      painter->drawText(option.rect.x()+15+48,currentHeight,ct->getPhoneNumbers()[0]->getNumber());
   }
   else {
      painter->drawText(option.rect.x()+15+48,currentHeight,QString::number(ct->getPhoneNumbers().size()) + i18n(" phone numbers"));
   }
}
