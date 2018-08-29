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
#include "abstractquickview.h"

#include <QtCore/QTimer>

#include <functional>

#include "treetraversalreflector_p.h"
#include "treetraversalrange_p.h"

#define V_ITEM(i) i


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

/*
 * The visual elements state changes.
 *
 * Note that the ::FAILED elements will always try to self-heal themselves and
 * go back into FAILED once the self-healing itself failed.
 */
#define S VisualTreeItem::State::
const VisualTreeItem::State VisualTreeItem::m_fStateMap[7][7] = {
/*              ATTACH ENTER_BUFFER ENTER_VIEW UPDATE    MOVE   LEAVE_BUFFER  DETACH  */
/*POOLING */ { S POOLING, S BUFFER, S ERROR , S ERROR , S ERROR , S ERROR  , S DANGLING },
/*POOLED  */ { S POOLED , S BUFFER, S ERROR , S ERROR , S ERROR , S ERROR  , S DANGLING },
/*BUFFER  */ { S ERROR  , S ERROR , S ACTIVE, S BUFFER, S ERROR , S POOLING, S DANGLING },
/*ACTIVE  */ { S ERROR  , S BUFFER, S ERROR , S ACTIVE, S ACTIVE, S POOLING, S POOLING  },
/*FAILED  */ { S ERROR  , S BUFFER, S ACTIVE, S ACTIVE, S ACTIVE, S POOLING, S DANGLING },
/*DANGLING*/ { S ERROR  , S ERROR , S ERROR , S ERROR , S ERROR , S ERROR  , S DANGLING },
/*ERROR   */ { S ERROR  , S ERROR , S ERROR , S ERROR , S ERROR , S ERROR  , S DANGLING },
};
#undef S

#define A &VisualTreeItem::
const VisualTreeItem::StateF VisualTreeItem::m_fStateMachine[7][7] = {
/*             ATTACH  ENTER_BUFFER  ENTER_VIEW   UPDATE     MOVE   LEAVE_BUFFER  DETACH  */
/*POOLING */ { A error  , A error  , A error  , A error  , A error  , A error  , A error   },
/*POOLED  */ { A nothing, A attach , A error  , A error  , A error  , A error  , A destroy },
/*BUFFER  */ { A error  , A error  , A move   , A refresh, A error  , A detach , A destroy },
/*ACTIVE  */ { A error  , A nothing, A error  , A refresh, A move   , A detach , A detach  },
/*FAILED  */ { A error  , A attach , A attach , A attach , A attach , A detach , A destroy },
/*DANGLING*/ { A error  , A error  , A error  , A error  , A error  , A error  , A destroy },
/*error   */ { A error  , A error  , A error  , A error  , A error  , A error  , A destroy },
};
#undef A

class AbstractQuickViewPrivate final : public QObject
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

    typedef bool(AbstractQuickViewPrivate::*StateF)();

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
    AbstractQuickView::RecyclingMode m_RecyclingMode {
        AbstractQuickView::RecyclingMode::NoRecycling
    };
    State m_State {State::UNFILLED};

    TreeTraversalReflector* m_pReflector {nullptr};

    TreeTraversalRange* m_pRange {nullptr};

    AbstractQuickView* q_ptr;

private:
    bool nothing     ();
    bool resetScoll  ();
    bool refresh     ();
    bool refreshFront();
    bool refreshBack ();
    bool error       () __attribute__ ((noreturn));

public Q_SLOTS:
    void slotViewportChanged();
    void slotDataChanged(const QModelIndex& tl, const QModelIndex& br);
    void slotContentChanged();
    void slotCountChanged();
};

#define S AbstractQuickViewPrivate::State::
const AbstractQuickViewPrivate::State AbstractQuickViewPrivate::m_fStateMap[5][5] = {
/*              INSERTION    REMOVAL       MOVE     RESET_SCROLL    SCROLL  */
/*UNFILLED */ { S ANCHORED, S UNFILLED , S UNFILLED , S UNFILLED, S UNFILLED },
/*ANCHORED */ { S ANCHORED, S ANCHORED , S ANCHORED , S ANCHORED, S SCROLLED },
/*SCROLLED */ { S SCROLLED, S SCROLLED , S SCROLLED , S ANCHORED, S SCROLLED },
/*AT_END   */ { S AT_END  , S AT_END   , S AT_END   , S ANCHORED, S SCROLLED },
/*ERROR    */ { S ERROR   , S ERROR    , S ERROR    , S ERROR   , S ERROR    },
};
#undef S

#define A &AbstractQuickViewPrivate::
const AbstractQuickViewPrivate::StateF AbstractQuickViewPrivate::m_fStateMachine[5][5] = {
/*              INSERTION           REMOVAL          MOVE        RESET_SCROLL     SCROLL  */
/*UNFILLED*/ { A refreshFront, A refreshFront , A refreshFront , A nothing   , A nothing  },
/*ANCHORED*/ { A refreshFront, A refreshFront , A refreshFront , A nothing   , A refresh  },
/*SCROLLED*/ { A refresh     , A refresh      , A refresh      , A resetScoll, A refresh  },
/*AT_END  */ { A refreshBack , A refreshBack  , A refreshBack  , A resetScoll, A refresh  },
/*ERROR   */ { A error       , A error        , A error        , A error     , A error    },
};
#undef A

AbstractQuickView::AbstractQuickView(QQuickItem* parent) : FlickableView(parent),
    d_ptr(new AbstractQuickViewPrivate())
{
    d_ptr->m_pReflector = new TreeTraversalReflector(this);

    d_ptr->m_pRange = new TreeTraversalRange();
    d_ptr->m_pReflector->addRange(d_ptr->m_pRange);

    d_ptr->q_ptr = this;

    d_ptr->m_pReflector->setItemFactory([this]() -> VisualTreeItem* {
        return V_ITEM(d_ptr->q_ptr->createItem());
    });

    connect(this, &AbstractQuickView::currentYChanged,
        d_ptr, &AbstractQuickViewPrivate::slotViewportChanged);
    connect(d_ptr->m_pReflector, &TreeTraversalReflector::contentChanged,
        d_ptr, &AbstractQuickViewPrivate::slotContentChanged);
    connect(d_ptr->m_pReflector, &TreeTraversalReflector::countChanged,
        d_ptr, &AbstractQuickViewPrivate::slotCountChanged);
}

AbstractQuickView::~AbstractQuickView()
{;
    delete d_ptr;
}

void AbstractQuickView::setModel(QSharedPointer<QAbstractItemModel> m)
{
    if (m == model())
        return;

    d_ptr->m_pReflector->setModel(m.data());

    if (auto oldM = model())
        disconnect(oldM.data(), &QAbstractItemModel::dataChanged, d_ptr,
            &AbstractQuickViewPrivate::slotDataChanged);

    FlickableView::setModel(m);

    connect(m.data(), &QAbstractItemModel::dataChanged, d_ptr,
        &AbstractQuickViewPrivate::slotDataChanged);

    d_ptr->m_pReflector->populate();
}

bool AbstractQuickView::hasUniformRowHeight() const
{
    return d_ptr->m_UniformRowHeight;
}

void AbstractQuickView::setUniformRowHeight(bool value)
{
    d_ptr->m_UniformRowHeight = value;
}

bool AbstractQuickView::hasUniformColumnWidth() const
{
    return d_ptr->m_UniformColumnWidth;
}

void AbstractQuickView::setUniformColumnColumnWidth(bool value)
{
    d_ptr->m_UniformColumnWidth = value;
}

bool AbstractQuickView::isCollapsable() const
{
    return d_ptr->m_Collapsable;
}

void AbstractQuickView::setCollapsable(bool value)
{
    d_ptr->m_Collapsable = value;
}

bool AbstractQuickView::isAutoExpand() const
{
    return d_ptr->m_AutoExpand;
}

void AbstractQuickView::setAutoExpand(bool value)
{
    d_ptr->m_AutoExpand = value;
}

int AbstractQuickView::maxDepth() const
{
    return d_ptr->m_MaxDepth;
}

void AbstractQuickView::setMaxDepth(int depth)
{
    d_ptr->m_MaxDepth = depth;
}

int AbstractQuickView::cacheBuffer() const
{
    return d_ptr->m_CacheBuffer;
}

void AbstractQuickView::setCacheBuffer(int value)
{
    d_ptr->m_CacheBuffer = value;
}

int AbstractQuickView::poolSize() const
{
    return d_ptr->m_PoolSize;
}

void AbstractQuickView::setPoolSize(int value)
{
    d_ptr->m_PoolSize = value;
}

AbstractQuickView::RecyclingMode AbstractQuickView::recyclingMode() const
{
    return d_ptr->m_RecyclingMode;
}

void AbstractQuickView::setRecyclingMode(AbstractQuickView::RecyclingMode mode)
{
    d_ptr->m_RecyclingMode = mode;
}

void AbstractQuickView::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    FlickableView::geometryChanged(newGeometry, oldGeometry);
//     d_ptr->m_pReflector->m_pRoot->performAction(TreeTraversalItems::Action::MOVE);
    d_ptr->m_pReflector->moveEverything(); //FIXME use the ranges
    contentItem()->setWidth(newGeometry.width());
}

void AbstractQuickView::reloadChildren(const QModelIndex& index) const
{
    /*
    if (auto p = d_ptr->m_pReflector->ttiForIndex(index)) {
        auto c = p->m_tChildren[FIRST];

        while (c && c != p->m_tChildren[LAST]) {
            if (c->m_pTreeItem) {
                c->m_pTreeItem->performAction( VisualTreeItem::Action::UPDATE );
                c->m_pTreeItem->performAction( VisualTreeItem::Action::MOVE   );
            }
            c = c->m_tSiblings[NEXT];
        }
    }*/

    d_ptr->m_pReflector->reloadRange(index);

    /*for (auto item : d_ptr->m_pRange->subset(index)) {
        item->performAction( TreeTraversalReflector::ViewAction::UPDATE );
        item->performAction( TreeTraversalReflector::ViewAction::MOVE   );
    }*/
}

QQuickItem* AbstractQuickView::parentTreeItem(const QModelIndex& index) const
{
    const auto i = d_ptr->m_pReflector->parentTreeItem(index);

    return i ? i->item() : nullptr;
}

VisualTreeItem* AbstractQuickView::itemForIndex(const QModelIndex& idx) const
{
    return d_ptr->m_pReflector->itemForIndex(idx);
}

void AbstractQuickView::reload()
{
    /*if (d_ptr->m_pReflector->m_hMapper.isEmpty())
        return;*/
}

void AbstractQuickViewPrivate::slotContentChanged()
{
    emit q_ptr->contentChanged();
}

void AbstractQuickViewPrivate::slotCountChanged()
{
    emit q_ptr->countChanged();
}

void AbstractQuickViewPrivate::slotDataChanged(const QModelIndex& tl, const QModelIndex& br)
{
    if (tl.model() && tl.model() != q_ptr->model()) {
        Q_ASSERT(false);
        return;
    }

    if (br.model() && br.model() != q_ptr->model()) {
        Q_ASSERT(false);
        return;
    }

    if ((!tl.isValid()) || (!br.isValid()))
        return;

    if (!m_pReflector->isActive(tl.parent(), tl.row(), br.row()))
        return;

    //FIXME tolerate other cases
    Q_ASSERT(q_ptr->model());
    Q_ASSERT(tl.model() == q_ptr->model() && br.model() == q_ptr->model());
    Q_ASSERT(tl.parent() == br.parent());

    //TODO Use a smaller range when possible

    //itemForIndex(const QModelIndex& idx) const final override;
    for (int i = tl.row(); i <= br.row(); i++) {
        const auto idx = q_ptr->model()->index(i, tl.column(), tl.parent());
        if (auto item = V_ITEM(q_ptr->itemForIndex(idx)))
            item->performAction(TreeTraversalReflector::ViewAction::UPDATE);
    }
}

void AbstractQuickViewPrivate::slotViewportChanged()
{
    //Q_ASSERT((!m_pReflector->m_pRoot->m_tChildren[FIRST]) || m_pReflector->m_tVisibleTTIRange[FIRST]);
    //Q_ASSERT((!m_pReflector->m_pRoot->m_tChildren[LAST ]) || m_pReflector->m_tVisibleTTIRange[LAST ]);
}

bool VisualTreeItem::performAction(TreeTraversalReflector::ViewAction a)
{
    //if (m_State == VisualTreeItem::State::FAILED)
    //    m_pTTI->d_ptr->m_FailedCount--;

    const int s = (int)m_State;
    m_State     = m_fStateMap [s][(int)a];
    Q_ASSERT(m_State != VisualTreeItem::State::ERROR);

    const bool ret = (this->*m_fStateMachine[s][(int)a])();

    if (m_State == VisualTreeItem::State::FAILED || !ret) {
        Q_ASSERT(false);
        m_State = VisualTreeItem::State::FAILED;
        //m_pTTI->d_ptr->m_FailedCount++;
    }

    return ret;
}

QWeakPointer<VisualTreeItem> VisualTreeItem::reference() const
{
    if (!m_pSelf)
        m_pSelf = QSharedPointer<VisualTreeItem>(
            const_cast<VisualTreeItem*>(this)
        );

    return m_pSelf;
}

int VisualTreeItem::depth() const
{
    return 0;//FIXME m_pTTI->m_Depth;
}

bool VisualTreeItem::fitsInView() const
{
    const auto geo  = geometry();
    const auto v = view()->visibleRect();

    //TODO support horizontal visibility
    return geo.y() >= v.y()
        && (geo.y() <= v.y() + v.height());
}

bool VisualTreeItem::isVisible() const
{
    return view()->d_ptr->m_pReflector->isItemVisible(this);
}

QPersistentModelIndex VisualTreeItem::index() const
{
    return view()->d_ptr->m_pReflector->indexForItem(this);
}

/**
 * Flatten the tree as a linked list.
 *
 * Returns the previous non-failed item.
 */
VisualTreeItem* VisualTreeItem::up() const
{
    Q_ASSERT(m_State == State::ACTIVE
        || m_State == State::BUFFER
        || m_State == State::FAILED
        || m_State == State::POOLING
        || m_State == State::DANGLING //FIXME add a new state for
        // "deletion in progress" or swap the f call and set state
    );

    return V_ITEM(m_pView->d_ptr->m_pReflector->up(this));
}

/**
 * Flatten the tree as a linked list.
 *
 * Returns the next non-failed item.
 */
VisualTreeItem* VisualTreeItem::down() const
{
    Q_ASSERT(m_State == State::ACTIVE
        || m_State == State::BUFFER
        || m_State == State::FAILED
        || m_State == State::POOLING
        || m_State == State::DANGLING //FIXME add a new state for
        // "deletion in progress" or swap the f call and set state
    );

    return V_ITEM(m_pView->d_ptr->m_pReflector->down(this));
}


int VisualTreeItem::row() const
{
    return V_ITEM(m_pView->d_ptr->m_pReflector->row(this));
}

int VisualTreeItem::column() const
{
    return V_ITEM(m_pView->d_ptr->m_pReflector->column(this));
}

bool VisualTreeItem::nothing()
{
    return true;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
bool VisualTreeItem::error()
{
    Q_ASSERT(false);
    return true;
}
#pragma GCC diagnostic pop

bool VisualTreeItem::destroy()
{
    auto ptrCopy = m_pSelf;

    QTimer::singleShot(0,[this, ptrCopy]() {
        if (!ptrCopy)
            delete this;
        // else the reference will be dropped and the destructor called
    });

    m_pSelf.clear();
    return true;
}

bool AbstractQuickViewPrivate::nothing()
{ return true; }

bool AbstractQuickViewPrivate::resetScoll()
{
    return true;
}

bool AbstractQuickViewPrivate::refresh()
{
    // Propagate
    m_pReflector->refreshEverything();

    return true;
}

bool AbstractQuickViewPrivate::refreshFront()
{
    return true;
}

bool AbstractQuickViewPrivate::refreshBack()
{
    return true;
}

bool AbstractQuickViewPrivate::error()
{
    Q_ASSERT(false);
    return true;
}

void VisualTreeItem::updateGeometry()
{
    const auto geo = geometry();

    //TODO handle up/left/right too

    if (!down()) {
        view()->contentItem()->setHeight(std::max(
            geo.y()+geo.height(), view()->height()
        ));

        emit view()->contentHeightChanged(view()->contentItem()->height());
    }

    if (view()->selectionModel() && view()->selectionModel()->currentIndex() == index())
        view()->updateSelection();
}

AbstractQuickView* VisualTreeItem::view() const
{
    return m_pView;
}

#include <abstractquickview.moc>
#undef V_ITEM
