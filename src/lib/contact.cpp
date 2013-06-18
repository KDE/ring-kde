/****************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                          *
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


Contact::PhoneNumber::PhoneNumber(const QString& number, const QString& type) : m_Number(number),m_Type(type)
{
}

Contact::PhoneNumbers::PhoneNumbers(Contact* parent) : QList<Contact::PhoneNumber*>(),ContactTreeBackend(ContactTreeBackend::NUMBER),m_pParent(parent)
{
}

Contact::PhoneNumbers::PhoneNumbers(Contact* parent, const QList<Contact::PhoneNumber*>& list)
: QList<Contact::PhoneNumber*>(list),ContactTreeBackend(ContactTreeBackend::NUMBER),m_pParent(parent)
{
}

Contact* Contact::PhoneNumbers::contact() const
{
   return m_pParent;
}

ContactTreeBackend::ContactTreeBackend(ContactTreeBackend::Type _type) : m_Type(_type),m_DropState(0)
{
}

ContactTreeBackend::~ContactTreeBackend()
{
}


char ContactTreeBackend::dropState()
{
   return m_DropState;
}

void ContactTreeBackend::setDropState(const char state)
{
   m_DropState = state;
}

ContactTreeBackend::Type ContactTreeBackend::type() const
{
   return m_Type;
}

QObject* Contact::self() {return this;}

///Constructor
Contact::Contact():m_pPhoto(0),ContactTreeBackend(ContactTreeBackend::Type::CONTACT),m_Numbers(this)
{
   initItem();
}

///Destructor
Contact::~Contact()
{
   delete m_pPhoto;
   foreach (Contact::PhoneNumber* ph, m_Numbers) {
      delete ph;
   }
}

///May be used in extended classes
void Contact::initItem()
{
   initItemWidget();
}

///May be used in extended classes
void Contact::initItemWidget()
{
   
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
const QString& Contact::uid() const
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

///Get the contact type
const QString& Contact::type() const
{
   return m_Type;
}

///Set the phone number (type and number) 
void Contact::setPhoneNumbers(PhoneNumbers numbers)
{
   m_Numbers    = numbers;
}

///Set the nickname
void Contact::setNickName(const QString& name)
{
   m_NickName   = name;
}

///Set the first name
void Contact::setFirstName(const QString& name)
{
   m_FirstName  = name;
}

///Set the family name
void Contact::setFamilyName(const QString& name)
{
   m_SecondName = name;
}

///Set the Photo/Avatar
void Contact::setPhoto(QPixmap* photo)
{
   m_pPhoto = photo;
}

///Set the formatted name (display name)
void Contact::setFormattedName(const QString& name)
{
   m_FormattedName = name;
}

///Set the organisation / business
void Contact::setOrganization(const QString& name)
{
   m_Organization = name;
}

///Set the default email
void Contact::setPreferredEmail(const QString& name)
{
   m_PreferredEmail = name;
}

///Set UID
void Contact::setUid(const QString& id)
{
   m_Uid = id;
}

///Set Group
void Contact::setGroup(const QString& name)
{
   m_Group = name;
}

///Set department
void Contact::setDepartment(const QString& name)
{
   m_Department = name;
}

///Turn the contact into QString-QString hash
QHash<QString,QVariant> Contact::toHash()
{
   QHash<QString,QVariant> aContact;
   //aContact[""] = PhoneNumbers   getPhoneNumbers()    const;
   aContact[ "nickName"       ] = nickName();
   aContact[ "firstName"      ] = firstName();
   aContact[ "secondName"     ] = secondName();
   aContact[ "formattedName"  ] = formattedName();
   aContact[ "organization"   ] = organization();
   aContact[ "uid"            ] = uid();
   aContact[ "preferredEmail" ] = preferredEmail();
   //aContact[ "Photo"          ] = QVariant(*getPhoto());
   aContact[ "type"           ] = type();
   aContact[ "group"          ] = group();
   aContact[ "department"     ] = department();
   return aContact;
}

///Return the number
QString& Contact::PhoneNumber::number() {
   return m_Number ;
}

///Return the phone number type
QString& Contact::PhoneNumber::type() {
   return m_Type   ;
}

QObject* Contact::PhoneNumbers::self() {
   return m_pParent;
}
