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
#include "windowevent.h"

#include <QtCore/QList>
#include <QtCore/QCoreApplication>
#include <QQmlEngine>
#include <QQmlContext>

class WindowEventPrivate
{
public:
    bool m_StartIconified;
};

WindowEventPrivate* WindowEvent::d_ptr = new WindowEventPrivate;

static QList<WindowEvent*>& instances()
{
    static QList<WindowEvent*> l;

    return l;
}

WindowEvent::WindowEvent(QObject* parent) : QObject(parent)
{
    instances() << this;
}

WindowEvent::~WindowEvent()
{
    instances().removeAll(this);
}

WindowEvent* WindowEvent::instance()
{
    // Since it has a parent, it cannot directly be a static object
    static WindowEvent* e = nullptr;
    e = e ? e : new WindowEvent(QCoreApplication::instance());

    return e;
}

void WindowEvent::raiseWindow()
{
    for (auto o : qAsConst(instances())) {
        emit o->requestsWindowRaised();
    }
}

void WindowEvent::quit()
{
    for (auto o : qAsConst(instances())) {
        emit o->requestsQuit();
    }
}

void WindowEvent::showWizard()
{
    for (auto o : qAsConst(instances())) {
        emit o->requestsWizard();
    }
}

void WindowEvent::configureAccounts()
{
    for (auto o : qAsConst(instances())) {
        emit o->requestsConfigureAccounts();
    }
}

void WindowEvent::hideWindow()
{
    for (auto o : qAsConst(instances())) {
        emit o->requestsHideWindow();
    }
}

bool WindowEvent::startIconified() const
{
    return d_ptr->m_StartIconified;
}

void WindowEvent::setStartIconified(bool ic)
{
    d_ptr->m_StartIconified = ic;

    for (auto o : qAsConst(instances())) {
        emit o->iconifiedChanged();
    }
}
