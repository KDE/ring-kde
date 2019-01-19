/************************************************************************************
 *   Copyright (C) 2019 by BlueSystems GmbH                                         *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                            *
 *                                                                                  *
 *   This library is free software; you can redistribute it and/or                  *
 *   modify it under the terms of the GNU Lesser General Public                     *
 *   License as published by the Free Software Foundation; either                   *
 *   version 2.1 of the License, or (at your option) any later version.             *
 *                                                                                  *
 *   This library is distributed in the hope that it will be useful,                *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of                 *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU              *
 *   Lesser General Public License for more details.                                *
 *                                                                                  *
 *   You should have received a copy of the GNU Lesser General Public               *
 *   License along with this library; if not, write to the Free Software            *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA *
 ***********************************************************************************/
#include "focuslistener.h"

// Qt
#include <QtCore/QEvent>
#include <QtGui/QWindow>

//This code detect if the window is active, innactive or minimzed
class FocusListenerPrivate final : public QObject
{
   Q_OBJECT
public:
    explicit FocusListenerPrivate(QObject* ev) : QObject(ev) {}

    bool m_MayHaveFocus {false};
    QObject* m_pWindow {nullptr};
    static QList<FocusListener*> m_slInstances;

protected:
    virtual bool eventFilter(QObject *, QEvent *event)  override {
        if (event->type() != QEvent::WindowActivate && event->type() != QEvent::WindowDeactivate)
            return false;

        m_MayHaveFocus = event->type() == QEvent::WindowActivate;

        for (auto o : qAsConst(m_slInstances))
            emit o->changed();

        return false;
    }
};

QList<FocusListener*> FocusListenerPrivate::m_slInstances;

FocusListener::FocusListener(QObject* parent) : QObject(parent),
    d_ptr(new FocusListenerPrivate(this))
{
    FocusListenerPrivate::m_slInstances << this;
}

FocusListener::~FocusListener()
{
    FocusListenerPrivate::m_slInstances.removeAll(this);
}

QObject* FocusListener::window() const
{
    return d_ptr->m_pWindow;
}

void FocusListener::setWindow(QObject* w)
{
    if (w && w->metaObject()->inherits(&QWindow::staticMetaObject))
        w->installEventFilter(d_ptr);

    d_ptr->m_pWindow = w;

    for (auto o : qAsConst(FocusListenerPrivate::m_slInstances))
        emit o->changed();
}

bool FocusListener::mayHaveFocus() const
{
    bool ret = false;

    for (auto o : qAsConst(FocusListenerPrivate::m_slInstances))
        ret |= o->d_ptr->m_MayHaveFocus;

    return ret;
}

#include <focuslistener.moc>
