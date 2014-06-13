/****************************************************************************
 *   Copyright (C) 2009-2014 by Savoir-Faire Linux                          *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>          *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
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

//Parent
#include "contact.h"

//Qt
#include <QtGui/QPixmap>

//SFLPhone library
#include "sflphone_const.h"
#include "phonenumber.h"
#include "abstractitembackend.h"
#include "transitionalcontactbackend.h"



Contact::PhoneNumbers::PhoneNumbers(Contact* parent) : QVector<PhoneNumber*>(),CategorizedCompositeNode(CategorizedCompositeNode::Type::NUMBER),
    m_pParent2(parent)
{
}

Contact::PhoneNumbers::PhoneNumbers(Contact* parent, const QVector<PhoneNumber*>& list)
: QVector<PhoneNumber*>(list),CategorizedCompositeNode(CategorizedCompositeNode::Type::NUMBER),m_pParent2(parent)
{
}

Contact* Contact::PhoneNumbers::contact() const
{
   return m_pParent2;
}

///Constructor
Contact::Contact(AbstractContactBackend* parent):QObject(parent?parent:TransitionalContactBackend::instance()),m_pPhoto(nullptr),
   m_Numbers(this),m_DisplayPhoto(nullptr),m_Active(true),m_pBackend(parent?parent:TransitionalContactBackend::instance())
{
}

///Destructor
Contact::~Contact()
{
   delete m_pPhoto;
}

///Get the phone number list
const Contact::PhoneNumbers& Contact::phoneNumbers() const
{
   return m_Numbers;
}

///Get the nickname
const QString& Contact::nickName() const
{
   return m_NickName;
}

///Get the firstname
const QString& Contact::firstName() const
{
   return m_FirstName;
}

///Get the second/family name
const QString& Contact::secondName() const
{
   return m_SecondName;
}

///Get the photo
const QPixmap* Contact::photo() const
{
   return m_pPhoto;
}

///Get the formatted name
const QString& Contact::formattedName() const
{
   return m_FormattedName;
}

///Get the organisation
const QString& Contact::organization()  const
{
   return m_Organization;
}

///Get the preferred email
const QString& Contact::preferredEmail()  const
{
   return m_PreferredEmail;
}

///Get the unique identifier (used for drag and drop) 
const QByteArray& Contact::uid() const
{
   return m_Uid;
}

///Get the group
const QString& Contact::group() const
{
   return m_Group;
}

const QString& Contact::department() const
{
   return m_Department;
}

///Set the phone number (type and number)
void Contact::setPhoneNumbers(PhoneNumbers numbers)
{
   const int oldCount(m_Numbers.size()),newCount(numbers.size());
   foreach(PhoneNumber* n, m_Numbers)
      disconnect(n,SIGNAL(presentChanged(bool)),this,SLOT(slotPresenceChanged()));
   m_Numbers = numbers;
   if (newCount < oldCount) //Rows need to be removed from models first
      emit phoneNumberCountAboutToChange(newCount,oldCount);
   foreach(PhoneNumber* n, m_Numbers)
      connect(n,SIGNAL(presentChanged(bool)),this,SLOT(slotPresenceChanged()));
   if (newCount > oldCount) //Need to be updated after the data to prevent invalid memory access
      emit phoneNumberCountChanged(newCount,oldCount);
   emit changed();
}

///Set the nickname
void Contact::setNickName(const QString& name)
{
   m_NickName = name;
   emit changed();
}

///Set the first name
void Contact::setFirstName(const QString& name)
{
   m_FirstName = name;
   setObjectName(formattedName());
   emit changed();
}

///Set the family name
void Contact::setFamilyName(const QString& name)
{
   m_SecondName = name;
   setObjectName(formattedName());
   emit changed();
}

///Set the Photo/Avatar
void Contact::setPhoto(QPixmap* photo)
{
   m_pPhoto = photo;
   emit changed();
}

///Set the formatted name (display name)
void Contact::setFormattedName(const QString& name)
{
   m_FormattedName = name;
   emit changed();
}

///Set the organisation / business
void Contact::setOrganization(const QString& name)
{
   m_Organization = name;
   emit changed();
}

///Set the default email
void Contact::setPreferredEmail(const QString& name)
{
   m_PreferredEmail = name;
   emit changed();
}

///Set UID
void Contact::setUid(const QByteArray& id)
{
   m_Uid = id;
   emit changed();
}

///Set Group
void Contact::setGroup(const QString& name)
{
   m_Group = name;
   emit changed();
}

///Set department
void Contact::setDepartment(const QString& name)
{
   m_Department = name;
   emit changed();
}

///If the contact have been deleted or not yet fully created
void Contact::setActive( bool active)
{
   m_Active = active;
   emit statusChanged(m_Active);
   emit changed();
}

///Return if one of the PhoneNumber is present
bool Contact::isPresent() const
{
   foreach(const PhoneNumber* n,m_Numbers) {
      if (n->isPresent())
         return true;
   }
   return false;
}

///Return if one of the PhoneNumber is tracked
bool Contact::isTracked() const
{
   foreach(const PhoneNumber* n,m_Numbers) {
      if (n->isTracked())
         return true;
   }
   return false;
}

///Have this contact been deleted or doesn't exist yet
bool Contact::isActive() const
{
   return m_Active;
}

///Return if one of the PhoneNumber support presence
bool Contact::supportPresence() const
{
   foreach(const PhoneNumber* n,m_Numbers) {
      if (n->supportPresence())
         return true;
   }
   return false;
}


QObject* Contact::PhoneNumbers::getSelf() const {
   return m_pParent2;
}

time_t Contact::PhoneNumbers::lastUsedTimeStamp() const
{
   time_t t = 0;
   for (int i=0;i<size();i++) {
      if (at(i)->lastUsed() > t)
         t = at(i)->lastUsed();
   }
   return t;
}

///Callback when one of the phone number presence change
void Contact::slotPresenceChanged()
{
   emit changed();
}

///Save the contact
bool Contact::save() const
{
   return m_pBackend->save(this);
}

///Show an implementation dependant dialog to edit the contact
bool Contact::edit()
{
   return m_pBackend->edit(this);
}

///Add a new phone number to the backend
///@note The backend is expected to notify the Contact (asynchronously) when done
bool Contact::addPhoneNumber(PhoneNumber* n)
{
   return m_pBackend->addPhoneNumber(this,n);
}
