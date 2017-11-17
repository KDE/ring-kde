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

#include <simpleflickable.h>
#include <modelscrolladapter.h> //For the Q_DECLARE_METATYPE

// Qt
#include <QtCore/QAbstractItemModel>
class QQmlComponent;
class QQmlContext;
class QItemSelectionModel;

class FlickableViewPrivate;

/**
 * This widget bridges the simple Flickable "cartesian plan" widget with a
 * model view.
 *
 * It allows the various view (hierarchy, list, table, tree) to share a common
 * API and be drop-in replacement for each other if the model topology is ever
 * extended.
 *
 * This is a base class and is not intended to be used directly.
 */
class FlickableView : public SimpleFlickable
{
    Q_OBJECT

public:
    Q_PROPERTY(QSharedPointer<QAbstractItemModel> model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QAbstractItemModel* rawModel WRITE setRawModel)
    Q_PROPERTY(QQmlComponent* delegate READ delegate WRITE setDelegate)
    Q_PROPERTY(Qt::Corner gravity READ gravity WRITE setGravity)
    Q_PROPERTY(QQmlComponent* highlight READ highlight WRITE setHighlight)
    Q_PROPERTY(QSharedPointer<QItemSelectionModel> selectionModel READ selectionModel WRITE setSelectionModel NOTIFY selectionModelChanged)

    explicit FlickableView(QQuickItem* parent = nullptr);
    virtual ~FlickableView();

    virtual void setModel(QSharedPointer<QAbstractItemModel> model);
    QSharedPointer<QAbstractItemModel> model() const;
    void setRawModel(QAbstractItemModel* m);

    virtual void setDelegate(QQmlComponent* delegate);
    QQmlComponent* delegate() const;

    QQmlContext* rootContext() const;

    Qt::Corner gravity() const;
    void setGravity(Qt::Corner g);

    QQmlComponent* highlight() const;
    void setHighlight(QQmlComponent* h);

    QSharedPointer<QItemSelectionModel> selectionModel() const;
    void setSelectionModel(QSharedPointer<QItemSelectionModel> m);

protected:
    virtual void refresh();
    void applyRoles(QQmlContext* ctx, const QModelIndex& self) const;
    QPair<QQuickItem*, QQmlContext*> loadDelegate(QQuickItem* parentI, QQmlContext* parentCtx, const QModelIndex& self) const;

Q_SIGNALS:
    void modelChanged(QSharedPointer<QAbstractItemModel> model);
    void selectionModelChanged() const;

private:
    FlickableViewPrivate* d_ptr;
    Q_DECLARE_PRIVATE(FlickableView)
};
Q_DECLARE_METATYPE(FlickableView*)
