/****************************************************************************
 *   Copyright (C) 2016 by Emmanuel Lepage Vallee                           *
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
#ifndef ACCOUNTINFODELEGATE_H
#define ACCOUNTINFODELEGATE_H

#include <QtWidgets/QStyledItemDelegate>

#include <proxies/pollingproxy.h>

class AccountInfoDelegate final : public QStyledItemDelegate
{
   Q_OBJECT
public:
   explicit AccountInfoDelegate(QObject* parent = nullptr);

   virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
   virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

class AccountInfoProxy : public PollingProxy
{
   Q_OBJECT
public:
   explicit AccountInfoProxy(QAbstractItemModel* m) : PollingProxy(m){}
   virtual QVariant data( const QModelIndex& index, int role ) const override;
};

#endif

//kate: space-indent on; indent-width 3; replace-tabs on;

// kate: space-indent on; indent-width 3; replace-tabs on;
