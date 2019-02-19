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
#pragma once

#include <QtCore/QObject>

class WindowEventPrivate;

/**
 * This class offers a proxy between the code unaware of how windows are
 * handled and the code unaware of the events it needs to handle.
 *
 * This way they can communicate with each other without knowing the
 * implementation details. This is useful to support Android and Plasma Mobile.
 * Their window model is very different from classic floating and tiling WMs.
 *
 * Note that there can be many instances of this class. The signals will be
 * emitted in all of them.
 */
class Q_DECL_EXPORT WindowEvent : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(bool startIconified READ startIconified WRITE setStartIconified NOTIFY iconifiedChanged)

    Q_INVOKABLE WindowEvent(QObject* parent = nullptr);
    virtual ~WindowEvent();

    static WindowEvent* instance();

    bool startIconified() const;
    void setStartIconified(bool ic);

public Q_SLOTS:
    void raiseWindow();
    void quit();
    void showWizard();
    void configureAccounts();
    void hideWindow();
    void configureVideo();

Q_SIGNALS:
    void requestsWindowRaised();
    void requestsQuit();
    void requestsWizard();
    void requestsVideo();
    void requestsConfigureAccounts();
    void requestsHideWindow();
    void iconifiedChanged();

private:
    static WindowEventPrivate* d_ptr;
    Q_DECLARE_PRIVATE(WindowEvent)
};
