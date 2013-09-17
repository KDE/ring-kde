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
#include <QtCore/QObject>

//SFLPhone
class Account;
class Contact;
class Call;
class PhoneNumberPrivate;
class TemporaryPhoneNumber;

///PhoneNumber: represent a phone number
class LIB_EXPORT PhoneNumber : public QObject {
   Q_OBJECT
public:
   friend class PhoneDirectoryModel;
   virtual ~PhoneNumber(){};

   //Properties
   Q_PROPERTY(Account*      account         READ account  WRITE setAccount)
   Q_PROPERTY(Contact*      contact         READ contact  WRITE setContact)
   Q_PROPERTY(time_t        lastUsed        READ lastUsed                 )
   Q_PROPERTY(QString       uri             READ uri                      )
   Q_PROPERTY(QString       type            READ type                     )
   Q_PROPERTY(int           callCount       READ callCount                )
   Q_PROPERTY(QList<Call*>  calls           READ calls                    )
   Q_PROPERTY(int           popularityIndex READ popularityIndex          )

   ///@enum PresenceStatus: Presence status
   enum class PresenceStatus {
      UNTRACKED = 0,
      PRESENT   = 1,
      ABSENT    = 2,
   };
   Q_ENUMS(PresenceStatus)

   ///@enum State: Is this temporary, blank, used or unused
   enum class State {
      BLANK     = 0, //This number represent no number
      TEMPORARY = 1, //This number is not yet complete
      USED      = 2, //This number have been called before
      UNUSED    = 3, //This number have never been called, but is in the address book
   };
   Q_ENUMS(State)

   //Getters
   QString            uri             () const;
   QString            type            () const;
   bool               tracked         () const;
   bool               present         () const;
   QString            presenceMessage () const;
   Account*           account         () const;
   Contact*           contact         () const;
   time_t             lastUsed        () const;
   PhoneNumber::State state           () const;
   int                callCount       () const;
   QList<Call*>       calls           () const;
   int                popularityIndex () const;
   QHash<QString,int> alternativeNames() const;
   QString            mostCommonName  () const;
   QString            hostname        () const;
   QString            fullUri         () const;

   //Setters
   Q_INVOKABLE void setAccount(Account* account);
   Q_INVOKABLE void setContact(Contact* contact);
   Q_INVOKABLE void setTracked(bool     track  );

   //Mutator
   Q_INVOKABLE void addCall(Call* call);

   //Static
   static const PhoneNumber* BLANK;

   //Helper
   QString toHash() const;

protected:
   //Constructor
   PhoneNumber(const QString& uri, const QString& type);

   //Attributes
   QString            m_Uri              ;
   PhoneNumber::State m_State            ;

   //Helper
   static QString stripUri(const QString& uri);

private:
   friend class PhoneNumberPrivate;

   //Attributes
   QString            m_Type             ;
   bool               m_Present          ;
   QString            m_PresentMessage   ;
   bool               m_Tracked          ;
   bool               m_Temporary        ;
   Contact*           m_pContact         ;
   Account*           m_pAccount         ;
   time_t             m_LastUsed         ;
   QList<Call*>       m_lCalls           ;
   int                m_PopularityIndex  ;
   QString            m_MostCommonName   ;
   QHash<QString,int> m_hNames           ;
   bool               m_hasType          ;

   //Static attributes
   static QHash<int,Call*> m_shMostUsed  ;

Q_SIGNALS:
   void callAdded(Call* call);
   void changed  (          );
};

Q_DECLARE_METATYPE( PhoneNumber* )

///@class TemporaryPhoneNumber: An incomplete phone number
class LIB_EXPORT TemporaryPhoneNumber : public PhoneNumber {
   Q_OBJECT
public:
   explicit TemporaryPhoneNumber(const PhoneNumber* number = nullptr);
   void setUri(const QString& uri);
};

#endif
