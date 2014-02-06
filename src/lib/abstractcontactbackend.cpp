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
}

bool AbstractContactBackend::saveContacts(const QList<Contact*> contacts)
{
   bool ret = true;
   foreach(const Contact* c, contacts) {
      ret &= saveContact(c);
   }
   return ret;
}
