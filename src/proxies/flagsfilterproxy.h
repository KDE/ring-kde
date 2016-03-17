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
#ifndef FLAGSFILTERPROXY_H
#define FLAGSFILTERPROXY_H

#include <QtCore/QSortFilterProxyModel>

class FlagsFilterProxyPrivate;

/**
 * Filter a model based on the source model flags
 *
 * This can be used to remove disabled and unselectable items
 *
 * The default retainedFlags are:
 *
 * * Qt::ItemIsEnabled
 * * Qt::ItemIsSelectable
 *
 */
class FlagsFilterProxy : public QSortFilterProxyModel
{
   Q_OBJECT
public:
   explicit FlagsFilterProxy(QObject* parent = nullptr);
   virtual ~FlagsFilterProxy();

   void setRetainedFlags(int flags);
   void setRecursive(bool rec); //TODO

   int retainedFlags() const;
   bool isRecursive() const; //TODO

   virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

   virtual void sort ( int column, Qt::SortOrder order) override;

private:
   FlagsFilterProxyPrivate* d_ptr;
   Q_DECLARE_PRIVATE(FlagsFilterProxy)
};

#endif
