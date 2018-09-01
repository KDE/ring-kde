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
#include "abstractviewcompat.h"

#include <abstractselectableview.h>

class AbstractViewCompatPrivate
{
public:
    Qt::Corner m_Corner {Qt::TopLeftCorner};
    bool m_IsSortingEnabled{ false };
};

AbstractViewCompat::AbstractViewCompat(QQuickItem* parent) : AbstractQuickView(parent),
    d_ptr(new AbstractViewCompatPrivate())
{
    // Ok, connecting signals to signals is not a very good idea, I am lazy
    connect(selectionManager(), &AbstractSelectableView::currentIndexChanged,
        this, &AbstractViewCompat::currentIndexChanged);
    connect(selectionManager(), &AbstractSelectableView::selectionModelChanged,
        this, &AbstractViewCompat::selectionModelChanged);
}

AbstractViewCompat::~AbstractViewCompat()
{
    delete d_ptr;
}


Qt::Corner AbstractViewCompat::gravity() const
{
    return d_ptr->m_Corner;
}

void AbstractViewCompat::setGravity(Qt::Corner g)
{
    d_ptr->m_Corner = g;
    refresh();
}

QQmlComponent* AbstractViewCompat::highlight() const
{
    return selectionManager()->highlight();
}

void AbstractViewCompat::setHighlight(QQmlComponent* h)
{
    selectionManager()->setHighlight(h);
}

QSharedPointer<QItemSelectionModel> AbstractViewCompat::selectionModel() const
{
    return selectionManager()->selectionModel();
}

void AbstractViewCompat::setSelectionModel(QSharedPointer<QItemSelectionModel> m)
{
    selectionManager()->setSelectionModel(m);
}

void AbstractViewCompat::setModel(QSharedPointer<QAbstractItemModel> m)
{
    if (d_ptr->m_IsSortingEnabled && m) {
        m->sort(0);
    }

    AbstractQuickView::setModel(m);
}

void AbstractViewCompat::applyRoles(QQmlContext* ctx, const QModelIndex& self) const
{
    FlickableView::applyRoles(ctx, self);
    selectionManager()->applySelectionRoles(ctx, self);
}

bool AbstractViewCompat::isSortingEnabled() const
{
    return d_ptr->m_IsSortingEnabled;
}

void AbstractViewCompat::setSortingEnabled(bool val)
{
    d_ptr->m_IsSortingEnabled = val;

    if (d_ptr->m_IsSortingEnabled && model()) {
        model()->sort(0);
    }
}

QModelIndex AbstractViewCompat::currentIndex() const
{
    return selectionModel()->currentIndex();
}

void AbstractViewCompat::setCurrentIndex(const QModelIndex& index, QItemSelectionModel::SelectionFlags f)
{
    selectionModel()->setCurrentIndex(index, f);
}
