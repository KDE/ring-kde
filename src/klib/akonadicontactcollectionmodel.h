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
#ifndef AKONADI_CONTACT_COLLECTION_MODEL_H
#define AKONADI_CONTACT_COLLECTION_MODEL_H

#include <QtGui/QSortFilterProxyModel>
#include "../lib/typedefs.h"

namespace Akonadi {
   class CollectionModel;
}

///Filter out notes and emails collections
class LIB_EXPORT AkonadiContactCollectionModel : public QSortFilterProxyModel
{
   Q_OBJECT
public:
   virtual ~AkonadiContactCollectionModel();

public:
   virtual QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
   virtual Qt::ItemFlags flags       ( const QModelIndex& index                                    ) const;
   virtual bool          setData     ( const QModelIndex& index, const QVariant &value, int role   );

   //Mutator
   void reload();
   void save();

   //Singleton
   static AkonadiContactCollectionModel* instance();

protected:
   virtual bool filterAcceptsRow( int source_row, const QModelIndex & source_parent ) const;

private:
   explicit AkonadiContactCollectionModel(QObject* parent);
   QHash<int,bool> m_hChecked;
   QHash<int,bool> m_hLoaded ;
   Akonadi::CollectionModel* m_pParentModel;

   //Singleton
   static AkonadiContactCollectionModel* m_spInstance;

   //Helper
   void digg(const QModelIndex& idx);

private Q_SLOTS:
   void slotInsertCollection(const QModelIndex&,int,int);
   void slotRemoveCollection(const QModelIndex&,int,int);

Q_SIGNALS:
   void changed();
};

#endif
