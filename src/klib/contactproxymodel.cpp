/****************************************************************************
 *   Copyright (C) 2013 by Savoir-Faire Linux                               *
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
#include "contactproxymodel.h"

#include <QtCore/QDebug>

#include "../lib/contactbackend.h"
// 
ContactByNameProxyModel::ContactByNameProxyModel(ContactBackend* parent) : QAbstractItemModel(parent),m_pModel(parent)
{
//    setSourceModel(m_pModel);
   connect(m_pModel,SIGNAL(collectionChanged()),this,SLOT(reloadCategories()));
}

ContactByNameProxyModel::~ContactByNameProxyModel()
{

}

void ContactByNameProxyModel::reloadCategories()
{
   m_hCategories.clear();
   m_lCategoryCounter.clear();
   foreach(Contact* cont, m_pModel->getContactList()) {
      QString val = cont->getFormattedName();
      if (val.size())
         val = val[0];
      if (!m_hCategories[val]) {
         TopLevelItem* item = new TopLevelItem(val.toUpper());
         m_hCategories[val] = item;
         m_lCategoryCounter << item;
         emit dataChanged(index(0,0),index(rowCount()-1,0));
      }
      TopLevelItem* item = m_hCategories[val];
      if (item) {
         QModelIndex parent = index(m_lCategoryCounter.indexOf(item),0);
         item->m_lChilds << cont;
//          emit dataChanged(index(item->m_lChilds.size()-1,0,parent),index(item->m_lChilds.size()-1,0,parent));
      }
      else
         qDebug() << "ERROR count";
   }
   reset();
   emit layoutChanged();
   emit dataChanged(index(0,0),index(rowCount()-1,0));
}

bool ContactByNameProxyModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

QVariant ContactByNameProxyModel::data( const QModelIndex& index, int role) const
{
   if (!index.isValid())
      return QVariant();
   
   ContactTreeBackend* modelItem = (ContactTreeBackend*)index.internalPointer();
   if (modelItem->type3() == ContactTreeBackend::Type::TOP_LEVEL || !index.parent().isValid()) {
      if (role == Qt::DisplayRole || role == Qt::EditRole)
         return ((TopLevelItem*)modelItem)->m_Name;
   }
   if (modelItem->type3() == ContactTreeBackend::Type::CONTACT && (role == Qt::DisplayRole || role == Qt::EditRole)) {
      return QVariant(m_lCategoryCounter[index.parent().row()]->m_lChilds[index.row()]->getFormattedName());
   }
   else if (modelItem->type3() == ContactTreeBackend::Type::NUMBER && (role == Qt::DisplayRole || role == Qt::EditRole)) {
      return QVariant(m_pModel->getContactList()[index.parent().row()]->getPhoneNumbers()[index.row()]->getNumber());
   }
   return QVariant();
}

QVariant ContactByNameProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   Q_UNUSED(section)
   if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
      return QVariant("Contacts");
   return QVariant();
}

int ContactByNameProxyModel::rowCount( const QModelIndex& parent ) const
{
   if (!parent.isValid())
      return m_lCategoryCounter.size();
   else if (!parent.parent().isValid()) {
      return m_lCategoryCounter[parent.row()]->m_lChilds.size();
   }
   else if (parent.parent().isValid() && !parent.parent().parent().isValid()) {
      return m_lCategoryCounter[parent.parent().row()]->m_lChilds[parent.row()]->getPhoneNumbers().size();
   }
   return 0;
}

Qt::ItemFlags ContactByNameProxyModel::flags( const QModelIndex& index ) const
{
   if (!index.isValid())
      return 0;
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

int ContactByNameProxyModel::columnCount ( const QModelIndex& parent) const
{
   Q_UNUSED(parent)
   return 1;
}

QModelIndex ContactByNameProxyModel::parent( const QModelIndex& index) const
{
   if (!index.isValid()) {
      return QModelIndex();
   }
   ContactTreeBackend* modelItem = (ContactTreeBackend*)index.internalPointer();
   if (modelItem && modelItem->type3() == ContactTreeBackend::Type::CONTACT) {
      Contact* ct = (Contact*)((ContactTreeBackend*)(index.internalPointer()))->getSelf();
      QString val = ct->getFormattedName();
      if (val.size())
         val = val[0];
      if (m_hCategories[val])
         return ContactByNameProxyModel::index(m_lCategoryCounter.indexOf(m_hCategories[val]),0);
   }
   else if (modelItem && modelItem->type3() == ContactTreeBackend::Type::NUMBER) {
      Contact* ct = (Contact*)modelItem->getSelf();
      QString val = ct->getFormattedName();
      if (val.size())
         val = val[0];
      if (m_hCategories[val]) {
         return ContactByNameProxyModel::index(
            (m_hCategories[val]->m_lChilds.indexOf(ct)),
            0,
            ContactByNameProxyModel::index(m_lCategoryCounter.indexOf(m_hCategories[val]),0));
      }
   }
   else if (modelItem && modelItem->type3() == ContactTreeBackend::Type::TOP_LEVEL) {
      return QModelIndex();
   }
   return QModelIndex();
}

QModelIndex ContactByNameProxyModel::index( int row, int column, const QModelIndex& parent) const
{
   if (!parent.isValid()) {
      return createIndex(row,column,m_lCategoryCounter[row]);
   }
   else if (!parent.parent().isValid() && column < m_lCategoryCounter[parent.row()]->m_lChilds.size() ) {
      return createIndex(row,column,(void*)dynamic_cast<ContactTreeBackend*>(m_lCategoryCounter[parent.row()]->m_lChilds[row]));
   }
   else if (parent.parent().isValid()) {
      return createIndex(row,column,(void*)&m_lCategoryCounter[parent.parent().row()]->m_lChilds[parent.row()]->getPhoneNumbers());
   }
   return QModelIndex();
}
