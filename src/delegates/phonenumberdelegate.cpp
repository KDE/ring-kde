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
#include "phonenumberdelegate.h"

//Qt
#include <QtGui/QPainter>
#include <QtGui/QApplication>
#include <QtGui/QBitmap>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QTreeView>
#include <QtGui/QPixmap>
#include <QtCore/QDebug>
#include <QtCore/QVariant>

//KDE
#include <KLocale>
#include <KIcon>
#include <KStandardDirs>

//SFLPhone
#include <lib/contact.h>
#include <lib/numbercategory.h>
#include <lib/phonenumber.h>
#include <lib/abstractitembackend.h>
#include "widgets/categorizedtreeview.h"

PhoneNumberDelegate::PhoneNumberDelegate(QObject* parent) : QStyledItemDelegate(parent),m_pView(nullptr),m_Lock(false)
{
}

QSize PhoneNumberDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
   QFontMetrics fm(QApplication::font());

   //This indentation is wrong, if set manually, Qt will override it, it needs to be done here
   QStyleOptionViewItem opt = option;
   opt.rect.setX((opt.rect.x()+48+6));
   opt.rect.setWidth(opt.rect.width()-48-6);

   return QSize(QStyledItemDelegate::sizeHint(opt, index).rwidth(),fm.height());
}

void PhoneNumberDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   Q_ASSERT(index.isValid());

   //This indentation is wrong, if set manually, Qt will override it, it needs to be done here
   QStyleOptionViewItem opt = option;
   opt.rect.setX((opt.rect.x()+48+6));
   painter->setClipRect(opt.rect);

   //Draw the parent and all it's childs. This is not optimal, but dealing with
   //Dirty regions from a delegate is worst
   //BEGIN repaint parent
   if (!m_Lock) {
      painter->save();
      const QModelIndex parent = index.parent();
      const QRect parentRect = m_pView->visualRect(parent);
      QStyleOptionViewItem parOpt;
      parOpt.rect = parentRect;
      //Set if the item is selected, active, hover and focus
      CategorizedCompositeNode* modelItem = ((CategorizedCompositeNode*)((static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(parent).internalPointer()));
      parOpt.state = (QStyle::State)modelItem->hoverState();

      m_pView->itemDelegate()->paint(painter,parOpt,parent);
      painter->restore();

      //Now repaint ALL numbers, including the current one
      const_cast<PhoneNumberDelegate*>(this)->m_Lock = true;
      for (int i=0;i<m_pView->model()->rowCount(parent);i++) {
         const QModelIndex numIdx = m_pView->model()->index(i,0,parent);
         QStyleOptionViewItem opt3 = option;
         if (numIdx != index) {
            opt3.state &= ~(QStyle::State_Selected | QStyle::State_MouseOver);
            opt3.state |= (m_pView->selectionModel()->currentIndex()==numIdx?QStyle::State_Selected: QStyle::State_None);
         }
         opt3.rect = m_pView->visualRect(numIdx);
         opt3.rect.setX(m_pView->indentation()*2);
         paint(painter,opt3,numIdx);
      }
      const_cast<PhoneNumberDelegate*>(this)->m_Lock = false;
      return;
   }
   //END repaint parent

   if (opt.state & QStyle::State_Selected || opt.state & QStyle::State_MouseOver) {
      QStyleOptionViewItem opt2 = opt;
      QPalette pal = opt.palette;
      pal.setBrush(QPalette::Text,QColor(0,0,0,0));
      pal.setBrush(QPalette::HighlightedText,QColor(0,0,0,0));
      opt2.palette = pal;
      QStyledItemDelegate::paint(painter,opt2,index);
   }

   static const int metric = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameVMargin)*2;
   const PhoneNumber* nb = ((Contact*) ((CategorizedCompositeNode*)((static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(index).internalPointer()))->getSelf())->phoneNumbers()[index.row()];
   const QFontMetrics fm(painter->font());
   painter->setPen(((opt.state & QStyle::State_Selected) || (m_pView && m_pView->selectionModel()->isSelected(index.parent())))?
      Qt::white:QApplication::palette().color(QPalette::Disabled,QPalette::Text));
   const int fmh = fm.height();
   painter->drawText(opt.rect.x()+3+16,opt.rect.y()+fmh-metric,nb->uri());
   painter->drawPixmap(opt.rect.x()+3,opt.rect.y()+fmh-9-metric,nb->category()->icon(nb->isTracked(),nb->isPresent()).value<QPixmap>());
}

void PhoneNumberDelegate::setView(CategorizedTreeView* model)
{
   m_pView = model;
}
