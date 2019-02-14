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

#include <libcard/event.h>

#include <QtCore/QAbstractProxyModel>
#include <QtGui/QPainter>
#include <QtGui/QIcon>
#include <QtGui/QPixmap>

#include <individualtimelinemodel.h>

class MultiCallPrivate
{
public:
    enum class Mode {
        UNDEFINED     , /*!< Initialization            */
        SINGLE_MISSED , /*!< Single missing call       */
        SINGLE_ENTRY  , /*!< Direction and length      */
        MULTI_ICON    , /*!< Single row of small icons */
        SUMMARY       , /*!< Too many calls to display */
    } m_Mode {Mode::UNDEFINED};

    QPersistentModelIndex m_Index;

    static bool     init;
    static QPixmap* iconCache[2][2];

    MultiCall* q_ptr;

    int getHeight() const;
    void updateMode();

    void paintRow    (QPainter *painter);
    void paintMissed (QPainter *painter);
    void paintSingle (QPainter *painter);
    void paintSummary(QPainter *painter);
};

bool MultiCallPrivate::init = false;
QPixmap* MultiCallPrivate::iconCache[2][2] = {
    { nullptr, nullptr }, { nullptr, nullptr }
};


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
    setImplicitHeight(1); //Otherwise it will be treated as dead code
}

MultiCall::~MultiCall()
{
    delete d_ptr;
}

void MultiCall::setModelIndex(const QPersistentModelIndex& idx)
{
    d_ptr->m_Index = idx;

    //TODO support HiDPI
    setHeight(d_ptr->getHeight());
    setImplicitHeight(d_ptr->getHeight());
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

    const int rc = m_Index.data(
        (int)IndividualTimelineModel::Role::CallCount
    ).toInt();

    return 32*((rc*32)/w + ((rc*32)%w ? 1 : 0));
}

void MultiCallPrivate::updateMode()
{
    const int count = m_Index.data(
        (int)IndividualTimelineModel::Role::CallCount
    ).toInt();

    if (count == 1) {
        const auto cidx = m_Index.model()->index(0, 0, m_Index);
        if (const auto event = qvariant_cast<Event*>(cidx.data((int)Ring::Role::Object))) {
            const bool isMissed   = event->status   () == Event::Status::X_MISSED;
            const bool isOutgoing = event->direction() == Event::Direction::INCOMING;

            if (isMissed && isOutgoing) {
                m_Mode = Mode::SINGLE_MISSED;
                return;
            }

            m_Mode = Mode::SINGLE_ENTRY;
            return;
        }
    }

    if (count <= 10) {
        m_Mode = Mode::MULTI_ICON;
        return;
    }

    m_Mode = Mode::SUMMARY;
}

void MultiCallPrivate::paintMissed(QPainter *painter)
{
    //
}

void MultiCallPrivate::paintSingle(QPainter *painter)
{
    //
}

void MultiCallPrivate::paintSummary(QPainter *painter)
{
    //
}

void MultiCallPrivate::paintRow(QPainter *painter)
{
    const int w(q_ptr->width());

    if (w < 32 || q_ptr->height() < 32)
        return;

    if ((!m_Index.isValid()) || (!w))
        return;

    // In case there's a resize
    const int rc = std::min(
        m_Index.data((int)IndividualTimelineModel::Role::CallCount).toInt(), 10
    );

    const int h  = getHeight();

    if (h > q_ptr->height() + 1 || h < q_ptr->height() - 1) {
//         setHeight(h + 200);
//         update();
        return;
    }

    const int perRow = w/32;

    // Handle the case where a QSortFilterProxyModel is being used
    const QAbstractProxyModel* pm = qobject_cast<const QAbstractProxyModel*>(m_Index.model());

    const QModelIndex pidx = pm ? pm->mapToSource(m_Index) : QModelIndex(m_Index);

    const QAbstractItemModel* m = m ? pm->sourceModel() : m_Index.model();

    q_ptr->setImplicitWidth(rc*32);

    for (int i = 0; i < rc; i++) {
        const auto cidx = m->index(i, 0, pidx);

        if (const auto event = qvariant_cast<Event*>(cidx.data((int)Ring::Role::Object))) {
            const int col = (i/perRow) * 32;
            const int row = (i%perRow) * 32;

            const bool isMissed   = event->status   () == Event::Status::X_MISSED;
            const bool isOutgoing = event->direction() == Event::Direction::OUTGOING;

            painter->drawPixmap(
                QPoint{row, col},
                *iconCache[isMissed][isOutgoing]
            );
        }
    }
}

void MultiCall::paint(QPainter *painter)
{
    d_ptr->updateMode();

    switch(d_ptr->m_Mode) {
        case MultiCallPrivate::Mode::UNDEFINED:
            break;
        case MultiCallPrivate::Mode::SINGLE_MISSED:
            d_ptr->paintMissed(painter);
            break;
        case MultiCallPrivate::Mode::SINGLE_ENTRY:
            d_ptr->paintSingle(painter);
            break;
        case MultiCallPrivate::Mode::MULTI_ICON:
            d_ptr->paintRow(painter);
            break;
        case MultiCallPrivate::Mode::SUMMARY:
            d_ptr->paintSummary(painter);
            break;
    }
}

int MultiCall::count() const
{
    if (!d_ptr->m_Index.model())
        return 0;

    return d_ptr->m_Index.model()->rowCount(d_ptr->m_Index);
}

void MultiCall::setCount(int c)
{
    Q_UNUSED(c)
    setHeight(d_ptr->getHeight());
    setImplicitHeight(height());
    update();
}
