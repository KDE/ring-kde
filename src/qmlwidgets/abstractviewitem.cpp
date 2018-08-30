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
#include "abstractviewitem.h"

// Qt
#include <QtCore/QTimer>

#include "abstractviewitem_p.h"
#include "abstractquickview.h"

class AbstractViewItemPrivate
{
public:
    typedef bool(AbstractViewItemPrivate::*StateF)();

    // Actions
    bool attach ();
    bool refresh();
    bool move   ();
    bool flush  ();
    bool remove ();
    bool nothing();
    bool error  ();
    bool destroy();
    bool detach ();

    static const VisualTreeItem::State  m_fStateMap    [7][7];
    static const StateF m_fStateMachine[7][7];

    // Attributes
    AbstractViewItem* q_ptr;
};


/*
 * The visual elements state changes.
 *
 * Note that the ::FAILED elements will always try to self-heal themselves and
 * go back into FAILED once the self-healing itself failed.
 */
#define S VisualTreeItem::State::
const VisualTreeItem::State AbstractViewItemPrivate::m_fStateMap[7][7] = {
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

#define A &AbstractViewItemPrivate::
const AbstractViewItemPrivate::StateF AbstractViewItemPrivate::m_fStateMachine[7][7] = {
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

AbstractViewItem::AbstractViewItem(AbstractQuickView* v) :
    d_ptr(new AbstractViewItemPrivate),
    s_ptr(new VisualTreeItem(v))
{
    d_ptr->q_ptr = this;
    s_ptr->d_ptr = this;
}

AbstractViewItem::~AbstractViewItem()
{}

AbstractQuickView* AbstractViewItem::view() const
{
    return s_ptr->m_pView;
}


void AbstractViewItem::resetPosition()
{
    //
}


int AbstractViewItem::row() const
{
    return s_ptr->row();
}

int AbstractViewItem::column() const
{
    return s_ptr->column();
}

QPersistentModelIndex AbstractViewItem::index() const
{
    return s_ptr->index();
}

AbstractViewItem* AbstractViewItem::up() const
{
    const auto i = s_ptr->up();
    return i ? i->d_ptr : nullptr;
}

AbstractViewItem* AbstractViewItem::down() const
{
    const auto i = s_ptr->down();
    return i ? i->d_ptr : nullptr;
}

AbstractViewItem* AbstractViewItem::left() const
{
    const auto i = s_ptr->left();
    return i ? i->d_ptr : nullptr;
}

AbstractViewItem* AbstractViewItem::right() const
{
    const auto i = s_ptr->right();
    return i ? i->d_ptr : nullptr;
}

AbstractViewItem* AbstractViewItem::parent() const
{
    return nullptr;
}

void AbstractViewItem::updateGeometry()
{
    s_ptr->updateGeometry();
}


void VisualTreeItem::setSelected(bool v)
{
    d_ptr->setSelected(v);
}

QRectF VisualTreeItem::geometry() const
{
    return d_ptr->geometry();
}

QQuickItem* VisualTreeItem::item() const
{
    return d_ptr->item();
}

bool AbstractViewItemPrivate::attach()
{
    return q_ptr->attach();
}

bool AbstractViewItemPrivate::refresh()
{
    return q_ptr->refresh();
}

bool AbstractViewItemPrivate::move()
{
    return q_ptr->move();
}

bool AbstractViewItemPrivate::flush()
{
    return q_ptr->flush();
}

bool AbstractViewItemPrivate::remove()
{
    return q_ptr->remove();
}

// This methodwrap the removal of the element from the view
bool AbstractViewItemPrivate::detach()
{
    remove();

    //FIXME
    q_ptr->s_ptr->m_State = VisualTreeItem::State::POOLED;

    return true;
}

bool AbstractViewItemPrivate::nothing()
{
    return true;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
bool AbstractViewItemPrivate::error()
{
    Q_ASSERT(false);
    return true;
}
#pragma GCC diagnostic pop

bool AbstractViewItemPrivate::destroy()
{
    auto ptrCopy = q_ptr->s_ptr->m_pSelf;

    QTimer::singleShot(0,[this, ptrCopy]() {
        if (!ptrCopy)
            delete this;
        // else the reference will be dropped and the destructor called
    });

    q_ptr->s_ptr->m_pSelf.clear();

    return true;
}

bool VisualTreeItem::performAction(VisualTreeItem::ViewAction a)
{
    //if (m_State == VisualTreeItem::State::FAILED)
    //    m_pTTI->d_ptr->m_FailedCount--;

    const int s = (int)m_State;
    m_State     = d_ptr->d_ptr->m_fStateMap [s][(int)a];
    Q_ASSERT(m_State != VisualTreeItem::State::ERROR);

    const bool ret = (d_ptr->d_ptr ->* d_ptr->d_ptr->m_fStateMachine[s][(int)a])();

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
