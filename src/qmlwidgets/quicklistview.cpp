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
struct QuickListViewSection final
{
    explicit QuickListViewSection(
        QuickListViewItem* owner,
        const QVariant&    value
    );
    ~QuickListViewSection();

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

    // Helpers
    void reparentSection(QuickListViewItem* newParent, FlickableView* view);
    void setOwner(QuickListViewItem* newOwner);
};

/**
 */
class QuickListViewItem : public VisualTreeItem
{
public:
    explicit QuickListViewItem(FlickableView* v);
    virtual ~QuickListViewItem();

    // Actions
    virtual bool attach () override;
    virtual bool refresh() override;
    virtual bool move   () override;
    virtual bool flush  () override;
    virtual bool remove () override;

    QQuickItem*           m_pItem    {nullptr};
    QQmlContext*          m_pContent {nullptr};
    QuickListViewSection* m_pSection {nullptr};

    // Setters
    virtual void setSelected(bool s) final override;
    QuickListViewSection* setSection(QuickListViewSection* s, const QVariant& val);

    /// Geometry relative to the FlickableView::view()
    virtual QRectF geometry() const final override;

    QuickListViewPrivate* d() const;
};

class QuickListViewPrivate : public QObject
{
    Q_OBJECT
public:
    explicit QuickListViewPrivate(QuickListView* p) : QObject(p), q_ptr(p){}

    // When all elements are assumed to have the same height, life is easy
    QVector<qreal>         m_DepthChart {   0   };
    QuickListViewSections* m_pSections  {nullptr};

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
    void slotCurrentIndexChanged(const QModelIndex& index);
    void slotDataChanged(const QModelIndex& tl, const QModelIndex& br);
};

QuickListView::QuickListView(QQuickItem* parent) : TreeView2(parent),
    d_ptr(new QuickListViewPrivate(this))
{
    connect(this, &FlickableView::currentIndexChanged,
        d_ptr, &QuickListViewPrivate::slotCurrentIndexChanged);
}

QuickListViewItem::~QuickListViewItem()
{
    // If this item is the section owner, assert before crashing
    if (m_pSection && m_pSection->m_pOwner == this) {
        Q_ASSERT(false);
    }

    if (m_pItem)
        delete m_pItem;

    if (m_pContent)
        delete m_pContent;
}

QuickListView::~QuickListView()
{
    // Delete the sections
    while(auto sec = d_ptr->m_pFirstSection)
        delete sec;

    if (d_ptr->m_pSections)
        delete d_ptr->m_pSections;
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

    TreeView2::setCurrentIndex(
        model()->index(index, 0),
        QItemSelectionModel::ClearAndSelect
    );
}

void QuickListView::setModel(QSharedPointer<QAbstractItemModel> m)
{
    if (auto oldM = model())
        disconnect(oldM.data(), &QAbstractItemModel::dataChanged, d_ptr,
            &QuickListViewPrivate::slotDataChanged);

    TreeView2::setModel(m);

    if (!m)
        return;

    connect(m.data(), &QAbstractItemModel::dataChanged, d_ptr,
        &QuickListViewPrivate::slotDataChanged);
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

FlickableView::ModelIndexItem* QuickListView::createItem() const
{
    return new QuickListViewItem(
        const_cast<QuickListView*>(this)
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

QuickListViewSection* QuickListViewItem::setSection(QuickListViewSection* s, const QVariant& val)
{
    if ((!s) || s->m_Value != val)
        return nullptr;

    const auto p = static_cast<QuickListViewItem*>(up  ());
    const auto n = static_cast<QuickListViewItem*>(down());

    // Garbage collect or change the old section owner
    if (m_pSection) {
        if (--m_pSection->m_RefCount <= 0)
            delete m_pSection;
        else if (p && n && p->m_pSection && p->m_pSection != m_pSection && n->m_pSection == m_pSection)
            m_pSection->setOwner(n);
        else if (p->m_pSection != m_pSection)
            Q_ASSERT(false); // There is a bug somewhere else
    }

    m_pSection = s;
    s->m_RefCount++;

    if ((!p) || p->m_pSection != s)
        s->setOwner(this);

    return s;
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

    const auto val = q_ptr->model()->data(i->index(), m_pSections->role());

    if (i->m_pSection && i->m_pSection->m_Value == val)
        return i->m_pSection;

    const auto prev = static_cast<QuickListViewItem*>(i->up  ());
    const auto next = static_cast<QuickListViewItem*>(i->down());

    // The section owner isn't currently loaded
    if ((!prev) && i->index().row() > 0)
        Q_ASSERT(false); //TODO when GC is enabled, the assert is to make sure I don't forget

    // Check if the nearby sections are compatible
    for (auto& s : {
        prev ? prev->m_pSection : nullptr, m_pFirstSection, next ? next->m_pSection : nullptr
    }) if (auto ret = i->setSection(s, val))
            return ret;

    // Create a section
    i->m_pSection = new QuickListViewSection(i, val);
    Q_ASSERT(i->m_pSection->m_RefCount == 1);

    // Update the double linked list
    if (prev && prev->m_pSection) {

        if (prev->m_pSection->m_pNext) {
            prev->m_pSection->m_pNext->m_pPrevious = i->m_pSection;
            i->m_pSection->m_pNext = prev->m_pSection->m_pNext;
        }

        prev->m_pSection->m_pNext  = i->m_pSection;
        i->m_pSection->m_pPrevious = prev->m_pSection;
        i->m_pSection->m_Index     = prev->m_pSection->m_Index + 1;

        Q_ASSERT(prev->m_pSection != prev->m_pSection->m_pNext);
        Q_ASSERT(i->m_pSection->m_pPrevious !=  i->m_pSection);
    }

    m_pFirstSection = m_pFirstSection ?
        m_pFirstSection : i->m_pSection;

    Q_ASSERT(m_pFirstSection);

    if (m_pSectionModel && !m_IndexLoaded)
        reloadSectionIndices();

    if (m_pSectionModel) {
        const auto idx = m_pSectionModel->index(i->m_pSection->m_Index, 0);
        Q_ASSERT((!idx.isValid()) || idx.model() == m_pSectionModel);

        q_ptr->applyRoles( i->m_pSection->m_pContent, idx);
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

QuickListViewItem::QuickListViewItem(FlickableView* p) : VisualTreeItem(p)
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
    m_pContent->setContextProperty("modelIndex", index());

    return move();
}

bool QuickListViewItem::refresh()
{
    if (m_pContent)
        d()->q_ptr->applyRoles(m_pContent, index());

    return true;
}
void QuickListViewSection::setOwner(QuickListViewItem* newParent)
{
    if (m_pOwner == newParent)
        return;

    if (m_pOwner->m_pItem) {
        auto anchors = qvariant_cast<QObject*>(m_pOwner->m_pItem->property("anchors"));
        anchors->setProperty("top", newParent->m_pItem ?
            newParent->m_pItem->property("bottom") : QVariant()
        );
    }
    else
        newParent->m_pItem->setY(0);

    m_pOwner = newParent;
}

void QuickListViewSection::reparentSection(QuickListViewItem* newParent, FlickableView* view)
{
    if (!m_pItem)
        return;

    if (newParent && newParent->m_pItem) {
        auto anchors = qvariant_cast<QObject*>(m_pItem->property("anchors"));
        anchors->setProperty("top", newParent->m_pItem->property("bottom"));

        m_pItem->setParentItem(m_pOwner->view()->contentItem());
    }
    else {
        m_pItem->setY(0);
    }

    if (!m_pItem->width())
        m_pItem->setWidth(view->contentItem()->width());
}

bool QuickListViewItem::move()
{
    const auto prev = static_cast<QuickListViewItem*>(up());

    const QQuickItem* prevItem = nullptr;

    if (d()->m_pSections)
        if (auto sec = d()->getSection(this)) {
            if (sec->m_pOwner == this && sec->m_pItem) {
                sec->reparentSection(prev ? prev : nullptr, view());

                prevItem = sec->m_pItem;
            }
        }

    const qreal y = d()->m_DepthChart.first()*index().row();

    if (m_pItem->width() != view()->contentItem()->width())
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

    updateGeometry();

    return true;
}

bool QuickListViewItem::flush()
{
    return true;
}

bool QuickListViewItem::remove()
{
    if (m_pSection && --m_pSection->m_RefCount <= 0) {
        delete m_pSection;
    }
    else if (m_pSection && m_pSection->m_pOwner == this) {
        // Reparent the section
        if (auto n = static_cast<QuickListViewItem*>(down())) {
            if (n->m_pSection == m_pSection)
                m_pSection->m_pOwner = n;
            else
                Q_ASSERT(false);
        }
        else
            Q_ASSERT(false);
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

QuickListViewSection::~QuickListViewSection()
{
    auto d_ptr = m_pOwner->d();

    if (m_pPrevious) {
        Q_ASSERT(m_pPrevious != m_pNext);
        m_pPrevious->m_pNext = m_pNext;
    }

    if (m_pNext)
        m_pNext->m_pPrevious = m_pPrevious;

    if (this == d_ptr->m_pFirstSection)
        d_ptr->m_pFirstSection = m_pNext;

    d_ptr->m_IndexLoaded = false;

    if (m_pItem)
        delete m_pItem;

    if (m_pContent)
        delete m_pContent;
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

void QuickListViewPrivate::slotCurrentIndexChanged(const QModelIndex& index)
{
    emit q_ptr->indexChanged(index.row());
}

// Make sure the section stay in sync with the content
void QuickListViewPrivate::slotDataChanged(const QModelIndex& tl, const QModelIndex& br)
{
    if (!m_pSections)
        return;

    auto tli = static_cast<QuickListViewItem*>(q_ptr->itemForIndex(tl));
    auto bri = static_cast<QuickListViewItem*>(q_ptr->itemForIndex(br));

    Q_ASSERT(tli);
    Q_ASSERT(bri);

    bool outdated = false;

    //TODO there is some possible optimizations here, not *all* subsequent
    // elements needs to be moved
    do {
        if (outdated || (outdated = (tli->m_pSection != getSection(tli))))
            tli->move();

    } while(tli != bri && (tli = static_cast<QuickListViewItem*>(tli->down())));
}

void QuickListViewItem::setSelected(bool s)
{
    m_pContent->setContextProperty("isCurrentItem", s);
}


QRectF QuickListViewItem::geometry() const
{
    const QPointF p = m_pItem->mapFromItem(view()->contentItem(), {0,0});
    return {
        -p.x(),
        -p.y(),
        m_pItem->width(),
        m_pItem->height()
    };
}

#include <quicklistview.moc>
