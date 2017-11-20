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
#include "hierarchyview.h"

// libstdc++
#include <functional>

// Qt
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlContext>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QDebug>

struct HierarchyViewEntry final
{
    explicit HierarchyViewEntry() {};
    HierarchyViewEntry(
        const QModelIndex& idx, QQuickItem* i,
        HierarchyViewEntry* p, QQmlContext* ctx
    );

    QPersistentModelIndex   m_ModelIndex {       };
    QQuickItem*             m_pItem      {nullptr};
    HierarchyViewEntry* m_pParent    {nullptr};
    QQmlContext*            m_pContext   {nullptr};
    int                     m_Sum        {   0   };
    int                     m_Y0         {   0   };
    int                     m_Index      {   0   };

    QVector<HierarchyViewEntry*> m_lChildren;
};

class HierarchyViewPrivate final : public QObject
{
    Q_OBJECT
public:
    explicit HierarchyViewPrivate(HierarchyView* parent) : QObject(parent), q_ptr(parent){}

    QHash<QPersistentModelIndex, HierarchyViewEntry*> m_hMapping;

    HierarchyViewEntry m_RootNode {};

    HierarchyView* q_ptr;

    void clear();
    void loadVisible();
    void loadRecursize( const QModelIndex& parent, const QModelIndex& self );

    void forEach(const QModelIndex& parent, int first, int last, const std::function<void(const QModelIndex&, HierarchyViewEntry* n)>& f) const;

public Q_SLOTS:
    void slotRowsInserted(const QModelIndex& parent, int first, int last);
    void slotRowsRemoved (const QModelIndex& parent, int first, int last);
    //void slotRowsMoved   (const QModelIndex& parent, int first, int last);
    void slotDataChanged (const QModelIndex& tl, const QModelIndex& br  );
};

HierarchyView::HierarchyView(QQuickItem* parent) : FlickableView(parent), d_ptr(new HierarchyViewPrivate(this))
{}

HierarchyView::~HierarchyView()
{
    delete d_ptr;
}

void HierarchyView::setModel(QSharedPointer<QAbstractItemModel> m)
{
    if (model()) {
        disconnect(model().data(), &QAbstractItemModel::rowsInserted , d_ptr, &HierarchyViewPrivate::slotRowsInserted );
        disconnect(model().data(), &QAbstractItemModel::rowsRemoved  , d_ptr, &HierarchyViewPrivate::slotRowsRemoved  );
        //connect(model(), &QAbstractItemModel::rowsMoved    , d_ptr, &HierarchyViewPrivate::slotRowsMoved    );
        disconnect(model().data(), &QAbstractItemModel::dataChanged  , d_ptr, &HierarchyViewPrivate::slotDataChanged  );
    }

    FlickableView::setModel(m);

    d_ptr->clear();

    d_ptr->loadVisible();
    setCurrentY(contentHeight());

    connect(model().data(), &QAbstractItemModel::rowsInserted , d_ptr, &HierarchyViewPrivate::slotRowsInserted );
    connect(model().data(), &QAbstractItemModel::rowsRemoved  , d_ptr, &HierarchyViewPrivate::slotRowsRemoved  );
    //connect(model(), &QAbstractItemModel::rowsMoved    , d_ptr, &HierarchyViewPrivate::slotRowsMoved    );
    connect(model().data(), &QAbstractItemModel::dataChanged  , d_ptr, &HierarchyViewPrivate::slotDataChanged  );

}

HierarchyViewEntry::HierarchyViewEntry(const QModelIndex& idx, QQuickItem* i, HierarchyViewEntry* p, QQmlContext* ctx )
 : m_ModelIndex(idx), m_pItem(i), m_pParent(p), m_pContext(ctx)
{
    Q_ASSERT(idx.isValid() && p);

    Q_ASSERT(idx.row() <= p->m_lChildren.size());

    // Make room
    for (int i = idx.row(); i < p->m_lChildren.size(); i++)
        p->m_lChildren[i]->m_Index++;

    m_Index = idx.row();
    p->m_lChildren.insert(m_Index, this);
}

void HierarchyViewPrivate::loadRecursize(const QModelIndex& parent, const QModelIndex& self)
{
    auto parentNode = m_hMapping.value(parent);

    parentNode = parentNode ? parentNode : &m_RootNode;

    // Avoid adding already present elements
    if (parentNode && parentNode->m_lChildren.size() > self.row()
      && parentNode->m_lChildren[self.row()]->m_ModelIndex == self)
        return;

    // Everything is lazy loader from the root down
    if (parent.isValid() && !parentNode)
        return;

    auto pair = q_ptr->loadDelegate(parentNode->m_pItem, parentNode->m_pContext, self);
    auto item(pair.first);
    auto ctx(pair.second);

    auto node = new HierarchyViewEntry (
        self, item, parentNode, ctx
    );

    node->m_Y0 = item->height();
    node->m_Sum += node->m_Y0;

    m_hMapping[self] = node;

    const int toAdd = item->height();

    // Increment every item height
    do {
        node->m_pParent->m_Sum += toAdd;
        node->m_pParent->m_pItem->setHeight(node->m_pParent->m_Sum);

        // The first item is always at 0x0+parent_delegate
        if (!node->m_Index) {
            node->m_pItem->setY(node->m_pParent->m_Y0);
            continue;
        }

        auto previous = node->m_pParent->m_lChildren[node->m_Index-1];
        Q_ASSERT(previous != node);

        auto anchors = qvariant_cast<QObject*>(node->m_pItem->property("anchors"));

        anchors->setProperty("top", previous->m_pItem->property("bottom"));
        //Q_ASSERT(anchors->property("top") == previous->m_pItem->property("bottom"));

    } while ((node = node->m_pParent) && node != &m_RootNode);

    for (int i = 0; i < q_ptr->model()->rowCount(self); i++)
        loadRecursize(self, q_ptr->model()->index(i, 0, self));
}

void HierarchyViewPrivate::clear()
{
    for (auto n : qAsConst(m_hMapping)) {
        n->m_pItem->setVisible(false);
        n->m_pItem->setParentItem(nullptr);
        delete n->m_pItem;
        delete n->m_pContext;
        delete n;
    }
    m_hMapping.clear();
    m_RootNode = HierarchyViewEntry();
}

void HierarchyView::refresh()
{
    d_ptr->loadVisible();
}

void HierarchyViewPrivate::loadVisible()
{
    if (!q_ptr->delegate() || !q_ptr->model())
        return;

    if (!m_RootNode.m_pItem)
        m_RootNode.m_pItem = q_ptr->contentItem();

    if (!m_RootNode.m_pContext)
        m_RootNode.m_pContext = q_ptr->rootContext();

    q_ptr->FlickableView::refresh();

    for (int i = 0; i < q_ptr->model()->rowCount(); i++)
        loadRecursize({}, q_ptr->model()->index(i, 0));
}

void HierarchyViewPrivate::forEach(const QModelIndex& parent, int first, int last, const std::function<void(const QModelIndex&, HierarchyViewEntry*)>& f) const
{
    auto parentNode = m_hMapping.value(parent);

    if (parent.isValid() && !parentNode)
        return;

    for (int i = first; i <= last; i++) {
        auto idx = q_ptr->model()->index(i, 0, parent);
        if (auto node = m_hMapping.value(idx))
            f(idx, node);
    }
}

void HierarchyViewPrivate::slotRowsInserted(const QModelIndex& parent, int first, int last)
{
    Q_UNUSED(last)

    for (int i = first; i <= last; i++) {
        loadRecursize(parent, q_ptr->model()->index(i, 0, parent));
    }

    q_ptr->setCurrentY(q_ptr->contentHeight());
}

void HierarchyViewPrivate::slotRowsRemoved(const QModelIndex& parent, int first, int last)
{
    forEach(parent, first, last, [this](const QModelIndex& idx, HierarchyViewEntry* n) {
        Q_UNUSED(idx)
        Q_UNUSED(n)
        //TODO
    });
}

// void HierarchyViewPrivate::slotRowsMoved(const QModelIndex& parent, int first, int last)
// {
//     forEach(parent, first, last, [this](const QModelIndex& idx, HierarchyViewEntry* n) {
//         Q_UNUSED(idx)
//         Q_UNUSED(n)
//         //TODO
//     });
// }

void HierarchyViewPrivate::slotDataChanged(const QModelIndex& tl, const QModelIndex& br)
{
    if (tl.parent() != br.parent())
        return;

    forEach(tl.parent(), tl.row(), br.row(), [this](const QModelIndex& idx, HierarchyViewEntry* n) {
        q_ptr->applyRoles(n->m_pContext, idx);
    });
}

FlickableView::ModelIndexItem* HierarchyView::createItem() const
{
    return nullptr;
}

FlickableView::ModelIndexItem* HierarchyView::itemForIndex(const QModelIndex& idx) const
{
    return nullptr;
}

#include <hierarchyview.moc>
