/****************************************************************************
 *   Copyright (C) 2015 by Emmanuel Lepage Vallee                           *
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
#include "filtertoplevelproxy.h"

class FilterTopLevelProxyPrivate final : public QObject
{
   Q_OBJECT
public:
   explicit FilterTopLevelProxyPrivate(FilterTopLevelProxy* parent);

   bool m_RecursionLock {false}; //Avoid infinite loop (not thread safe)

public Q_SLOTS:
   void changeParent(const QModelIndex& parent, int start, int end );

private:
   FilterTopLevelProxy* q_ptr;
};

FilterTopLevelProxyPrivate::FilterTopLevelProxyPrivate(FilterTopLevelProxy* parent) : QObject(parent),
q_ptr(parent)
{
}


FilterTopLevelProxy::FilterTopLevelProxy(QAbstractItemModel* parent) : QSortFilterProxyModel(parent),
d_ptr(new FilterTopLevelProxyPrivate(this))
{
   setSourceModel(parent);

   connect(parent, &QAbstractItemModel::rowsRemoved , d_ptr, &FilterTopLevelProxyPrivate::changeParent);
   connect(parent, &QAbstractItemModel::rowsInserted, d_ptr, &FilterTopLevelProxyPrivate::changeParent);
}

FilterTopLevelProxy::~FilterTopLevelProxy()
{
   delete d_ptr;
}

bool FilterTopLevelProxy::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
   if (d_ptr->m_RecursionLock && !source_parent.isValid())
      return sourceModel()->rowCount(
         sourceModel()->index(source_row, 0, source_parent)
      );

   return true;
}

///This proxy doesn't sort anything
void FilterTopLevelProxy::sort ( int column, Qt::SortOrder order)
{
   sourceModel()->sort(column, order);
}

///Force filterAcceptsRow to be called when children cound change
void FilterTopLevelProxyPrivate::changeParent(const QModelIndex& parent, int start, int end )
{
   Q_UNUSED(end)

   /// (performance) Only bother when the first child is involved
   if ((!start) && parent.isValid() && !parent.parent().isValid()) {
      m_RecursionLock = true;
      emit q_ptr->sourceModel()->dataChanged(parent,parent);
      m_RecursionLock = false;
   }
}

#include "filtertoplevelproxy.moc"
