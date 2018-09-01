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

// Qt
#include <QtCore/QObject>
class QItemSelectionModel;
class QQmlComponent;
class QQmlContext;

class AbstractSelectableViewSyncInterface;
class AbstractSelectableViewPrivate;

/**
 * This class adds support for multi-selection using selection models.
 *
 * It must be attached to a single instance of a `AbstractQuickView` object.
 */
class AbstractSelectableView : public QObject
{
    Q_OBJECT
    friend class AbstractQuickView; // Notify of all relevant events
    friend class VisualTreeItem; // Notify of all relevant events
    friend class AbstractSelectableViewSyncInterface; // Its own internals
public:
    explicit AbstractSelectableView(QObject* parent = nullptr);
    virtual ~AbstractSelectableView();

    QQmlComponent* highlight() const;
    void setHighlight(QQmlComponent* h);

    QSharedPointer<QItemSelectionModel> selectionModel() const;
    void setSelectionModel(QSharedPointer<QItemSelectionModel> sm);
    void applySelectionRoles(QQmlContext* ctx, const QModelIndex& self) const;

Q_SIGNALS:
    void currentIndexChanged(const QModelIndex& index);
    void selectionModelChanged() const;

private:
    AbstractSelectableViewSyncInterface* s_ptr;
    AbstractSelectableViewPrivate* d_ptr;
    Q_DECLARE_PRIVATE(AbstractSelectableView);
};
