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

class AbstractQuickViewPrivate;
class AbstractQuickView;

#include <treetraversalreflector_p.h> //FIXME remove
struct TreeTraversalItems; //FIXME remove

class VisualTreeItem;

/**
* Abstract base class of the visual representation of a QModelIndex.
*/
class MoodelIndexItem
{
public:
    virtual ~MoodelIndexItem() {}

    explicit MoodelIndexItem(AbstractQuickView* view);

    /// Allow implementations to be notified when it becomes selected
    virtual void setSelected(bool) {}


    /// The model index
    virtual QPersistentModelIndex index() const = 0;

    /// Geometry relative to the AbstractQuickView::view()
    virtual QRectF geometry() const = 0;

    // Spacial navigation
    virtual VisualTreeItem* left () const { return nullptr ;}
    virtual VisualTreeItem* right() const { return nullptr ;}

    //TODO ::above() and ::firstBelow() and ::lastBelow()

    virtual QQuickItem* item() const { return nullptr; }


    AbstractQuickView* m_pView {nullptr};
};

/**
 * Polymorphic tree item for the AbstractQuickView.
 *
 * Classes implementing AbstractQuickView need to provide an implementation of the pure
 * virtual functions. It is useful, for example, to manage both a raster and
 * QQuickItem based version of a view.
 *
 * The state is managed by the AbstractQuickView and it's own protected virtual methods.
 */
class VisualTreeItem : public MoodelIndexItem
{
    friend class AbstractQuickView;
    friend struct TreeTraversalItems;
    friend class AbstractQuickViewPrivate;
    friend class TreeTraversalReflector;
public:

    explicit VisualTreeItem(AbstractQuickView* p) : MoodelIndexItem(p) {}
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

    /// Call to notify that the geometry changed (for the selection delegate)
    void updateGeometry();

    // Helpers
    VisualTreeItem* up  () const;
    VisualTreeItem* down() const;
    int row   () const;
    int column() const;

    // Getters
    QPersistentModelIndex index() const;
    int depth() const;
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

    // Actions
    virtual bool attach () = 0;
    virtual bool refresh() = 0;
    virtual bool move   () = 0;
    virtual bool flush  () = 0;
    virtual bool remove () = 0;

    bool detach();

private:

    typedef bool(VisualTreeItem::*StateF)();
    State m_State {State::POOLED};
    TreeTraversalItems* m_pTTI {nullptr};
    mutable QSharedPointer<VisualTreeItem> m_pSelf;

    static const State  m_fStateMap    [7][7];
    static const StateF m_fStateMachine[7][7];

    bool performAction(TreeTraversalReflector::ViewAction); //FIXME make private, remove #include

    bool nothing();
    bool error  ();
    bool destroy();
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
class AbstractQuickView : public FlickableView
{
    Q_OBJECT
    friend struct TreeTraversalItems;
    friend class VisualTreeItem;
public:
    /// Assume each hierarchy level have the same height (for performance)
    Q_PROPERTY(bool uniformRowHeight READ hasUniformRowHeight   WRITE setUniformRowHeight)
    /// Assume each column has the same width (for performance)
    Q_PROPERTY(bool uniformColumnWidth READ hasUniformColumnWidth WRITE setUniformColumnColumnWidth)
    /// The view can be collapsed
    Q_PROPERTY(bool collapsable READ isCollapsable WRITE setCollapsable)
    /// Expand all elements by default
    Q_PROPERTY(bool autoExpand READ isAutoExpand WRITE setAutoExpand)
    /// The maximum depth of a tree (for performance)
    Q_PROPERTY(int maxDepth READ maxDepth WRITE setMaxDepth)
    /// Recycle existing QQuickItem delegates for new QModelIndex (for performance)
    Q_PROPERTY(RecyclingMode recyclingMode READ recyclingMode WRITE setRecyclingMode)
    /// The number of elements to be preloaded outside of the visible area (for latency)
    Q_PROPERTY(int cacheBuffer READ cacheBuffer WRITE setCacheBuffer)
    /// The number of delegates to be kept in a recycling pool (for performance)
    Q_PROPERTY(int poolSize READ poolSize WRITE setPoolSize)

    enum RecyclingMode {
        NoRecycling    , /*!< Destroy and create new QQuickItems all the time         */
        RecyclePerDepth, /*!< Keep different pools buffer for each levels of children */
        AlwaysRecycle  , /*!< Assume all depth level use the same delegate            */
    };
    Q_ENUM(RecyclingMode)

    explicit AbstractQuickView(QQuickItem* parent = nullptr);

    virtual ~AbstractQuickView();

    virtual void setModel(QSharedPointer<QAbstractItemModel> model) override;

    bool hasUniformRowHeight() const;
    void setUniformRowHeight(bool value);

    bool hasUniformColumnWidth() const;
    void setUniformColumnColumnWidth(bool value);

    bool isCollapsable() const;
    void setCollapsable(bool value);

    bool isAutoExpand() const;
    void setAutoExpand(bool value);

    int maxDepth() const;
    void setMaxDepth(int depth);

    int cacheBuffer() const;
    void setCacheBuffer(int value);

    int poolSize() const;
    void setPoolSize(int value);

    RecyclingMode recyclingMode() const;
    void setRecyclingMode(RecyclingMode mode);

    /**
     * If some changes on the QML side require moving elements. It's the
     * equivalent of calling dataChanged on every direct children index of
     * `index`.
     */
    Q_INVOKABLE void reloadChildren(const QModelIndex& index) const;

    /**
     * Get the parent tree QQuickItem for a given index
     */
    Q_INVOKABLE QQuickItem* parentTreeItem(const QModelIndex& index) const;

protected:
    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

    /**
     * Get the VisualTreeItem associated with a model index.
     *
     * Note that if the index is not currently visible or buferred, it will
     * return nullptr.
     */
    VisualTreeItem* itemForIndex(const QModelIndex& idx) const final override;

    /// To be used with moderation. Necessary when the delegate is replaced.
    void reload();

Q_SIGNALS:
    void modelChanged(QSharedPointer<QAbstractItemModel> model);
    virtual void contentChanged() = 0;

private:
    AbstractQuickViewPrivate* d_ptr;
    Q_DECLARE_PRIVATE(AbstractQuickView)
};
