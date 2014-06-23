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
#include "contactdelegate.h"

//Qt
#include <QtGui/QPainter>
#include <QtGui/QApplication>
#include <QtGui/QBitmap>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QTreeView>
#include <QtGui/QPixmap>

//KDE
#include <KLocale>
#include <KStandardDirs>
#include <KIcon>

//SFLPhone
#include <lib/contact.h>
#include <lib/numbercategory.h>
#include <lib/phonenumber.h>
#include <lib/abstractitembackend.h>
#include <lib/contactmodel.h>
#include "delegatedropoverlay.h"
#include "lib/visitors/pixmapmanipulationvisitor.h"
#include "phonenumberdelegate.h"
#include "widgets/categorizedtreeview.h"
#include "klib/kcfg_settings.h"

//Macro
#define BG_STATE(____) ((QStyle::State_Selected | QStyle::State_MouseOver)&____)

//Metrics
namespace { //TODO GCC46 uncomment when dropping support for Gcc 4.6
   /*constexpr */static const int PX_HEIGHT    = 48                 ;
   /*constexpr */static const int PX_RADIUS    = 7                  ;
   /*constexpr */static const int PADDING      = 2                  ;
   /*constexpr */static const int MIN_HEIGHT   = PX_HEIGHT+2*PADDING;
   /*constexpr */static const int LEFT_PADDING = 15;
}


ContactDelegate::ContactDelegate(CategorizedTreeView* parent) : QStyledItemDelegate(parent),m_pDelegatedropoverlay(nullptr),
m_pChildDelegate(nullptr),m_pView(parent)
{
}

QSize ContactDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
   QSize sh = QStyledItemDelegate::sizeHint(option, index);
   const int rowCount = index.model()->rowCount(index);
   bool displayEmail = ConfigurationSkeleton::displayEmail();
   bool displayOrg   = ConfigurationSkeleton::displayOrganisation();
   Contact* ct = (Contact*)((CategorizedCompositeNode*)(static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(index).internalPointer())->getSelf();

   //Compute only once, the value is unlikely to change
   static QFontMetrics fm(QApplication::font());
   static const int lineHeight = fm.height()+2;

   int lines = ((displayOrg && !ct->organization().isEmpty()) + (displayEmail && !ct->preferredEmail().isEmpty()))*lineHeight + 2*lineHeight - ((ct->phoneNumbers().size()>0?1:0)*lineHeight);
   lines += lines==lineHeight?3:0; //Bottom margin for contact with only multiple phone numbers
   return QSize(sh.rwidth(),(lines+rowCount*lineHeight)<MIN_HEIGHT?MIN_HEIGHT:lines);
}

void ContactDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   Q_ASSERT(index.isValid());

   if (!m_pChildDelegate)
      return;

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

   //BEGIN draw photo
   QPixmap pxm = PixmapManipulationVisitor::instance()->contactPhoto(ct,QSize(PX_HEIGHT,PX_HEIGHT)).value<QPixmap>();
   painter->drawPixmap(option.rect.x()+4,option.rect.y()+(fullRect.height()-PX_HEIGHT)/2,pxm);
   //END draw photo


   //Config options
   bool displayEmail = ConfigurationSkeleton::displayEmail();
   bool displayOrg   = ConfigurationSkeleton::displayOrganisation();
   QFont font = painter->font();
   static QFontMetrics fm(font);
   static int fontH = fm.height();
   int currentHeight = option.rect.y()+fontH+2;

   //BEGIN display name
   font.setBold(true);
   painter->setFont(font);
   painter->drawText(option.rect.x()+LEFT_PADDING+PX_HEIGHT,currentHeight,index.data(Qt::DisplayRole).toString());
   currentHeight +=fontH;
   font.setBold(false);
   painter->setPen((option.state & QStyle::State_Selected)?Qt::white:QApplication::palette().color(QPalette::Disabled,QPalette::Text));
   painter->setFont(font);
   //END display name

   //BEGIN Display organization
   if (displayOrg && !ct->organization().isEmpty()) {
      painter->drawText(option.rect.x()+LEFT_PADDING+PX_HEIGHT,currentHeight,ct->organization());
      currentHeight +=fontH;
   }
   //END Display organization

   //BEGIN Display email
   if (displayEmail && !ct->preferredEmail().isEmpty()) {
      const int fmh = fontH;
      const static QPixmap* mail = nullptr;
      if (!mail)
         mail = new QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/mail.png"));
      painter->drawPixmap(option.rect.x()+LEFT_PADDING+PX_HEIGHT,currentHeight-12+(fmh-12),*mail);
      painter->drawText(option.rect.x()+2*LEFT_PADDING+PX_HEIGHT+1,currentHeight,ct->preferredEmail());
      currentHeight +=fmh;
   }
   //END Display email

   //BEGIN Display first phone number (if only one)
   if (ct->phoneNumbers().size() == 1) {
      painter->drawPixmap(option.rect.x()+LEFT_PADDING+PX_HEIGHT,currentHeight-12+(fontH-12),ct->phoneNumbers()[0]->category()->icon(
         ct->isTracked(),ct->isPresent()).value<QPixmap>());
      painter->drawText(option.rect.x()+2*LEFT_PADDING+PX_HEIGHT+1,currentHeight,ct->phoneNumbers()[0]->uri());
   }
   //END Display first phone number


   //BEGIN overlay path
   if (index.data(ContactModel::Role::DropState).toInt() != 0) {
      if (!m_pDelegatedropoverlay) {
         const_cast<ContactDelegate*>(this)->m_pDelegatedropoverlay = new DelegateDropOverlay((QObject*)this);
         static QMap<QString,DelegateDropOverlay::OverlayButton*> contactMap;
         contactMap.insert(i18n("Transfer")   ,new DelegateDropOverlay::OverlayButton(new QImage(KStandardDirs::locate("data","sflphone-client-kde/transferarrow.png")),Call::DropAction::Conference));
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

#undef BG_STATE
