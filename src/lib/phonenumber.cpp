/****************************************************************************
 *   Copyright (C) 2013 by Savoir-Faire Linux                               *
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
#include "phonenumber.h"
#include "phonedirectorymodel.h"
#include "contact.h"
#include "account.h"
#include "call.h"

QHash<int,Call*> PhoneNumber::m_shMostUsed = QHash<int,Call*>();

class PhoneNumberPrivate {
public:
   static PhoneNumber* initBlank()
   {
      PhoneNumber* blanc = new PhoneNumber("","");
      blanc->m_State = PhoneNumber::State::BLANK;
      return blanc;
   }
};
const PhoneNumber* PhoneNumber::BLANK = PhoneNumberPrivate::initBlank();

///Constructor
PhoneNumber::PhoneNumber(const QString& number, const QString& type) : QObject(PhoneDirectoryModel::instance()),
   m_Uri(number),m_Type(type),m_Tracked(false),m_Present(false),m_LastUsed(0),m_Temporary(false),
   m_State(PhoneNumber::State::UNUSED)
{
   setObjectName(number);
}

///Is this number present
bool PhoneNumber::present() const
{
   return m_Tracked && m_Present;
}

///This number presence status string
QString PhoneNumber::presentMessage() const
{
   return m_PresentMessage;
}

///Return the number
QString PhoneNumber::uri() const {
   return m_Uri ;
}

///Return the phone number type
QString PhoneNumber::type() const {
   return m_Type   ;
}

///Return this number associated account, if any
Account* PhoneNumber::account() const
{
   return m_pAccount;
}

///Return this number associated contact, if any
Contact* PhoneNumber::contact() const
{
   return m_pContact;
}

///Return when this number was last used
time_t PhoneNumber::lastUsed() const
{
   return m_LastUsed;
}

///Set this number default account
void PhoneNumber::setAccount(Account* account)
{
   m_pAccount = account;
}

///Set this number contact
void PhoneNumber::setContact(Contact* contact)
{
   m_pContact = contact;
}

///Return the current state of the number
PhoneNumber::State PhoneNumber::state() const
{
   return m_State;
}

///Return the number of calls from this number
int PhoneNumber::callCount() const
{
   return m_lCalls.size();
}

///Return all calls from this number
QList<Call*> PhoneNumber::calls() const
{
   return m_lCalls;
}
