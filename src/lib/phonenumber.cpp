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


PhoneNumber::PhoneNumber(const QString& number, const QString& type) : m_Number(number),m_Type(type)
   ,m_Tracked(false),m_Present(false)
{
   
}


PhoneNumber::PhoneNumber(const PhoneNumber& number) : m_Number(number.m_Number),m_Type(number.m_Type)
   ,m_Tracked(false),m_Present(false)
{
}

bool PhoneNumber::present() const
{
   return m_Tracked && m_Present;
}

QString PhoneNumber::presentMessage() const
{
   return m_PresentMessage;
}

///Return the number
QString PhoneNumber::number() const {
   return m_Number ;
}

///Return the phone number type
QString PhoneNumber::type() const {
   return m_Type   ;
}
