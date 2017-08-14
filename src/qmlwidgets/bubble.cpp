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

#include <QtCore/QDateTime>
#include <QtGui/QPainter>
#include <QtGui/QPalette>
#include <QtGui/QFontMetricsF>
#include <QtGui/QGuiApplication>

#include <QQuickWindow>

class BubblePrivate
{
public:
    int m_Align {Qt::AlignmentFlag::AlignLeft};
    QColor m_Color;
    QString m_Text;
    QFont m_Font {QStringLiteral("Noto Color Emoji")};
    QFontMetricsF m_FontMetrics{m_Font};
    qreal m_MaximumWidth {-1};
};

Bubble::Bubble(QQuickItem* parent) : QQuickPaintedItem(parent), d_ptr(new BubblePrivate)
{
    d_ptr->m_Color = QGuiApplication::palette().base().color();
    connect(this, &Bubble::windowChanged, this, &Bubble::slotWindowChanged);
}

Bubble::~Bubble()
{
    delete d_ptr;
}

void Bubble::paint(QPainter *painter)
{
    painter->setWorldMatrixEnabled(true);

    const qreal w(boundingRect().width()), h(boundingRect().height());

    // arrow size width, height, radius, bottom padding
    const qreal aw(10), ah(0), r(10), p(10);

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

    painter->setPen({});
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

void Bubble::setColor(const QColor& c)
{
    d_ptr->m_Color = c;
}

qreal Bubble::maximumWidth() const
{
    return d_ptr->m_MaximumWidth;
}

void Bubble::setMaximumWidth(qreal value)
{
    d_ptr->m_MaximumWidth = value;
    slotWindowChanged(window());
}

QString Bubble::text() const
{
    return d_ptr->m_Text;
}

void Bubble::setText(const QString& c)
{
    d_ptr->m_Text = c;
    slotWindowChanged(window());
}

QFont& Bubble::font() const
{
    return d_ptr->m_Font;
}

void Bubble::setFont(const QFont& f)
{
    d_ptr->m_Font = f;
    d_ptr->m_FontMetrics = QFontMetricsF(f);
    slotWindowChanged(nullptr);
    emit fontChanged(d_ptr->m_Font);
}

void Bubble::slotWindowChanged(QQuickWindow *w)
{
    w = window();

    static qreal ratio   = 0;
    static qreal arrow   = 20;
    static qreal margins = 20;
    static qreal dateW   = 0;

    // There is a race condition, the item are created before the window
    if (!ratio) {
        ratio   = w ? w->effectiveDevicePixelRatio():0;

        //TODO make point size configurable
        QFont f = d_ptr->m_Font;
        f.setPointSize(8);
        QFontMetricsF fm(f);
        dateW = fm.width(QDateTime::currentDateTime().toString());
    }

    // At first, don't limit the height
    const auto r = d_ptr->m_FontMetrics.boundingRect(
        QRectF {0, 0, d_ptr->m_MaximumWidth, 9999.0},
        Qt::AlignLeft|Qt::TextWordWrap,
        d_ptr->m_Text
    );

    // Prevent bubble larger than the screen
    const qreal mw = std::max(dateW*1.66, r.width())+arrow+2*margins;

    setWidth(std::min(d_ptr->m_MaximumWidth, mw));
    setHeight(std::max(50.0, r.height() + 2*d_ptr->m_FontMetrics.height()));
}
