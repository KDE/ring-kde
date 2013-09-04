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
#include "contactdelegate.h"

#include <QtGui/QPainter>
#include <QtGui/QApplication>
#include <QtGui/QBitmap>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QTreeView>

#include <QtCore/QDebug>

#include <KLocale>
#include <KStandardDirs>
#include <KIcon>

#include <lib/contact.h>
#include <lib/abstractcontactbackend.h>
#include "delegatedropoverlay.h"

ContactDelegate::ContactDelegate(QObject* parent) : QStyledItemDelegate(parent),m_pDelegatedropoverlay(nullptr)
{
}

QSize ContactDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
   QSize sh = QStyledItemDelegate::sizeHint(option, index);
   QFontMetrics fm(QApplication::font());
   Contact* ct = (Contact*)((ContactTreeBackend*)(static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(index).internalPointer())->self();
   int lineHeight = fm.height()+2;
   int lines = ((!ct->organization().isEmpty()) + (!ct->preferredEmail().isEmpty()))*lineHeight + 2*lineHeight;
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
   Contact* ct = (Contact*)((ContactTreeBackend*)((static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(index).internalPointer()))->self();
   if (ct->photo()) {
      QPixmap pxm = *ct->photo();
      const QRect pxRect = pxm.rect();
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
   if (!ct->organization().isEmpty()) {
      painter->drawText(option.rect.x()+15+48,currentHeight,ct->organization());
      currentHeight +=fm.height();
   }
   if (!ct->preferredEmail().isEmpty()) {
      painter->drawText(option.rect.x()+15+48,currentHeight,ct->preferredEmail());
      currentHeight +=fm.height();
   }
   if (ct->phoneNumbers().size() == 1) {
      painter->drawText(option.rect.x()+15+48,currentHeight,ct->phoneNumbers()[0]->number());
   }
   else {
      painter->drawText(option.rect.x()+15+48,currentHeight,i18np("%1 phone number", "%1 phone numbers", QString::number(ct->phoneNumbers().size())));
   }

   //BEGIN overlay path
   if (index.data(AbstractContactBackend::Role::DropState).toInt() != 0) {
      if (!m_pDelegatedropoverlay) {
         const_cast<ContactDelegate*>(this)->m_pDelegatedropoverlay = new DelegateDropOverlay((QObject*)this);
         static QMap<QString,DelegateDropOverlay::OverlayButton> contactMap;
         contactMap.insert(i18n("Transfer")   ,{new QImage(KStandardDirs::locate("data","sflphone-client-kde/transferarrow.png")),Call::DropAction::Conference});
         m_pDelegatedropoverlay->setButtons(&contactMap);
      }
      m_pDelegatedropoverlay->paintEvent(painter, option, index);
   }
   //END overlay path
}

// This would be nice if it worked, but it doesn't work so well. The other code path for this is far from perfect, but is a little bit more reliable
// void ContactDelegateStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const {
//    if (widget) {
//       QPoint point(option->rect.x(),option->rect.y());
//       point.ry()+=1;
//       QModelIndex index = static_cast<const QTreeView*>(widget)->indexAt(point);
//       if (element != 31 && element == QStyle::PE_IndicatorItemViewItemDrop) {
//          qDebug() << "\n\n\nCAST WORK" << index.isValid() << element << index.parent().isValid() << index.parent().parent().isValid() << index.data(Qt::DisplayRole) << point << QCursor::pos();
//          qDebug() << index << m_CurrentIndex << index.data(ContactBackend::Role::DropState).toInt();
//       }
//       if ((element != 31) && index.isValid() && index.parent().isValid() && !index.parent().parent().isValid()) {
//          int current = (element == 31)?0:index.data(ContactBackend::Role::DropState).toInt();//It crash on 0xFE, I don't know why
//          if (m_CurrentIndex.isValid() && index != m_CurrentIndex) {
//             qDebug() << "DROP LEAVE";
//             ((QAbstractItemModel*)m_CurrentIndex.model())->setData(m_CurrentIndex,-1,ContactBackend::Role::DropState);
// //             ((ContactDelegateStyle*)this)->m_CurrentIndex = QModelIndex();
//          }
//          if (element == QStyle::PE_IndicatorItemViewItemDrop && index != m_CurrentIndex) {
//             ((QAbstractItemModel*)index.model())->setData(index,1,ContactBackend::Role::DropState);
//             ((ContactDelegateStyle*)this)->m_CurrentIndex = index;
//             qDebug() << "DROP" << current << index.data(ContactBackend::Role::DropState).toInt() << ((ContactDelegateStyle*)this)->m_CurrentIndex;
//          }
//       }
//    }
//    QProxyStyle::drawPrimitive(element, option, painter, widget);
// }
