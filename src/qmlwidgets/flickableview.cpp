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
#include <QtCore/QItemSelectionModel>

class FlickableViewPrivate : public QObject
{
    Q_OBJECT
public:
    QSharedPointer<QAbstractItemModel>  m_pModel          {nullptr};
    QSharedPointer<QItemSelectionModel> m_pSelectionModel {nullptr};
    QQmlComponent*                      m_pDelegate       {nullptr};
    QQmlEngine*                         m_pEngine         {nullptr};
    QQmlComponent*                      m_pComponent      {nullptr};
    QQmlComponent*                      m_pHighlight      {nullptr};
    mutable QQmlContext*                m_pRootContext    {nullptr};
    bool                                m_IsSortingEnabled{nullptr};

    // Selection
    QQuickItem* m_pSelectedItem   {nullptr};
    QWeakPointer<FlickableView::ModelIndexItem> m_pSelectedViewItem;

    Qt::Corner m_Corner {Qt::TopLeftCorner};

    mutable QHash<int, QString> m_hRoleNames;
    mutable QHash<const QAbstractItemModel*, QHash<int, QString>*> m_hhOtherRoleNames;

    QHash<int, QString>* reloadRoleNames(const QModelIndex& index) const;

    FlickableView* q_ptr;

public Q_SLOTS:
    void slotCurrentIndexChanged(const QModelIndex& idx);
    void slotSelectionModelChanged();
    void slotModelDestroyed();
};

FlickableView::FlickableView(QQuickItem* parent) : SimpleFlickable(parent),
    d_ptr(new FlickableViewPrivate())
{
    d_ptr->q_ptr = this;
}

FlickableView::~FlickableView()
{
    delete d_ptr;
}

void FlickableView::setModel(QSharedPointer<QAbstractItemModel> model)
{
    if (d_ptr->m_pModel == model)
        return;

    if (d_ptr->m_pSelectionModel && d_ptr->m_pSelectionModel->model() != model)
        d_ptr->m_pSelectionModel = nullptr;

    if (d_ptr->m_IsSortingEnabled && model) {
        model->sort(0);
    }

    if (d_ptr->m_pModel)
        disconnect(d_ptr->m_pModel.data(), &QObject::destroyed,
            d_ptr, &FlickableViewPrivate::slotModelDestroyed);

    // In theory it can cause an issue when QML set both properties in random
    // order when replacing this model, but for until that happens, better be
    // strict.
    Q_ASSERT((!d_ptr->m_pSelectionModel)
        || (!model)
        || model == d_ptr->m_pSelectionModel->model()
    );

    d_ptr->m_pModel = model;
    d_ptr->m_hRoleNames.clear();

    if (d_ptr->m_pModel)
        connect(d_ptr->m_pModel.data(), &QObject::destroyed,
            d_ptr, &FlickableViewPrivate::slotModelDestroyed);

    emit modelChanged(model);
    emit countChanged();

    refresh();
    setCurrentY(contentHeight());
}

void FlickableView::setRawModel(QAbstractItemModel* m)
{
    //FIXME blatant leak
    auto p = new QSharedPointer<QAbstractItemModel>(m);

    setModel(*p);
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


QQmlComponent* FlickableView::highlight() const
{
    return d_ptr->m_pHighlight;
}

void FlickableView::setHighlight(QQmlComponent* h)
{
    d_ptr->m_pHighlight = h;
}

QSharedPointer<QItemSelectionModel> FlickableView::selectionModel() const
{
    if (model() && !d_ptr->m_pSelectionModel) {
        auto sm = new QItemSelectionModel(model().data());
        d_ptr->m_pSelectionModel = QSharedPointer<QItemSelectionModel>(sm);
        d_ptr->slotSelectionModelChanged();
        Q_EMIT selectionModelChanged();
        connect(d_ptr->m_pSelectionModel.data(), &QItemSelectionModel::currentChanged,
            d_ptr, &FlickableViewPrivate::slotCurrentIndexChanged);
    }

    return d_ptr->m_pSelectionModel;
}

void FlickableView::setSelectionModel(QSharedPointer<QItemSelectionModel> m)
{
    d_ptr->m_pSelectionModel = m;

    // This will cause undebugable issues. Better ban it early
    Q_ASSERT((!model()) || (!m) || model() == m->model());

    d_ptr->slotSelectionModelChanged();
    Q_EMIT selectionModelChanged();
}

bool FlickableView::isSortingEnabled() const
{
    return d_ptr->m_IsSortingEnabled;
}

void FlickableView::setSortingEnabled(bool val)
{
    d_ptr->m_IsSortingEnabled = val;

    if (d_ptr->m_IsSortingEnabled && d_ptr->m_pModel) {
        d_ptr->m_pModel->sort(0);
    }
}

QModelIndex FlickableView::currentIndex() const
{
    return selectionModel()->currentIndex();
}

void FlickableView::setCurrentIndex(const QModelIndex& index, QItemSelectionModel::SelectionFlags f)
{
    selectionModel()->setCurrentIndex(index, f);
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
QHash<int, QString>* FlickableViewPrivate::reloadRoleNames(const QModelIndex& index) const
{
    if (!m_pModel)
        return nullptr;

    const auto m = index.model() ? index.model() : m_pModel.data();

    auto* hash = m == m_pModel ? &m_hRoleNames : m_hhOtherRoleNames.value(m);

    if (!hash)
        m_hhOtherRoleNames[m] = hash = new QHash<int, QString>;

    hash->clear();

    const auto roleNames = m->roleNames();

    for (auto i = roleNames.constBegin(); i != roleNames.constEnd(); ++i)
        (*hash)[i.key()] = i.value();

    return hash;
}

#include <QSortFilterProxyModel>

void FlickableView::applyRoles(QQmlContext* ctx, const QModelIndex& self) const
{
    auto m = self.model();

    if (Q_UNLIKELY(!m))
        return;

    auto* hash = m == d_ptr->m_pModel ?
        &d_ptr->m_hRoleNames : d_ptr->m_hhOtherRoleNames.value(m);

    // Refresh the cache
    if ((!hash) || model()->roleNames().size() != hash->size())
        hash = d_ptr->reloadRoleNames(self);

    // Add all roles to the
    for (auto i = hash->constBegin(); i != hash->constEnd(); ++i)
        ctx->setContextProperty(i.value() , self.data(i.key()));

    // Set extra index to improve ListView compatibility
    ctx->setContextProperty(QStringLiteral("index"        ) , self.row()        );
    ctx->setContextProperty(QStringLiteral("rootIndex"    ) , self              );
    ctx->setContextProperty(QStringLiteral("rowCount"     ) , m->rowCount(self) );

    const bool sel = d_ptr->m_pSelectionModel && d_ptr->m_pSelectionModel->currentIndex() == self;

    ctx->setContextProperty(QStringLiteral("isCurrentItem") , sel );
}

QPair<QQuickItem*, QQmlContext*> FlickableView::loadDelegate(QQuickItem* parentI, QQmlContext* parentCtx, const QModelIndex& self) const
{
    if (!delegate())
        return {};

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

    applyRoles(pctx, self);

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


void FlickableViewPrivate::slotCurrentIndexChanged(const QModelIndex& idx)
{
    if ((!idx.isValid()) && !m_pSelectedItem)
        return;

    Q_EMIT q_ptr->currentIndexChanged(idx);


    if (m_pSelectedItem && !idx.isValid()) {
        delete m_pSelectedItem;
        m_pSelectedItem = nullptr;
        return;
    }

    if (!q_ptr->highlight())
        return;

    auto elem = q_ptr->itemForIndex(idx);

    // QItemSelectionModel::setCurrentIndex isn't protected against setting the item many time
    if (m_pSelectedViewItem && elem == m_pSelectedViewItem.data()) {
        // But it may have moved
        const auto geo = elem->geometry();
        m_pSelectedItem->setWidth(geo.width());
        m_pSelectedItem->setHeight(geo.height());
        m_pSelectedItem->setY(geo.y());

        return;
    }

    // There is no need to waste effort if the element is not visible
    if ((!elem) || (!elem->isVisible())) {
        if (m_pSelectedItem)
            m_pSelectedItem->setVisible(false);
        return;
    }

    // Create the highlighter
    if (!m_pSelectedItem) {
        m_pSelectedItem = qobject_cast<QQuickItem*>(q_ptr->highlight()->create(
            q_ptr->rootContext()
        ));
        m_pSelectedItem->setParentItem(q_ptr->contentItem());
        q_ptr->rootContext()->engine()->setObjectOwnership(
            m_pSelectedItem,QQmlEngine::CppOwnership
        );
        m_pSelectedItem->setX(0);
    }

    const auto geo = elem->geometry();
    m_pSelectedItem->setVisible(true);
    m_pSelectedItem->setWidth(geo.width());
    m_pSelectedItem->setHeight(geo.height());

    elem->setSelected(true);

    if (m_pSelectedViewItem)
        m_pSelectedViewItem.toStrongRef()->setSelected(false);

    // Use X/Y to allow behavior to perform the silly animation
    m_pSelectedItem->setY(
        geo.y()
    );

    m_pSelectedViewItem = elem->reference();
}

void FlickableViewPrivate::slotSelectionModelChanged()
{
    if (m_pSelectionModel)
        disconnect(m_pSelectionModel.data(), &QItemSelectionModel::currentChanged,
            this, &FlickableViewPrivate::slotCurrentIndexChanged);

    m_pSelectionModel = q_ptr->selectionModel();

    if (m_pSelectionModel)
        connect(m_pSelectionModel.data(), &QItemSelectionModel::currentChanged,
            this, &FlickableViewPrivate::slotCurrentIndexChanged);

    slotCurrentIndexChanged(
        m_pSelectionModel ? m_pSelectionModel->currentIndex() : QModelIndex()
    );
}

// Because smart pointer are ref counted and QItemSelectionModel is not, there
// is a race condition where ::loadDelegate or ::applyRoles can be called
// on a QItemSelectionModel::currentIndex while the model has just been destroyed
void FlickableViewPrivate::slotModelDestroyed()
{
    if (m_pSelectionModel && m_pSelectionModel->model() == QObject::sender())
        q_ptr->setSelectionModel(nullptr);
}

bool FlickableView::isEmpty() const
{
    return d_ptr->m_pModel ? !d_ptr->m_pModel->rowCount() : true;
}

FlickableView::ModelIndexItem::ModelIndexItem(FlickableView* v) : m_pView(v){}

void FlickableView::ModelIndexItem::updateGeometry()
{
    const auto geo = geometry();

    //TODO handle up/left/right too

    if (!down())
        view()->contentItem()->setHeight(std::max(
            geo.y()+geo.height(), view()->height()
        ));

    if (view()->d_ptr->m_pSelectionModel && view()->d_ptr->m_pSelectionModel->currentIndex() == index())
        view()->d_ptr->slotCurrentIndexChanged(index());
}

FlickableView* FlickableView::ModelIndexItem::view() const
{
    return m_pView;
}

#include <flickableview.moc>
