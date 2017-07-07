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

// Qt
#include <QtCore/QAbstractItemModel>

class MultiCallPrivate;

/**
 * The pure QML version of this widget created too many elements and was too
 * slow.
 */
class MultiCall : public QQuickPaintedItem
{
    Q_OBJECT
public:
    Q_PROPERTY(QPersistentModelIndex modelIndex READ modelIndex WRITE setModelIndex)

    Q_PROPERTY(bool skipChildren READ skipChildren CONSTANT)

    explicit MultiCall(QQuickItem* parent = nullptr);
    virtual ~MultiCall();

    void setModelIndex(QPersistentModelIndex idx);
    QPersistentModelIndex modelIndex() const;

    bool skipChildren() const;

    virtual void paint(QPainter *painter) override;

private:
    MultiCallPrivate* d_ptr;
    Q_DECLARE_PRIVATE(MultiCall)
};
