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


class LIB_EXPORT CommonItemBackendModel : public QAbstractItemModel
{
   Q_OBJECT
public:
   explicit CommonItemBackendModel(QObject* parent = nullptr);
   ~CommonItemBackendModel();

   QVariant data                (const QModelIndex& index, int role = Qt::DisplayRole      ) const;
   virtual int rowCount         (const QModelIndex& parent = QModelIndex()                 ) const;
   virtual int columnCount      (const QModelIndex& parent = QModelIndex()                 ) const;
   virtual Qt::ItemFlags flags  (const QModelIndex& index                                  ) const;
   virtual QVariant headerData  (int section, Qt::Orientation orientation, int role        ) const;
   virtual bool setData         (const QModelIndex& index, const QVariant &value, int role );
   virtual QModelIndex   parent      ( const QModelIndex& index                                    ) const;
   virtual QModelIndex   index       ( int row, int column, const QModelIndex& parent=QModelIndex()) const;

   bool save();
   bool load();

private Q_SLOTS:
   void slotUpdate();

Q_SIGNALS:
   void checkStateChanged();

private:
   /*
    * This is not very efficient, it doesn't really have to be given the low
    * volume. If it ever have to scale, a better mapToSource using persistent
    * index have to be implemented.
    */
   struct ProxyItem {
      ProxyItem() : parent(0),col(1){}
      int row;
      int col;
      AbstractContactBackend* backend;
      ProxyItem* parent;
      QVector<ProxyItem*> m_Children;
   };
   QHash<AbstractContactBackend*,ProxyItem*> m_hBackendsNodes;
   QVector<ProxyItem*> m_lTopLevelBackends;

};

#endif
