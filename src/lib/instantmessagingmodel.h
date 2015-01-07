/****************************************************************************
 *   Copyright (C) 2012-2015 by Savoir-Faire Linux                          *
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
#ifndef INSTANTMESSAGINGMODELMANAGER_H
#define INSTANTMESSAGINGMODELMANAGER_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>
#include "typedefs.h"
#include <QtCore/QDebug>

class InstantMessagingModelPrivate;

class Call;

///Qt model for the Instant Messaging (IM) features
class LIB_EXPORT InstantMessagingModel : public QAbstractListModel
{
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop
   friend class Call;
   friend class IMConversationManager;
   friend class IMConversationManagerPrivate;

public:
   //Role const
   enum Role {
      TYPE    = 100,
      FROM    = 101,
      TEXT    = 102,
      IMAGE   = 103,
      CONTACT = 104,
   };

   ///Represent the direction a message is taking
   enum class MessageRole {
      INCOMMING_IM ,
      OUTGOING_IM  ,
      MIME_TRANSFER,
   };

   //Constructor
   explicit InstantMessagingModel(Call* call, QObject* parent = nullptr);
   virtual ~InstantMessagingModel();

   //Abstract model function
   QVariant      data     ( const QModelIndex& index, int role = Qt::DisplayRole     ) const override;
   int           rowCount ( const QModelIndex& parent = QModelIndex()                ) const override;
   Qt::ItemFlags flags    ( const QModelIndex& index                                 ) const override;
   virtual bool  setData  ( const QModelIndex& index, const QVariant &value, int role)       override;

private:
   const QScopedPointer<InstantMessagingModelPrivate> d_ptr;
   Q_DECLARE_PRIVATE(InstantMessagingModel)
};
#endif
