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
#include "akonadicontactcollectionmodel.h"

//Qt
#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>

// KDE
#include <akonadi/collectionmodel.h>

// SFLphone
#include "../lib/contactmodel.h"
#include "akonadibackend.h"
#include "kcfg_settings.h"

AkonadiContactCollectionModel* AkonadiContactCollectionModel::m_spInstance = nullptr;


AkonadiContactCollectionModel* AkonadiContactCollectionModel::instance()
{
   if (!m_spInstance)
      m_spInstance = new AkonadiContactCollectionModel(QCoreApplication::instance());
   return m_spInstance;
}

AkonadiContactCollectionModel::AkonadiContactCollectionModel(QObject* parent) : QSortFilterProxyModel(parent) {
   m_pParentModel = new Akonadi::CollectionModel(this);
   setSourceModel(m_pParentModel);
   setDynamicSortFilter(true);
   reload();
   connect(this,SIGNAL(rowsInserted(QModelIndex,int,int)),this,SLOT(slotInsertCollection(QModelIndex,int,int)));
   connect(this,SIGNAL(rowsRemoved(QModelIndex,int,int)),this,SLOT(slotRemoveCollection(QModelIndex,int,int)));
}

AkonadiContactCollectionModel::~AkonadiContactCollectionModel()
{
   setSourceModel(nullptr);
   delete m_pParentModel;
}

bool AkonadiContactCollectionModel::filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const
{
   const QModelIndex idx = sourceModel()->index(source_row,0,source_parent);
   Akonadi::Collection col = qvariant_cast<Akonadi::Collection>(idx.data(Akonadi::CollectionModel::Roles::CollectionRole));
   return col.contentMimeTypes().indexOf("text/directory") != -1;
}

Qt::ItemFlags AkonadiContactCollectionModel::flags ( const QModelIndex& index ) const
{
   return QSortFilterProxyModel::flags(index) | Qt::ItemIsUserCheckable;
}

QVariant AkonadiContactCollectionModel::data( const QModelIndex& index, int role ) const
{
   if (role == Qt::CheckStateRole) {
      const int id = index.data(Akonadi::CollectionModel::Roles::CollectionIdRole).toInt();
      return m_hChecked[id]?Qt::Unchecked:Qt::Checked;
   }
   return QSortFilterProxyModel::data(index,role);
}

bool AkonadiContactCollectionModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   if (role == Qt::CheckStateRole) {
      const int id = index.data(Akonadi::CollectionModel::Roles::CollectionIdRole).toInt();
      m_hChecked[id] = !value.toBool();
      emit dataChanged(index,index);
      emit changed();
      return false;
   }
   else
      return QSortFilterProxyModel::setData(index,value,role);
}


void AkonadiContactCollectionModel::reload()
{
//    m_hChecked.clear();
//    const QList<int> disabled = ConfigurationSkeleton::disabledCollectionList();
//    foreach(const int str, disabled) {
//       m_hChecked[str] = true; //Disabled == true, enabled == false
//    }
}

void AkonadiContactCollectionModel::digg(const QModelIndex& idx)
{
   for (int i = 0;i < rowCount(idx);i++) {
      QModelIndex current = index(i,0,idx);
      if (!m_hLoaded[current.data(Akonadi::CollectionModel::Roles::CollectionIdRole).toInt()]) {
         ContactModel::instance()->addBackend(new AkonadiBackend(
            qvariant_cast<Akonadi::Collection>(index(i,0,idx).data(Akonadi::CollectionModel::Roles::CollectionRole)),this));
      }
      digg(current);
   }
}

void AkonadiContactCollectionModel::save()
{
//    QList<int> ret;
//    for (QHash<int,bool>::iterator i = m_hChecked.begin(); i != m_hChecked.end(); ++i) {
//       if (i.value())
//          ret << i.key();
//    }
   digg(QModelIndex());
//    ConfigurationSkeleton::setDisabledCollectionList(ret);
}


void AkonadiContactCollectionModel::slotInsertCollection(const QModelIndex& parentIdx, int start, int end)
{
   for (int i =start; i <= end;i++) {
      Akonadi::Collection col = qvariant_cast<Akonadi::Collection>(index(i,0,parentIdx).data(Akonadi::CollectionModel::Roles::CollectionRole));

      ContactModel::instance()->addBackend(new AkonadiBackend(col,this));
      m_hLoaded[col.id()] = !m_hChecked[col.id()];
   }
}

void AkonadiContactCollectionModel::slotRemoveCollection(const QModelIndex& index , int start, int end)
{
   Q_UNUSED(index)
   Q_UNUSED(start)
   Q_UNUSED(end)
   for (int i =start; i <= end;i++) {
      //TODO
   }
}
