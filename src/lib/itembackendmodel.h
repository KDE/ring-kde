/****************************************************************************
 *   Copyright (C) 2014 by Savoir-Faire Linux                               *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Lesser General Public             *
 *   License as published by the Free Software Foundation; either           *
 *   version 2.1 of the License, or (at your option) any later version.     *
 *                                                                          *
 *   This library is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#ifndef COMMONITEMBACKENDMODEL_H
#define COMMONITEMBACKENDMODEL_H

#include "typedefs.h"
#include "contact.h"

#include <QtCore/QAbstractItemModel>

#include "commonbackendmanagerinterface.h"
#include "contactmodel.h"
#include "abstractitembackend.h"

//SFLPhone
class AbstractItemBackendModelExtension;

class CommonItemBackendModelPrivate;

class LIB_EXPORT CommonItemBackendModel : public QAbstractTableModel
{
   Q_OBJECT
public:
   explicit CommonItemBackendModel(QObject* parent = nullptr);
   virtual ~CommonItemBackendModel();

   virtual QVariant data        (const QModelIndex& index, int role = Qt::DisplayRole         ) const override;
   virtual int rowCount         (const QModelIndex& parent = QModelIndex()                    ) const override;
   virtual int columnCount      (const QModelIndex& parent = QModelIndex()                    ) const override;
   virtual Qt::ItemFlags flags  (const QModelIndex& index                                     ) const override;
   virtual QVariant headerData  (int section, Qt::Orientation orientation, int role           ) const override;
   virtual bool setData         (const QModelIndex& index, const QVariant &value, int role    )       override;
   virtual QModelIndex   parent ( const QModelIndex& index                                    ) const override;
   virtual QModelIndex   index  ( int row, int column, const QModelIndex& parent=QModelIndex()) const override;

   AbstractContactBackend* backendAt(const QModelIndex& index);

   void addExtension(AbstractItemBackendModelExtension* extension);

   bool save();
   bool load();

Q_SIGNALS:
   void checkStateChanged();

private:
   QScopedPointer<CommonItemBackendModelPrivate> d_ptr;
   Q_DECLARE_PRIVATE(CommonItemBackendModel)

};

#endif
