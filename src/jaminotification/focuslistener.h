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
#pragma once

#include <QtCore/QObject>

class FocusListenerPrivate;

/**
 * Get the focus information anywhere in the code without pulling dependencies
 * on the window system.
 *
 * This solution is superior to lets say, KF5::KWindowSystem because it doesn't
 * have platform specific code.
 */
class Q_DECL_EXPORT FocusListener : public QObject
{
    Q_OBJECT
public:
    /**
     * An optional QWindow derived objects.
     *
     * As long as *one* of the FocusListener instance has a valid window it
     * will still work.
     */
    Q_PROPERTY(QObject* window READ window WRITE setWindow NOTIFY changed)

    Q_PROPERTY(bool mayHaveFocus READ mayHaveFocus NOTIFY changed)

    Q_INVOKABLE explicit FocusListener(QObject* parent = nullptr);
    virtual ~FocusListener();

    QObject* window() const;
    void setWindow(QObject* w);

    bool mayHaveFocus() const;

Q_SIGNALS:
    void changed();

private:
    FocusListenerPrivate* d_ptr;
    Q_DECLARE_PRIVATE(FocusListener)
};
