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
class QuickListViewItem;

/**
 * Holds the metadata associated with a section.
 *
 * A section is created when a QuickListViewItem has a property that differs
 * from the previous QuickListViewItem in the list. It can also cross reference
 * into an external model to provide more flexibility.
 */
struct QuickListViewSection
{
    explicit QuickListViewSection(
        QuickListViewItem* owner,
        const QVariant&    value
    );

    QuickListViewItem*    m_pOwner   {nullptr};
    QQuickItem*           m_pItem    {nullptr};
    QQmlContext*          m_pContent {nullptr};
    int                   m_Index    {   0   };
    int                   m_RefCount {   1   };
    QVariant              m_Value    {       };
    QuickListViewSection* m_pPrevious{nullptr};
    QuickListViewSection* m_pNext    {nullptr};

    // Mutator
    QQuickItem* item(QQmlComponent* component);
};

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

    QQuickItem*           m_pItem    {nullptr};
    QQmlContext*          m_pContent {nullptr};
    QuickListViewSection* m_pSection {nullptr};

private:
    QuickListViewPrivate* d() const;
};

class QuickListViewPrivate : public QObject
{
    Q_OBJECT
public:

    // When all elements are assumed to have the same height, life is easy
    QVector<qreal>                      m_DepthChart      {   0   };
    QuickListViewSections*              m_pSections       {nullptr};
    QSharedPointer<QItemSelectionModel> m_pSelectionModel {nullptr};
    QQuickItem*                         m_pSelectedItem   {nullptr};
    QWeakPointer<VolatileTreeItem>      m_pSelectedViewItem;

    // Sections
    QQmlComponent*        m_pDelegate     {nullptr};
    QString               m_Property      {       };
    QStringList           m_Roles         {       };
    int                   m_CachedRole    {   0   };
    mutable bool          m_IndexLoaded   { false };
    QuickListViewSection* m_pFirstSection {nullptr};
    QSharedPointer<QAbstractItemModel> m_pSectionModel;

    // Helpers
    QuickListViewSection* getSection(QuickListViewItem* i);
    void reloadSectionIndices() const;

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
    if (!d_ptr->m_pSections) {
        d_ptr->m_pSections = new QuickListViewSections(
            const_cast<QuickListView*>(this)
        );

        const_cast<QuickListView*>(this)->reload();
    }

    return d_ptr->m_pSections;
}

VolatileTreeItem* QuickListView::createItem() const
{
    return new QuickListViewItem();
}

void QuickListViewPrivate::slotCurrentIndexChanged(const QModelIndex& idx)
{

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

QuickListViewSection::QuickListViewSection(
    QuickListViewItem* owner,
    const QVariant& value
)
{
    m_pOwner = owner;
    m_Value  = value;

    m_pContent = new QQmlContext(owner->view()->rootContext());
    m_pContent->setContextProperty("section", value);
}

QQuickItem* QuickListViewSection::item(QQmlComponent* component)
{
    if (m_pItem)
        return m_pItem;

    m_pItem = qobject_cast<QQuickItem*>(component->create(
        m_pContent
    ));

    m_pItem->setParentItem(m_pOwner->view()->contentItem());

    return m_pItem;
}

/**
 * No lookup is performed, it is based on the previous entry and nothing else.
 *
 * This view only supports list. If it's with a tree, it will break and "don't
 * do this".
 */
QuickListViewSection* QuickListViewPrivate::getSection(QuickListViewItem* i)
{
    if (m_pSections->property().isEmpty() || !m_pDelegate)
        return nullptr;

    if (i->m_pSection)
        return i->m_pSection;

    const auto prev = static_cast<QuickListViewItem*>(i->previous());

    // The section owner isn't currently loaded
    if ((!prev) && i->index().row() > 0) {
        Q_ASSERT(false); //TODO when GC is enabled, the assert is to make sure I don't forget
    }

    const auto val = q_ptr->model()->data(i->index(), m_pSections->role());

    // Use the previous section
    if (prev && prev->m_pSection && prev->m_pSection->m_Value == val) {
        i->m_pSection = prev->m_pSection;
        i->m_pSection->m_RefCount++;
        return prev->m_pSection;
    }

    // Create a section
    i->m_pSection = new QuickListViewSection(i, val);

    // Update the double linked list
    if (prev && prev->m_pSection) {
        prev->m_pSection->m_pNext  = i->m_pSection;
        i->m_pSection->m_pPrevious = prev->m_pSection;
        i->m_pSection->m_Index     = prev->m_pSection->m_Index + 1;

        Q_ASSERT(prev->m_pSection != prev->m_pSection->m_pNext);
        Q_ASSERT(i->m_pSection->m_pPrevious !=  i->m_pSection);
    }

    m_pFirstSection = m_pFirstSection || (!prev) ?
        m_pFirstSection : prev->m_pSection;

    if (m_pSectionModel && !m_IndexLoaded)
        reloadSectionIndices();

    if (m_pSectionModel) {
        q_ptr->applyRoles(
            i->m_pSection->m_pContent,
            m_pSectionModel->index(i->m_pSection->m_Index, 0)
        );
    }

    // Create the item *after* applyRoles to avoid O(N) number of reloads
    q_ptr->rootContext()->engine()->setObjectOwnership(
        i->m_pSection->item(m_pDelegate), QQmlEngine::CppOwnership
    );

    return i->m_pSection;
}

/**
 * Set indices for each sections to the right value.
 */
void QuickListViewPrivate::reloadSectionIndices() const
{
    int idx = 0;
    for (auto i = m_pFirstSection; i; i = i->m_pNext) {
        Q_ASSERT(i != i->m_pNext);
        i->m_Index = idx++;
    }

    //FIXME this assumes all sections are always loaded, this isn't correct

    m_IndexLoaded = m_pFirstSection != nullptr;
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

    if (!pair.first) {
        qDebug() << "Item failed to load" << index().data();
        return false;
    }

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
    if (m_pContent)
        d()->q_ptr->applyRoles(m_pContent, index());

    return true;
}

bool QuickListViewItem::move()
{
    const auto prev = static_cast<QuickListViewItem*>(previous());

    const QQuickItem* prevItem = nullptr;

    if (auto sec = d()->getSection(this)) {
        if (sec->m_pOwner == this && sec->m_pItem) {
            if (prev && prev->m_pItem) {
                auto anchors = qvariant_cast<QObject*>(sec->m_pItem->property("anchors"));
                anchors->setProperty("top", prev->m_pItem->property("bottom"));
            }
            else {
                sec->m_pItem->setY(0);
            }

            if (!sec->m_pItem->width())
                sec->m_pItem->setWidth(view()->contentItem()->width());

            prevItem = sec->m_pItem;
        }
    }

    const qreal y = d()->m_DepthChart.first()*index().row();

    if (!m_pItem->width())
        m_pItem->setWidth(view()->contentItem()->width());

    prevItem = prevItem ? prevItem : prev ? prev->m_pItem : nullptr;

    // So other items can be GCed without always resetting to 0x0, note that it
    // might be a good idea to extend SimpleFlickable to support a virtual
    // origin point.
    if (!prevItem)
        m_pItem->setY(y);
    else {
        auto anchors = qvariant_cast<QObject*>(m_pItem->property("anchors"));
        anchors->setProperty("top", prevItem->property("bottom"));
    }

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
    if (m_pSection && --m_pSection->m_RefCount >= 0) {
        if (m_pSection->m_pPrevious) {
            Q_ASSERT(m_pSection->m_pPrevious != m_pSection->m_pNext);
            m_pSection->m_pPrevious->m_pNext = m_pSection->m_pNext;
        }

        if (m_pSection == d()->m_pFirstSection)
            d()->m_pFirstSection = m_pSection->m_pNext;

        d()->m_IndexLoaded = false;

        delete m_pSection;
    }

    m_pSection = nullptr;

    //TODO move back into treeview2
    //TODO check if the item has references, if it does, just release the shared
    // pointer and move on.
    return true;
}

QuickListViewSections::QuickListViewSections(QuickListView* parent) :
    QObject(parent), d_ptr(parent->d_ptr)
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

int QuickListViewSections::role() const
{
    if (d_ptr->m_Property.isEmpty() || !d_ptr->q_ptr->model())
        return Qt::DisplayRole;

    if (d_ptr->m_CachedRole)
        return d_ptr->m_CachedRole;

    const auto roles = d_ptr->q_ptr->model()->roleNames();

    if (!(d_ptr->m_CachedRole = roles.key(d_ptr->m_Property.toLatin1()))) {
        qWarning() << d_ptr->m_Property << "is not a model property";
        return Qt::DisplayRole;
    }

    return d_ptr->m_CachedRole;
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

QSharedPointer<QAbstractItemModel> QuickListViewSections::model() const
{
    return d_ptr->m_pSectionModel;
}

void QuickListViewSections::setModel(const QSharedPointer<QAbstractItemModel>& m)
{
    d_ptr->m_pSectionModel = m;
}


#include <quicklistview.moc>
