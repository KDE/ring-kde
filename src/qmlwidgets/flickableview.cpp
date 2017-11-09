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
#include "flickableview.h"

#include <QQmlContext>
#include <QQmlEngine>

class FlickableViewPrivate
{
public:
    QSharedPointer<QAbstractItemModel> m_pModel      {nullptr};
    QQmlComponent*                     m_pDelegate   {nullptr};
    QQmlEngine*                        m_pEngine     {nullptr};
    QQmlComponent*                     m_pComponent  {nullptr};
    mutable QQmlContext*               m_pRootContext{nullptr};

    Qt::Corner m_Corner {Qt::TopLeftCorner};

    mutable QHash<int, QString>        m_hRoleNames{       };

    void reloadRoleNames() const;
};

FlickableView::FlickableView(QQuickItem* parent) : SimpleFlickable(parent),
    d_ptr(new FlickableViewPrivate())
{
    //
}

FlickableView::~FlickableView()
{
    delete d_ptr;
}

void FlickableView::setModel(QSharedPointer<QAbstractItemModel> model)
{
    if (d_ptr->m_pModel == model)
        return;

    d_ptr->m_pModel = model;
    d_ptr->m_hRoleNames.clear();

    emit modelChanged(model);

    refresh();
    setCurrentY(contentHeight());
}

QSharedPointer<QAbstractItemModel> FlickableView::model() const
{
    return d_ptr->m_pModel;
}

Qt::Corner FlickableView::gravity() const
{
    return d_ptr->m_Corner;
}

void FlickableView::setGravity(Qt::Corner g)
{
    d_ptr->m_Corner = g;
    refresh();
}

void FlickableView::setDelegate(QQmlComponent* delegate)
{
    d_ptr->m_pDelegate = delegate;
    refresh();
}

QQmlComponent* FlickableView::delegate() const
{
    return d_ptr->m_pDelegate;
    //refresh();
}

QQmlContext* FlickableView::rootContext() const
{
    if (!d_ptr->m_pRootContext)
        d_ptr->m_pRootContext = QQmlEngine::contextForObject(this);

    return d_ptr->m_pRootContext;
}

void FlickableView::refresh()
{
    if (!d_ptr->m_pEngine) {
        d_ptr->m_pEngine = rootContext()->engine();
        d_ptr->m_pComponent = new QQmlComponent(d_ptr->m_pEngine);
        d_ptr->m_pComponent->setData("import QtQuick 2.4; Item {}", {});
    }
}

/**
 * This helper method convert the model role names (QByteArray) to QML context
 * properties (QString) only once.
 *
 * If this wasn't done, it would cause millions of QByteArray->QString temporary
 * allocations.
 */
void FlickableViewPrivate::reloadRoleNames() const
{
    if (!m_pModel)
        return;

    m_hRoleNames.clear();

    const auto roleNames = m_pModel->roleNames();

    for (auto i = roleNames.constBegin(); i != roleNames.constEnd(); ++i)
        m_hRoleNames[i.key()] = i.value();
}

void FlickableView::applyRoles(QQmlContext* ctx, const QModelIndex& self) const
{
    // Refresh the cache
    if (model()->roleNames().size() != d_ptr->m_hRoleNames.size())
        d_ptr->reloadRoleNames();

    // Add all roles to the
    for (auto i = d_ptr->m_hRoleNames.constBegin(); i != d_ptr->m_hRoleNames.constEnd(); ++i)
        ctx->setContextProperty(i.value() , self.data(i.key()));

    // Set extra index to improve ListView compatibility
    ctx->setContextProperty(QStringLiteral("index"    ) , self.row()              );
    ctx->setContextProperty(QStringLiteral("rootIndex") , self                    );
    ctx->setContextProperty(QStringLiteral("rowCount" ) , model()->rowCount(self));
}

QPair<QQuickItem*, QQmlContext*> FlickableView::loadDelegate(QQuickItem* parentI, QQmlContext* parentCtx, const QModelIndex& self) const
{
    // Create a context for the container, it's the only way to force anchors
    // to work
    auto pctx = new QQmlContext(parentCtx);

    // Create a parent item to hold the delegate and all children
    auto container = qobject_cast<QQuickItem *>(d_ptr->m_pComponent->create(pctx));
    container->setWidth(width());
    d_ptr->m_pEngine->setObjectOwnership(container,QQmlEngine::CppOwnership);
    container->setParentItem(parentI);

    // Create a context with all the tree roles
    auto ctx = new QQmlContext(pctx);

    applyRoles(ctx, self);

    // Create the delegate
    auto item = qobject_cast<QQuickItem *>(delegate()->create(ctx));

    // It allows the children to be added anyway
    if(!item)
        return {container, pctx};

    item->setWidth(width());
    item->setParentItem(container);

    // Resize the container
    container->setHeight(item->height());

    return {container, pctx};
}
