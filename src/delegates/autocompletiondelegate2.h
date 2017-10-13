/****************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                          *
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
#ifndef AUTOCOMPLETIONDELEGATE2_H
#define AUTOCOMPLETIONDELEGATE2_H

#include <QtWidgets/QStyledItemDelegate>

class AutoCompletionDelegate2 final : public QStyledItemDelegate
{
   Q_OBJECT
public:
   AutoCompletionDelegate2();

   virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
   virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif

// kate: space-indent on; indent-width 3; replace-tabs on;
