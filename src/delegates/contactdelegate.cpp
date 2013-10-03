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
#include <lib/phonenumber.h>
#include <lib/abstractcontactbackend.h>
#include "delegatedropoverlay.h"

namespace { //TODO GCC46 uncomment when dropping support for Gcc 4.6
   /*constexpr */static const int PX_HEIGHT  = 48                 ;
   /*constexpr */static const int PX_RADIUS  = 7                  ;
   /*constexpr */static const int PADDING    = 2                  ;
   /*constexpr */static const int MIN_HEIGHT = PX_HEIGHT+2*PADDING;
}

QHash<QString,QPixmap> ContactDelegate::m_hIcons;

ContactDelegate::ContactDelegate(QObject* parent) : QStyledItemDelegate(parent),m_pDelegatedropoverlay(nullptr),
m_pChildDelegate(nullptr)
{
   if (!m_hIcons.size()) {
      m_hIcons["Home"       ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/home.png"));
      m_hIcons["Work"       ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/work.png"));
      m_hIcons["Msg"        ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/mail.png"));
      m_hIcons["Preferred"  ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/preferred.png"));
      m_hIcons["Voice"      ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/mail.png"));
      m_hIcons["Fax"        ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/mail.png"));
      m_hIcons["Cell"       ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/mobile.png"));
      m_hIcons["Video"      ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/video.png"));
      m_hIcons["Bbs"        ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/mail.png"));
      m_hIcons["Modem"      ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/mail.png"));
      m_hIcons["Car"        ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/cat.png"));
      m_hIcons["Isdn"       ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/mail.png"));
      m_hIcons["Pcs"        ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/mail.png"));
      m_hIcons["Pager"      ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/pager.png"));
   }
}

QSize ContactDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
   QSize sh = QStyledItemDelegate::sizeHint(option, index);
   QFontMetrics fm(QApplication::font());
   const int rowCount = index.model()->rowCount(index);
   Contact* ct = (Contact*)((CategorizedCompositeNode*)(static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(index).internalPointer())->getSelf();
   static const int lineHeight = fm.height()+2;
   int lines = ((!ct->organization().isEmpty()) + (!ct->preferredEmail().isEmpty()))*lineHeight + 2*lineHeight - ((ct->phoneNumbers().size()>0)*lineHeight);
   lines += lines==lineHeight?3:0; //Bottom margin for contact with only multiple phone numbers
   return QSize(sh.rwidth(),(lines+rowCount*lineHeight)<MIN_HEIGHT?MIN_HEIGHT:lines);
}

void ContactDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   Q_ASSERT(index.isValid());

//    static const int metric = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameVMargin)*2;
   QRect fullRect = option.rect;

   //BEGIN is selected
   {
      QStyleOptionViewItem opt2 = option;
      if (index.model()->rowCount(index) && m_pChildDelegate) {
         fullRect.setHeight(fullRect.height()+index.model()->rowCount(index)*(m_pChildDelegate->sizeHint(option,index.child(0,0)).height()));
         opt2.rect = fullRect;
         painter->setClipRect(fullRect);
         //Clear the area (because of all the dirty little hacks)
         painter->fillRect(fullRect,QApplication::palette().color(QPalette::Base));
      }

      if (option.state & QStyle::State_Selected || option.state & QStyle::State_MouseOver) {
         QPalette pal = option.palette;
         pal.setBrush(QPalette::Text,QColor(0,0,0,0));
         pal.setBrush(QPalette::HighlightedText,QColor(0,0,0,0));
         opt2.palette = pal;
         QStyledItemDelegate::paint(painter,opt2,index);
      }
   }
   //END is selected


   painter->setPen(QApplication::palette().color(QPalette::Active,(option.state & QStyle::State_Selected)?QPalette::HighlightedText:QPalette::Text));
   Contact* ct = (Contact*)((CategorizedCompositeNode*)((static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(index).internalPointer()))->getSelf();
   if (ct->photo()) {
      QPixmap pxm = *ct->photo();
      const QRect pxRect = pxm.rect();
      QBitmap mask(pxRect.size());
      QPainter customPainter(&mask);
      customPainter.setRenderHint  (QPainter::Antialiasing, true   );
      customPainter.fillRect       (pxRect                ,"white" );
      customPainter.setBackground  (QColor("black")                );
      customPainter.setBrush       (QColor("black")                );
      customPainter.drawRoundedRect(pxRect,PX_RADIUS,PX_RADIUS);
      pxm.setMask(mask);
      painter->save();
      painter->drawPixmap(option.rect.x()+4,option.rect.y()+(fullRect.height()-PX_HEIGHT)/2,pxm);
      painter->setBrush(Qt::NoBrush);
      QPen pen(QApplication::palette().color(QPalette::Disabled,QPalette::Text));
      pen.setWidth(1);
      painter->setPen(pen);
      painter->setRenderHint  (QPainter::Antialiasing, true   );
      painter->drawRoundedRect(option.rect.x()+4,option.rect.y()+(fullRect.height()-PX_HEIGHT)/2,pxRect.width(),pxRect.height(),PX_RADIUS,PX_RADIUS);
      painter->restore();
      
   }
   else {
      painter->drawPixmap(option.rect.x()+4,option.rect.y()+(fullRect.height()-PX_HEIGHT)/2,QPixmap(KIcon("user-identity").pixmap(QSize(PX_HEIGHT,PX_HEIGHT))));
   }

   QFont font = painter->font();
   QFontMetrics fm(font);
   int currentHeight = option.rect.y()+fm.height()+2;
   font.setBold(true);
   painter->setFont(font);
   painter->drawText(option.rect.x()+15+PX_HEIGHT,currentHeight,index.data(Qt::DisplayRole).toString());
   currentHeight +=fm.height();
   font.setBold(false);
   painter->setPen((option.state & QStyle::State_Selected)?Qt::white:QApplication::palette().color(QPalette::Disabled,QPalette::Text));
   painter->setFont(font);
   if (!ct->organization().isEmpty()) {
      painter->drawText(option.rect.x()+15+PX_HEIGHT,currentHeight,ct->organization());
      currentHeight +=fm.height();
   }
   if (!ct->preferredEmail().isEmpty()) {
      const int fmh = fm.height();
      const static QPixmap* mail = nullptr;
      if (!mail)
         mail = new QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/mail.png"));
      painter->drawPixmap(option.rect.x()+15+PX_HEIGHT,currentHeight-12+(fmh-12),*mail);
      painter->drawText(option.rect.x()+15+PX_HEIGHT+16,currentHeight,ct->preferredEmail());
      currentHeight +=fmh;
   }
   if (ct->phoneNumbers().size() == 1) {
      static const int fmh = fm.height();
      if (m_hIcons.contains(ct->phoneNumbers()[0]->type() )) {
         painter->drawPixmap(option.rect.x()+15+PX_HEIGHT,currentHeight-(fmh)+4,m_hIcons[ct->phoneNumbers()[0]->type()]);
      }
      else {
         const static QPixmap* callPxm = nullptr;
         if (!callPxm)
            callPxm = new QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/call.png"));
         painter->drawPixmap(option.rect.x()+15+PX_HEIGHT,currentHeight-12+(fmh-12),*callPxm);
      }
      painter->drawText(option.rect.x()+15+PX_HEIGHT+16,currentHeight,ct->phoneNumbers()[0]->uri());
   }
   else {
//       painter->drawText(option.rect.x()+15+PX_HEIGHT,currentHeight,i18np("%1 phone number", "%1 phone numbers", QString::number(ct->phoneNumbers().size())));
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

void ContactDelegate::setChildDelegate(QStyledItemDelegate* child)
{
   m_pChildDelegate = child;
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
