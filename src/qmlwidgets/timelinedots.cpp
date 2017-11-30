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
#include "timelinedots.h"

#include <QtGui/QPainter>
#include <QtGui/QGuiApplication>
#include <QtGui/QColor>
#include <QtGui/QPalette>

#include <cmath>

TimelineDots::TimelineDots(QQuickItem* parent) : QQuickPaintedItem(parent)
{}

TimelineDots::~TimelineDots()
{}

void TimelineDots::paint(QPainter *painter)
{
    static bool init = false;
    static QPen   pen;
    static QBrush brush;

    if (!init) {
        auto pal = QGuiApplication::palette();

        pen = QPen(pal.brush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text).color(), 1.5);

        auto bg = pen.color();
        bg.setAlpha(0.1);
        brush = QBrush( bg, Qt::SolidPattern);

        init = true;
    }

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen  (pen);
    painter->setBrush(brush);

    // Draw the spine
    painter->drawLine(
        width()/2.0,
        0.0,
        width()/2.0,
        height()
    );

    // The logic behind the dot spacing is that the "biggest" dots fit and still
    // have 50% of the common distance between them and the next small dot. The
    // big dots currently have a fixed radius of 10 points (DPI aware) and the
    // smaller ones 2.5 points.

    const qreal hCenter  = width()/2.0;
    const qreal spacing  = 14.0;
    const qreal radius   = 4;
    const qreal distance = spacing + 2*radius;

    // Draw each dots
    for (int i = 0; i < std::floor((height()+(distance/2.0))/distance); i++) {
        const QRectF rect {
            hCenter - radius,
            (distance/2.0) + i*distance - radius,
            radius*2.0,
            radius*2.0,
        };

        painter->setCompositionMode(QPainter::CompositionMode_Clear);
        painter->drawEllipse(rect);
        painter->setCompositionMode(QPainter::CompositionMode_Source);
        painter->drawEllipse(rect);
    }
}
