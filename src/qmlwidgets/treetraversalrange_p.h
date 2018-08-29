/***************************************************************************
 *   Copyright (C) 2018 by Bluesystems                                     *
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

/**
* This class exposes a way to track and iterate a subset of the model.
*
* It prevents all of the model reflection to have to be loaded in memory
* and offers a simpler API to access the loaded sections.
*
* This class is for internal use and should not be used by views. Please use
* `AbstractQuickView` for all relevant use cases.
*/
class TreeTraversalRange
{
public:
    /**
     * A generic iterator for the ModelIndexItem interface.
     *
     * The user of this class should not have to care about the model indices
     * themselves but rather the subset they "really" display.
     */
    class Iterator {
    public:
        explicit Iterator ();
        bool operator!= (const Iterator& other) const;
        VisualTreeItem* operator* () const;
        const Iterator& operator++ ();

    private:
        const Iterator *p_vec_;
    };

    /**
     * Iterate a range subset from a (relative) root index.
     */
    class Subset {
    public:
        Iterator constBegin() const;
        Iterator constEnd() const;
    };

    //Iterator();

    //Murator

    /// Track an additional QModelIndex toward to model root.
    bool incrementUpward();
    /// Track an additional QModelIndex toward to model tail.
    bool incrementDownward();
    /// Untrack an additional QModelIndex from the model root.
    bool decrementUpward();
    /// Untrack an additional QModelIndex from the model tail.
    bool decrementDownward();

    /**
     * Get an iterator for an index and all its children indices.
     */
    const Subset subset(const QModelIndex& idx) const;

    //Iterators
    /*Iterator begin();
    Iterator end();*/
};
