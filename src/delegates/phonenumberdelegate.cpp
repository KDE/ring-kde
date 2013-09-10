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
#include <QtCore/QDebug>

//KDE
#include <KLocale>
#include <KIcon>
#include <KStandardDirs>

//SFLPhone
#include <lib/contact.h>

QHash<QString,QPixmap> PhoneNumberDelegate::m_hIcons;

PhoneNumberDelegate::PhoneNumberDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
   if (!m_hIcons.size()) {
      m_hIcons["Home"  ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/home.png"));
      m_hIcons["Work"  ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/work.png"));
      m_hIcons["Msg"   ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/mail.png"));
      m_hIcons["Pref"  ] = QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/preferred.png"));
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
   return QSize(QStyledItemDelegate::sizeHint(option, index).rwidth(),fm.height() + 6);
}

void PhoneNumberDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
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

   Contact::PhoneNumber* nb = ((Contact*) ((ContactTreeBackend*)((static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(index).internalPointer()))->self())->phoneNumbers()[index.row()];
   QFontMetrics fm(painter->font());
   painter->setPen(QApplication::palette().color(QPalette::Disabled,QPalette::Text));
   painter->drawText(option.rect.x()+option.rect.width()-fm.width(nb->type().trimmed())-7/*padding*/,option.rect.y()+fm.height()+3,nb->type());
   painter->setPen(QApplication::palette().color(QPalette::Active,(option.state & QStyle::State_Selected)?QPalette::HighlightedText:QPalette::Text));
   painter->drawText(option.rect.x()+3+22,option.rect.y()+fm.height()+3,nb->number());
   if (m_hIcons.contains(nb->type() )) {
         const int fmh = fm.height();
         painter->drawPixmap(option.rect.x()+3,option.rect.y()+fmh-12+3,m_hIcons[nb->type()]);
   }
}
