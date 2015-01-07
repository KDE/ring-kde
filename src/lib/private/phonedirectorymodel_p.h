/****************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                          *
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
#ifndef PHONEDIRECTORYMODEL_PRIVATE_H
#define PHONEDIRECTORYMODEL_PRIVATE_H
#include <QtCore/QObject>

//SFLphone
class PhoneDirectoryModel;
#include "phonenumber.h"

//Internal data structures
///@struct NumberWrapper Wrap phone numbers to prevent collisions
struct NumberWrapper {
   QVector<PhoneNumber*> numbers;
};

class PhoneDirectoryModelPrivate : public QObject
{
   Q_OBJECT
public:
   PhoneDirectoryModelPrivate(PhoneDirectoryModel* parent);


   //Model columns
   enum class Columns {
      URI              = 0,
      TYPE             = 1,
      CONTACT          = 2,
      ACCOUNT          = 3,
      STATE            = 4,
      CALL_COUNT       = 5,
      WEEK_COUNT       = 6,
      TRIM_COUNT       = 7,
      HAVE_CALLED      = 8,
      LAST_USED        = 9,
      NAME_COUNT       = 10,
      TOTAL_SECONDS    = 11,
      POPULARITY_INDEX = 12,
      BOOKMARED        = 13,
      TRACKED          = 14,
      PRESENT          = 15,
      PRESENCE_MESSAGE = 16,
      UID              = 17,
   };


   //Helpers
   void indexNumber(PhoneNumber* number, const QStringList& names   );
   void setAccount (PhoneNumber* number,       Account*     account );
   PhoneNumber* fillDetails(NumberWrapper* wrap, const URI& strippedUri, Account* account, Contact* contact, const QString& type);

   //Attributes
   QVector<PhoneNumber*>         m_lNumbers         ;
   QHash<QString,NumberWrapper*> m_hDirectory       ;
   QVector<PhoneNumber*>         m_lPopularityIndex ;
   QMap<QString,NumberWrapper*>  m_lSortedNames     ;
   QMap<QString,NumberWrapper*>  m_hSortedNumbers   ;
   QHash<QString,NumberWrapper*> m_hNumbersByNames  ;
   bool                          m_CallWithAccount  ;

private:
   PhoneDirectoryModel* q_ptr;

private Q_SLOTS:
   void slotCallAdded(Call* call);
   void slotChanged();

   //From DBus
   void slotNewBuddySubscription(const QString& uri, const QString& accountId, bool status, const QString& message);
};

#endif