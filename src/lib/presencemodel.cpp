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
#include "presencemodel.h"

//Qt
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

//SFLPhone
#include "contact.h"
#include "account.h"
#include "dbus/presencemanager.h"

///Constructor
PresenceModel::PresenceModel(QObject* par) : QAbstractListModel(par?par:QCoreApplication::instance())
{
   //Setup roles
   QHash<int, QByteArray> roles = roleNames();
   roles.insert(PresenceModel::Role::CONTACT , QByteArray( "contact" ));
   roles.insert(PresenceModel::Role::URI     , QByteArray( "uri"     ));
   roles.insert(PresenceModel::Role::MESSAGE , QByteArray( "message" ));
   roles.insert(PresenceModel::Role::PRESENT , QByteArray( "present" ));
   setRoleNames(roles);

   //Connect
   connect(&DBus::PresenceManager::instance(),SIGNAL(newPresSubServerRequest(QString)),this,SLOT(slotRequest(QString)));
   connect(&DBus::PresenceManager::instance(),SIGNAL(newPresSubClientNotification(QString,bool,QString)),
      this,SLOT(slotIncomingNotifications(QString,bool,QString)));
}

///Model data
QVariant PresenceModel::data(const QModelIndex& idx, int role) const {
   if (idx.column() == 0) {
      switch (role) {
         case Qt::DisplayRole:
            return QVariant(m_lTracker[idx.row()]->uri);
            break;
         case PresenceModel::Role::CONTACT:
            return qVariantFromValue(m_lTracker[idx.row()]->contact);
            break;
         case PresenceModel::Role::URI:
            return m_lTracker[idx.row()]->uri;
            break;
         case PresenceModel::Role::MESSAGE:
            return m_lTracker[idx.row()]->message;
            break;
         case PresenceModel::Role::PRESENT:
            return m_lTracker[idx.row()]->present;
            break;
         default:
            break;
      }
   }
   return QVariant();
}

///Number of credentials
int PresenceModel::rowCount(const QModelIndex& par) const {
   Q_UNUSED(par)
   return m_lTracker.size();
}

///Model flags
Qt::ItemFlags PresenceModel::flags(const QModelIndex& idx) const {
   if (idx.column() == 0)
      return QAbstractItemModel::flags(idx) /*| Qt::ItemIsUserCheckable*/ | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   return QAbstractItemModel::flags(idx);
}

///Set credential data
bool PresenceModel::setData( const QModelIndex& idx, const QVariant &value, int role) {
   if (idx.column() == 0 && role == PresenceModel::Role::CONTACT) {
      m_lTracker[idx.row()]->contact = value.value<Contact*>();
      emit dataChanged(idx, idx);
      return true;
   }
   else if (idx.column() == 0 && role == PresenceModel::Role::URI) {
      m_lTracker[idx.row()]->uri = value.toString();
      emit dataChanged(idx, idx);
      return true;
   }
   else if (idx.column() == 0 && role == PresenceModel::Role::MESSAGE) {
      m_lTracker[idx.row()]->message = value.toString();
      emit dataChanged(idx, idx);
      return true;
   }
   else if (idx.column() == 0 && role == PresenceModel::Role::PRESENT) {
      m_lTracker[idx.row()]->present = value.toBool();
      emit dataChanged(idx, idx);
      return true;
   }
   return false;
}

///Remove everything
void PresenceModel::reset()
{
   foreach(TrackedContact* item,m_lTracker)
      delete item;
   m_lTracker.clear();
}

void PresenceModel::slotIncomingNotifications(QString uri, bool status, QString message)
{
   Q_UNUSED(uri)
   Q_UNUSED(status)
   Q_UNUSED(message)
}

void PresenceModel::slotRequest(QString uri)
{
   Q_UNUSED(uri)
   DBus::PresenceManager::instance().approvePresSubServer(uri,true);
}
