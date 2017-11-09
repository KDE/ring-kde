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
#pragma once

#include <flickableview.h>

// Qt
#include <QtCore/QAbstractItemModel>
class QQmlComponent;

class HierarchyViewPrivate;

/**
 * Model view intended for the tree topology.
 *
 * Each index has a big container QQuickItem that encompass the item and all
 * its children.
 *
 * This view currently doesn't support lazy loading. It differs fromm the
 * TreeView2 for the lazy loading part, but allows more flexibility without
 * having to care about keeping everything in sync.
 */
class HierarchyView : public FlickableView
{
    Q_OBJECT
public:
    explicit HierarchyView(QQuickItem* parent = nullptr);
    virtual ~HierarchyView();

    virtual void setModel(QSharedPointer<QAbstractItemModel> model) override;

protected:
    virtual void refresh() override;

private:
    HierarchyViewPrivate* d_ptr;
    Q_DECLARE_PRIVATE(HierarchyView)
};
