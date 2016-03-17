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
#ifndef DEDUPLICATEPROXY_H
#define DEDUPLICATEPROXY_H

#include <functional>

#include <QtCore/QSortFilterProxyModel>

class DeduplicateProxyPrivate;

/**
 * This proxy take a role and remove additional rows matching the operator==
 * of the role. It only deduplication indexes without child.
 *
 * Further QIdentityProxyModels can be created to extract the number of
 *
 * Example:
 *                         ==Without addChildren==   ==With addChildren==
 * Foo                          Foo                    Foo
 *   |->  Bar                     |->  Bar               |->  Bar
 *   |->  Bar                     |->  Baz                 |->  Bar
 *   |->  Bar                     |->  Bar                 |->  Bar
 *   |->  Baz                   Bar                      |->  Baz
 *   |->  Bar                     |->  Foo               |->  Bar
 * Bar             ====== >       |->  Baz             Bar
 *   |->  Foo                   Bar                      |->  Foo
 *   |->  Baz                     |->  Foo               |->  Baz
 * Bar                                                 Bar
 *   |->  Foo                                            |->  Foo
 *   |->  Foo                                              |->  Foo
 *
 * Additional roles can be added to filter prevent some items from being
 * hidden.
 *
 * @warning The default role comparaison only work for QVariant basic types \
 * for custom types, it is better to use the function comparaison
 *
 * @note The filter string and sort role will be forwarded down the proxy chain\
 * so this model can be used as a drop in replacment in existing code TODO
 */
class DeduplicateProxy : public QSortFilterProxyModel
{
   Q_OBJECT

public:
   enum class Mode {
      ROLE,
      FUNCTION //TODO
   };

   explicit DeduplicateProxy(QObject* parent = nullptr);
   virtual ~DeduplicateProxy();

   void addFilterRole(int role);

   QVector<int> filterRoles() const;

   void setComparator(const std::function<bool(const QModelIndex&, const QModelIndex&)>& f); //TODO

   void setHiddenCountRole(int role, const QString& name = QString());

   QList<QModelIndex> hiddenSiblings(const QModelIndex& idx) const;

   void setAddChildren(bool add); //TODO

   int hiddenSiblingCount(const QModelIndex& idx) const;

   bool areChildrenAdded(); //TODO
   std::function<bool(const QModelIndex&, const QModelIndex&)> comparator(); //TODO

   // Model override
   virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;
   virtual void sort ( int column, Qt::SortOrder order) override;
   virtual void setSourceModel ( QAbstractItemModel * sourceModel ) override;
   virtual int  rowCount(const QModelIndex& parent) const override;
   virtual QVariant data( const QModelIndex& index, int role ) const override;



private:
   DeduplicateProxyPrivate* d_ptr;
   Q_DECLARE_PRIVATE(DeduplicateProxy)
};

#endif
