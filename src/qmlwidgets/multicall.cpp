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
#include "multicall.h"

#include <call.h>

#include <QtGui/QPainter>
#include <QtGui/QIcon>
#include <QtGui/QPixmap>

class MultiCallPrivate
{
public:
    QPersistentModelIndex m_Index;

    static bool     init;
    static QPixmap* iconCache[2][2];

    MultiCall* q_ptr;

    int getHeight() const;
};

bool MultiCallPrivate::init = false;
QPixmap* MultiCallPrivate::iconCache[2][2] = {{{},{}},{{},{}}};


MultiCall::MultiCall(QQuickItem* parent) :
    QQuickPaintedItem(parent), d_ptr(new MultiCallPrivate)
{
    d_ptr->q_ptr = this;

    if (!d_ptr->init) {
        d_ptr->init = true;
        d_ptr->iconCache[1][0] = new QPixmap(QIcon(":/sharedassets/phone_dark/missed_incoming.svg").pixmap(28, 28));
        d_ptr->iconCache[1][1] = new QPixmap(QIcon(":/sharedassets/phone_dark/missed_outgoing.svg").pixmap(28, 28));
        d_ptr->iconCache[0][0] = new QPixmap(QIcon(":/sharedassets/phone_dark/incoming.svg"       ).pixmap(28, 28));
        d_ptr->iconCache[0][1] = new QPixmap(QIcon(":/sharedassets/phone_dark/outgoing.svg"       ).pixmap(28, 28));
    }

    setHeight(1); //Otherwise it will be treated as dead code
}

MultiCall::~MultiCall()
{
    delete d_ptr;
}

void MultiCall::setModelIndex(QPersistentModelIndex idx)
{
    d_ptr->m_Index = idx;

    //TODO support HiDPI
    setHeight(d_ptr->getHeight());
    update();
}

QPersistentModelIndex MultiCall::modelIndex() const
{
    return d_ptr->m_Index;
}

bool MultiCall::skipChildren() const
{
    return true;
}

int MultiCallPrivate::getHeight() const
{
    const int w(q_ptr->width());
    if ((!m_Index.isValid()) || (!w))
        return 1;

    const int rc = m_Index.model()->rowCount(m_Index);
    return 32*((rc*32)/w + ((rc*32)%w ? 1 : 0));
}

void MultiCall::paint(QPainter *painter)
{
    const int w(width());

    if ((!d_ptr->m_Index.isValid()) || (!w))
        return;

    // In case there's a resize
    const int rc = d_ptr->m_Index.model()->rowCount(d_ptr->m_Index);
    const int h  = d_ptr->getHeight();

    if (h > height() + 1 || h < height() - 1) {
        setHeight(h + 200);
        update();
        return;
    }

    const int perRow = w/32;

    for (int i = 0; i < rc; i++) {
        const auto cidx = d_ptr->m_Index.model()->index(i, 0, d_ptr->m_Index);
        if (const auto call = qvariant_cast<Call*>(cidx.data((int)Ring::Role::Object))) {
            const int col = (i/perRow) * 32;
            const int row = (i%perRow) * 32;
            painter->drawPixmap(
                QPoint{row, col},
                *d_ptr->iconCache[call->isMissed()][call->direction() == Call::Direction::OUTGOING]
            );
        }
    }

}
