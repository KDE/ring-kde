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
#include "cmiconengine.h"

// Qt
#include <QtGui/QPainter>
#include <QtGui/QBitmap>

// KDE
#include <KColorScheme>

// Ring
#include <contactmethod.h>

class CMIconEnginePrivate
{
public:
    ContactMethod* m_pCM;
};

CMIconEngine::CMIconEngine(ContactMethod* cm) : QIconEngine(),
    d_ptr(new CMIconEnginePrivate)
{
    d_ptr->m_pCM = cm;
}

CMIconEngine::~CMIconEngine()
{
    delete d_ptr;
}

void CMIconEngine::staticPaint(QPainter* painter, const QRect& rect, bool isPresent, bool isTracked)
{
    painter->save();

    // Given this has a transparent area, clear the rectangle
    painter->setCompositionMode(QPainter::CompositionMode_Clear);
    painter->fillRect(rect, QBrush(Qt::white));
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);

    const int s = std::min(rect.height(), rect.width());

    if (s == -1)
        return;

    const int type = (!isTracked) ? 1 : (isPresent ? 100 : 10);

    static QHash<int, QPixmap> pxCache;

    // Add the pixmap to the cache
    if (!pxCache.contains(s*456*type)) {
        QPixmap px(s, s);

        QPainter p(&px);
        p.setCompositionMode(QPainter::CompositionMode_Clear);
        p.fillRect(QRect{0,0,s,s}, QBrush(Qt::white));
        p.setCompositionMode(QPainter::CompositionMode_SourceOver);

        QIcon icn;

        switch(type) {
            case 1: {
                static QIcon i = QIcon::fromTheme(QStringLiteral("im-user"));
                icn = i;
            }
            break;
            case 10: {
                static QIcon i = QIcon::fromTheme(QStringLiteral("im-user-offline"));
                icn = i;
            }
            break;
            case 100:{
                static QIcon i = QIcon::fromTheme(QStringLiteral("im-user-online"));
                icn = i;
            }
            break;
        }

        p.drawPixmap(3,3, icn.pixmap(QSize(rect.height()-6,rect.width()-6)));

        pxCache[s*456*type] = px;
    }

    Q_ASSERT(s == pxCache[s*456*type].width());

    painter->drawPixmap(rect.x(), rect.y(), pxCache[s*456*type]);

    //Create a region where the pixmap is not fully transparent
    //FIXME add caching
    if (type) {
        static QHash<int,QRegion> r,ri;
        static QHash<int,bool> init;

        if (!init[s]) {
            r   [s] = QRegion(QPixmap::fromImage(pxCache[s].toImage().createAlphaMask()));
            ri  [s] = r[s].xored({0,0,s,s});
            init[s] = true;
        }

        static QColor presentBrush = KStatefulBrush( KColorScheme::Window, KColorScheme::PositiveText ).brush(QPalette::Normal).color();
        static QColor awayBrush    = KStatefulBrush( KColorScheme::Window, KColorScheme::NegativeText ).brush(QPalette::Normal).color();

        painter->setOpacity(0.05);
        painter->setCompositionMode(QPainter::CompositionMode_SourceAtop);
        painter->fillRect(rect,isPresent?Qt::green:Qt::red);
        painter->setCompositionMode(QPainter::CompositionMode_SourceOver);

        //Paint only outside of the pixmap, it looks better
        QRegion region(r[s]), regionInter(ri[s]);
        region.translate(rect.x(), rect.y());
        regionInter.translate(rect.x(), rect.y());
        painter->setClipRegion(regionInter);

        QPainterPath p;
        p.addRegion(region);
        QPen pen = painter->pen();
        pen.setColor(isPresent?presentBrush:awayBrush);
        painter->setBrush(Qt::NoBrush);
        painter->setRenderHint(QPainter::Antialiasing, true);

        for (int i=2;i<=9;i+=2) {
            pen.setWidth(i);
            painter->setPen(pen);
            painter->setOpacity(0.30f-(((i-2)*0.45f)/10.0f));
            painter->drawPath(p);
        }

    }

    painter->restore();
}

void CMIconEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state)
{
    Q_UNUSED(mode)
    Q_UNUSED(state)

    const bool isTracked = d_ptr->m_pCM->isTracked();
    const bool isPresent = d_ptr->m_pCM->isPresent();

    staticPaint(painter, rect, isPresent, isTracked);
}

auto CMIconEngine::clone() const -> QIconEngine*
{
    auto e = new CMIconEngine(d_ptr->m_pCM);
    delete e->d_ptr;
    e->d_ptr = d_ptr;

    return e;
}

QSize CMIconEngine::actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    Q_UNUSED(mode)
    Q_UNUSED(state)
    return {
        std::min(size.height(), size.width()),
        std::min(size.height(), size.width())
    };
}
