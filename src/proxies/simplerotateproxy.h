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
#ifndef SimpleRotateProxy_H
#define SimpleRotateProxy_H

#include <QtCore/QAbstractProxyModel>

/**
 * This proxy model turn each top level rows into columns and columns into rows.
 *
 * This is equivalent to I * {{0,1},{-1,0}}
 *
 * This models proxy children items as is with no transformation.
 *
 * This model can be used when:
 *
 * 1) The model is based on a QAbtractListModel and it is preferable to
 * display it into a QTableView horizontally.
 *
 * 2) The top level items represent a concept different from the lower levels and
 * displaying them horizontally create a better visual separation.
 *
 * 3) The source model is external and display data in inverted order, such as
 * many real world CSV export tools or badly designed spreadsheets
 *
 * Example:
 *
 * |-> Foo
 * |-> Bar                    -----------------
 * |-> Baz          ====>      |    |    |
 *   |-> Foobar               Foo  Bar  Baz
 *   |-> Foobaz                          |-> Foobar
 *                                       |-> Foobaz
 */
class SimpleRotateProxy : public QAbstractProxyModel
{
   Q_OBJECT
public:
   explicit SimpleRotateProxy(QObject * parent = 0);
   virtual QModelIndex  mapFromSource(const QModelIndex & sourceIndex) const override;
   virtual QModelIndex  mapToSource  (const QModelIndex & proxyIndex ) const override;
   virtual QModelIndex  index        (int, int, const QModelIndex&   ) const override;
   virtual QModelIndex  parent       (const QModelIndex&             ) const override;
   virtual int          rowCount     (const QModelIndex&             ) const override;
   virtual int          columnCount  (const QModelIndex&             ) const override;
};
Q_DECLARE_METATYPE(SimpleRotateProxy*)

#endif
