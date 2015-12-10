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
#ifndef FILTERTOPLEVELPROXY_H
#define FILTERTOPLEVELPROXY_H

#include <QtCore/QSortFilterProxyModel>

class FilterTopLevelProxyPrivate;

/**
 * Remove top level items without any children
 *
 * This model is useful for 2+ level trees where the top level represent a
 * category. In some case, empty categories may be present and do not add
 * value while poluting the output.
 *
 * For filtering use case, KRecursiveFilterProxyModel is usually recomanded,
 * but a FilterTopLevelProxy may in some specific case, such as when proxies
 * are provided by external libraries, be useful.
 *
 * Example:
 *
 * |->Foo               |->Bar
 * |->Bar                |-> Foo
 *  |-> Foo    =====>    |-> Baz
 *  |-> Baz
 * |->Baz
 *
 */
class FilterTopLevelProxy : public QSortFilterProxyModel
{
   Q_OBJECT
public:
   explicit FilterTopLevelProxy(QAbstractItemModel* parent);

   virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

   virtual void sort ( int column, Qt::SortOrder order) override;

private:
   FilterTopLevelProxyPrivate* d_ptr;
   Q_DECLARE_PRIVATE(FilterTopLevelProxy)
};

#endif
