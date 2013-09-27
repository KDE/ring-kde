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
#include "phonenumberdelegate.h"

//Qt
#include <QtGui/QPainter>
#include <QtGui/QApplication>
#include <QtGui/QBitmap>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QTreeView>
#include <QtCore/QDebug>

//KDE
#include <KLocale>
#include <KIcon>
#include <KStandardDirs>

//SFLPhone
#include <lib/contact.h>
#include <lib/phonenumber.h>
#include <lib/abstractcontactbackend.h>

QHash<QString,QPixmap> PhoneNumberDelegate::m_hIcons;

PhoneNumberDelegate::PhoneNumberDelegate(QObject* parent) : QStyledItemDelegate(parent),m_pView(nullptr),m_Lock(false)
{
   if (!m_hIcons.size()) {
      m_hIcons["Home"  ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/home.png"));
      m_hIcons["Work"  ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/work.png"));
      m_hIcons["Msg"   ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/mail.png"));
      m_hIcons["Preferred"] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/preferred.png"));
      m_hIcons["Voice" ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/mail.png"));
      m_hIcons["Fax"   ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/mail.png"));
      m_hIcons["Cell"  ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/mobile.png"));
      m_hIcons["Video" ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/video.png"));
      m_hIcons["Bbs"   ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/mail.png"));
      m_hIcons["Modem" ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/mail.png"));
      m_hIcons["Car"   ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/cat.png"));
      m_hIcons["Isdn"  ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/mail.png"));
      m_hIcons["Pcs"   ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/mail.png"));
      m_hIcons["Pager" ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/pager.png"));
   }
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

   //Draw the parent in the clip
   //BEGIN repaint parent
   if (!m_Lock) {
      painter->save();
      const QModelIndex parent = index.parent();
      const QRect parentRect = m_pView->visualRect(parent);
      QStyleOptionViewItem parOpt;
      parOpt.rect = parentRect;
      //Set if the item is selected, active, hover and focus
      parOpt.state |= QStyle::State_Enabled | (m_pView->selectionModel()->currentIndex()==parent?QStyle::State_Selected: QStyle::State_None);
      parOpt.state |= m_pView->hasFocus()?QStyle::State_HasFocus|QStyle::State_Active:QStyle::State_None;
      parOpt.state |= (parent.data(AbstractContactBackend::Role::HoverState).toInt()==true)?
         opt.state & QStyle::State_MouseOver:QStyle::State_None;
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
   painter->drawText(opt.rect.x()+3+16,opt.rect.y()+fm.height()-metric,nb->uri());
   const int fmh = fm.height();
   if (m_hIcons.contains(nb->type() )) {
      painter->drawPixmap(opt.rect.x()+3,opt.rect.y()+fmh-12-metric,m_hIcons[nb->type()]);
   }
   else {
      const static QPixmap* callPxm = nullptr;
      if (!callPxm)
         callPxm = new QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/call.png"));
      painter->drawPixmap(opt.rect.x()+3,opt.rect.y()+fmh-12-metric,*callPxm);
   }
}

void PhoneNumberDelegate::setView(QTreeView* model)
{
   m_pView = model;
}

