/***************************************************************************
 *   Copyright (C) 2015 by Savoir-Faire Linux                              *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/
#include "righticondelegate.h"

#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtCore/QDebug>

RightIconDelegate::RightIconDelegate(QObject* parent, int role, float opacity) : QStyledItemDelegate(parent),
m_Opacity(opacity), m_Role(role)
{

}

void RightIconDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   QStyledItemDelegate::paint(painter, option, index);

   QPixmap px = index.data(m_Role).value<QPixmap>();

   if (px.isNull()) {
      const QIcon icn = index.data(m_Role).value<QIcon>();

      if (!icn.isNull()) //TODO do not hardcode margins
         px = icn.pixmap(QSize(option.rect.height()-2, option.rect.height()-2));
   }

   const float op = painter->opacity();
   painter->setOpacity(m_Opacity);

   if (!px.isNull())
      painter->drawPixmap(QRect(
         option.rect.x()+option.rect.width()-option.rect.height() - 1,
         option.rect.y() + 1,
         option.rect.height() - 2,
         option.rect.height() - 2
      ), px);
   painter->setOpacity(op);
}
