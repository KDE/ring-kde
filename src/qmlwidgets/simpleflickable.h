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

#include <QQuickItem>

class SimpleFlickablePrivate;

/**
 * This file re-implements the flickable view.
 *
 * It is necessary to avoid a dependency on Qt private APIs in order to
 * re-implement higher level views such as the tree view. The upstream code
 * could also hardly have been copy/pasted in this project as it depends on
 * yet more hidden APIs and the code (along with dependencies) is over an order
 * or magnitude larger than this implementation. It would have been a
 * maintainability nightmare.
 *
 * This implementation is API compatible with a small subset of the Flickable
 * properties and uses a 200 lines of code inertial state machine instead of
 * 1.5k line of vomit code to do the exact same job.
 */
class SimpleFlickable : public QQuickItem
{
    Q_OBJECT
public:
    Q_PROPERTY(qreal contentY READ currentY WRITE setCurrentY NOTIFY currentYChanged)
    Q_PROPERTY(qreal contentHeight READ contentHeight NOTIFY contentHeightChanged )

    explicit SimpleFlickable(QQuickItem* parent = nullptr);
    virtual ~SimpleFlickable();

    qreal currentY() const;
    virtual void setCurrentY(qreal y);

    qreal contentHeight() const;

    QQuickItem* contentItem();

Q_SIGNALS:
    void contentHeightChanged(qreal height);
    void currentYChanged(qreal y);
    void percentageChanged(qreal percent);

protected:
    bool event(QEvent *ev) override;
    bool childMouseEventFilter(QQuickItem *, QEvent *) override;
    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;

private:
    SimpleFlickablePrivate* d_ptr;
    Q_DECLARE_PRIVATE(SimpleFlickable)
};
