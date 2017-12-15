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
#include "modelviewtester.h"

#include <QtCore/QDebug>

struct ModelViewTesterItem
{
    ModelViewTesterItem() {}
    ModelViewTesterItem(ModelViewTesterItem* p, const QHash<int, QVariant>& vals, int i = -1);

    int m_Index {0};;
    QHash<int, QVariant> m_hValues;
    ModelViewTesterItem* m_pParent {nullptr};
    QVector<ModelViewTesterItem*> m_lChildren;
};

ModelViewTester::ModelViewTester(QObject* parent)
{
    m_pRoot = new ModelViewTesterItem;
}

ModelViewTester::~ModelViewTester()
{

}

bool ModelViewTester::setData( const QModelIndex& index, const QVariant &value, int role   )
{
    Q_UNUSED(index)
    Q_UNUSED(value)
    Q_UNUSED(role)
    return false;
}

QVariant ModelViewTester::data( const QModelIndex& index, int role ) const
{
    if (!index.isValid())
        return {};

    auto item = static_cast<ModelViewTesterItem*>(index.internalPointer());

    return item->m_hValues[role];
}

int ModelViewTester::rowCount( const QModelIndex& parent) const
{
    if (!parent.isValid())
        return m_pRoot->m_lChildren.size();

    auto item = static_cast<ModelViewTesterItem*>(parent.internalPointer());

    return item->m_lChildren.size();
}

int ModelViewTester::columnCount( const QModelIndex& parent ) const
{
    return 1; //FIXME not really true
}

QModelIndex ModelViewTester::parent( const QModelIndex& index ) const
{
    if (!index.isValid())
        return {};

    auto item = static_cast<ModelViewTesterItem*>(index.internalPointer());

    Q_ASSERT(item != m_pRoot);

    if (item->m_pParent == m_pRoot)
        return {};

    return createIndex(item->m_pParent->m_Index, 0, item->m_pParent);
}

QModelIndex ModelViewTester::index( int row, int column, const QModelIndex& parent ) const
{
    auto parItem = parent.isValid() ?
        static_cast<ModelViewTesterItem*>(parent.internalPointer()): m_pRoot;

    if (column || row >= parItem->m_lChildren.size() || row < 0)
        return {};

    return createIndex(row, column, parItem->m_lChildren[row]);
}

QMimeData* ModelViewTester::mimeData( const QModelIndexList &indexes) const
{
    Q_UNUSED(indexes)
    return nullptr; //TODO
}

bool ModelViewTester::dropMimeData( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
    Q_UNUSED(data)
    Q_UNUSED(action)
    Q_UNUSED(row)
    Q_UNUSED(column)
    return false; //TODO
}

QHash<int,QByteArray> ModelViewTester::roleNames() const
{
    return {
        {Qt::DisplayRole, "display"},
        {Qt::UserRole, "offset"}
    };
}

// Qt::ItemFlags ModelViewTester::flags( const QModelIndex& index) const

ModelViewTesterItem::ModelViewTesterItem(ModelViewTesterItem* p, const QHash<int, QVariant>& vals, int i) :
    m_pParent(p), m_hValues(vals)
{
    if (i == -1) {
        m_Index = p->m_lChildren.size();
        p->m_lChildren << this;
    }
    else {
        m_Index = i;

        p->m_lChildren.insert(i, this);
        for (int j=i+1; j < p->m_lChildren.size(); j++)
            p->m_lChildren[j]->m_Index++;
    }
}

void ModelViewTester::prependSimpleRoot()
{
    beginInsertRows({}, 0, 0);

    QHash<int, QVariant> vals = {
        {Qt::DisplayRole, "prep root 1"},
        {Qt::UserRole, 0}
    };

    new ModelViewTesterItem(m_pRoot, vals, 0);

    endInsertRows();
    beginInsertRows({}, 1, 1);

    vals = {
        {Qt::DisplayRole, "prep root 2"},
        {Qt::UserRole, 0}
    };

    new ModelViewTesterItem(m_pRoot, vals, 1);

    endInsertRows();
    beginInsertRows({}, 0, 0);

    vals = {
        {Qt::DisplayRole, "prep root 0"},
        {Qt::UserRole, 0}
    };

    new ModelViewTesterItem(m_pRoot, vals, 0);

    endInsertRows();
}

void ModelViewTester::appendSimpleRoot()
{
    beginInsertRows({}, m_pRoot->m_lChildren.size(), m_pRoot->m_lChildren.size());

    QHash<int, QVariant> vals = {
        {Qt::DisplayRole, "root "+QString::number(m_pRoot->m_lChildren.size())},
        {Qt::UserRole, 0}
    };

    new ModelViewTesterItem(m_pRoot, vals);

    endInsertRows();
}

void ModelViewTester::appendRootChildren()
{
    auto par = m_pRoot->m_lChildren[1];
    beginInsertRows(index(1,0), par->m_lChildren.size(), par->m_lChildren.size());

    QHash<int, QVariant> vals = {
        {Qt::DisplayRole, "child "+QString::number(par->m_lChildren.size())},
        {Qt::UserRole, 10}
    };

    new ModelViewTesterItem(par, vals);

    endInsertRows();
}

void ModelViewTester::moveRootToFront()
{
    beginMoveRows({}, 2,2, {}, 0);

    auto elem = m_pRoot->m_lChildren[2];

    m_pRoot->m_lChildren.remove(2);
    m_pRoot->m_lChildren.insert(0, elem);

    for (int i =0; i < m_pRoot->m_lChildren.size(); i++)
        m_pRoot->m_lChildren[i]->m_Index = i;

    endMoveRows();
}

void ModelViewTester::moveChildByOne()
{
    auto parentIdx = index(4,0);

    beginMoveRows(parentIdx, 2,2, parentIdx, 1);

    auto elem = m_pRoot->m_lChildren[4]->m_lChildren[2];

    m_pRoot->m_lChildren[4]->m_lChildren.remove(2);
    m_pRoot->m_lChildren[4]->m_lChildren.insert(0, elem);

    for (int i =0; i < m_pRoot->m_lChildren[4]->m_lChildren.size(); i++)
        m_pRoot->m_lChildren[4]->m_lChildren[i]->m_Index = i;

    endMoveRows();
}

void ModelViewTester::moveChildByParent()
{
    auto elem = m_pRoot->m_lChildren[4]->m_lChildren[3];

    auto oldParentIdx = index(4,0);
    auto newParentIdx = index(m_pRoot->m_lChildren.size()-1,0);

    beginMoveRows(oldParentIdx, 3,3, newParentIdx, 0);

    m_pRoot->m_lChildren[4]->m_lChildren.remove(3);
    m_pRoot->m_lChildren[m_pRoot->m_lChildren.size()-1]->m_lChildren.insert(0, elem);

    elem->m_Index = 0;
    endMoveRows();
}

void ModelViewTester::moveToGrandChildren()
{
    auto elem1 = m_pRoot->m_lChildren[1];
    auto elem2 = m_pRoot->m_lChildren[2];
    auto newPar = m_pRoot->m_lChildren[4]->m_lChildren[2];
    auto newParentIdx = createIndex(newPar->m_Index, 0, newPar);

    beginMoveRows({}, 1,2, newParentIdx, 0);

    elem1->m_pParent = newPar;
    elem2->m_pParent = newPar;

    elem1->m_hValues = {
        {Qt::DisplayRole, elem1->m_hValues[0].toString()+" gc"},
        {Qt::UserRole, 20}
    };
    elem2->m_hValues = {
        {Qt::DisplayRole, elem2->m_hValues[0].toString()+" gc"},
        {Qt::UserRole, 20}
    };

    m_pRoot->m_lChildren.remove(1);
    m_pRoot->m_lChildren.remove(2);

    newPar->m_lChildren << elem1 << elem2;

    for (int i =0; i < newPar->m_lChildren.size(); i++)
        newPar->m_lChildren[i]->m_Index = i;

    for (int i =0; i < m_pRoot->m_lChildren.size(); i++)
        m_pRoot->m_lChildren[i]->m_Index = i;

    endMoveRows();

    Q_EMIT dataChanged(index(0, 0, newParentIdx), index(1, 0, newParentIdx));
}


void ModelViewTester::insertRoot()
{
    beginInsertRows({}, 1, 1);

    QHash<int, QVariant> vals = {
        {Qt::DisplayRole, "inserted root 1"},
        {Qt::UserRole, 0}
    };

    new ModelViewTesterItem(m_pRoot, vals, 1);

    endInsertRows();
}

void ModelViewTester::insertFirst()
{
    beginInsertRows({}, 0, 0);

    QHash<int, QVariant> vals = {
        {Qt::DisplayRole, "inserted root 0"},
        {Qt::UserRole, 0}
    };

    new ModelViewTesterItem(m_pRoot, vals, 0);

    endInsertRows();
}

void ModelViewTester::insertChild()
{
    auto newPar = m_pRoot->m_lChildren[4];
    auto parIdx = createIndex(4, 0, newPar);

    Q_ASSERT(parIdx.isValid());

    beginInsertRows(parIdx, 0, 0);

    QHash<int, QVariant> vals = {
        {Qt::DisplayRole, "inserted child 0"},
        {Qt::UserRole, 0}
    };

    new ModelViewTesterItem(newPar, vals, 0);

    endInsertRows();
}

void ModelViewTester::removeRoot()
{
    beginRemoveRows({}, 0, 0);

    QHash<int, QVariant> vals = {
        {Qt::DisplayRole, "inserted root 0"},
        {Qt::UserRole, 0}
    };

    m_pRoot->m_lChildren.remove(1);

    for (int i =0; i < m_pRoot->m_lChildren.size(); i++)
        m_pRoot->m_lChildren[i]->m_Index = i;

    endRemoveRows();
}

void ModelViewTester::resetModel()
{
    beginResetModel();
    m_pRoot->m_lChildren.clear();
    endResetModel();
}
