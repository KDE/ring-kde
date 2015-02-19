/****************************************************************************
 *   Copyright (C) 2015 by Savoir-Faire Linux                               *
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
#include "simplerotateproxy.h"

SimpleRotateProxy::SimpleRotateProxy(QObject * parent) : QAbstractProxyModel(parent)
{
   connect(this, &SimpleRotateProxy::sourceModelChanged, [this]() {
      connect(this->sourceModel(),&QAbstractItemModel::dataChanged,[this](const QModelIndex&, const QModelIndex&) {
         emit this->layoutChanged();
      });
      connect(this->sourceModel(),&QAbstractItemModel::layoutChanged,[this]() {
         emit this->layoutChanged();
      });
      connect(this->sourceModel(),&QAbstractItemModel::rowsInserted, [this](const QModelIndex & , int , int ) {
         emit this->layoutChanged();
      });
      connect(this->sourceModel(),&QAbstractItemModel::rowsMoved,    [this](const QModelIndex&, int, int, const QModelIndex&, int) {
         emit this->layoutChanged();
      });
      connect(this->sourceModel(),&QAbstractItemModel::rowsRemoved,  [this](const QModelIndex & , int , int ) {
         emit this->layoutChanged();
      });
   });
}

QModelIndex SimpleRotateProxy::mapFromSource(const QModelIndex& sourceIndex) const
{
   if (!sourceIndex.isValid())
      return QModelIndex();

   if (sourceIndex.parent().isValid()) {
      return createIndex(sourceIndex.row(),sourceIndex.column(),sourceIndex.internalPointer());
   }
   else {
      return createIndex(sourceIndex.column(),sourceIndex.row(),sourceIndex.internalPointer());
   }
}

QModelIndex SimpleRotateProxy::mapToSource(const QModelIndex& proxyIndex) const
{
   if (!proxyIndex.isValid())
      return QModelIndex();

   if (proxyIndex.parent().isValid()) {
      return sourceModel()->index(proxyIndex.row(),proxyIndex.column());
   }
   else {
      return sourceModel()->index(proxyIndex.column(),proxyIndex.row());
   }
}

QModelIndex SimpleRotateProxy::index(int row, int column, const QModelIndex& parent) const
{
   Q_UNUSED(parent)
   return createIndex(row,column,nullptr);
}

QModelIndex SimpleRotateProxy::parent(const QModelIndex& ) const
{
   return QModelIndex();
}

int SimpleRotateProxy::rowCount(const QModelIndex& idx) const
{
   return sourceModel()->columnCount(idx);
}

int SimpleRotateProxy::columnCount(const QModelIndex& idx) const
{
   return sourceModel()->rowCount(idx);
}
