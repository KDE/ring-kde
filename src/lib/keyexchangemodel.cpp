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
#include "keyexchangemodel.h"

#include <QtCore/QCoreApplication>

KeyExchangeModel* KeyExchangeModel::m_spInstance = nullptr;

KeyExchangeModel::KeyExchangeModel() : QAbstractListModel(QCoreApplication::instance()) {}

//Model functions
QVariant KeyExchangeModel::data( const QModelIndex& index, int role) const
{
   if (!index.isValid()) return QVariant();
   KeyExchangeModel::Type method = static_cast<KeyExchangeModel::Type>(index.row());
   if (role == Qt::DisplayRole) {
      switch (method) {
         case KeyExchangeModel::Type::NONE:
            return KeyExchangeModel::Name::NONE;
            break;
         case KeyExchangeModel::Type::ZRTP:
            return KeyExchangeModel::Name::ZRTP;
            break;
         case KeyExchangeModel::Type::SDES:
            return KeyExchangeModel::Name::SDES;
            break;
      };
   }
   return QVariant();
}

int KeyExchangeModel::rowCount( const QModelIndex& parent ) const
{
   return parent.isValid()?0:3;
}

Qt::ItemFlags KeyExchangeModel::flags( const QModelIndex& index ) const
{
   if (!index.isValid()) return Qt::NoItemFlags;
   return Qt::ItemIsEnabled|Qt::ItemIsSelectable;
}

bool KeyExchangeModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role )
   return false;
}

KeyExchangeModel* KeyExchangeModel::instance()
{
   if (!m_spInstance)
      m_spInstance = new KeyExchangeModel();
   return m_spInstance;
}

///Translate enum type to QModelIndex
QModelIndex KeyExchangeModel::toIndex(KeyExchangeModel::Type type) const
{
   return index(static_cast<int>(type),0,QModelIndex());
}

///Translate enum to daemon name
const char* KeyExchangeModel::toDaemonName(KeyExchangeModel::Type type)
{
   switch (type) {
      case KeyExchangeModel::Type::NONE:
         return KeyExchangeModel::DaemonName::NONE;
         break;
      case KeyExchangeModel::Type::ZRTP:
         return KeyExchangeModel::DaemonName::ZRTP;
         break;
      case KeyExchangeModel::Type::SDES:
         return KeyExchangeModel::DaemonName::SDES;
         break;
   };
   return nullptr; //Cannot heppen
}

KeyExchangeModel::Type KeyExchangeModel::fromDaemonName(const QString& name)
{
   if (name.isEmpty())
      return KeyExchangeModel::Type::NONE;
   else if (name == KeyExchangeModel::DaemonName::SDES)
      return KeyExchangeModel::Type::SDES;
   else if (name == KeyExchangeModel::DaemonName::ZRTP)
      return KeyExchangeModel::Type::ZRTP;
   qDebug() << "Undefined Key exchange mechanism" << name;
   return KeyExchangeModel::Type::NONE;
}
