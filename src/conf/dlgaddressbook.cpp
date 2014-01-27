/****************************************************************************
 *   Copyright (C) 2009-2014 by Savoir-Faire Linux                          *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>          *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
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

#include "dlgaddressbook.h"

#include "klib/kcfg_settings.h"
#include "lib/numbercategorymodel.h"
#include "delegates/autocompletiondelegate.h"
#include <akonadi/collectionmodel.h>
#include <kcheckableproxymodel.h>

bool AkonadiCollectionTypeFilter::filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const
{
   const QModelIndex idx = sourceModel()->index(source_row,0,source_parent);
   Akonadi::Collection col = qvariant_cast<Akonadi::Collection>(idx.data(Akonadi::CollectionModel::Roles::CollectionRole));
   return col.contentMimeTypes().indexOf("text/directory") != -1;
}

Qt::ItemFlags AkonadiCollectionTypeFilter::flags ( const QModelIndex& index ) const
{
   return QSortFilterProxyModel::flags(index) | Qt::ItemIsUserCheckable;
}

QVariant AkonadiCollectionTypeFilter::data( const QModelIndex& index, int role ) const
{
   if (role == Qt::CheckStateRole) {
      const int id = index.data(Akonadi::CollectionModel::Roles::CollectionIdRole).toInt();
      return m_hChecked[id]?Qt::Unchecked:Qt::Checked;
   }
   return QSortFilterProxyModel::data(index,role);
}

bool AkonadiCollectionTypeFilter::setData( const QModelIndex& index, const QVariant &value, int role)
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


void AkonadiCollectionTypeFilter::reload()
{
   m_hChecked.clear();
   const QList<int> disabled = ConfigurationSkeleton::disabledCollectionList();
   foreach(const int str, disabled) {
      m_hChecked[str] = true; //Disabled == true, enabled == false
   }
}

void AkonadiCollectionTypeFilter::save()
{
   QList<int> ret;
   QHash<int,bool>::const_iterator i = m_hChecked.constBegin();
   while (i != m_hChecked.end()) {
      if (i.value())
         ret << i.key();
      i++;
   }
   ConfigurationSkeleton::setDisabledCollectionList(ret);
}


///Constructor
DlgAddressBook::DlgAddressBook(KConfigDialog* parent)
 : QWidget(parent),m_HasChanged(false)
{
   setupUi(this);
   m_pPhoneTypeList->setModel(NumberCategoryModel::instance());
   m_pDelegate = new AutoCompletionDelegate();
   m_pPhoneTypeList->setItemDelegate(m_pDelegate);

   Akonadi::CollectionModel* model = new Akonadi::CollectionModel(this);

   m_pFilterModel = new AkonadiCollectionTypeFilter(this);
   m_pFilterModel->setSourceModel(model);
   m_pFilterModel->reload();

   collections->setModel( m_pFilterModel );

   connect(m_pPhoneTypeList->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this   , SLOT(changed())      );
   connect(this            , SIGNAL(updateButtons())              , parent , SLOT(updateButtons()));
   connect(m_pFilterModel  , SIGNAL(changed())                    , this   , SLOT(changed()));
} //DlgAddressBook

///Destructor
DlgAddressBook::~DlgAddressBook()
{
   m_pPhoneTypeList->setItemDelegate(nullptr);
   delete m_pDelegate;
   delete m_pFilterModel;
}

///Reload the widget
void DlgAddressBook::updateWidgets()
{
   m_pFilterModel->reload();
}

///Save the settings
void DlgAddressBook::updateSettings()
{
   NumberCategoryModel::instance()->save();
   m_pFilterModel->save();
   m_HasChanged = false;
}

///Tag this dialog as changed
void DlgAddressBook::changed()
{
   m_HasChanged = true;
   emit updateButtons();
}

///If the "Apply" button need to be enabled
bool DlgAddressBook::hasChanged()
{
   return m_HasChanged;
}
