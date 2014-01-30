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

// KDE
#include <akonadi/collectionmodel.h>

// SFLphone
#include "kcfg_settings.h"

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
   m_hChecked.clear();
   const QList<int> disabled = ConfigurationSkeleton::disabledCollectionList();
   foreach(const int str, disabled) {
      m_hChecked[str] = true; //Disabled == true, enabled == false
   }
}

void AkonadiContactCollectionModel::save()
{
   QList<int> ret;
   for (QHash<int,bool>::iterator i = m_hChecked.begin(); i != m_hChecked.end(); ++i) {
      if (i.value())
         ret << i.key();
   }
   ConfigurationSkeleton::setDisabledCollectionList(ret);
}
