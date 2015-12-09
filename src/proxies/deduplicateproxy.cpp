/****************************************************************************
 *   Copyright (C) 2015 by Emmanuel Lepage Vallee                           *
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
#include "deduplicateproxy.h"

#include <QtCore/QDebug>

typedef std::function<bool(const QModelIndex&, const QModelIndex&)> Comparator;

class DeduplicateProxyPrivate final
{
public:
   // Attributes
   bool                   m_AddChildren    { false                         };
   Comparator             m_fCompare       {                               };
   DeduplicateProxy::Mode m_Mode           { DeduplicateProxy::Mode::ROLE  };
   bool                   m_isSrcProxy     { false                         };
   QVector<int>           m_lOtherRoles    {                               };
   int                    m_HiddenRoleId   { -1                            };
   bool                   m_isHiddenRoleSet{ false                         };
   QString                m_HiddenRoleName {                               };
};

DeduplicateProxy::DeduplicateProxy(QObject* parent) : QSortFilterProxyModel(parent),
d_ptr(new DeduplicateProxyPrivate())
{
}

void DeduplicateProxy::setComparator(const Comparator& f)
{
   d_ptr->m_Mode = DeduplicateProxy::Mode::FUNCTION;
   d_ptr->m_fCompare = f;

   invalidateFilter();
}

/**
 * Useful when there is a main filter role, but some items need to be kept.
 *
 * If any role added to this list is different, the index wont be hidden
 *
 * For example using this proxy to create instant messaging thread if they
 * come from the same person, but also create different threads if the date
 * differ.
 */
void DeduplicateProxy::addFilterRole(int role)
{
   d_ptr->m_lOtherRoles << role;

   invalidateFilter();
}

QVector<int> DeduplicateProxy::filterRoles() const
{
   return d_ptr->m_lOtherRoles;// + filterRole();
}

/**
   * Add rejected items as children of the non-rejected matching item
   *
   * @warning This is ignored for indexes that already have children
   */
void DeduplicateProxy::setAddChildren(bool add)
{
   d_ptr->m_AddChildren = add;
}

bool DeduplicateProxy::areChildrenAdded()
{
   return d_ptr->m_AddChildren;
}

Comparator DeduplicateProxy::comparator()
{
   return d_ptr->m_fCompare;
}

/**
 * Check if the source_row index is identical to the previous index
 */
bool DeduplicateProxy::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
   // Get both source QModelIndex and compare them
   if (source_row > 0) {
      const QModelIndex idx = sourceModel()->index(source_row, filterKeyColumn(), source_parent);

      // It is correct to assume sourceModelIndex - 1 will be part of the same group
      if (idx.row() > 0) {
         const QModelIndex sibling = sourceModel()->index(idx.row() - 1, idx.column(), idx.parent());

         bool accept = false;

         // Allow to filter with more than one role, apply only if they exist
         foreach(const int r, d_ptr->m_lOtherRoles) {
            const QVariant v1 = idx.data    (r);
            const QVariant v2 = sibling.data(r);

            // Yes, this is an "=", not "=="
            if ((accept = !(v1.isValid() && v2.isValid() && (v1 == v2))))
               break;
         }

         return accept || idx.data(filterRole()) != sibling.data(filterRole());
      }
   }

   return true;
}

//TODO support for columns too

///Return the number of hidden siblings of idx
int DeduplicateProxy::hiddenSiblingCount(const QModelIndex& idx) const
{
   if ((!sourceModel()) || (!idx.isValid()) || idx.model() != this)
      return 0;

   const QModelIndex nextIdx = index(idx.row()+1, idx.column(), idx.parent());

   const QModelIndex srcIdx  = mapToSource(idx);

   if (!nextIdx.isValid())
      return sourceModel()->rowCount(srcIdx.parent()) - srcIdx.row();

   const QModelIndex srcNextIdx = mapToSource(nextIdx);

   return srcNextIdx.row() - srcIdx.row();
}

/**
 * Return the list of source model QModelIndex that have been hidden
 */
QList<QModelIndex> DeduplicateProxy::hiddenSiblings(const QModelIndex& idx) const
{
   const int count = hiddenSiblingCount(idx);

   if (!count)
      return {};

   QList<QModelIndex> ret;

   const int         parentRow    = mapToSource(idx).row();
   const QModelIndex srcParentIdx = mapToSource(idx).parent();

   for (int i = 0; i < count; i++)
      ret << sourceModel()->index(parentRow + count, idx.column(), srcParentIdx);

   return ret;
}

/**
 * Add a model data role to report the number of hidden siblings
 */
void DeduplicateProxy::setHiddenCountRole(int role, const QString& name)
{
   d_ptr->m_isHiddenRoleSet = true;
   d_ptr->m_HiddenRoleId    = role;
   d_ptr->m_HiddenRoleName  = name;
}

///This proxy doesn't sort anything
void DeduplicateProxy::sort ( int column, Qt::SortOrder order)
{
   sourceModel()->sort(column, order);
}

void DeduplicateProxy::setSourceModel ( QAbstractItemModel * sourceModel )
{
   QSortFilterProxyModel::setSourceModel(sourceModel);

   d_ptr->m_isSrcProxy = sourceModel && qobject_cast<QAbstractProxyModel*>(sourceModel);

   //TODO find a way to catch setFilterString to forward it as this proxy
   // is likely in front of another QSortFilterProxyModel re-implement ::invalidateFilter?
}

int DeduplicateProxy::rowCount(const QModelIndex& parent) const
{
   if (!d_ptr->m_AddChildren)
      return QSortFilterProxyModel::rowCount(parent);


   //TODO ::index, ::mapToSource and ::mapFromSource still need to be implemented
   /*int hidden = 0;
   // Handle when the rejected items need to be added as children
   if (d_ptr->m_AddChildren && (hidden = hiddenSiblingCount(parent))) {
      const QModelIndex src = mapToSource(parent);

      // Only add the children if there is none
      int parentRowCount = sourceModel()->rowCount(src);

      if (!parentRowCount)
         return hidden;
   }*/

   return QSortFilterProxyModel::rowCount(parent); //TODO
}

QVariant DeduplicateProxy::data( const QModelIndex& index, int role ) const
{
   if (d_ptr->m_isHiddenRoleSet && role == d_ptr->m_HiddenRoleId)
      return hiddenSiblingCount(index);

   return QSortFilterProxyModel::data(index, role);
}
