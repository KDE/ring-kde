/****************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                          *
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
#include "categorizedaccountmodel.h"

#include "accountlistmodel.h"

CategorizedAccountModel* CategorizedAccountModel::m_spInstance = nullptr;

namespace {
   const int TYPE_TOP_LEVEL = 0;
   const int TYPE_IP2IP     = 2;
   const int TYPE_SERVER    = 1;
}

CategorizedAccountModel* CategorizedAccountModel::instance()
{
   if (!m_spInstance)
      m_spInstance = new CategorizedAccountModel();
   return m_spInstance;
}

CategorizedAccountModel::CategorizedAccountModel(QObject* parent) : QAbstractItemModel(parent)
{
   connect(AccountListModel::instance(),SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(slotDataChanged(QModelIndex,QModelIndex)));
   connect(AccountListModel::instance(),SIGNAL(layoutChanged()),this,SLOT(slotLayoutchanged()));
}

CategorizedAccountModel::~CategorizedAccountModel()
{
   
}

QModelIndex CategorizedAccountModel::mapToSource(const QModelIndex& idx) const
{
   if (!idx.isValid() || !idx.parent().isValid() || idx.model() != this)
      return QModelIndex();
   switch (idx.parent().row()) {
      case Categories::IP2IP:
         return AccountListModel::instance()->ip2ip()->index();
         break;
      case Categories::SERVER: {
         const QModelIndex& ip2ipIdx = AccountListModel::instance()->ip2ip()->index();
         //TODO DHT: this will stop working
         return AccountListModel::instance()->index((ip2ipIdx.row() <= idx.row())?idx.row()+1:idx.row(),0);
      }
         break;
      default:
         qDebug() << "The account list has too many top levels";
         break;
   };
   return QModelIndex();
}

QModelIndex CategorizedAccountModel::mapFromSource(const QModelIndex& idx) const
{
   if (!idx.isValid() || idx.model() != AccountListModel::instance())
      return QModelIndex();

   //TODO DHT: this will stop working
   const QModelIndex& ip2ipIdx = AccountListModel::instance()->ip2ip()->index();
   return idx.row() == ip2ipIdx.row()?index(0,0,index(Categories::IP2IP,0,QModelIndex())): (
      index(idx.row()+(idx.row() > ip2ipIdx.row()?-1:0),0,index(Categories::SERVER,0,QModelIndex()))
   );
}

QVariant CategorizedAccountModel::data(const QModelIndex& index, int role ) const
{
   if (!index.isValid())
      return QVariant();
   else if (index.parent().isValid()) {
      return mapToSource(index).data(role);
   }
   else {
      switch (role) {
         case Qt::DisplayRole:
            if (index.row() == Categories::IP2IP)
               return tr("Peer to peer");
            else
               return tr("Server");
      };
   }
   return QVariant();
}

int CategorizedAccountModel::rowCount(const QModelIndex& parent ) const
{
   if (parent.parent().isValid())
      return 0;
   else if (parent.isValid()) {
      if (parent.row() == 1)
         return 1;
      return AccountListModel::instance()->size()-1;
   }
   return 2;
}

int CategorizedAccountModel::columnCount(const QModelIndex& parent ) const
{
   Q_UNUSED(parent)
   return 1;
}


QModelIndex CategorizedAccountModel::parent(const QModelIndex& idx) const
{
   switch (*static_cast<int*>(idx.internalPointer())) {
      case TYPE_TOP_LEVEL:
         return QModelIndex();
      case TYPE_SERVER:
         return createIndex((int)Categories::SERVER,0,(void*)&TYPE_TOP_LEVEL);
      case TYPE_IP2IP:
         return createIndex((int)Categories::IP2IP,0,(void*)&TYPE_TOP_LEVEL);
   };
   return QModelIndex();
}

QModelIndex CategorizedAccountModel::index( int row, int column, const QModelIndex& parent ) const
{
   if (parent.isValid() && *static_cast<int*>(parent.internalPointer()) == 0) {
      if (row >= rowCount(parent))
         return QModelIndex();
      switch (parent.row()) {
         case Categories::SERVER:
            return createIndex(row,column,(void*)&TYPE_SERVER);
            break;
         case Categories::IP2IP:
            return createIndex(row,column,(void*)&TYPE_IP2IP);
            break;
      };
   }
   else if (parent.isValid())
      return QModelIndex();
   return createIndex(row,column,(void*)&TYPE_TOP_LEVEL);
}

Qt::ItemFlags CategorizedAccountModel::flags(const QModelIndex& index ) const
{
   if (index.parent().isValid())
      return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   return Qt::ItemIsEnabled;
}

bool CategorizedAccountModel::setData(const QModelIndex& index, const QVariant &value, int role )
{
   if (!index.isValid())
      return false;
   else if (index.parent().isValid()) {
      return AccountListModel::instance()->setData(mapToSource(index),value,role);
   }
   return false;
}

QVariant CategorizedAccountModel::headerData(int section, Qt::Orientation orientation, int role ) const
{
   Q_UNUSED(section)
   Q_UNUSED(orientation)
   if (role == Qt::DisplayRole) return tr("Accounts");
   return QVariant();
}

void CategorizedAccountModel::slotDataChanged(const QModelIndex& tl,const QModelIndex& br)
{
   Q_UNUSED(tl)
   Q_UNUSED(br)
   emit layoutChanged();
}

void CategorizedAccountModel::slotLayoutchanged()
{
   emit layoutChanged();
}
