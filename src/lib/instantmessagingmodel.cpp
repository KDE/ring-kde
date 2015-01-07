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
#include "instantmessagingmodel.h"

#include "callmodel.h"
#include "dbus/callmanager.h"
#include "call.h"
#include "contact.h"
#include "phonenumber.h"
#include "private/instantmessagingmodel_p.h"

InstantMessagingModelPrivate::InstantMessagingModelPrivate(InstantMessagingModel* parent) : QObject(parent), q_ptr(parent)
{

}

///Constructor
InstantMessagingModel::InstantMessagingModel(Call* call, QObject* par) : QAbstractListModel(par), d_ptr(new InstantMessagingModelPrivate(this))
{
   d_ptr->m_pCall = call;
   //QStringList callList = callManager.getCallList();
   QHash<int, QByteArray> roles = roleNames();
   roles.insert(InstantMessagingModel::Role::TYPE    ,QByteArray("type"));
   roles.insert(InstantMessagingModel::Role::FROM    ,QByteArray("from"));
   roles.insert(InstantMessagingModel::Role::TEXT    ,QByteArray("text"));
   roles.insert(InstantMessagingModel::Role::IMAGE   ,QByteArray("image"));
   roles.insert(InstantMessagingModel::Role::CONTACT ,QByteArray("contact"));
   setRoleNames(roles);
}

InstantMessagingModel::~InstantMessagingModel()
{
//    delete d_ptr;
}

///Get data from the model
QVariant InstantMessagingModel::data( const QModelIndex& idx, int role) const
{
   if (idx.column() == 0) {
      switch (role) {
         case Qt::DisplayRole:
            return QVariant(d_ptr->m_lMessages[idx.row()].message);
            break;
         case InstantMessagingModel::Role::TYPE:
            return QVariant(d_ptr->m_lMessages[idx.row()].message);
            break;
         case InstantMessagingModel::Role::FROM:
            return QVariant(d_ptr->m_lMessages[idx.row()].from);
            break;
         case InstantMessagingModel::Role::TEXT:
            return static_cast<int>(MessageRole::INCOMMING_IM);
            break;
         case InstantMessagingModel::Role::CONTACT:
            if (d_ptr->m_pCall->peerPhoneNumber()->contact()) {
               return QVariant();
            }
            break;
         case InstantMessagingModel::Role::IMAGE: {
            if (d_ptr->m_lImages.find(idx) != d_ptr->m_lImages.end())
               return d_ptr->m_lImages[idx];
            const Contact* c = d_ptr->m_pCall->peerPhoneNumber()->contact();
            if (c && c->photo()) {
               return QVariant::fromValue<void*>((void*)c->photo());
            }
            return QVariant();
            break;
         }
         default:
            break;
      }
   }
   return QVariant();
}

///Number of row
int InstantMessagingModel::rowCount(const QModelIndex& parentIdx) const
{
   Q_UNUSED(parentIdx)
   return d_ptr->m_lMessages.size();
}

///Model flags
Qt::ItemFlags InstantMessagingModel::flags(const QModelIndex& idx) const
{
   Q_UNUSED(idx)
   return Qt::ItemIsEnabled;
}

///Set model data
bool InstantMessagingModel::setData(const QModelIndex& idx, const QVariant &value, int role)
{
   Q_UNUSED(idx)
   Q_UNUSED(value)
   Q_UNUSED(role)
   if (idx.column() == 0 && role == InstantMessagingModel::Role::IMAGE   ) {
      d_ptr->m_lImages[idx] = value;
   }
   return false;
}

///Add new incoming message (to be used internally)
void InstantMessagingModelPrivate::addIncommingMessage(const QString& from, const QString& message)
{
   InternalIM im;
   im.from    = from;
   im.message = message;
   m_lMessages << im;
   emit q_ptr->dataChanged(q_ptr->index(m_lMessages.size() -1,0), q_ptr->index(m_lMessages.size()-1,0));
}

///Add new outgoing message (to be used internally and externally)
void InstantMessagingModelPrivate::addOutgoingMessage(const QString& message)
{
   InternalIM im;
   im.from    = tr("Me");
   im.message = message;
   m_lMessages << im;
   emit q_ptr->dataChanged(q_ptr->index(m_lMessages.size() -1,0), q_ptr->index(m_lMessages.size()-1,0));
}
