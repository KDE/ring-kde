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

class VolatileTreeItemPrivate
{
public:

};

#define S VolatileTreeItem::State::
const VolatileTreeItem::State VolatileTreeItem::m_fStateMap[5][7] = {
/*              ATTACH ENTER_BUFFER ENTER_VIEW UPDATE    MOVE   LEAVE_BUFFER  DETACH  */
/*POOLED  */ { S POOLED, S BUFFER, S ERROR , S ERROR , S ERROR , S ERROR , S DANGLING },
/*BUFFER  */ { S ERROR , S ERROR , S ACTIVE, S BUFFER, S ERROR , S POOLED, S DANGLING },
/*ACTIVE  */ { S ERROR , S BUFFER, S ERROR , S ACTIVE, S ACTIVE, S POOLED, S DANGLING },
/*DANGLING*/ { S ERROR , S ERROR , S ERROR , S ERROR , S ERROR , S ERROR , S DANGLING },
/*ERROR   */ { S ERROR , S ERROR , S ERROR , S ERROR , S ERROR , S ERROR , S DANGLING },
};
#undef S

#define A &VolatileTreeItem::
const VolatileTreeItem::StateF VolatileTreeItem::m_fStateMachine[5][7] = {
/*             ATTACH  ENTER_BUFFER  ENTER_VIEW   UPDATE     MOVE   LEAVE_BUFFER  DETACH  */
/*POOLED  */ { A nothing, A attach , A error  , A error  , A error  , A error , A destroy },
/*BUFFER  */ { A error  , A error  , A move   , A refresh, A error  , A detach, A destroy },
/*ACTIVE  */ { A error  , A nothing, A error  , A refresh, A move   , A detach, A destroy },
/*DANGLING*/ { A error  , A error  , A error  , A error  , A error  , A error , A destroy },
/*error   */ { A error  , A error  , A error  , A error  , A error  , A error , A destroy },
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
    TreeTraversalItems* m_pParent;
    State m_State {State::BUFFER};
    TreeTraversalItems* m_pFirstChild {nullptr};
    TreeTraversalItems* m_pLastChild {nullptr};
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
    TreeView2::RecyclingMode m_RecyclingMode {
        TreeView2::RecyclingMode::NoRecycling
    };
    State m_State {State::UNFILLED};

    TreeTraversalItems* m_pRoot {new TreeTraversalItems(nullptr, this)};
    QHash<QPersistentModelIndex, TreeTraversalItems*> m_hMapper;

    // Helpers
    bool isActive(const QModelIndex& parent, int first, int last);
    void initTree(const QModelIndex& parent);
    TreeTraversalItems* addChildren(TreeTraversalItems* parent, const QModelIndex& index);
    void cleanup();

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
    connect(model().data(), &QAbstractItemModel::rowsMoved, d_ptr,
        &TreeView2Private::slotRowsMoved);
    connect(model().data(), &QAbstractItemModel::dataChanged, d_ptr,
        &TreeView2Private::slotDataChanged  );

    if (auto rc = m->rowCount())
        d_ptr->slotRowsInserted({}, 0, rc);
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
    d_ptr->m_pRoot->performAction(TreeTraversalItems::Action::MOVE);
    contentItem()->setWidth(newGeometry.width());
    FlickableView::geometryChanged(newGeometry, oldGeometry);
}

/// Return true if the indices affect the current view
bool TreeView2Private::isActive(const QModelIndex& parent, int first, int last)
{
    if (m_State == State::UNFILLED)
        return true;

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
    auto e = new TreeTraversalItems(parent, this);
    e->m_Index = index;

    m_hMapper        [index] = e;
    parent->m_hLookup[index] = e;

    if ((!parent->m_pFirstChild) || index.row() <= parent->m_pFirstChild->m_Index.row())
        parent->m_pFirstChild = e;

    if ((!parent->m_pLastChild) || index.row() > parent->m_pLastChild->m_Index.row())
        parent->m_pLastChild = e;

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
    m_hMapper.clear();
    m_pRoot->m_pFirstChild = nullptr;
    m_pRoot->m_pLastChild  = nullptr;
    m_pRoot->m_hLookup.clear();
}

void TreeView2Private::slotRowsInserted(const QModelIndex& parent, int first, int last)
{
    qDebug() << "\n\nADD" << first << last;
    if (!isActive(parent, first, last))
        return;
    qDebug() << "\n\nADD2" << q_ptr->width() << q_ptr->height();

    auto pitem = parent.isValid() ? m_hMapper.value(parent) : m_pRoot;

    //FIXME support smaller ranges
    for (int i = first; i <= last; i++)
        addChildren(pitem, q_ptr->model()->index(i, 0, parent))
            ->performAction(TreeTraversalItems::Action::ATTACH);
}

void TreeView2Private::slotRowsRemoved(const QModelIndex& parent, int first, int last)
{
    if (!isActive(parent, first, last))
        return;
}

void TreeView2Private::slotLayoutChanged()
{
    cleanup();
}

void TreeView2Private::slotRowsMoved(const QModelIndex &parent, int start, int end,
                                     const QModelIndex &destination, int row)
{
    if ((!isActive(parent, start, end)) && !isActive(destination, row, row+(end-start)))
        return;
}


void TreeView2Private::slotDataChanged(const QModelIndex& tl, const QModelIndex& br)
{
    if (!isActive(tl.parent(), tl.row(), br.row()))
        return;
}

bool VolatileTreeItem::performAction(Action a)
{
    const int s = (int)m_State;
    m_State     = m_fStateMap            [s][(int)a];
    bool ret    = (this->*m_fStateMachine[s][(int)a])();

    return ret;
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
    qDebug() << "SHOW";

    if (!m_pTreeItem) {
        m_pTreeItem = d_ptr->q_ptr->createItem();
        m_pTreeItem->m_pParent = this;
    }

    m_pTreeItem->performAction(VolatileTreeItem::Action::ENTER_BUFFER);
    m_pTreeItem->performAction(VolatileTreeItem::Action::ENTER_VIEW  );

    return true;
}

bool TreeTraversalItems::hide()
{
    qDebug() << "HIDE";
    return true;
}

bool TreeTraversalItems::attach()
{
    qDebug() << "ATTACH" << (int)m_State;
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
    qDebug() << "REFRESH";

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
