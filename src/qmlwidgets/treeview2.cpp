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
#include "treeview2.h"

/*
 * Design:
 *
 * This class implement 3 embedded state machines.
 *
 * The first machine has a single instance and manage the whole view. It allows
 * different behavior (corner case) to be handled in a stateful way without "if".
 * It tracks a moving window of elements coarsely equivalent to the number of
 * elements on screen.
 *
 * The second layer reflects the model and corresponds to a QModelIndex currently
 * being tracked by the system. They are lazy-loaded and mostly unaware of the
 * model topology. While they are hierarchical, they are so compared to themselves,
 * not the actual model hierarchy. This allows the model to have unlimited depth
 * without any performance impact.
 *
 * The last layer of state machine represents the visual elements. Being separated
 * from the model view allows the QQuickItem elements to fail to load without
 * spreading havoc. This layer also implements an abstraction that allows the
 * tree to be browsed as a list. This greatly simplifies the widget code. The
 * abstract class has to be implemented by the view implementations. All the
 * heavy lifting is done in this class and the implementation can assume all is
 * valid and perform minimal validation.
 *
 */

class VolatileTreeItemPrivate
{
public:

};

/*
 * The visual elements state changes.
 *
 * Note that the ::FAILED elements will always try to self-heal themselves and
 * go back into FAILED once the self-healing itself failed.
 */
#define S VolatileTreeItem::State::
const VolatileTreeItem::State VolatileTreeItem::m_fStateMap[6][7] = {
/*              ATTACH ENTER_BUFFER ENTER_VIEW UPDATE    MOVE   LEAVE_BUFFER  DETACH  */
/*POOLED  */ { S POOLED, S BUFFER, S ERROR , S ERROR , S ERROR , S ERROR , S DANGLING },
/*BUFFER  */ { S ERROR , S ERROR , S ACTIVE, S BUFFER, S ERROR , S POOLED, S DANGLING },
/*ACTIVE  */ { S ERROR , S BUFFER, S ERROR , S ACTIVE, S ACTIVE, S POOLED, S DANGLING },
/*FAILED  */ { S ERROR , S BUFFER, S ACTIVE, S ACTIVE, S ACTIVE, S POOLED, S DANGLING },
/*DANGLING*/ { S ERROR , S ERROR , S ERROR , S ERROR , S ERROR , S ERROR , S DANGLING },
/*ERROR   */ { S ERROR , S ERROR , S ERROR , S ERROR , S ERROR , S ERROR , S DANGLING },
};
#undef S

#define A &VolatileTreeItem::
const VolatileTreeItem::StateF VolatileTreeItem::m_fStateMachine[6][7] = {
/*             ATTACH  ENTER_BUFFER  ENTER_VIEW   UPDATE     MOVE   LEAVE_BUFFER  DETACH  */
/*POOLED  */ { A nothing, A attach , A error  , A error  , A error  , A error  , A destroy },
/*BUFFER  */ { A error  , A error  , A move   , A refresh, A error  , A detach , A destroy },
/*ACTIVE  */ { A error  , A nothing, A error  , A refresh, A move   , A detach , A destroy },
/*FAILED  */ { A error  , A attach , A attach , A attach , A attach , A detach , A destroy },
/*DANGLING*/ { A error  , A error  , A error  , A error  , A error  , A error  , A destroy },
/*error   */ { A error  , A error  , A error  , A error  , A error  , A error  , A destroy },
};
#undef A

/**
 * Hold the QPersistentModelIndex and the metadata associated with them.
 */
struct TreeTraversalItems
{
    explicit TreeTraversalItems(TreeTraversalItems* parent, TreeView2Private* d):
        m_pParent(parent), d_ptr(d) {}

    enum class State {
        BUFFER    = 0, /*!< Not in use by any visible indexes, but pre-loaded */
        REMOVED   = 1, /*!< Currently in a removal transaction                */
        REACHABLE = 2, /*!< The [grand]parent of visible indexes              */
        VISIBLE   = 3, /*!< The element is visible on screen                  */
        ERROR     = 4, /*!< Something went wrong                              */
        DANGLING  = 5, /*!< Being destroyed                                   */
    };

    enum class Action {
        SHOW   = 0, /*!< Make visible on screen (or buffer) */
        HIDE   = 1, /*!< Remove from the screen (or buffer) */
        ATTACH = 2, /*!< Track, but do not show             */
        DETACH = 3, /*!< Stop tracking for changes          */
        UPDATE = 4, /*!< Update the element                 */
        MOVE   = 5, /*!< Update the depth and lookup        */
    };

    typedef bool(TreeTraversalItems::*StateF)();

    static const State  m_fStateMap    [6][6];
    static const StateF m_fStateMachine[6][6];

    bool performAction(Action);

    bool nothing();
    bool error  () __attribute__ ((noreturn));
    bool show   ();
    bool hide   ();
    bool attach ();
    bool detach ();
    bool refresh();
    bool index  ();
    bool destroy() __attribute__ ((noreturn));

    //TODO use a btree, not an hash
    QHash<QPersistentModelIndex, TreeTraversalItems*> m_hLookup;

    uint m_Depth {0};
    State m_State {State::BUFFER};

    // Keep the parent to be able to get back to the root
    TreeTraversalItems* m_pParent;

    // There is no need to keep an ordered vector, random access is not
    // necessary. However to keep the double chained list in sync, the edges
    // are.
    TreeTraversalItems* m_pFirstChild {nullptr};
    TreeTraversalItems* m_pLastChild {nullptr};

    // Those are only for the elements in the same depth level
    TreeTraversalItems* m_pPrevious {nullptr};
    TreeTraversalItems* m_pNext {nullptr};

    QPersistentModelIndex m_Index;
    VolatileTreeItem* m_pTreeItem {nullptr};

    TreeView2Private* d_ptr;
};

#define S TreeTraversalItems::State::
const TreeTraversalItems::State TreeTraversalItems::m_fStateMap[6][6] = {
/*                 SHOW         HIDE        ATTACH     DETACH      UPDATE       MOVE     */
/*BUFFER   */ { S VISIBLE, S BUFFER   , S REACHABLE, S DANGLING , S BUFFER , S BUFFER    },
/*REMOVED  */ { S ERROR  , S ERROR    , S ERROR    , S BUFFER   , S ERROR  , S ERROR     },
/*REACHABLE*/ { S VISIBLE, S REACHABLE, S ERROR    , S BUFFER   , S ERROR  , S REACHABLE },
/*VISIBLE  */ { S VISIBLE, S REACHABLE, S ERROR    , S BUFFER   , S VISIBLE, S VISIBLE   },
/*ERROR    */ { S ERROR  , S ERROR    , S ERROR    , S ERROR    , S ERROR  , S ERROR     },
/*DANGLING */ { S ERROR  , S ERROR    , S ERROR    , S ERROR    , S ERROR  , S ERROR     },
};
#undef S

#define A &TreeTraversalItems::
const TreeTraversalItems::StateF TreeTraversalItems::m_fStateMachine[6][6] = {
/*                 SHOW       HIDE      ATTACH    DETACH      UPDATE    MOVE   */
/*BUFFER   */ { A show   , A nothing, A attach, A destroy , A refresh, A index },
/*REMOVED  */ { A error  , A error  , A error , A detach  , A error  , A error },
/*REACHABLE*/ { A show   , A nothing, A error , A detach  , A error  , A index },
/*VISIBLE  */ { A nothing, A hide   , A error , A detach  , A refresh, A index },
/*ERROR    */ { A error  , A error  , A error , A error   , A error  , A error },
/*DANGLING */ { A error  , A error  , A error , A error   , A error  , A error },
};
#undef A

class TreeView2Private : public QObject
{
    Q_OBJECT
public:

    enum class State {
        UNFILLED, /*!< There is less items that the space available          */
        ANCHORED, /*!< Some items are out of view, but it's at the beginning */
        SCROLLED, /*!< It's scrolled to a random point                       */
        AT_END  , /*!< It's at the end of the items                          */
        ERROR   , /*!< Something went wrong                                  */
    };

    enum class Action {
        INSERTION    = 0,
        REMOVAL      = 1,
        MOVE         = 2,
        RESET_SCROLL = 3,
        SCROLL       = 4,
    };

    typedef bool(TreeView2Private::*StateF)();

    static const State  m_fStateMap    [5][5];
    static const StateF m_fStateMachine[5][5];

    bool m_UniformRowHeight   {false};
    bool m_UniformColumnWidth {false};
    bool m_Collapsable        {true };
    bool m_AutoExpand         {false};
    int  m_MaxDepth           { -1  };
    int  m_CacheBuffer        { 10  };
    int  m_PoolSize           { 10  };
    int  m_FailedCount        {  0  };
    TreeView2::RecyclingMode m_RecyclingMode {
        TreeView2::RecyclingMode::NoRecycling
    };
    State m_State {State::UNFILLED};

    TreeTraversalItems* m_pRoot {new TreeTraversalItems(nullptr, this)};

    /// All elements with loaded children
    QHash<QPersistentModelIndex, TreeTraversalItems*> m_hMapper;

    // Helpers
    bool isActive(const QModelIndex& parent, int first, int last);
    void initTree(const QModelIndex& parent);
    TreeTraversalItems* addChildren(TreeTraversalItems* parent, const QModelIndex& index);
    void cleanup();

    // Tests
    void _test_validateTree(TreeTraversalItems* p);

    TreeView2* q_ptr;

private:
    bool nothing     ();
    bool resetScoll  ();
    bool refresh     ();
    bool refreshFront();
    bool refreshBack ();
    bool error       () __attribute__ ((noreturn));

public Q_SLOTS:
    void slotRowsInserted  (const QModelIndex& parent, int first, int last);
    void slotRowsRemoved   (const QModelIndex& parent, int first, int last);
    void slotLayoutChanged (                                              );
    void slotDataChanged   (const QModelIndex& tl, const QModelIndex& br  );
    void slotRowsMoved     (const QModelIndex &p, int start, int end,
                            const QModelIndex &dest, int row);
};

#define S TreeView2Private::State::
const TreeView2Private::State TreeView2Private::m_fStateMap[5][5] = {
/*              INSERTION    REMOVAL       MOVE     RESET_SCROLL    SCROLL  */
/*UNFILLED */ { S ANCHORED, S UNFILLED , S UNFILLED , S UNFILLED, S UNFILLED },
/*ANCHORED */ { S ANCHORED, S ANCHORED , S ANCHORED , S ANCHORED, S SCROLLED },
/*SCROLLED */ { S SCROLLED, S SCROLLED , S SCROLLED , S ANCHORED, S SCROLLED },
/*AT_END   */ { S AT_END  , S AT_END   , S AT_END   , S ANCHORED, S SCROLLED },
/*ERROR    */ { S ERROR   , S ERROR    , S ERROR    , S ERROR   , S ERROR    },
};
#undef S

#define A &TreeView2Private::
const TreeView2Private::StateF TreeView2Private::m_fStateMachine[5][5] = {
/*              INSERTION           REMOVAL          MOVE        RESET_SCROLL     SCROLL  */
/*UNFILLED*/ { A refreshFront, A refreshFront , A refreshFront , A nothing   , A nothing  },
/*ANCHORED*/ { A refreshFront, A refreshFront , A refreshFront , A nothing   , A refresh  },
/*SCROLLED*/ { A refresh     , A refresh      , A refresh      , A resetScoll, A refresh  },
/*AT_END  */ { A refreshBack , A refreshBack  , A refreshBack  , A resetScoll, A refresh  },
/*ERROR   */ { A error       , A error        , A error        , A error     , A error    },
};
#undef A

TreeView2::TreeView2(QQuickItem* parent) : FlickableView(parent),
    d_ptr(new TreeView2Private())
{
    d_ptr->q_ptr = this;
}

TreeView2::~TreeView2()
{
    delete d_ptr;
}

void TreeView2::setModel(QSharedPointer<QAbstractItemModel> m)
{
    if (m == model())
        return;

    if (model()) {
        disconnect(model().data(), &QAbstractItemModel::rowsInserted, d_ptr,
            &TreeView2Private::slotRowsInserted);
        disconnect(model().data(), &QAbstractItemModel::rowsRemoved, d_ptr,
            &TreeView2Private::slotRowsRemoved);
        disconnect(model().data(), &QAbstractItemModel::layoutChanged, d_ptr,
            &TreeView2Private::slotLayoutChanged);
        disconnect(model().data(), &QAbstractItemModel::modelReset, d_ptr,
            &TreeView2Private::slotLayoutChanged);
        disconnect(model().data(), &QAbstractItemModel::rowsMoved, d_ptr,
            &TreeView2Private::slotRowsMoved);
        disconnect(model().data(), &QAbstractItemModel::dataChanged, d_ptr,
            &TreeView2Private::slotDataChanged);
    }

    FlickableView::setModel(m);

    if (!m)
        return;

    connect(model().data(), &QAbstractItemModel::rowsInserted, d_ptr,
        &TreeView2Private::slotRowsInserted );
    connect(model().data(), &QAbstractItemModel::rowsRemoved, d_ptr,
        &TreeView2Private::slotRowsRemoved  );
    connect(model().data(), &QAbstractItemModel::layoutChanged, d_ptr,
        &TreeView2Private::slotLayoutChanged  );
    connect(model().data(), &QAbstractItemModel::modelReset, d_ptr,
        &TreeView2Private::slotLayoutChanged  );
    connect(model().data(), &QAbstractItemModel::rowsMoved, d_ptr,
        &TreeView2Private::slotRowsMoved);
    connect(model().data(), &QAbstractItemModel::dataChanged, d_ptr,
        &TreeView2Private::slotDataChanged  );

    if (auto rc = m->rowCount())
        d_ptr->slotRowsInserted({}, 0, rc - 1);
}

bool TreeView2::hasUniformRowHeight() const
{
    return d_ptr->m_UniformRowHeight;
}

void TreeView2::setUniformRowHeight(bool value)
{
    d_ptr->m_UniformRowHeight = value;
}

bool TreeView2::hasUniformColumnWidth() const
{
    return d_ptr->m_UniformColumnWidth;
}

void TreeView2::setUniformColumnColumnWidth(bool value)
{
    d_ptr->m_UniformColumnWidth = value;
}

bool TreeView2::isCollapsable() const
{
    return d_ptr->m_Collapsable;
}

void TreeView2::setCollapsable(bool value)
{
    d_ptr->m_Collapsable = value;
}

bool TreeView2::isAutoExpand() const
{
    return d_ptr->m_AutoExpand;
}

void TreeView2::setAutoExpand(bool value)
{
    d_ptr->m_AutoExpand = value;
}

int TreeView2::maxDepth() const
{
    return d_ptr->m_MaxDepth;
}

void TreeView2::setMaxDepth(int depth)
{
    d_ptr->m_MaxDepth = depth;
}

int TreeView2::cacheBuffer() const
{
    return d_ptr->m_CacheBuffer;
}

void TreeView2::setCacheBuffer(int value)
{
    d_ptr->m_CacheBuffer = value;
}

int TreeView2::poolSize() const
{
    return d_ptr->m_PoolSize;
}

void TreeView2::setPoolSize(int value)
{
    d_ptr->m_PoolSize = value;
}

TreeView2::RecyclingMode TreeView2::recyclingMode() const
{
    return d_ptr->m_RecyclingMode;
}

void TreeView2::setRecyclingMode(TreeView2::RecyclingMode mode)
{
    d_ptr->m_RecyclingMode = mode;
}

void TreeView2::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    FlickableView::geometryChanged(newGeometry, oldGeometry);
    d_ptr->m_pRoot->performAction(TreeTraversalItems::Action::MOVE);
    contentItem()->setWidth(newGeometry.width());
}

/// Return true if the indices affect the current view
bool TreeView2Private::isActive(const QModelIndex& parent, int first, int last)
{
    return true; //FIXME

    if (m_State == State::UNFILLED)
        return true;

    //FIXME only insert elements with loaded children into m_hMapper
    auto pitem = parent.isValid() ? m_hMapper.value(parent) : m_pRoot;

    if (parent.isValid() && pitem == m_pRoot)
        return false;

    if ((!pitem->m_pLastChild) || (!pitem->m_pFirstChild))
        return true;

    if (pitem->m_pLastChild->m_Index.row() >= first)
        return true;

    if (pitem->m_pFirstChild->m_Index.row() <= last)
        return true;

    return false;
}

/// Add new entries to the mapping
TreeTraversalItems* TreeView2Private::addChildren(TreeTraversalItems* parent, const QModelIndex& index)
{
    Q_ASSERT(index.isValid());
    Q_ASSERT(index.parent() != index);

    auto e = new TreeTraversalItems(parent, this);
    e->m_Index = index;

    m_hMapper        [index] = e;
    parent->m_hLookup[index] = e;

    return e;
}

/// Make sure all elements exists all the way to the root
void TreeView2Private::initTree(const QModelIndex& parent)
{
    //
}

void TreeView2Private::cleanup()
{
    //TODO remove the leaks, use the state machine

    std::function<void(TreeTraversalItems*)>* deleter;
    std::function<void(TreeTraversalItems*)> del = [this, &deleter](TreeTraversalItems* item) {
        for (auto i = item->m_hLookup.begin(); i != item->m_hLookup.end(); i++) {
            TreeTraversalItems* child = i.value();
            (*deleter)(child);
            child->m_pTreeItem->performAction(VolatileTreeItem::Action::LEAVE_BUFFER);
            child->performAction(TreeTraversalItems::Action::DETACH);
        }
    };
    deleter = &del;

    (*deleter)(m_pRoot);

    m_pRoot->m_hLookup.clear();
    m_hMapper.clear();
    m_pRoot->m_pFirstChild = nullptr;
    m_pRoot->m_pLastChild  = nullptr;
}

FlickableView::ModelIndexItem* TreeView2::itemForIndex(const QModelIndex& idx) const
{
    if (!idx.isValid())
        return nullptr;

    if (!idx.parent().isValid()) {
        const auto tti = d_ptr->m_pRoot->m_hLookup[idx];
        return tti ? tti->m_pTreeItem : nullptr;
    }

    if (auto parent = d_ptr->m_hMapper[idx.parent()]) {
        const auto tti = parent->m_hLookup[idx];
        return tti ? tti->m_pTreeItem : nullptr;
    }

    return nullptr;
}

void TreeView2::reload()
{
    if (d_ptr->m_hMapper.isEmpty())
        return;


}

void TreeView2Private::_test_validateTree(TreeTraversalItems* p)
{
    // The asserts below only work on valid models with valid delegates.
    // If those conditions are not met, it *could* work anyway, but cannot be
    // validated.
    Q_ASSERT(m_FailedCount >= 0);
    if (m_FailedCount) {
        qWarning() << "The tree is fragmented and failed to self heal: disable validation";
        return;
    }

    // First, let's check the linked list to avoid running more test on really
    // corrupted data
    if (auto i = p->m_pFirstChild) {
        auto idx = i->m_Index;

        while ((i = i->m_pNext)) {
            Q_ASSERT(i->m_pPrevious->m_Index == idx);
            Q_ASSERT(i->m_Index.row() == idx.row()+1);
            Q_ASSERT(i->m_pPrevious->m_pNext == i);
            idx = i->m_Index;
        }
    }

    // Do that again in the other direction
    if (auto i = p->m_pLastChild) {
        auto idx = i->m_Index;

        while ((i = i->m_pPrevious)) {
            Q_ASSERT(i->m_pNext->m_Index == idx);
            Q_ASSERT(i->m_Index.row() == idx.row()-1);
            Q_ASSERT(i->m_pNext->m_pPrevious == i);
            idx = i->m_Index;
        }
    }

    //TODO remove once stable
    // Brute force recursive validations
    TreeTraversalItems *old(nullptr), *newest(nullptr);
    for (auto i = p->m_hLookup.constBegin(); i != p->m_hLookup.constEnd(); i++) {
        if ((!newest) || i.key().row() < newest->m_Index.row())
            newest = i.value();

        if ((!old) || i.key().row() > old->m_Index.row())
            old = i.value();

        // Check that m_FailedCount is valid
        Q_ASSERT(i.value()->m_pTreeItem->m_State != VolatileTreeItem::State::FAILED);

        // Test the indices
        Q_ASSERT(i.key().internalPointer() == i.value()->m_Index.internalPointer());
        Q_ASSERT((p->m_Index.isValid()) || p->m_Index.internalPointer() != i.key().internalPointer());
        Q_ASSERT(old == i.value() || old->m_Index.row() > i.key().row());
        Q_ASSERT(newest == i.value() || newest->m_Index.row() < i.key().row());

        // Test that there is no trivial duplicate TreeTraversalItems for the same index
        if(i.value()->m_pPrevious && i.value()->m_pPrevious->m_hLookup.isEmpty()) {
            Q_ASSERT(i.value()->m_pTreeItem->previous()->m_pParent == i.value()->m_pPrevious);
            Q_ASSERT(i.value()->m_pTreeItem->previous()->next()->m_pParent == i.value());
        }

        // Test the virtual linked list between the leafs and branches
        if(auto next = i.value()->m_pTreeItem->next()) {
            Q_ASSERT(next->previous() == i.value()->m_pTreeItem);
            Q_ASSERT(next->m_pParent != i.value());
        }
        else {
            // There is always a next is those conditions are not met unless there
            // is failed elements creating (auto-corrected) holes in the chains.
            Q_ASSERT(!i.value()->m_pNext);
            Q_ASSERT(i.value()->m_hLookup.isEmpty());
        }

        if(auto prev = i.value()->m_pTreeItem->previous()) {
            Q_ASSERT(prev->next() == i.value()->m_pTreeItem);
            Q_ASSERT(prev->m_pParent != i.value());
        }
        else {
            // There is always a previous if those conditions are not met unless there
            // is failed elements creating (auto-corrected) holes in the chains.
            Q_ASSERT(!i.value()->m_pPrevious);
            Q_ASSERT(!i.value()->m_pParent->m_pTreeItem);
        }

        _test_validateTree(i.value());
    }

    // Test that the list edges are valid
    Q_ASSERT(!(!!p->m_pLastChild ^ !!p->m_pFirstChild));
    Q_ASSERT(p->m_pLastChild  == old);
    Q_ASSERT(p->m_pFirstChild == newest);
}

void TreeView2Private::slotRowsInserted(const QModelIndex& parent, int first, int last)
{
    Q_ASSERT((!parent.isValid()) || parent.model() == q_ptr->model());
//     qDebug() << "\n\nADD" << first << last;

    if (!isActive(parent, first, last))
        return;

//     qDebug() << "\n\nADD2" << q_ptr->width() << q_ptr->height();

    auto pitem = parent.isValid() ? m_hMapper.value(parent) : m_pRoot;

    TreeTraversalItems *prev(nullptr);

    //FIXME use previous()
    if (first && pitem)
        prev = pitem->m_hLookup.value(q_ptr->model()->index(first-1, 0, parent));

    //FIXME support smaller ranges
    for (int i = first; i <= last; i++) {
        auto idx = q_ptr->model()->index(i, 0, parent);
        Q_ASSERT(idx.parent() != idx);

        auto e = addChildren(pitem, idx);

        // Keep a dual chained linked list between the visual elements
        e->m_pPrevious = prev ? prev : nullptr; //FIXME incorrect

        //FIXME It can happen if the previous is out of the visible range
        Q_ASSERT( e->m_pPrevious || e->m_Index.row() == 0);

        if (prev)
            prev->m_pNext = e;

        e->performAction(TreeTraversalItems::Action::ATTACH);

        if ((!pitem->m_pFirstChild) || e->m_Index.row() <= pitem->m_pFirstChild->m_Index.row()) {
            Q_ASSERT(pitem != e);
            pitem->m_pFirstChild = e;
            if (auto pe = e->m_pTreeItem->previous())
                pe->performAction(VolatileTreeItem::Action::MOVE);
        }

        if ((!pitem->m_pLastChild) || e->m_Index.row() > pitem->m_pLastChild->m_Index.row()) {
            Q_ASSERT(pitem != e);
            pitem->m_pLastChild = e;
            if (auto ne = e->m_pTreeItem->next())
                ne->performAction(VolatileTreeItem::Action::MOVE);
        }

        if (auto rc = q_ptr->model()->rowCount(idx))
            slotRowsInserted(idx, 0, rc-1);

        prev = e;
    }

    if ((!pitem->m_pLastChild) || last > pitem->m_pLastChild->m_Index.row())
        pitem->m_pLastChild = prev;

    Q_ASSERT(pitem->m_pLastChild);

    //FIXME use next()
    if (q_ptr->model()->rowCount(parent) > last) {
        if (auto i = pitem->m_hLookup.value(q_ptr->model()->index(last+1, 0, parent))) {
            i->m_pPrevious = prev;
            prev->m_pNext = i;
        }
//         else //FIXME it happens
//             Q_ASSERT(false);
    }

    _test_validateTree(m_pRoot);

    Q_EMIT q_ptr->contentChanged();
}

void TreeView2Private::slotRowsRemoved(const QModelIndex& parent, int first, int last)
{
    Q_ASSERT((!parent.isValid()) || parent.model() == q_ptr->model());
    Q_EMIT q_ptr->contentChanged();

    if (!isActive(parent, first, last))
        return;
}

void TreeView2Private::slotLayoutChanged()
{
    cleanup();
    Q_EMIT q_ptr->contentChanged();
}

void TreeView2Private::slotRowsMoved(const QModelIndex &parent, int start, int end,
                                     const QModelIndex &destination, int row)
{
    Q_ASSERT((!parent.isValid()) || parent.model() == q_ptr->model());
    Q_ASSERT((!destination.isValid()) || destination.model() == q_ptr->model());

    if ((!isActive(parent, start, end)) && !isActive(destination, row, row+(end-start)))
        return;

    //FIXME implement, it will crash if ignored
    Q_ASSERT(false);
}

void TreeView2Private::slotDataChanged(const QModelIndex& tl, const QModelIndex& br)
{
    if (!isActive(tl.parent(), tl.row(), br.row()))
        return;
}

bool VolatileTreeItem::performAction(Action a)
{
    if (m_State == VolatileTreeItem::State::FAILED)
        m_pParent->d_ptr->m_FailedCount--;

    const int s = (int)m_State;
    m_State     = m_fStateMap [s][(int)a];
    Q_ASSERT(m_State != VolatileTreeItem::State::ERROR);

    const bool ret = (this->*m_fStateMachine[s][(int)a])();

    if (m_State == VolatileTreeItem::State::FAILED || !ret) {
        m_State = VolatileTreeItem::State::FAILED;
        m_pParent->d_ptr->m_FailedCount++;
    }

    return ret;
}

QWeakPointer<FlickableView::ModelIndexItem> VolatileTreeItem::reference() const
{
    if (!m_pSelf)
        m_pSelf = QSharedPointer<VolatileTreeItem>(
            const_cast<VolatileTreeItem*>(this)
        );

    return m_pSelf;
}

int VolatileTreeItem::depth() const
{
    return m_pParent->m_Depth;
}

TreeView2* VolatileTreeItem::view() const
{
    return m_pParent->d_ptr->q_ptr;
}

QModelIndex VolatileTreeItem::index() const
{
    return m_pParent->m_Index;
}

/**
 * Flatten the tree as a linked list.
 *
 * Returns the previous non-failed item.
 */
VolatileTreeItem* VolatileTreeItem::previous() const
{
    TreeTraversalItems* ret = nullptr;

    // Another simple case, there is no parent
    if (!m_pParent->m_pParent) {
        Q_ASSERT(!index().parent().isValid()); //TODO remove, no longer true when partial loading is implemented

        return nullptr;
    }

    // The parent has no previous siblings, therefor it is directly above the item
    if (!m_pParent->m_pPrevious) {
        Q_ASSERT(index().row() == 0);

        // This is the root, there is no previous element
        if (!m_pParent->m_pParent->m_pTreeItem) {
            Q_ASSERT(!index().parent().isValid());
            return nullptr;
        }

        ret = m_pParent->m_pParent;

        // Avoids useless unreadable indentation
        goto sanitize;
    }

    ret = m_pParent->m_pPrevious;

    while (ret->m_pLastChild)
        ret = ret->m_pLastChild;

sanitize:

    // Recursively look for a valid element. Doing this here allows the views
    // that implement this (abstract) class to work without having to always
    // check if some of their item failed to load. This is non-fatal in the
    // other Qt views, so it isn't fatal here either.
    if (ret->m_pTreeItem->m_State == VolatileTreeItem::State::FAILED)
        return ret->m_pTreeItem->previous();

    return ret->m_pTreeItem;
}

/**
 * Flatten the tree as a linked list.
 *
 * Returns the next non-failed item.
 */
VolatileTreeItem* VolatileTreeItem::next() const
{
    VolatileTreeItem* ret = nullptr;
    auto i = m_pParent;

    if (m_pParent->m_pFirstChild) {
        Q_ASSERT(m_pParent->m_pFirstChild->m_Index.row() == 0);
        ret = m_pParent->m_pFirstChild->m_pTreeItem;
        goto sanitizeNext;
    }


    // Recursively unwrap the tree until an element is found
    while(i) {
        if (i->m_pNext) {
            ret = i->m_pNext->m_pTreeItem;
            goto sanitizeNext;
        }

        i = i->m_pParent;
    }

    // Can't happen, exists to detect corrupted code
    if (m_pParent->m_Index.parent().isValid()) {
        Q_ASSERT(m_pParent->m_pParent);
        Q_ASSERT(m_pParent->m_pParent->m_pParent->m_hLookup.size()
            == m_pParent->m_Index.parent().row()+1);
    }

sanitizeNext:

    // Recursively look for a valid element. Doing this here allows the views
    // that implement this (abstract) class to work without having to always
    // check if some of their item failed to load. This is non-fatal in the
    // other Qt views, so it isn't fatal here either.
    if (ret && ret->m_State == VolatileTreeItem::State::FAILED)
        return ret->next();

    return ret;
}

bool VolatileTreeItem::nothing()
{
    return true;
}

bool VolatileTreeItem::error()
{
    Q_ASSERT(false);
}

bool VolatileTreeItem::destroy()
{
    //TODO check if the item has references,  if it does, just release the shared
    // pointer and move on.
    m_pSelf = nullptr;

    delete this;
    //noreturn
}

bool TreeTraversalItems::performAction(Action a)
{
    const int s = (int)m_State;
    m_State     = m_fStateMap            [s][(int)a];
    bool ret    = (this->*m_fStateMachine[s][(int)a])();

    return ret;
}

bool TreeTraversalItems::nothing()
{ return true; }

bool TreeTraversalItems::error()
{
    Q_ASSERT(false);
}

bool TreeTraversalItems::show()
{
//     qDebug() << "SHOW";

    if (!m_pTreeItem) {
        m_pTreeItem = static_cast<VolatileTreeItem*>(d_ptr->q_ptr->createItem());
        m_pTreeItem->m_pParent = this;
    }

    m_pTreeItem->performAction(VolatileTreeItem::Action::ENTER_BUFFER);
    m_pTreeItem->performAction(VolatileTreeItem::Action::ENTER_VIEW  );

    return true;
}

bool TreeTraversalItems::hide()
{
//     qDebug() << "HIDE";
    return true;
}

bool TreeTraversalItems::attach()
{
    if (m_pTreeItem)
        m_pTreeItem->performAction(VolatileTreeItem::Action::ATTACH);

//     qDebug() << "ATTACH" << (int)m_State;
    performAction(Action::MOVE); //FIXME don't
    return performAction(Action::SHOW); //FIXME don't
}

bool TreeTraversalItems::detach()
{
    return true;
}

bool TreeTraversalItems::refresh()
{
    //
//     qDebug() << "REFRESH";

    for (auto i = m_hLookup.constBegin(); i != m_hLookup.constEnd(); i++)
        i.value()->performAction(TreeTraversalItems::Action::UPDATE);

    return true;
}

bool TreeTraversalItems::index()
{
    m_Depth = m_pParent ? m_pParent->m_Depth : 0;

//     if (m_Depth >= d_ptr->m_DepthChart.size())
//         d_ptr->m_DepthChart.resize(m_Depth+1);

    // Propagate
    for (auto i = m_hLookup.constBegin(); i != m_hLookup.constEnd(); i++)
        i.value()->performAction(TreeTraversalItems::Action::MOVE);

    if (m_pTreeItem)
        m_pTreeItem->performAction(VolatileTreeItem::Action::MOVE); //FIXME don't

    return true;
}

bool TreeTraversalItems::destroy()
{
    m_pTreeItem->performAction(VolatileTreeItem::Action::DETACH); //FIXME keep in buffer

    delete this;
    //noreturn
}

bool TreeView2Private::nothing()
{ return true; }

bool TreeView2Private::resetScoll()
{
    return true;
}

bool TreeView2Private::refresh()
{
    // Propagate
    for (auto i = m_pRoot->m_hLookup.constBegin(); i != m_pRoot->m_hLookup.constEnd(); i++)
        i.value()->performAction(TreeTraversalItems::Action::UPDATE);

    return true;
}

bool TreeView2Private::refreshFront()
{
    return true;
}

bool TreeView2Private::refreshBack()
{
    return true;
}

bool TreeView2Private::error()
{
    Q_ASSERT(false);
}

#include <treeview2.moc>
