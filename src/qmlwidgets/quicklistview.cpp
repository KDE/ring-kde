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
#include "quicklistview.h"

// Qt
#include <QQmlEngine>
#include <QQmlContext>
#include <QtCore/QItemSelectionModel>

class QuickListViewPrivate;

/**
 */
class QuickListViewItem : public VolatileTreeItem
{
public:
    explicit QuickListViewItem();
    virtual ~QuickListViewItem() {}

    // Actions
    virtual bool attach () override;
    virtual bool refresh() override;
    virtual bool move   () override;
    virtual bool flush  () override;
    virtual bool detach () override;

    QQuickItem*  m_pItem    {nullptr};
    QQmlContext* m_pContent {nullptr};

private:
    QuickListViewPrivate* d() const;
};

class QuickListViewSectionsPrivate
{
public:
    QQmlComponent* m_pDelegate {nullptr};
    QString        m_Property;
    QStringList    m_Roles;
};

class QuickListViewPrivate : public QObject
{
    Q_OBJECT
public:

    // When all elements are assumed to have the same height, life is easy
    QVector<qreal> m_DepthChart {0};
    QuickListViewSections* m_pSection {nullptr};
    QSharedPointer<QItemSelectionModel> m_pSelectionModel {nullptr};
    QQuickItem* m_pSelectedItem {nullptr};
    QWeakPointer<VolatileTreeItem> m_pSelectedViewItem;

    QuickListView* q_ptr;

public Q_SLOTS:
    void slotCurrentIndexChanged(const QModelIndex& idx);
    void slotSelectionModelChanged();
};

QuickListView::QuickListView(QQuickItem* parent) : TreeView2(parent),
    d_ptr(new QuickListViewPrivate)
{
    d_ptr->q_ptr = this;

    connect(this, &FlickableView::selectionModelChanged,
        d_ptr, &QuickListViewPrivate::slotSelectionModelChanged);
}

QuickListView::~QuickListView()
{
    delete d_ptr;
}

int QuickListView::count() const
{
    return model() ? model()->rowCount() : 0;
}

int QuickListView::currentIndex() const
{
    return selectionModel()->currentIndex().row();
}

void QuickListView::setCurrentIndex(int index)
{
    if (!model())
        return;

    selectionModel()->setCurrentIndex(
        model()->index(index, 0),
        QItemSelectionModel::ClearAndSelect
    );
}

QuickListViewSections* QuickListView::section() const
{
    if (!d_ptr->m_pSection)
        d_ptr->m_pSection = new QuickListViewSections(
            const_cast<QuickListView*>(this)
        );

    return d_ptr->m_pSection;
}

VolatileTreeItem* QuickListView::createItem() const
{
    return new QuickListViewItem();
}

void QuickListViewPrivate::slotCurrentIndexChanged(const QModelIndex& idx)
{
    qDebug() << "\n\nSET SELECTION" << idx;

    if ((!idx.isValid()) && !m_pSelectedItem)
        return;

    Q_EMIT q_ptr->indexChanged(idx.row());

    if (m_pSelectedItem && !idx.isValid()) {
        delete m_pSelectedItem;
        m_pSelectedItem = nullptr;
        return;
    }

    if (!q_ptr->highlight())
        return;

    auto elem = static_cast<QuickListViewItem*>(q_ptr->itemForIndex(idx));

    // There is no need to waste effort if the element is not visible
    if ((!elem) || (!elem->m_pItem)) {
        if (m_pSelectedItem)
            m_pSelectedItem->setVisible(false);
        return;
    }

    // Create the highlighter
    if (!m_pSelectedItem) {
        m_pSelectedItem = qobject_cast<QQuickItem*>(q_ptr->highlight()->create(
            elem->view()->rootContext()
        ));
        m_pSelectedItem->setParentItem(elem->view()->contentItem());
        q_ptr->rootContext()->engine()->setObjectOwnership(
            m_pSelectedItem,QQmlEngine::CppOwnership
        );
        m_pSelectedItem->setX(0);
    }

    m_pSelectedItem->setVisible(true);
    m_pSelectedItem->setWidth(elem->m_pItem->width());
    m_pSelectedItem->setHeight(elem->m_pItem->height());

    elem->m_pContent->setContextProperty("isCurrentItem", true);

    if (m_pSelectedViewItem) {
        auto prev = static_cast<QuickListViewItem*>(m_pSelectedViewItem.data());
        prev->m_pContent->setContextProperty("isCurrentItem", false);
    }

    // Use X/Y to allow behavior to perform the silly animation
    m_pSelectedItem->setY(
        -elem->m_pItem->mapFromItem(q_ptr->contentItem(), {0,0}).y()
    );

    m_pSelectedViewItem = elem->reference();
}

void QuickListViewPrivate::slotSelectionModelChanged()
{
    if (m_pSelectionModel)
        disconnect(m_pSelectionModel.data(), &QItemSelectionModel::currentChanged,
            this, &QuickListViewPrivate::slotCurrentIndexChanged);

    m_pSelectionModel = q_ptr->selectionModel();

    if (m_pSelectionModel)
        connect(m_pSelectionModel.data(), &QItemSelectionModel::currentChanged,
            this, &QuickListViewPrivate::slotCurrentIndexChanged);

    slotCurrentIndexChanged(
        m_pSelectionModel ? m_pSelectionModel->currentIndex() : QModelIndex()
    );
}

QuickListViewItem::QuickListViewItem() : VolatileTreeItem()
{
}

QuickListViewPrivate* QuickListViewItem::d() const
{
    return static_cast<QuickListView*>(view())->QuickListView::d_ptr;
}

bool QuickListViewItem::attach()
{
    auto pair = static_cast<QuickListView*>(view())->loadDelegate(
        view()->contentItem(),
        view()->rootContext(),
        index()
    );

    if (!pair.first)
        return false;

    if (!pair.first->z())
        pair.first->setZ(1);

    d()->m_DepthChart[depth()] = std::max(
        d()->m_DepthChart[depth()],
        pair.first->height()
    );

    m_pContent = pair.second;
    m_pItem    = pair.first;

    m_pContent->setContextProperty("isCurrentItem", false);

    return move();
}

bool QuickListViewItem::refresh()
{
    return true;
}

bool QuickListViewItem::move()
{

    const qreal y = d()->m_DepthChart.first()*index().row();

    m_pItem->setWidth(view()->contentItem()->width());

//     qDebug() << "MOVE" <<  view()->width() << previous();

    // So other items can be GCed without always resetting to 0x0, note that it
    // might be a good idea to extend SimpleFlickable to support a virtual
    // origin point.
    if (!previous())
        m_pItem->setY(y);
    else if (auto otheri = static_cast<QuickListViewItem*>(previous())->m_pItem) {
//         qDebug() << "SET ANCHORS";
        auto anchors = qvariant_cast<QObject*>(m_pItem->property("anchors"));
        anchors->setProperty("top", otheri->property("bottom"));
    }
    else
        Q_ASSERT(false); // The chain must be corrupted

    if (view()->contentItem()->height() < y+m_pItem->height())
        view()->contentItem()->setHeight(y+m_pItem->height());

    return true;
}

bool QuickListViewItem::flush()
{
    return true;
}

bool QuickListViewItem::detach()
{
    //TODO move back into treeview2
    //TODO check if the item has references, if it does, just release the shared
    // pointer and move on.
    return true;
}

QuickListViewSections::QuickListViewSections(QObject* parent) : QObject(parent),
    d_ptr(new QuickListViewSectionsPrivate)
{

}

QuickListViewSections::~QuickListViewSections()
{
    delete d_ptr;
}

QQmlComponent* QuickListViewSections::delegate() const
{
    return d_ptr->m_pDelegate;
}

void QuickListViewSections::setDelegate(QQmlComponent* component)
{
    d_ptr->m_pDelegate = component;
}

QString QuickListViewSections::property() const
{
    return d_ptr->m_Property;
}

void QuickListViewSections::setProperty(const QString& property)
{
    d_ptr->m_Property = property;
}

QStringList QuickListViewSections::roles() const
{
    return d_ptr->m_Roles;
}

void QuickListViewSections::setRoles(const QStringList& list)
{
    d_ptr->m_Roles = list;
}

#include <quicklistview.moc>
