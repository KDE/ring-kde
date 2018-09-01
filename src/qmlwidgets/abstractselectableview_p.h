/***************************************************************************
 *   Copyright (C) 2017 by Bluesystems                                     *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/

// Qt
class QAbstractItemModel;

class AbstractQuickView;

// This class exists for the AbstractItemView to be able to internally
// notify the AbstractSelectableView about some events. In theory the public
// interface of both of these class should be extended to handle such events
// but for now a private interface allows more flexibility.
class AbstractSelectableViewSyncInterface
{
public:
    virtual void updateSelection(const QModelIndex& idx);

    QAbstractItemModel* model() const;
    void setModel(QAbstractItemModel* m);

    AbstractQuickView* view() const;
    void setView(AbstractQuickView* v);

    AbstractSelectableView* q_ptr;
};
