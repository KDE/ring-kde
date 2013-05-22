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
ContactBackend::ContactBackend(QObject* par) : QAbstractItemModel(par),m_UpdatesCounter(0)
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


bool ContactBackend::setData( const QModelIndex& idx, const QVariant &value, int role)
{
   Q_UNUSED(idx)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

QVariant ContactBackend::data( const QModelIndex& idx, int role) const
{
   if (!idx.isValid())
      return QVariant();
   if (!idx.parent().isValid() && (role == Qt::DisplayRole || role == Qt::EditRole)) {
      return QVariant(getContactList()[idx.row()]->getFormattedName());
   }
   else if (idx.parent().isValid() && (role == Qt::DisplayRole || role == Qt::EditRole)) {
      return QVariant(getContactList()[idx.parent().row()]->getPhoneNumbers()[idx.row()]->getNumber());
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

int ContactBackend::rowCount( const QModelIndex& par ) const
{
   if (!par.isValid()) {
      return getContactList().size();
   }
   else if (!par.parent().isValid() && par.row() < getContactList().size()) {
      return getContactList()[par.row()]->getPhoneNumbers().size();
   }
   return 0;
}

Qt::ItemFlags ContactBackend::flags( const QModelIndex& idx ) const
{
   if (!idx.isValid())
      return 0;
   return Qt::ItemIsEnabled | ((idx.parent().isValid())?Qt::ItemIsSelectable:Qt::ItemIsEnabled);
}

int ContactBackend::columnCount ( const QModelIndex& par) const
{
   Q_UNUSED(par)
   return 1;
}

QModelIndex ContactBackend::parent( const QModelIndex& idx) const
{
   if (!idx.isValid())
      return QModelIndex();
   ContactTreeBackend* modelItem = (ContactTreeBackend*)idx.internalPointer();
   if (modelItem && modelItem->type3() == ContactTreeBackend::Type::NUMBER) {
      int idx2 = getContactList().indexOf(((Contact::PhoneNumbers*)modelItem)->contact());
      if (idx2 != -1) {
         return ContactBackend::index(idx2,0,QModelIndex());
      }
   }
   return QModelIndex();
}

QModelIndex ContactBackend::index( int row, int column, const QModelIndex& par) const
{
   if (!par.isValid() && m_ContactByPhone.size() > row) {
      return createIndex(row,column,getContactList()[row]);
   }
   else if (par.isValid() && getContactList()[par.row()]->getPhoneNumbers().size() > row) {
      return createIndex(row,column,(ContactTreeBackend*)(&(getContactList()[par.row()]->getPhoneNumbers())));
   }
   return QModelIndex();
}
