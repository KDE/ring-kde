/****************************************************************************
 *   Copyright (C) 2012-2013 by Savoir-Faire Linux                          *
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
#include "credentialmodel.h"

#include <QtCore/QDebug>

///Constructor
CredentialModel::CredentialModel(QObject* par) : QAbstractListModel(par) {

}

///Model data
QVariant CredentialModel::data(const QModelIndex& idx, int role) const {
   if (idx.column() == 0) {
      switch (role) {
         case Qt::DisplayRole:
            return QVariant(m_lCredentials[idx.row()]->name);
            break;
         case CredentialModel::NAME_ROLE:
            return m_lCredentials[idx.row()]->name;
            break;
         case CredentialModel::PASSWORD_ROLE:
            return m_lCredentials[idx.row()]->password;
            break;
         case CredentialModel::REALM_ROLE:
            return m_lCredentials[idx.row()]->realm;
            break;
         default:
            break;
      }
   }
   return QVariant();
}

///Number of credentials
int CredentialModel::rowCount(const QModelIndex& par) const {
   Q_UNUSED(par)
   return m_lCredentials.size();
}

///Model flags
Qt::ItemFlags CredentialModel::flags(const QModelIndex& idx) const {
   if (idx.column() == 0)
      return QAbstractItemModel::flags(idx) /*| Qt::ItemIsUserCheckable*/ | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   return QAbstractItemModel::flags(idx);
}

///Set credential data
bool CredentialModel::setData( const QModelIndex& idx, const QVariant &value, int role) {
   if (idx.column() == 0 && role == CredentialModel::NAME_ROLE) {
      m_lCredentials[idx.row()]->name = value.toString();
      emit dataChanged(idx, idx);
      return true;
   }
   else if (idx.column() == 0 && role == CredentialModel::PASSWORD_ROLE) {
      m_lCredentials[idx.row()]->password = value.toString();
      emit dataChanged(idx, idx);
      return true;
   }
   else if (idx.column() == 0 && role == CredentialModel::REALM_ROLE) {
      m_lCredentials[idx.row()]->realm = value.toString();
      emit dataChanged(idx, idx);
      return true;
   }
   return false;
}

///Add a new credential
QModelIndex CredentialModel::addCredentials() {
   m_lCredentials << new CredentialData2;
   emit dataChanged(index(m_lCredentials.size()-1,0), index(m_lCredentials.size()-1,0));
   return index(m_lCredentials.size()-1,0);
}

///Remove credential at 'idx'
void CredentialModel::removeCredentials(QModelIndex idx) {
   qDebug() << "REMOVING" << idx.row() << m_lCredentials.size();
   if (idx.isValid()) {
      m_lCredentials.removeAt(idx.row());
      emit dataChanged(idx, index(m_lCredentials.size()-1,0));
      qDebug() << "DONE" << m_lCredentials.size();
   }
   else {
      qDebug() << "Failed to remove an invalid credential";
   }
}

///Remove everything
void CredentialModel::clear()
{
   foreach(CredentialData2* data2, m_lCredentials) {
      delete data2;
   }
   m_lCredentials.clear();
}
