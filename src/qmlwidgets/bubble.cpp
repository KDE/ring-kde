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
#include "bubble.h"

#include <QtGui/QPainter>
#include <QtGui/QPalette>
#include <QtGui/QGuiApplication>

class BubblePrivate
{
public:
    int m_Align {Qt::AlignmentFlag::AlignLeft};
    QColor m_Color;
};

Bubble::Bubble(QQuickItem* parent) : QQuickPaintedItem(parent), d_ptr(new BubblePrivate)
{
    d_ptr->m_Color = QGuiApplication::palette().base().color();
}

Bubble::~Bubble()
{
    delete d_ptr;
}

void Bubble::paint(QPainter *painter)
{
    painter->setWorldMatrixEnabled(true);
    painter->setOpacity(0.3);

    const qreal w(boundingRect().width()), h(boundingRect().height());
    const qreal aw(10), ah(20), r(10), p(10); //arrow size width, height, radius, bottom padding

    // Point left or right
    if (d_ptr->m_Align == Qt::AlignmentFlag::AlignRight) {
        painter->scale(-1, 1);
        painter->translate(QPointF{-w, 0});
    }

    QPainterPath path;
    path.moveTo(0, h-ah-p-r);
    path.lineTo(aw, h-ah-r);
    path.lineTo(aw, h-r);
    path.cubicTo(
        path.currentPosition(),
        {aw, h},
        {aw+r, h}
    );
    path.lineTo(w- r, h);
    path.cubicTo(
        path.currentPosition(),
        {w, h},
        {w, h-r}
    );
    path.lineTo(w, r);

    path.cubicTo(
        path.currentPosition(),
        {w, 0},
        {w-r, 0}
    );
    path.lineTo(aw+r, 0);

    path.cubicTo(
        path.currentPosition(),
        {aw, 0},
        {aw, r}
    );
    path.lineTo(aw, h-r-p-ah-r);
    path.lineTo(0, h-ah-p-r);

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setBrush(d_ptr->m_Color);
    painter->drawPath(path);
}

int Bubble::alignment() const
{
    return d_ptr->m_Align;
}

void Bubble::setAlignment(int a)
{
    d_ptr->m_Align = a;
}

QColor Bubble::color() const
{
    return d_ptr->m_Color;
}

void Bubble::setColor(QColor c)
{
    d_ptr->m_Color = c;
}
