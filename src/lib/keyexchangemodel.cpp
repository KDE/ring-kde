/****************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                          *
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

#include "account.h"

class KeyExchangeModelPrivate
{
public:
   KeyExchangeModelPrivate();
   Account* m_pAccount;
   static const TypedStateMachine< TypedStateMachine< bool , KeyExchangeModel::Type > , KeyExchangeModel::Options > availableOptions;
};

const TypedStateMachine< TypedStateMachine< bool , KeyExchangeModel::Type > , KeyExchangeModel::Options > KeyExchangeModelPrivate::availableOptions = {{
   /*                  */  /* ZRTP */ /* SDES */ /* NONE */
   /* RTP_FALLBACK     */ {{ false    , true     , false   }},
   /* DISPLAY_SAS      */ {{ true     , false    , false   }},
   /* NOT_SUPP_WARNING */ {{ true     , false    , false   }},
   /* HELLO_HASH       */ {{ true     , false    , false   }},
   /* DISPLAY_SAS_ONCE */ {{ true     , false    , false   }},
}};

KeyExchangeModelPrivate::KeyExchangeModelPrivate() : m_pAccount(nullptr)
{
}


KeyExchangeModel::KeyExchangeModel(Account* account) : QAbstractListModel(account),d_ptr(new KeyExchangeModelPrivate())
{
   d_ptr->m_pAccount = account;
}

KeyExchangeModel::~KeyExchangeModel()
{
//    delete d_ptr;
}

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
         case KeyExchangeModel::Type::__COUNT:
            break;
      };
   }
   return QVariant();
}

int KeyExchangeModel::rowCount( const QModelIndex& parent ) const
{
   return parent.isValid()?0:2;
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
      case KeyExchangeModel::Type::__COUNT:
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

void KeyExchangeModel::enableSRTP(bool enable)
{
   if (enable && d_ptr->m_pAccount->keyExchange() == KeyExchangeModel::Type::NONE) {
      d_ptr->m_pAccount->setKeyExchange(KeyExchangeModel::Type::ZRTP);
   }
   else if (!enable) {
      d_ptr->m_pAccount->setKeyExchange(KeyExchangeModel::Type::NONE);
   }
}

bool KeyExchangeModel::isRtpFallbackEnabled() const
{
   return d_ptr->availableOptions[Options::RTP_FALLBACK][d_ptr->m_pAccount->keyExchange()];
}

bool KeyExchangeModel::isDisplaySASEnabled() const
{
   return d_ptr->availableOptions[Options::DISPLAY_SAS][d_ptr->m_pAccount->keyExchange()];
}

bool KeyExchangeModel::isDisplaySasOnce() const
{
   return d_ptr->availableOptions[Options::DISPLAY_SAS_ONCE][d_ptr->m_pAccount->keyExchange()];
}

bool KeyExchangeModel::areWarningSupressed() const
{
   return d_ptr->availableOptions[Options::NOT_SUPP_WARNING][d_ptr->m_pAccount->keyExchange()];
}

bool KeyExchangeModel::isHelloHashEnabled() const
{
   return d_ptr->availableOptions[Options::HELLO_HASH][d_ptr->m_pAccount->keyExchange()];
}
