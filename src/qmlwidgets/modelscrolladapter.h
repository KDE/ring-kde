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

#include <QtCore/QObject>
#include <QtCore/QSharedPointer>

class QQuickItem;
class QAbstractItemModel;

class ModelScrollAdapterPrivate;

class ModelScrollAdapter : public QObject
{
    Q_OBJECT

public:
    explicit ModelScrollAdapter(QObject* parent = nullptr);
    virtual ~ModelScrollAdapter();

    Q_PROPERTY(QSharedPointer<QAbstractItemModel> model READ model WRITE setModel)
    Q_PROPERTY(QQuickItem* target READ target WRITE setTarget)

    QSharedPointer<QAbstractItemModel> model() const;
    void setModel(QSharedPointer<QAbstractItemModel> m);

    QQuickItem* target() const;
    void setTarget(QQuickItem* item);

private Q_SLOTS:
    void rowsInserted();

private:
    ModelScrollAdapterPrivate* d_ptr;
    Q_DECLARE_PRIVATE(ModelScrollAdapter)
};

Q_DECLARE_METATYPE(ModelScrollAdapter*)
Q_DECLARE_METATYPE(QSharedPointer<QAbstractItemModel>)
