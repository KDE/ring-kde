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
#ifndef KEYEXCHANGEMODEL_H
#define KEYEXCHANGEMODEL_H

#include "typedefs.h"
#include <QtCore/QAbstractListModel>

///Static model for handling encryption types
class LIB_EXPORT KeyExchangeModel : public QAbstractListModel {
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop

public:
   ///@enum Type Every supported encryption types
   enum class Type {
      NONE = 0,
      ZRTP = 1,
      SDES = 2,
   };

   class Name {
   public:
      constexpr static const char* NONE = "none";
      constexpr static const char* ZRTP = "zrtp";
      constexpr static const char* SDES = "sdes";
   };

   //Private constructor, can only be called by 'Account'
   explicit KeyExchangeModel();

   //Model functions
   QVariant      data     ( const QModelIndex& index, int role = Qt::DisplayRole     ) const;
   int           rowCount ( const QModelIndex& parent = QModelIndex()                ) const;
   Qt::ItemFlags flags    ( const QModelIndex& index                                 ) const;
   virtual bool  setData  ( const QModelIndex& index, const QVariant &value, int role)      ;

   //Getters
   QModelIndex   toIndex  (KeyExchangeModel::Type type);

   //Singleton
   static KeyExchangeModel* instance();

private:
   static KeyExchangeModel* m_spInstance;
};
Q_DECLARE_METATYPE(KeyExchangeModel*)
#endif
