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
#include "abstractselectableview.h"

#include "abstractselectableview_p.h"
#include "abstractquickview.h"
#include "abstractviewitem.h"

// Qt
#include <QtCore/QItemSelectionModel>
#include <QtCore/QSharedPointer>
#include <QQuickItem>
#include <QQmlEngine>
#include <QQmlContext>

using ItemRef = QPair<QWeakPointer<AbstractViewItem::SelectionLocker>, AbstractViewItem*>;

class AbstractSelectableViewPrivate : public QObject
{
    friend class AbstractSelectableViewSyncInterface;
    Q_OBJECT
public:
    QSharedPointer<QItemSelectionModel> m_pSelectionModel {nullptr};
    QAbstractItemModel*  m_pModel          {nullptr};

    QQuickItem* m_pSelectedItem   {nullptr};
    ItemRef m_pSelectedViewItem;
    QQmlComponent* m_pHighlight {nullptr};
    AbstractQuickView* m_pView {nullptr};

    AbstractSelectableView* q_ptr;
public Q_SLOTS:
    void slotCurrentIndexChanged(const QModelIndex& idx);
    void slotSelectionModelChanged();
    void slotModelDestroyed();
};

AbstractSelectableView::AbstractSelectableView(QObject* parent) : QObject(parent),
    d_ptr(new AbstractSelectableViewPrivate()),
    s_ptr(new AbstractSelectableViewSyncInterface())
{
    s_ptr->q_ptr = this;
    d_ptr->q_ptr = this;
}

AbstractSelectableView::~AbstractSelectableView()
{
    //
}

QSharedPointer<QItemSelectionModel> AbstractSelectableView::selectionModel() const
{
    if (s_ptr->model() && !d_ptr->m_pSelectionModel) {
        auto sm = new QItemSelectionModel(s_ptr->model());
        d_ptr->m_pSelectionModel = QSharedPointer<QItemSelectionModel>(sm);
        d_ptr->slotSelectionModelChanged();
        Q_EMIT selectionModelChanged();
        connect(d_ptr->m_pSelectionModel.data(), &QItemSelectionModel::currentChanged,
            d_ptr, &AbstractSelectableViewPrivate::slotCurrentIndexChanged);
    }

    return d_ptr->m_pSelectionModel;
}

void AbstractSelectableView::setSelectionModel(QSharedPointer<QItemSelectionModel> m)
{
    d_ptr->m_pSelectionModel = m;

    // This will cause undebugable issues. Better ban it early
    Q_ASSERT((!s_ptr->model()) || (!m) || s_ptr->model() == m->model());

    d_ptr->slotSelectionModelChanged();
    Q_EMIT selectionModelChanged();
}

QAbstractItemModel* AbstractSelectableViewSyncInterface::model() const
{
    return q_ptr->d_ptr->m_pModel;
}

void AbstractSelectableViewSyncInterface::setModel(QAbstractItemModel* m)
{
    if (q_ptr->d_ptr->m_pModel)
        QObject::disconnect(q_ptr->d_ptr->m_pModel, &QObject::destroyed,
            q_ptr->d_ptr, &AbstractSelectableViewPrivate::slotModelDestroyed);

    if (q_ptr->d_ptr->m_pSelectionModel && q_ptr->d_ptr->m_pSelectionModel->model() != m)
        q_ptr->d_ptr->m_pSelectionModel = nullptr;

    // In theory it can cause an issue when QML set both properties in random
    // order when replacing this model, but for until that happens, better be
    // strict.
    Q_ASSERT((!q_ptr->d_ptr->m_pSelectionModel)
        || (!m)
        || m == q_ptr->d_ptr->m_pSelectionModel->model()
    );


    q_ptr->d_ptr->m_pModel = m;

    if (m)
        QObject::connect(m, &QObject::destroyed,
            q_ptr->d_ptr, &AbstractSelectableViewPrivate::slotModelDestroyed);

}

AbstractQuickView* AbstractSelectableViewSyncInterface::view() const
{
    return q_ptr->d_ptr->m_pView;
}

void AbstractSelectableViewSyncInterface::setView(AbstractQuickView* v)
{
    q_ptr->d_ptr->m_pView = v;
}


void AbstractSelectableViewSyncInterface::updateSelection(const QModelIndex& idx)
{
    q_ptr->d_ptr->slotCurrentIndexChanged(idx);
}

void AbstractSelectableViewPrivate::slotCurrentIndexChanged(const QModelIndex& idx)
{
    if ((!idx.isValid()))
        return;

    Q_ASSERT(m_pView);

    Q_EMIT q_ptr->currentIndexChanged(idx);


    if (m_pSelectedItem && !idx.isValid()) {
        delete m_pSelectedItem;
        m_pSelectedItem = nullptr;
        return;
    }

    if (!m_pHighlight)
        return;

    auto elem = m_pView->itemForIndex(idx);

    // QItemSelectionModel::setCurrentIndex isn't protected against setting the item many time
    if (m_pSelectedViewItem.first && elem == m_pSelectedViewItem.second) {
        // But it may have moved
        const auto geo = elem->geometry();
        m_pSelectedItem->setWidth(geo.width());
        m_pSelectedItem->setHeight(geo.height());
        m_pSelectedItem->setY(geo.y());

        return;
    }

    // There is no need to waste effort if the element is not visible
    /*if ((!elem) || (!elem->isVisible())) {
        if (m_pSelectedItem)
            m_pSelectedItem->setVisible(false);
        return;
    }*/ //FIXME

    // Create the highlighter
    if (!m_pSelectedItem) {
        m_pSelectedItem = qobject_cast<QQuickItem*>(q_ptr->highlight()->create(
            m_pView->rootContext()
        ));
        m_pSelectedItem->setParentItem(m_pView->contentItem());
        m_pView->rootContext()->engine()->setObjectOwnership(
            m_pSelectedItem,QQmlEngine::CppOwnership
        );
        m_pSelectedItem->setX(0);
    }

    const auto geo = elem->geometry();
    m_pSelectedItem->setVisible(true);
    m_pSelectedItem->setWidth(geo.width());
    m_pSelectedItem->setHeight(geo.height());

    elem->setSelected(true);

    if (m_pSelectedViewItem.first)
        m_pSelectedViewItem.second->setSelected(false);

    // Use X/Y to allow behavior to perform the silly animation
    m_pSelectedItem->setY(
        geo.y()
    );

    m_pSelectedViewItem = elem->weakReference();
}

void AbstractSelectableViewPrivate::slotSelectionModelChanged()
{
    if (m_pSelectionModel)
        disconnect(m_pSelectionModel.data(), &QItemSelectionModel::currentChanged,
            this, &AbstractSelectableViewPrivate::slotCurrentIndexChanged);

    m_pSelectionModel = q_ptr->selectionModel();

    if (m_pSelectionModel)
        connect(m_pSelectionModel.data(), &QItemSelectionModel::currentChanged,
            this, &AbstractSelectableViewPrivate::slotCurrentIndexChanged);

    slotCurrentIndexChanged(
        m_pSelectionModel ? m_pSelectionModel->currentIndex() : QModelIndex()
    );
}

// Because smart pointer are ref counted and QItemSelectionModel is not, there
// is a race condition where ::loadDelegate or ::applyRoles can be called
// on a QItemSelectionModel::currentIndex while the model has just been destroyed
void AbstractSelectableViewPrivate::slotModelDestroyed()
{
    if (m_pSelectionModel && m_pSelectionModel->model() == QObject::sender())
        q_ptr->setSelectionModel(nullptr);
}

QQmlComponent* AbstractSelectableView::highlight() const
{
    return d_ptr->m_pHighlight;
}

void AbstractSelectableView::setHighlight(QQmlComponent* h)
{
    d_ptr->m_pHighlight = h;
}

void AbstractSelectableView::applySelectionRoles(QQmlContext* ctx, const QModelIndex& self) const
{
    const bool sel = d_ptr->m_pSelectionModel && d_ptr->m_pSelectionModel->currentIndex() == self;

    ctx->setContextProperty(QStringLiteral("isCurrentItem") , sel );
}

#include <abstractselectableview.moc>
