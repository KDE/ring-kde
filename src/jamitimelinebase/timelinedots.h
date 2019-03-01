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
#pragma once

#include <QQuickPaintedItem>

class TimelineDotsPrivate;

/**
 * The pure QML version of this widget created too many elements and was too
 * slow.
 */
class TimelineDots : public QQuickPaintedItem
{
    Q_OBJECT
public:
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY changed)

    QColor color() const;
    void setColor(const QColor& color);

    explicit TimelineDots(QQuickItem* parent = nullptr);
    virtual ~TimelineDots();

    virtual void paint(QPainter *painter) override;

Q_SIGNALS:
    void changed();

private:
    TimelineDotsPrivate* d_ptr;
    Q_DECLARE_PRIVATE(TimelineDots)
};
