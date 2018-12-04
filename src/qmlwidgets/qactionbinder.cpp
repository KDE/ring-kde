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
#include "qactionbinder.h"

#include <QtCore/QDebug>
#include <QQuickItem>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <kactioncollection.h>

#include <../ringapplication.h> //FIXME

struct ActionBinder {
    QAction*     m_pAction;
    QKeySequence m_CurrentShortcut;
    QObject*     m_pQmlShortcut;
};

class QActionBinderPrivate final : public QObject
{
    Q_OBJECT

public:
    QHash<QAction*, ActionBinder*> m_hMapper;
    KActionCollection*             m_pActionCollection {nullptr};
    QQuickItem*                    m_pParent           {nullptr};
    QQmlComponent*                 m_pComponent        {nullptr};

public Q_SLOTS:
    void slotActionInserted(QAction* a);
    void slotActionRemoved(QAction* a);
    void slotActionChanged();
    void slotActionActivated();
};

QActionBinder::QActionBinder(QObject* p) :
    QObject(p), d_ptr(new QActionBinderPrivate)
{
}

QActionBinder::~QActionBinder()
{
    delete d_ptr;
}


void QActionBinderPrivate::slotActionInserted(QAction* a)
{
    Q_ASSERT(!m_hMapper.contains(a));

    auto binder = new ActionBinder {
        a,
        a->shortcut(),
        nullptr
    };

    //auto ctx    = QQmlEngine::contextForObject(m_pParent);
    //Q_ASSERT(ctx);
    auto engine = RingApplication::engine(); //ctx->engine();

    if (!m_pComponent) {
        m_pComponent = new QQmlComponent(engine, m_pParent);
        m_pComponent->setData("import QtQuick 2.7; Shortcut {}", {});
    }

    binder->m_pQmlShortcut = m_pComponent->create();

    Q_ASSERT(binder->m_pQmlShortcut);

    binder->m_pQmlShortcut->setProperty("sequence", a->shortcut().toString());
    binder->m_pQmlShortcut->setProperty("action", QVariant::fromValue(a));

    engine->setObjectOwnership(binder->m_pQmlShortcut, QQmlEngine::CppOwnership);
    binder->m_pQmlShortcut->setParent(m_pParent);

    connect(binder->m_pQmlShortcut, SIGNAL(activated()), this, SLOT(slotActionActivated()));
    connect(a, &QAction::changed, this, &QActionBinderPrivate::slotActionChanged);

    m_hMapper[a] = binder;
}

void QActionBinderPrivate::slotActionRemoved(QAction* a)
{
    Q_ASSERT(m_hMapper.contains(a));

    auto binder = m_hMapper[a];

    m_hMapper.remove(a);

    delete binder;
}

void QActionBinderPrivate::slotActionChanged()
{
    QAction* a = qobject_cast<QAction*>(sender());
    Q_ASSERT(a);

    auto binder = m_hMapper.value(a);

    if (!binder)
        return;

    binder->m_pQmlShortcut->setProperty("sequence", a->shortcut().toString());
}

QObject* QActionBinder::actionCollection() const
{
    return d_ptr->m_pActionCollection;
}

void QActionBinder::setActionCollection(QObject* ac)
{
    if (d_ptr->m_pActionCollection == ac)
        return;

    //TODO clear if the collection changed

    Q_ASSERT(parent());
    d_ptr->m_pParent = qobject_cast<QQuickItem*>(parent());

    d_ptr->m_pActionCollection = qobject_cast<KActionCollection*>(ac);
    Q_ASSERT(d_ptr->m_pActionCollection);

    connect(d_ptr->m_pActionCollection, &KActionCollection::inserted,
        d_ptr, &QActionBinderPrivate::slotActionInserted);

    connect(d_ptr->m_pActionCollection, &KActionCollection::removed,
        d_ptr, &QActionBinderPrivate::slotActionRemoved);

    const auto actions = d_ptr->m_pActionCollection->actions();

    for (auto a : qAsConst(actions)) {
        d_ptr->slotActionInserted(a);
    }
}

void QActionBinderPrivate::slotActionActivated()
{
    QAction* a = qvariant_cast<QAction*>(sender()->property("action"));
    Q_ASSERT(a);

    a->trigger();

}

#include <qactionbinder.moc>
