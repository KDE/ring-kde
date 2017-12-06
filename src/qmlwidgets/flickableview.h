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
#include <QtCore/QItemSelectionModel>
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
 *
 * This is an abstract class and expect the implementations to properly
 * manage the `ModelIndexItem` elements. In return, this class provides an
 * unified way to handle the selection and drag&drop boiler plate code.
 */
class FlickableView : public SimpleFlickable
{
    Q_OBJECT

public:
    /**
     * Abstract base class of the visual representation of a QModelIndex.
     */
    class ModelIndexItem
    {
    public:
        virtual ~ModelIndexItem() {}

        /// Geometry relative to the FlickableView::view()
        virtual QRectF geometry() const = 0;

        /// Visibility relative to the displayed window of the FlickableView::view()
        virtual bool isVisible() const = 0;

        /// Get a weak pointer into itself so the implementation can notify of deletion
        virtual QWeakPointer<ModelIndexItem> reference() const = 0;

        /// Allow implementations to be notified when it becomes selected
        virtual void setSelected(bool) {}
    };

    Q_PROPERTY(QSharedPointer<QAbstractItemModel> model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QAbstractItemModel* rawModel WRITE setRawModel)
    Q_PROPERTY(QQmlComponent* delegate READ delegate WRITE setDelegate)
    Q_PROPERTY(Qt::Corner gravity READ gravity WRITE setGravity)
    Q_PROPERTY(QQmlComponent* highlight READ highlight WRITE setHighlight)
    Q_PROPERTY(QSharedPointer<QItemSelectionModel> selectionModel READ selectionModel WRITE setSelectionModel NOTIFY selectionModelChanged)

    // QML support for selectionModels is rather bad since many Q_INVOKABLE are missing
    Q_PROPERTY(QModelIndex currentIndex READ currentIndex WRITE setCurrentIndex)

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

    QModelIndex currentIndex() const;
    Q_INVOKABLE void setCurrentIndex(const QModelIndex& index,
        QItemSelectionModel::SelectionFlags f = QItemSelectionModel::ClearAndSelect
    );

protected:
    virtual void refresh();
    void applyRoles(QQmlContext* ctx, const QModelIndex& self) const;
    QPair<QQuickItem*, QQmlContext*> loadDelegate(QQuickItem* parentI, QQmlContext* parentCtx, const QModelIndex& self) const;

    /**
     * To be implemented by the final class.
     */
    virtual ModelIndexItem* createItem() const = 0;

    /**
     * Get the VolatileTreeItem associated with a model index.
     *
     * Note that if the index is not currently visible or buferred, it will
     * return nullptr.
     */
    virtual ModelIndexItem* itemForIndex(const QModelIndex& idx) const = 0;

Q_SIGNALS:
    void currentIndexChanged(const QModelIndex& index);
    void modelChanged(QSharedPointer<QAbstractItemModel> model);
    void selectionModelChanged() const;

private:

    FlickableViewPrivate* d_ptr;
    Q_DECLARE_PRIVATE(FlickableView)
};
Q_DECLARE_METATYPE(FlickableView*)
Q_DECLARE_METATYPE(QSharedPointer<QItemSelectionModel>)
