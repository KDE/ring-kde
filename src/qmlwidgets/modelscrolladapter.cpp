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
#include "modelscrolladapter.h"

#include <QtCore/QAbstractItemModel>
#include <QtCore/QDebug>
#include <QtQuick/QQuickItem>

class ModelScrollAdapterPrivate
{
public:
    QSharedPointer<QAbstractItemModel> m_pModel;
    QQuickItem* m_pItem;
};

ModelScrollAdapter::ModelScrollAdapter(QObject* parent) : QObject(parent),
    d_ptr(new ModelScrollAdapterPrivate)
{}

ModelScrollAdapter::~ModelScrollAdapter()
{
    delete d_ptr;
}

QSharedPointer<QAbstractItemModel> ModelScrollAdapter::model() const
{
    return d_ptr->m_pModel;
}

void ModelScrollAdapter::setModel(QSharedPointer<QAbstractItemModel> m)
{
    if (d_ptr->m_pModel)
        disconnect(d_ptr->m_pModel.data(), &QAbstractItemModel::rowsInserted,
            this, &ModelScrollAdapter::rowsInserted);

    d_ptr->m_pModel = m;

    connect(d_ptr->m_pModel.data(), &QAbstractItemModel::rowsInserted,
        this, &ModelScrollAdapter::rowsInserted);
}

QQuickItem* ModelScrollAdapter::target() const
{
    return d_ptr->m_pItem;
}

void ModelScrollAdapter::setTarget(QQuickItem* item)
{
    d_ptr->m_pItem = item;
    rowsInserted();
}

void ModelScrollAdapter::rowsInserted()
{
    if (!d_ptr->m_pItem)
        return;

    QMetaObject::invokeMethod(d_ptr->m_pItem, "positionViewAtEnd");
}
