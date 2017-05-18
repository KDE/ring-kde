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

class BubblePrivate;

class Bubble : public QQuickPaintedItem
{
   Q_OBJECT

public:
    Q_PROPERTY(int alignment READ alignment WRITE setAlignment)
    Q_PROPERTY(QColor color READ color WRITE setColor)

    explicit Bubble(QQuickItem* parent = nullptr);
    virtual ~Bubble();

    virtual void paint(QPainter *painter) override;

    int alignment() const;
    void setAlignment(int a);

    QColor color() const;
    void setColor(QColor c);

private:
    BubblePrivate* d_ptr;
    Q_DECLARE_PRIVATE(Bubble)
};
