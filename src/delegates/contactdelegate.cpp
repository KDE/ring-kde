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
#include <QtGui/QPixmap>

#include <QtCore/QDebug>

#include <KLocale>
#include <KStandardDirs>
#include <KIcon>

#include <lib/contact.h>
#include <lib/numbercategory.h>
#include <lib/phonenumber.h>
#include <lib/abstractcontactbackend.h>
#include "delegatedropoverlay.h"
#include "lib/visitors/pixmapmanipulationvisitor.h"
#include "phonenumberdelegate.h"
#include "widgets/categorizedtreeview.h"
#include "klib/kcfg_settings.h"

#define BG_STATE(____) ((QStyle::State_Selected | QStyle::State_MouseOver)&____)

namespace { //TODO GCC46 uncomment when dropping support for Gcc 4.6
   /*constexpr */static const int PX_HEIGHT  = 48                 ;
   /*constexpr */static const int PX_RADIUS  = 7                  ;
   /*constexpr */static const int PADDING    = 2                  ;
   /*constexpr */static const int MIN_HEIGHT = PX_HEIGHT+2*PADDING;
}


ContactDelegate::ContactDelegate(CategorizedTreeView* parent) : QStyledItemDelegate(parent),m_pDelegatedropoverlay(nullptr),
m_pChildDelegate(nullptr),m_pView(parent)
{
}

QSize ContactDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
   QSize sh = QStyledItemDelegate::sizeHint(option, index);
   QFontMetrics fm(QApplication::font());
   const int rowCount = index.model()->rowCount(index);
   static bool displayEmail = ConfigurationSkeleton::displayEmail();
   static bool displayOrg   = ConfigurationSkeleton::displayOrganisation();
   Contact* ct = (Contact*)((CategorizedCompositeNode*)(static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(index).internalPointer())->getSelf();
   static const int lineHeight = fm.height()+2;
   int lines = ((displayOrg && !ct->organization().isEmpty()) + (displayEmail && !ct->preferredEmail().isEmpty()))*lineHeight + 2*lineHeight - ((ct->phoneNumbers().size()>0)*lineHeight);
   lines += lines==lineHeight?3:0; //Bottom margin for contact with only multiple phone numbers
   return QSize(sh.rwidth(),(lines+rowCount*lineHeight)<MIN_HEIGHT?MIN_HEIGHT:lines);
}

void ContactDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   Q_ASSERT(index.isValid());

   QRect fullRect = option.rect;
   fullRect.setHeight(fullRect.height()+index.model()->rowCount(index)*(m_pChildDelegate->sizeHint(option,index.child(0,0)).height()));

   //HACK There is two reliable way to set hover state, both are hacks. One is to intercept
   //the view events using an eventFilter delegate and the other is this:
   //The BG_STATE macro limit the number of redraw associated with irrelevant states
   //setDirty force the phone number to redraw themselves as they need up upgrade their own
   //background to match this one
   CategorizedCompositeNode* modelItem = (CategorizedCompositeNode*)static_cast<const QSortFilterProxyModel*>(index.model())->mapToSource(index).internalPointer();
   if (modelItem && BG_STATE(modelItem->hoverState()) != BG_STATE(option.state)) {
      modelItem->setHoverState(option.state);
      m_pView->setDirty(QRect(option.rect.x(),option.rect.y()+option.rect.height(),option.rect.width(),(fullRect.height()-option.rect.height())));
   }

   painter->setClipRect(fullRect);
   Contact* ct = (Contact*)((CategorizedCompositeNode*)((static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(index).internalPointer()))->getSelf();

   //BEGIN is selected
   {
      if (index.model()->rowCount(index) && m_pChildDelegate) {
         //Clear the area (because of all the dirty little hacks)
         painter->fillRect(fullRect,QApplication::palette().color(QPalette::Base));
      }

      if (option.state & QStyle::State_Selected || option.state & QStyle::State_MouseOver) {
         QStyleOptionViewItem opt2 = option;
         opt2.rect = fullRect;
         QPalette pal = option.palette;
         pal.setBrush(QPalette::Text,QColor(0,0,0,0));
         pal.setBrush(QPalette::HighlightedText,QColor(0,0,0,0));
         opt2.palette = pal;
         QStyledItemDelegate::paint(painter,opt2,index);
      }
   }
   //END is selected


   painter->setPen(QApplication::palette().color(QPalette::Active,(option.state & QStyle::State_Selected)?QPalette::HighlightedText:QPalette::Text));

   QPixmap pxm = PixmapManipulationVisitor::instance()->contactPhoto(ct,QSize(PX_HEIGHT,PX_HEIGHT)).value<QPixmap>();
   painter->drawPixmap(option.rect.x()+4,option.rect.y()+(fullRect.height()-PX_HEIGHT)/2,pxm);


   //Config options
   //Load them only once, it is a "bug", but it is faster to ignore a very, very
   //rare event
   static bool displayEmail = ConfigurationSkeleton::displayEmail();
   static bool displayOrg   = ConfigurationSkeleton::displayOrganisation();

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
   if (displayOrg && !ct->organization().isEmpty()) {
      painter->drawText(option.rect.x()+15+PX_HEIGHT,currentHeight,ct->organization());
      currentHeight +=fm.height();
   }
   if (displayEmail && !ct->preferredEmail().isEmpty()) {
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
      painter->drawPixmap(option.rect.x()+15+PX_HEIGHT,currentHeight-12+(fmh-12),ct->phoneNumbers()[0]->category()->icon(
         ct->isTracked(),ct->isPresent()).value<QPixmap>());
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

void ContactDelegate::setChildDelegate(PhoneNumberDelegate* child)
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
#undef BG_STATE