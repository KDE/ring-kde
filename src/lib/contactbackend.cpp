/************************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                                  *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>                  *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>         *
 *                                                                                  *
 *   This library is free software; you can redistribute it and/or                  *
 *   modify it under the terms of the GNU Lesser General Public                     *
 *   License as published by the Free Software Foundation; either                   *
 *   version 2.1 of the License, or (at your option) any later version.             *
 *                                                                                  *
 *   This library is distributed in the hope that it will be useful,                *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of                 *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU              *
 *   Lesser General Public License for more details.                                *
 *                                                                                  *
 *   You should have received a copy of the GNU Lesser General Public               *
 *   License along with this library; if not, write to the Free Software            *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA *
 ***********************************************************************************/

//Parent
#include "contactbackend.h"

//SFLPhone library
#include "contact.h"
#include "call.h"

//Qt
#include <QtCore/QHash>
#include <QtCore/QDebug>

///Constructor
ContactBackend::ContactBackend(QObject* parent) : QAbstractItemModel(parent),m_UpdatesCounter(0)
{
   connect(this,SIGNAL(collectionChanged()),this,SLOT(slotReloadModel()));
}

///Destructor
ContactBackend::~ContactBackend()
{
   if (Call::getContactBackend() == this)
      Call::setContactBackend(nullptr);
   foreach (Contact* c,m_ContactByUid) {
      delete c;
   }
}

///Update slot
ContactList ContactBackend::update()
{
   return update_slot();
}

///Called when the new contacts are added
void ContactBackend::slotReloadModel()
{
   reset();
   emit layoutChanged();
   emit dataChanged(index(0,0),index(rowCount(),0));
}

/*****************************************************************************
 *                                                                           *
 *                                  Helpers                                  *
 *                                                                           *
 ****************************************************************************/

///Return the extension/user of an URI (<sip:12345@exemple.com>)
QString ContactBackend::getUserFromPhone(QString phoneNumber)
{
   if (phoneNumber.indexOf('@') != -1) {
      QString user = phoneNumber.split('@')[0];
      return (user.indexOf(':') != -1)?user.split(':')[1]:user;
   }
   return phoneNumber;
} //getUserFromPhone

///Return the domaine of an URI (<sip:12345@exemple.com>)
QString ContactBackend::getHostNameFromPhone(QString phoneNumber)
{
   if (phoneNumber.indexOf('@') != -1) {
      return phoneNumber.split('@')[1].left(phoneNumber.split('@')[1].size()-1);
   }
   return "";
}


/*****************************************************************************
 *                                                                           *
 *                                   Model                                   *
 *                                                                           *
 ****************************************************************************/


bool ContactBackend::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

QVariant ContactBackend::data( const QModelIndex& index, int role) const
{
   if (!index.isValid())
      return QVariant();
   if (!index.parent().isValid() && (role == Qt::DisplayRole || role == Qt::EditRole)) {
      return QVariant(getContactList()[index.row()]->getFormattedName());
   }
   else if (index.parent().isValid() && (role == Qt::DisplayRole || role == Qt::EditRole)) {
      return QVariant(getContactList()[index.parent().row()]->getPhoneNumbers()[index.row()]->getNumber());
   }
   return QVariant();
}

QVariant ContactBackend::headerData(int section, Qt::Orientation orientation, int role) const
{
   Q_UNUSED(section)
   if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
      return QVariant("Contacts");
   return QVariant();
}

int ContactBackend::rowCount( const QModelIndex& parent ) const
{
   if (!parent.isValid()) {
      return getContactList().size();
   }
   else if (!parent.parent().isValid() && parent.row() < getContactList().size()) {
      return getContactList()[parent.row()]->getPhoneNumbers().size();
   }
   return 0;
}

Qt::ItemFlags ContactBackend::flags( const QModelIndex& index ) const
{
   if (!index.isValid())
      return 0;
   return Qt::ItemIsEnabled | ((index.parent().isValid())?Qt::ItemIsSelectable:Qt::ItemIsEnabled);
}

int ContactBackend::columnCount ( const QModelIndex& parent) const
{
   Q_UNUSED(parent)
   return 1;
}

QModelIndex ContactBackend::parent( const QModelIndex& index) const
{
   if (!index.isValid())
      return QModelIndex();
   ContactTreeBackend* modelItem = (ContactTreeBackend*)index.internalPointer();
   if (modelItem && modelItem->type3() == ContactTreeBackend::Type::NUMBER) {
      int idx = getContactList().indexOf(((Contact::PhoneNumbers*)modelItem)->contact());
      if (idx != -1) {
         return ContactBackend::index(idx,0,QModelIndex());
      }
   }
   return QModelIndex();
}

QModelIndex ContactBackend::index( int row, int column, const QModelIndex& parent) const
{
   if (!parent.isValid() && m_ContactByPhone.size() > row) {
      return createIndex(row,column,getContactList()[row]);
   }
   else if (parent.isValid() && getContactList()[parent.row()]->getPhoneNumbers().size() > row) {
      return createIndex(row,column,(ContactTreeBackend*)(&(getContactList()[parent.row()]->getPhoneNumbers())));
   }
   return QModelIndex();
}
