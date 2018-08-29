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

#include "abstractviewitem_p.h"

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
