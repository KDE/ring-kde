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

#include <QtCore/QPersistentModelIndex>
#include <QtCore/QRectF>

/**
 * This class must be extended by the views to bind QModelIndex with a GUI element.
 *
 * This is the class implementation should use to navigate the model indices. It
 * provides a simple geometric linked list between indices and provide built-in
 * memory management, including recycling.
 *
 * This is optionally a template class to avoid having to perform static casts
 * when implementing.
 *
 * All moderately cartesian views such as list, table, trees and most charts
 * should use this class along with `AbstractQuickView`. Graphs and advanced
 * visualizations should probably use a lower level API.
 *
 * These objects are created if and only if those 2 conditions are met:
 *
 *  * The QModelIndex is currently part of a tracked range (see TreeTraversalRange).
 *  * It didn't fail to load.
 *
 * If the overloaded functions returns false, then the consumer of this
 * class should no longer use the object as it may have been deleted or
 * recycled.
 *
 * Note that this should **NEVER** be stored and an instance may be reused for
 * other indices or deleted without prior notice. The memory management of this
 * class is designed for high performance, not ease of use. If you are
 * implementing views in C++, then this is probably what you want, so think
 * twice before cursing at how low level this is.
 *
 * Note that this object usage should be strictly limited to its overloaded
 * functions and external usage (from the view class or elsewhere) is
 * strongly discouraged. If you think the provided interface isn't enough,
 * report a bug rather than work around it to avoid crashes.
 */
class AbstractViewItem
{
public:

    /**
     * The external state if the item.
     *
     * This is different from the internal state and only has a subset the
     * consumer (views) care about. Note that internally, it can be part of a
     * recycling pool or other types of caches.
     */
    enum class State {
        BUFFER , /*!< Is close enough to the visible area to have been loaded */
        VISIBLE, /*!< Is currently displayed                                  */
        INVALID, /*!< You should not use this instance                        */
    };

    /**
     * The item above this one when viewing the model as a Cartesian map.
     *
     * It can either be a sibling (for example, if the model is a list or table),
     * the parent (in case of a tree) or a (sub) children of the parent upper
     * sibling.
     *
     */
    AbstractViewItem* up() const;

    /**
     * The item below this one when viewing the model as a Cartesian map.
     *
     * It can be a sibling or an item with a lower depth level.
     */
    AbstractViewItem* down () const;

    ///TODO
    AbstractViewItem* left () const;

    ///TODO
    AbstractViewItem* right() const;

    /**
     * This method return the item representing the QModelIndex parent.
     *
     * It will return it **ONLY IF THE PARENT IS PART OF THE VISIBLE RANGE**.
     */
    AbstractViewItem* parent() const;

    /**
     * The item row.
     *
     * Note that this doesn't always match QModelIndex::row() because this
     * value is updated when the `rowsAboutToBeModed` signal is sent rather
     * than after the change takes effect.
     */
    int row   () const;

    /**
     * The item column.
     *
     * Note that this doesn't always match QModelIndex::column() because this
     * value is updated when the `columnsAboutToBeModed` signal is sent rather
     * than after the change takes effect.
     */
    int column() const;

    /**
     * The model index.
     *
     * Please do not use .row() and .column() on the index and rather use
     * ::row() and ::column() provided by this class. Otherwise items being
     * moved wont be rendered correctly.
     */
    QPersistentModelIndex index() const;

    // Actions

    /**
     * Force the position to be computed again.
     *
     * The item position is managed by the implementation. However external
     * events such as resizing the window or switching from mobile to desktop
     * mode can require the position to be reconsidered.
     *
     * This will call `AbstractViewItem::move` and all the actions such
     * as moving the children items, updating the visible range and general
     * housekeeping tasks so the view implementation does not have to care
     * about them.
     */
    void resetPosition();


    /**
     * The size and position necessary to draw this item.
     */
    virtual QRectF geometry() const = 0;

protected:
    virtual bool attach () = 0;
    virtual bool refresh() = 0;
    virtual bool move   () = 0;
    virtual bool flush  () = 0;
    virtual bool remove () = 0;
};
