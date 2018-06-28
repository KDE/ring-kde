/***************************************************************************
 *   Copyright (C) 2017-2018 by Bluesystems                                *
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

#include <QQuickItem>

class SimpleFlickable;
class FlickableScrollBarPrivate;

/**
 * A scrollbar widget designed for the Simpleflickable based views.
 *
 * In theory it works with the QtQuick2.FlickableView based ones too, but it's
 * unsupported.
 *
 * The idea is to have both a desktop and mobile mode and good support for
 * categorized views. The shell is C++, but the widget implementation is
 * pure QML.
 *
 * The idea is that with the categories and everything being known internally
 * by the Simpleflickable views, having everything reverse engineered by QML
 * widget doesn't scale. It always requires to be fixed to match the internal
 * changes.
 */
class FlickableScrollBar : public QQuickItem
{
    Q_OBJECT
public:
    Q_PROPERTY(QObject* view READ view WRITE setView)
    Q_PROPERTY(qreal position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(qreal handleHeight READ handleHeight NOTIFY handleHeightChanged)
    Q_PROPERTY(bool handleVisible READ isHandleVisible NOTIFY handleHeightChanged)

    explicit FlickableScrollBar(QQuickItem* parent = nullptr);
    virtual ~FlickableScrollBar();

    QObject* view() const;
    void setView(QObject* v);

    qreal handleHeight() const;
    bool isHandleVisible() const;

    qreal position() const;
    void setPosition(qreal p);

Q_SIGNALS:
    void handleHeightChanged();
    void positionChanged();

private:
    FlickableScrollBarPrivate* d_ptr;
    Q_DECLARE_PRIVATE(FlickableScrollBar)
};
