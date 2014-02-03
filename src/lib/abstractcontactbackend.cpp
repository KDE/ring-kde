/************************************************************************************
 *   Copyright (C) 2009-2014 by Savoir-Faire Linux                                  *
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
AbstractContactBackend::AbstractContactBackend(QObject* par) : QObject(par?par:QCoreApplication::instance())
{
}

///Destructor
AbstractContactBackend::~AbstractContactBackend()
{
//    if (Call::contactBackend() == this)
//       Call::setContactBackend(nullptr);
//    foreach (Contact* c,m_ContactByUid) {
//       delete c;
//    } //TODO uncomment
}

///Update slot
// ContactList AbstractContactBackend::update()
// {
//    return update_slot();
   //TODO do something
// }


/*****************************************************************************
 *                                                                           *
 *                                   Model                                   *
 *                                                                           *
 ****************************************************************************/


// bool AbstractContactBackend::setData( const QModelIndex& idx, const QVariant &value, int role)
// {
//    Q_UNUSED(idx)
//    Q_UNUSED(value)
//    Q_UNUSED(role)
//    return false;
// }

// QVariant AbstractContactBackend::data( const QModelIndex& idx, int role) const
// {
//    if (!idx.isValid())
//       return QVariant();
//    if (!idx.parent().isValid() && (role == Qt::DisplayRole || role == Qt::EditRole)) {
//       const Contact* c = getContactList()[idx.row()];
//       if (c)
//          return QVariant(c->formattedName());
//    }
//    else if (idx.parent().isValid() && (role == Qt::DisplayRole || role == Qt::EditRole)) {
//       const Contact* c = getContactList()[idx.parent().row()];
//       if (c)
//          return QVariant(c->phoneNumbers()[idx.row()]->uri());
//    }
//    return QVariant();
// }

// QVariant AbstractContactBackend::headerData(int section, Qt::Orientation orientation, int role) const
// {
//    Q_UNUSED(section)
//    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
//       return QVariant(tr("Contacts"));
//    return QVariant();
// }

// int AbstractContactBackend::rowCount( const QModelIndex& par ) const
// {
//    if (!par.isValid()) {
//       return getContactList().size();
//    }
//    else if (!par.parent().isValid() && par.row() < getContactList().size()) {
//       const Contact* c = getContactList()[par.row()];
//       if (c) {
//          const int size = c->phoneNumbers().size();
//          return size==1?0:size;
//       }
//    }
//    return 0;
// }

// Qt::ItemFlags AbstractContactBackend::flags( const QModelIndex& idx ) const
// {
//    if (!idx.isValid())
//       return Qt::NoItemFlags;
//    return Qt::ItemIsEnabled | ((idx.parent().isValid())?Qt::ItemIsSelectable:Qt::ItemIsEnabled);
// }

// int AbstractContactBackend::columnCount ( const QModelIndex& par) const
// {
//    Q_UNUSED(par)
//    return 1;
// }

// QModelIndex AbstractContactBackend::parent( const QModelIndex& idx) const
// {
//    if (!idx.isValid())
//       return QModelIndex();
//    CategorizedCompositeNode* modelItem = (CategorizedCompositeNode*)idx.internalPointer();
//    if (modelItem && modelItem->type() == CategorizedCompositeNode::Type::NUMBER) {
//       int idx2 = getContactList().indexOf(((Contact::PhoneNumbers*)modelItem)->contact());
//       if (idx2 != -1) {
//          return AbstractContactBackend::index(idx2,0,QModelIndex());
//       }
//    }
//    return QModelIndex();
// }

// QModelIndex AbstractContactBackend::index( int row, int column, const QModelIndex& par) const
// {
//    if (!par.isValid() && getContactList().size() > row) {
//       return createIndex(row,column,getContactList()[row]);
//    }
//    else if (par.isValid() && getContactList()[par.row()]->phoneNumbers().size() > row) {
//       return createIndex(row,column,(CategorizedCompositeNode*)(&(getContactList()[par.row()]->phoneNumbers())));
//    }
//    return QModelIndex();
// }


bool AbstractContactBackend::saveContacts(const QList<Contact*> contacts)
{
   bool ret = true;
   foreach(const Contact* c, contacts) {
      ret &= saveContact(c);
   }
   return ret;
}
