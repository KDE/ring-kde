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

#include <abstractquickview.h>

class AbstractViewCompatPrivate;

/**
 * Random code to get close to drop-in compatibility with both QML and QtWidgets
 * views.
 *
 * This is in own class to keep the lower level classes codebase smaller. It
 * also bind many components into a single "flat" class API. This library
 * tries to enforce smaller components with well defined scope. But that makes
 * its API larger and more complex. This class helps to collapse all the
 * concepts into a single thing.
 */
class AbstractViewCompat : public AbstractQuickView
{
    Q_OBJECT
public:
    Q_PROPERTY(Qt::Corner gravity READ gravity WRITE setGravity)
    Q_PROPERTY(QQmlComponent* highlight READ highlight WRITE setHighlight)
    Q_PROPERTY(QSharedPointer<QItemSelectionModel> selectionModel READ selectionModel WRITE setSelectionModel NOTIFY selectionModelChanged)
    Q_PROPERTY(bool sortingEnabled READ isSortingEnabled WRITE setSortingEnabled)
    Q_PROPERTY(QModelIndex currentIndex READ currentIndex WRITE setCurrentIndex)

    explicit AbstractViewCompat(QQuickItem* parent = nullptr);
    virtual ~AbstractViewCompat();

    Qt::Corner gravity() const;
    void setGravity(Qt::Corner g);

    QQmlComponent* highlight() const;
    void setHighlight(QQmlComponent* h);

    virtual void setModel(QSharedPointer<QAbstractItemModel> model) override;

    QSharedPointer<QItemSelectionModel> selectionModel() const;
    void setSelectionModel(QSharedPointer<QItemSelectionModel> m);

    QModelIndex currentIndex() const;
    Q_INVOKABLE void setCurrentIndex(const QModelIndex& index,
        QItemSelectionModel::SelectionFlags f = QItemSelectionModel::ClearAndSelect
    );

    bool isSortingEnabled() const;
    void setSortingEnabled(bool val);

protected:
    virtual void applyRoles(QQmlContext* ctx, const QModelIndex& self) const override;

Q_SIGNALS:
    void currentIndexChanged(const QModelIndex& index);
    void selectionModelChanged() const;

private:
    AbstractViewCompatPrivate* d_ptr;
};
