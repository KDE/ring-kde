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

#include <treeview2.h>

// Qt
class QQuickItem;
class QQmlContext;

class QuickTreeViewPrivate;
class TreeViewPage;

/**
 * Polymorphic tree item for the TreeView2.
 *
 * Classes implementing TreeView2 need to provide an implementation of the pure
 * virtual functions. It is useful, for example, to manage both a raster and
 * QQuickItem based version of a view.
 *
 * The state is managed by the TreeView2 and it's own protected virtual methods.
 */
class QuickTreeViewItem : public VolatileTreeItem
{
public:
    explicit QuickTreeViewItem();
    virtual ~QuickTreeViewItem() {}

    // Actions
    virtual bool attach () override;
    virtual bool refresh() override;
    virtual bool move   () override;
    virtual bool flush  () override;
    virtual bool detach () override;

private:
    QQuickItem* m_pItem     {nullptr};
    QQmlContext* m_pContent {nullptr};
    TreeViewPage* m_pPage   {nullptr};

    QuickTreeViewPrivate* d() const;
};

/**
 * Second generation of QtQuick treeview.
 *
 * The first one was designed for the chat view. It had a limited number of
 * requirement when it came to QtModel. However it required total control of
 * the layout.
 *
 * This is the opposite use case. The layout is classic, but the model support
 * has to be complete. Performance and lazy loading is also more important.
 *
 * It require less work to write a new treeview than refector the first one to
 * support the additional requirements. In the long run, the first generation
 * could be folded into this widget (if it ever makes sense, otherwise they will
 * keep diverging).
 */
class QuickTreeView : public TreeView2
{
    Q_OBJECT

    friend class QuickTreeViewItem;
public:

    explicit QuickTreeView(QQuickItem* parent = nullptr);
    virtual ~QuickTreeView();

Q_SIGNALS:
    void contentChanged() final override;

private:
    virtual VolatileTreeItem* createItem() const override;

    QuickTreeViewPrivate* d_ptr;
    Q_DECLARE_PRIVATE(QuickTreeView)
};
