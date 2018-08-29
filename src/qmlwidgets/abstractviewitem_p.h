/***************************************************************************
 *   Copyright (C) 2017-2018 by Bluesystems                                *
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

class AbstractQuickView;
class TreeTraversalItems;
class AbstractViewItem;

// Qt
class QQuickItem;
#include <QtCore/QSharedPointer>

/**
 * Polymorphic tree item for the AbstractQuickView.
 *
 * Classes implementing AbstractQuickView need to provide an implementation of the pure
 * virtual functions. It is useful, for example, to manage both a raster and
 * QQuickItem based version of a view.
 *
 * The state is managed by the AbstractQuickView and it's own protected virtual methods.
 */
class VisualTreeItem
{
    friend class AbstractQuickView;
    friend struct TreeTraversalItems;
    friend class AbstractQuickViewPrivate;
    friend class TreeTraversalReflector;
    friend class AbstractViewItem;
public:

    explicit VisualTreeItem(AbstractQuickView* p) : m_pView(p) {}
    virtual ~VisualTreeItem() {}

    enum class State {
        POOLING , /*!< Being currently removed from view                      */
        POOLED  , /*!< Not currently in use, either new or waiting for re-use */
        BUFFER  , /*!< Not currently on screen, pre-loaded for performance    */
        ACTIVE  , /*!< Visible                                                */
        FAILED  , /*!< Loading the item was attempted, but failed             */
        DANGLING, /*!< Pending deletion, invalid pointers                     */
        ERROR   , /*!< Something went wrong                                   */
    };

    enum class ViewAction { //TODO make this private to AbstractQuickViewPrivate
        ATTACH       = 0, /*!< Activate the element (do not sync it) */
        ENTER_BUFFER = 1, /*!< Sync all roles                        */
        ENTER_VIEW   = 2, /*!< NOP (todo)                            */
        UPDATE       = 3, /*!< Reload the roles                      */
        MOVE         = 4, /*!< Move to a new position                */
        LEAVE_BUFFER = 5, /*!< Stop keeping track of data changes    */
        DETACH       = 6, /*!< Delete                                */
    };

    /// Call to notify that the geometry changed (for the selection delegate)
    void updateGeometry();

    // Helpers


    // Spacial navigation
    VisualTreeItem* up  () const;
    VisualTreeItem* down() const;
    VisualTreeItem* left () const { return nullptr ;}
    VisualTreeItem* right() const { return nullptr ;}
    int row   () const;
    int column() const;
    int depth() const;
    //TODO firstChild, lastChild, parent

    // Getters
    QPersistentModelIndex index() const;
    // Getters
    bool hasFailed() const{
        return m_State == State::FAILED;
    }

    /// Reference to the item own view
    AbstractQuickView* view() const;

    /// Allows to keep a reference while still being tracked by the state machine
    QWeakPointer<VisualTreeItem> reference() const;

    /// Visibility relative to the displayed window of the AbstractQuickView::view()
    bool isVisible() const;
    bool fitsInView() const;

    /// Allow implementations to be notified when it becomes selected
    virtual void setSelected(bool) final;

    /// Geometry relative to the AbstractQuickView::view()
    virtual QRectF geometry() const final;


    //TODO ::above() and ::firstBelow() and ::lastBelow()

    virtual QQuickItem* item() const final;

    // Actions
    virtual bool attach () final;
    virtual bool refresh() final;
    virtual bool move   () final;
    virtual bool flush  () final;
    virtual bool remove () final;

    bool detach();

private:

    typedef bool(VisualTreeItem::*StateF)();
    State m_State {State::POOLED};
    TreeTraversalItems* m_pTTI {nullptr};
    mutable QSharedPointer<VisualTreeItem> m_pSelf;
    AbstractQuickView* m_pView {nullptr};

    static const State  m_fStateMap    [7][7];
    static const StateF m_fStateMachine[7][7];

    bool performAction(ViewAction); //FIXME make private, remove #include

    bool nothing();
    bool error  ();
    bool destroy();

    AbstractViewItem* d_ptr;
};
