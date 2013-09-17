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
#include "abstractcontactbackend.h"

//SFLPhone library
#include "contact.h"
#include "call.h"
#include "phonenumber.h"

//Qt
#include <QtCore/QHash>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

///Constructor
AbstractContactBackend::AbstractContactBackend(QObject* par) : QAbstractItemModel(par?par:QCoreApplication::instance()),m_UpdatesCounter(0)
{
   connect(this,SIGNAL(collectionChanged()),this,SLOT(slotReloadModel()));
}

///Destructor
AbstractContactBackend::~AbstractContactBackend()
{
   if (Call::contactBackend() == this)
      Call::setContactBackend(nullptr);
   foreach (Contact* c,m_ContactByUid) {
      delete c;
   }
}

///Update slot
ContactList AbstractContactBackend::update()
{
   return update_slot();
}

///Called when the new contacts are added
void AbstractContactBackend::slotReloadModel()
{
   reset();
   emit layoutChanged();
   emit dataChanged(index(0,0),index(rowCount(),0));
}

/*****************************************************************************
 *                                                                           *
 *                                  Getters                                  *
 *                                                                           *
 ****************************************************************************/

int AbstractContactBackend::getUpdateCount()
{
   return m_UpdatesCounter;
}

/*****************************************************************************
 *                                                                           *
 *                                  Helpers                                  *
 *                                                                           *
 ****************************************************************************/

///Return the extension/user of an URI (<sip:12345@exemple.com>)
QString AbstractContactBackend::getUserFromPhone(QString phoneNumber)
{
   //Too slow
//    if (phoneNumber.indexOf('@') != -1) {
//       QString user = phoneNumber.split('@')[0];
//       return (user.indexOf(':') != -1)?user.split(':')[1]:user;
//    }
   int start(0),stop(0);
   for (int i=0;i<phoneNumber.size();i++) {
      const char c = phoneNumber[i].cell(); //Because it is fast
      if (c == ':')
         start = i;
      else if (c == '@') {
         stop = i;
         break;
      }
   }
   if (stop)
      return phoneNumber.mid(start,stop);
   return phoneNumber;
} //getUserFromPhone

/*****************************************************************************
 *                                                                           *
 *                                   Model                                   *
 *                                                                           *
 ****************************************************************************/


bool AbstractContactBackend::setData( const QModelIndex& idx, const QVariant &value, int role)
{
   Q_UNUSED(idx)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

QVariant AbstractContactBackend::data( const QModelIndex& idx, int role) const
{
   if (!idx.isValid())
      return QVariant();
   if (!idx.parent().isValid() && (role == Qt::DisplayRole || role == Qt::EditRole)) {
      return QVariant(getContactList()[idx.row()]->formattedName());
   }
   else if (idx.parent().isValid() && (role == Qt::DisplayRole || role == Qt::EditRole)) {
      return QVariant(getContactList()[idx.parent().row()]->phoneNumbers()[idx.row()]->uri());
   }
   return QVariant();
}

QVariant AbstractContactBackend::headerData(int section, Qt::Orientation orientation, int role) const
{
   Q_UNUSED(section)
   if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
      return QVariant("Contacts");
   return QVariant();
}

int AbstractContactBackend::rowCount( const QModelIndex& par ) const
{
   if (!par.isValid()) {
      return getContactList().size();
   }
   else if (!par.parent().isValid() && par.row() < getContactList().size()) {
      const int size = getContactList()[par.row()]->phoneNumbers().size();
      return size==1?0:size;
   }
   return 0;
}

Qt::ItemFlags AbstractContactBackend::flags( const QModelIndex& idx ) const
{
   if (!idx.isValid())
      return Qt::NoItemFlags;
   return Qt::ItemIsEnabled | ((idx.parent().isValid())?Qt::ItemIsSelectable:Qt::ItemIsEnabled);
}

int AbstractContactBackend::columnCount ( const QModelIndex& par) const
{
   Q_UNUSED(par)
   return 1;
}

QModelIndex AbstractContactBackend::parent( const QModelIndex& idx) const
{
   if (!idx.isValid())
      return QModelIndex();
   CategorizedCompositeNode* modelItem = (CategorizedCompositeNode*)idx.internalPointer();
   if (modelItem && modelItem->type() == CategorizedCompositeNode::Type::NUMBER) {
      int idx2 = getContactList().indexOf(((Contact::PhoneNumbers*)modelItem)->contact());
      if (idx2 != -1) {
         return AbstractContactBackend::index(idx2,0,QModelIndex());
      }
   }
   return QModelIndex();
}

QModelIndex AbstractContactBackend::index( int row, int column, const QModelIndex& par) const
{
   if (!par.isValid() && getContactList().size() > row) {
      return createIndex(row,column,getContactList()[row]);
   }
   else if (par.isValid() && getContactList()[par.row()]->phoneNumbers().size() > row) {
      return createIndex(row,column,(CategorizedCompositeNode*)(&(getContactList()[par.row()]->phoneNumbers())));
   }
   return QModelIndex();
}
