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

#ifndef AKONADI_BACKEND_H
#define AKONADI_BACKEND_H

#include "../lib/abstractcontactbackend.h"
#include "../lib/typedefs.h"
#include <akonadi/collectionmodel.h>

//Qt
class QObject;

//KDE
namespace KABC {
   class Addressee    ;
   class PhoneNumber  ;
}
#include <kabc/phonenumber.h>

namespace Akonadi {
   class Session        ;
   class Collection     ;
   class Item           ;
}

//SFLPhone
class Contact;

///AkonadiBackend: Implement a backend for Akonadi
class LIB_EXPORT AkonadiBackend : public AbstractContactBackend {
   Q_OBJECT
public:
   static   AbstractContactBackend* instance();
   Contact* getContactByPhone ( const QString& phoneNumber ,bool resolveDNS = false, Account* a=nullptr);
   Contact* getContactByUid   ( const QString& uid                                                     );
   void     editContact       ( Contact*       contact , QWidget* parent = 0                           );
   void     addNewContact     ( Contact*       contact , QWidget* parent = 0                           );
   virtual void addPhoneNumber( Contact*       contact , QString  number, QString type                 );

   virtual void     editContact   ( Contact*   contact                                                 );
   virtual void     addNewContact ( Contact*   contact                                                 );
   virtual ~AkonadiBackend        (                                                                    );

   virtual const ContactList& getContactList() const;

private:
   //Singleton constructor
   explicit AkonadiBackend(QObject* parent);

   //Attributes
   static AkonadiBackend*         m_pInstance  ;
   Akonadi::Session*              m_pSession   ;
   Akonadi::Collection            m_Collection ;
   QHash<QString,KABC::Addressee> m_AddrHash   ;
   QHash<QString,Akonadi::Item>   m_ItemHash   ;
   ContactList                    m_pContacts  ;

   //Helper
   KABC::PhoneNumber::Type nameToType(const QString& name);

protected:
   ContactList update_slot();

public Q_SLOTS:
   ContactList update(Akonadi::Collection collection);
   void collectionsReceived( const Akonadi::Collection::List& );
};

#endif
