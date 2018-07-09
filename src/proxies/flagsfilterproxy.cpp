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
#include "flagsfilterproxy.h"

class FlagsFilterProxyPrivate final
{
public:
   bool m_IsRecursive   { false                                    };
   uint m_RetainedFlags { Qt::ItemIsEnabled | Qt::ItemIsSelectable };
};

FlagsFilterProxy::FlagsFilterProxy(QObject* parent) : QSortFilterProxyModel(parent),
d_ptr(new FlagsFilterProxyPrivate())
{}

FlagsFilterProxy::~FlagsFilterProxy()
{
   delete d_ptr;
}

void FlagsFilterProxy::setRetainedFlags(int flags)
{
   d_ptr->m_RetainedFlags = flags;
   invalidateFilter();
}

void FlagsFilterProxy::setRecursive(bool rec)
{
   //TODO
   d_ptr->m_IsRecursive = rec;
   invalidateFilter();
}

int FlagsFilterProxy::retainedFlags() const
{
   return d_ptr->m_RetainedFlags;
}

bool FlagsFilterProxy::isRecursive() const
{
   return d_ptr->m_IsRecursive;
}

bool FlagsFilterProxy::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
   const QModelIndex srcIdx = sourceModel()->index(source_row, filterKeyColumn(), source_parent);

   return (srcIdx.flags() & d_ptr->m_RetainedFlags) == d_ptr->m_RetainedFlags;
}

void FlagsFilterProxy::sort( int column, Qt::SortOrder order)
{
   sourceModel()->sort(column, order);
}
