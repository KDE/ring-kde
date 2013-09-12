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
#ifndef PHONENUMBER_H
#define PHONENUMBER_H

#include "typedefs.h"

//Qt
#include <QStringList>
#include <QtCore/QSize>

///PhoneNumber: represent a phone number
class LIB_EXPORT PhoneNumber {
public:

   ///@enum PresenceStatus: Presence status
   enum class PresenceStatus {
      UNTRACKED = 0,
      PRESENT   = 1,
      ABSENT    = 2,
   };

   ///Constructor
   PhoneNumber(const QString& number, const QString& type);
   PhoneNumber(const PhoneNumber& number);

   //Getters
   QString number        () const;
   QString type          () const;
   bool    present       () const;
   QString presentMessage() const;

private:
   QString m_Number        ;
   QString m_Type          ;
   bool    m_Present       ;
   QString m_PresentMessage;
   bool    m_Tracked       ;
};

#endif
