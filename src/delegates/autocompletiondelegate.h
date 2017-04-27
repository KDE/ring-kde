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
#ifndef AUTOCOMPLETIONDELEGATE_H
#define AUTOCOMPLETIONDELEGATE_H

#include <QtWidgets/QStyledItemDelegate>

class AutoCompletionDelegate : public QStyledItemDelegate
{
   Q_OBJECT
public:
   explicit AutoCompletionDelegate(int role = Qt::UserRole);

   virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
private:
   int m_Role;
};

#endif

// kate: space-indent on; indent-width 3; replace-tabs on;
