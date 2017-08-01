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
#include "treeview.h"

// libstdc++
#include <functional>

// Qt
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlContext>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QDebug>

struct TreeViewHierarchyEntry final
{
    explicit TreeViewHierarchyEntry() {};
    TreeViewHierarchyEntry(
        const QModelIndex& idx, QQuickItem* i,
        TreeViewHierarchyEntry* p, QQmlContext* ctx
    );

    QPersistentModelIndex   m_ModelIndex {       };
    QQuickItem*             m_pItem      {nullptr};
    TreeViewHierarchyEntry* m_pParent    {nullptr};
    QQmlContext*            m_pContext   {nullptr};
    int                     m_Sum        {   0   };
    int                     m_Y0         {   0   };
    int                     m_Index      {   0   };

    QVector<TreeViewHierarchyEntry*> m_lChildren;
};

class TreeViewPrivate final : public QObject
{
    Q_OBJECT
public:
    explicit TreeViewPrivate(TreeView* parent) : QObject(parent), q_ptr(parent){}

    QSharedPointer<QAbstractItemModel> m_pModel    {nullptr};
    QQmlComponent*                     m_pDelegate {nullptr};
    QQmlEngine*                        m_pEngine   {nullptr};
    QQmlComponent*                     m_pComponent{nullptr};

    QHash<QPersistentModelIndex, TreeViewHierarchyEntry*> m_hMapping;

    TreeViewHierarchyEntry m_RootNode {};

    TreeView* q_ptr;

    void clear();
    void loadVisible();
    void applyRoles(QQmlContext* ctx, const QModelIndex& self) const;
    void loadRecursize( const QModelIndex& parent, const QModelIndex& self );
    QPair<QQuickItem*, QQmlContext*> loadDelegate(TreeViewHierarchyEntry* parent, const QModelIndex& self) const;

    void forEach(const QModelIndex& parent, int first, int last, const std::function<void(const QModelIndex&, TreeViewHierarchyEntry* n)>& f) const;

public Q_SLOTS:
    void slotRowsInserted(const QModelIndex& parent, int first, int last);
    void slotRowsRemoved (const QModelIndex& parent, int first, int last);
    //void slotRowsMoved   (const QModelIndex& parent, int first, int last);
    void slotDataChanged (const QModelIndex& tl, const QModelIndex& br  );
};

TreeView::TreeView(QQuickItem* parent) : SimpleFlickable(parent), d_ptr(new TreeViewPrivate(this))
{}

TreeView::~TreeView()
{
    delete d_ptr;
}

void TreeView::setModel(QSharedPointer<QAbstractItemModel> model)
{
    if (d_ptr->m_pModel) {
        disconnect(d_ptr->m_pModel.data(), &QAbstractItemModel::rowsInserted , d_ptr, &TreeViewPrivate::slotRowsInserted );
        disconnect(d_ptr->m_pModel.data(), &QAbstractItemModel::rowsRemoved  , d_ptr, &TreeViewPrivate::slotRowsRemoved  );
        //connect(d_ptr->m_pModel.data(), &QAbstractItemModel::rowsMoved    , d_ptr, &TreeViewPrivate::slotRowsMoved    );
        disconnect(d_ptr->m_pModel.data(), &QAbstractItemModel::dataChanged  , d_ptr, &TreeViewPrivate::slotDataChanged  );
    }

    d_ptr->m_pModel = model;
    emit modelChanged(model);

    d_ptr->clear();

    d_ptr->loadVisible();
    setCurrentY(contentHeight());

    connect(d_ptr->m_pModel.data(), &QAbstractItemModel::rowsInserted , d_ptr, &TreeViewPrivate::slotRowsInserted );
    connect(d_ptr->m_pModel.data(), &QAbstractItemModel::rowsRemoved  , d_ptr, &TreeViewPrivate::slotRowsRemoved  );
    //connect(d_ptr->m_pModel.data(), &QAbstractItemModel::rowsMoved    , d_ptr, &TreeViewPrivate::slotRowsMoved    );
    connect(d_ptr->m_pModel.data(), &QAbstractItemModel::dataChanged  , d_ptr, &TreeViewPrivate::slotDataChanged  );

}

QSharedPointer<QAbstractItemModel> TreeView::model() const
{
    return d_ptr->m_pModel;
}

void TreeView::setDelegate(QQmlComponent* delegate)
{
    d_ptr->m_pDelegate = delegate;

    d_ptr->loadVisible();
}

QQmlComponent* TreeView::delegate() const
{
    return d_ptr->m_pDelegate;

    d_ptr->loadVisible();
}

TreeViewHierarchyEntry::TreeViewHierarchyEntry(const QModelIndex& idx, QQuickItem* i, TreeViewHierarchyEntry* p, QQmlContext* ctx )
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

void TreeViewPrivate::applyRoles(QQmlContext* ctx, const QModelIndex& self) const
{
    // Add all roles to the
    for (auto i = m_pModel->roleNames().constBegin(); i != m_pModel->roleNames().constEnd(); ++i)
        ctx->setContextProperty(i.value() , self.data(i.key()));

    // Set extra index to improve ListView compatibility
    ctx->setContextProperty("index"     , self.row()              );
    ctx->setContextProperty("rootIndex" , self                    );
    ctx->setContextProperty("rowCount"  , m_pModel->rowCount(self));
}

QPair<QQuickItem*, QQmlContext*> TreeViewPrivate::loadDelegate(TreeViewHierarchyEntry* parent, const QModelIndex& self) const
{
    // Create a context for the container, it's the only way to force anchors
    // to work
    auto pctx = new QQmlContext(parent->m_pContext);

    // Create a parent item to hold the delegate and all children
    auto container = qobject_cast<QQuickItem *>(m_pComponent->create(pctx));
    container->setWidth(q_ptr->width());
    m_pEngine->setObjectOwnership(container,QQmlEngine::CppOwnership);
    container->setParentItem(parent->m_pItem);

    // Create a context with all the tree roles
    auto ctx = new QQmlContext(pctx);

    applyRoles(ctx, self);

    // Create the delegate
    auto item = qobject_cast<QQuickItem *>(m_pDelegate->create(ctx));

    // It allows the children to be added anyway
    if(!item)
        return {container, pctx};

    item->setWidth(q_ptr->width());
    item->setParentItem(container);

    // Resize the container
    container->setHeight(item->height());

    return {container, pctx};
}

void TreeViewPrivate::loadRecursize(const QModelIndex& parent, const QModelIndex& self)
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

    auto pair = loadDelegate(parentNode, self);
    auto item(pair.first);
    auto ctx(pair.second);

    auto node = new TreeViewHierarchyEntry (
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

    for (int i = 0; i < m_pModel->rowCount(self); i++)
        loadRecursize(self, m_pModel->index(i, 0, self));
}

void TreeViewPrivate::clear()
{
    for (auto n : qAsConst(m_hMapping)) {
        n->m_pItem->setVisible(false);
        n->m_pItem->setParentItem(nullptr);
        delete n->m_pItem;
        delete n->m_pContext;
        delete n;
    }
    m_hMapping.clear();
    m_RootNode = TreeViewHierarchyEntry();
}

void TreeViewPrivate::loadVisible()
{
    if (!m_pDelegate || !m_pModel)
        return;

    if (!m_RootNode.m_pItem)
        m_RootNode.m_pItem = q_ptr->contentItem();

    if (!m_RootNode.m_pContext)
        m_RootNode.m_pContext = QQmlEngine::contextForObject(q_ptr);

    if (!m_pEngine) {
        m_pEngine = m_RootNode.m_pContext->engine();
        m_pComponent = new QQmlComponent(m_pEngine);
        m_pComponent->setData("import QtQuick 2.4; Item {}", {});
    }

    for (int i = 0; i < m_pModel->rowCount(); i++)
        loadRecursize({}, m_pModel->index(i, 0));
}


void TreeViewPrivate::forEach(const QModelIndex& parent, int first, int last, const std::function<void(const QModelIndex&, TreeViewHierarchyEntry*)>& f) const
{
    auto parentNode = m_hMapping.value(parent);

    if (parent.isValid() && !parentNode)
        return;

    for (int i = first; i <= last; i++) {
        auto idx = m_pModel->index(i, 0, parent);
        if (auto node = m_hMapping.value(idx))
            f(idx, node);
    }
}

void TreeViewPrivate::slotRowsInserted(const QModelIndex& parent, int first, int last)
{
    Q_UNUSED(last)

    for (int i = first; i <= last; i++) {
        loadRecursize(parent, m_pModel->index(i, 0, parent));
    }

    q_ptr->setCurrentY(q_ptr->contentHeight());
}

void TreeViewPrivate::slotRowsRemoved(const QModelIndex& parent, int first, int last)
{
    forEach(parent, first, last, [this](const QModelIndex& idx, TreeViewHierarchyEntry* n) {
        Q_UNUSED(idx)
        Q_UNUSED(n)
        //TODO
    });
}

// void TreeViewPrivate::slotRowsMoved(const QModelIndex& parent, int first, int last)
// {
//     forEach(parent, first, last, [this](const QModelIndex& idx, TreeViewHierarchyEntry* n) {
//         Q_UNUSED(idx)
//         Q_UNUSED(n)
//         //TODO
//     });
// }

void TreeViewPrivate::slotDataChanged(const QModelIndex& tl, const QModelIndex& br)
{
    if (tl.parent() != br.parent())
        return;

    forEach(tl.parent(), tl.row(), br.row(), [this](const QModelIndex& idx, TreeViewHierarchyEntry* n) {
        applyRoles(n->m_pContext, idx);
    });
}

#include <treeview.moc>
