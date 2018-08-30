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
};

AbstractViewItem::AbstractViewItem(AbstractQuickView* v) :
    d_ptr(new AbstractViewItemPrivate),
    s_ptr(new VisualTreeItem(v))
{
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

bool VisualTreeItem::attach()
{
    return d_ptr->attach();
}

bool VisualTreeItem::refresh()
{
    return d_ptr->refresh();
}

bool VisualTreeItem::move()
{
    return d_ptr->move();
}

bool VisualTreeItem::flush()
{
    return d_ptr->flush();
}

bool VisualTreeItem::remove()
{
    return d_ptr->remove();
}

// This methodwrap the removal of the element from the view
bool VisualTreeItem::detach()
{
    remove();
    m_State = VisualTreeItem::State::POOLED;

    return true;
}

bool VisualTreeItem::performAction(VisualTreeItem::ViewAction a)
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
