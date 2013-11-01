/****************************************************************************
 *   Copyright (C) 2011-2013 by Savoir-Faire Linux                          *
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

//Parent
#include "akonadibackend.h"

//Qt
#include <QtCore/QTimer>
#include <QtCore/QObject>
#include <QtCore/QPointer>

//KDE
#include <KDebug>
#include <kdialog.h>
#include <akonadi/control.h>
#include <akonadi/collectionfilterproxymodel.h>
#include <akonadi/kmime/messagemodel.h>
#include <akonadi/recursiveitemfetchjob.h>
#include <akonadi/itemfetchscope.h>
#include <akonadi/collectionfetchjob.h>
#include <akonadi/collectionfetchscope.h>
#include <akonadi/contact/contacteditor.h>
#include <akonadi/contact/contacteditordialog.h>
#include <akonadi/session.h>
#include <kabc/addressee.h>
#include <kabc/addresseelist.h>
#include <kabc/contactgroup.h>

//SFLPhone library
#include "../lib/contact.h"
#include "../lib/accountlistmodel.h"
#include "../lib/account.h"
#include "../lib/call.h"
#include "../lib/callmodel.h"
#include "../lib/phonenumber.h"
#include "../lib/phonedirectorymodel.h"
#include "../lib/numbercategorymodel.h"
#include "../lib/numbercategory.h"
#include "kcfg_settings.h"

///Init static attributes
AkonadiBackend*  AkonadiBackend::m_pInstance = nullptr;

///Constructor
AkonadiBackend::AkonadiBackend(QObject* parent) : AbstractContactBackend(parent)
{
   m_pSession = new Akonadi::Session( "SFLPhone::instance" );

   // fetching all collections containing emails recursively, starting at the root collection
   m_pJob = new Akonadi::CollectionFetchJob( Akonadi::Collection::root(), Akonadi::CollectionFetchJob::Recursive, this );
   m_pJob->fetchScope().setContentMimeTypes( QStringList() << "text/directory" );
   connect( m_pJob, SIGNAL(collectionsReceived(Akonadi::Collection::List)), this, SLOT(collectionsReceived(Akonadi::Collection::List)) );
} //AkonadiBackend

///Destructor
AkonadiBackend::~AkonadiBackend()
{
   delete m_pSession;
   if (Call::contactBackend() == this)
      Call::setContactBackend(nullptr);
   if (m_pJob)
      delete m_pJob;
}


/*****************************************************************************
 *                                                                           *
 *                                  Getters                                  *
 *                                                                           *
 ****************************************************************************/

///Singleton
AbstractContactBackend* AkonadiBackend::instance()
{
   if (m_pInstance == nullptr) {
      m_pInstance = new AkonadiBackend(0);
   }
   return m_pInstance;
}

///Find contact by UID
Contact* AkonadiBackend::getContactByUid(const QString& uid)
{
   return m_ContactByUid[uid];
}

///Return contact list
const ContactList& AkonadiBackend::getContactList() const
{
   return m_pContacts;
}


/*****************************************************************************
 *                                                                           *
 *                                  Helper                                   *
 *                                                                           *
 ****************************************************************************/

///Convert string to akonadi KABC::PhoneNumber
KABC::PhoneNumber::Type AkonadiBackend::nameToType(const QString& name)
{
   if      (name == "Home"   ) return KABC::PhoneNumber::Home ;
   else if (name == "Work"   ) return KABC::PhoneNumber::Work ;
   else if (name == "Msg"    ) return KABC::PhoneNumber::Msg  ;
   else if (name == "Pref"   ) return KABC::PhoneNumber::Pref ;
   else if (name == "Voice"  ) return KABC::PhoneNumber::Voice;
   else if (name == "Fax"    ) return KABC::PhoneNumber::Fax  ;
   else if (name == "Cell"   ) return KABC::PhoneNumber::Cell ;
   else if (name == "Video"  ) return KABC::PhoneNumber::Video;
   else if (name == "Bbs"    ) return KABC::PhoneNumber::Bbs  ;
   else if (name == "Modem"  ) return KABC::PhoneNumber::Modem;
   else if (name == "Car"    ) return KABC::PhoneNumber::Car  ;
   else if (name == "Isdn"   ) return KABC::PhoneNumber::Isdn ;
   else if (name == "Pcs"    ) return KABC::PhoneNumber::Pcs  ;
   else if (name == "Pager"  ) return KABC::PhoneNumber::Pager;
   return KABC::PhoneNumber::Home;
}


/*****************************************************************************
 *                                                                           *
 *                                  Mutator                                  *
 *                                                                           *
 ****************************************************************************/

///Update the contact list when a new Akonadi collection is added
ContactList AkonadiBackend::update(Akonadi::Collection collection)
{
   m_UpdatesCounter++;
//    Account* defaultAccount = AccountListModel::instance()->getDefaultAccount();
   m_Collection = collection;
   if ( !collection.isValid() ) {
      kDebug() << "The current collection is not valid";
      return ContactList();
   }

   Akonadi::RecursiveItemFetchJob *job = new Akonadi::RecursiveItemFetchJob( collection, QStringList() << KABC::Addressee::mimeType() << KABC::ContactGroup::mimeType());
   job->fetchScope().fetchFullPayload();
   if ( job->exec() ) {

      const Akonadi::Item::List items = job->items();

      foreach ( const Akonadi::Item &item, items ) {
         if ( item.hasPayload<KABC::Addressee>() ) {
            KABC::Addressee tmp = item.payload<KABC::Addressee>();
            Contact* aContact   = new Contact(this);

            //This need to be done first because of the phone numbers indexes
            aContact->setNickName       (tmp.nickName()       );
            aContact->setFormattedName  (tmp.formattedName()  );
            aContact->setFirstName      (tmp.givenName()      );
            aContact->setFamilyName     (tmp.familyName()     );
            aContact->setOrganization   (tmp.organization()   );
            aContact->setPreferredEmail (tmp.preferredEmail() );
            aContact->setDepartment     (tmp.department()     );
            aContact->setUid            (tmp.uid()            );

            const KABC::PhoneNumber::List numbers = tmp.phoneNumbers();
            Contact::PhoneNumbers newNumbers(aContact);
            foreach (const KABC::PhoneNumber& number, numbers) {
               newNumbers << PhoneDirectoryModel::instance()->getNumber(number.number(),aContact,nullptr,number.typeLabel());
               //new PhoneNumber(number.number(),number.typeLabel());
               QString number2 = number.number();
               if (number2.left (5) == "<sip:")
                  number2 = number2.remove(0,5);
               if (number2.right(1) == ">"    )
                  number2 = number2.remove(number2.size()-2,1);
            }
            m_ContactByUid[tmp.uid()] = aContact;
            aContact->setPhoneNumbers   (newNumbers           );

            if (!tmp.photo().data().isNull())
               aContact->setPhoto(new QPixmap(QPixmap::fromImage( tmp.photo().data()).scaled(QSize(48,48))));
            else
               aContact->setPhoto(0);

            m_AddrHash[ tmp.uid() ] = tmp ;
            m_ItemHash[ tmp.uid() ] = item;
         }
      }
      m_pContacts = m_ContactByUid.values();
   }
   return m_ContactByUid.values();
} //update

///Edit backend value using an updated frontend contact
void AkonadiBackend::editContact(Contact* contact,QWidget* parent)
{
   Akonadi::Item item = m_ItemHash[contact->uid()];
   if (!(item.hasPayload<KABC::Addressee>() && item.payload<KABC::Addressee>().uid() == contact->uid())) {
      kDebug() << "Contact not found";
      return;
   }

   if ( item.isValid() ) {
      QPointer<Akonadi::ContactEditor> editor = new Akonadi::ContactEditor( Akonadi::ContactEditor::EditMode, parent );
      editor->loadContact(item);
      QPointer<KDialog> dlg = new KDialog(parent);
      dlg->setMainWidget(editor);
      if ( dlg->exec() == QDialog::Accepted ) {
         if ( !editor->saveContact() ) {
            kDebug() << "Unable to save new contact to storage";
            return;
         }
      }
      delete editor;
      delete dlg   ;
   }
} //editContact

///Add a new contact
void AkonadiBackend::addNewContact(Contact* contact,QWidget* parent)
{
   KABC::Addressee newContact;
   newContact.setNickName       ( contact->nickName()        );
   newContact.setFormattedName  ( contact->formattedName()   );
   newContact.setGivenName      ( contact->firstName()       );
   newContact.setFamilyName     ( contact->secondName()      );
   newContact.setOrganization   ( contact->organization()    );
   newContact.setDepartment     ( contact->department()      );
   //newContact.setPreferredEmail ( contact->getPreferredEmail()  );//TODO

   foreach (PhoneNumber* nb, contact->phoneNumbers()) {
      KABC::PhoneNumber pn;
      pn.setType(nameToType(nb->category()->name()));

      pn.setNumber(nb->uri());
      newContact.insertPhoneNumber(pn);
   }

   //aContact->setPhoneNumbers   (newNumbers           );//TODO

   QPointer<Akonadi::ContactEditor> editor = new Akonadi::ContactEditor( Akonadi::ContactEditor::CreateMode, parent );

   editor->setContactTemplate(newContact);

   QPointer<KDialog> dlg = new KDialog(parent);
   dlg->setMainWidget(editor);
   if ( dlg->exec() == QDialog::Accepted ) {
      if ( !editor->saveContact() ) {
         kDebug() << "Unable to save new contact to storage";
         return;
      }
   }
   delete dlg;
} //addNewContact

///Implement virtual pure method
void AkonadiBackend::editContact(Contact* contact)
{
   editContact(contact,nullptr);
}

///Implement virtual pure method
void AkonadiBackend::addNewContact(Contact* contact)
{
   addNewContact(contact,nullptr);
}

///Add a new phone number to an existing contact
void AkonadiBackend::addPhoneNumber(Contact* contact, QString number, QString type)
{
   Akonadi::Item item = m_ItemHash[contact->uid()];
   if (!(item.hasPayload<KABC::Addressee>() && item.payload<KABC::Addressee>().uid() == contact->uid())) {
      kDebug() << "Contact not found";
      return;
   }
   if ( item.isValid() ) {
      KABC::Addressee payload = item.payload<KABC::Addressee>();
      payload.insertPhoneNumber(KABC::PhoneNumber(number,nameToType(type)));
      item.setPayload<KABC::Addressee>(payload);
      QPointer<Akonadi::ContactEditor> editor = new Akonadi::ContactEditor( Akonadi::ContactEditor::EditMode, (QWidget*)nullptr );
      editor->loadContact(item);

      QPointer<KDialog> dlg = new KDialog(nullptr);
      dlg->setMainWidget(editor);
      if ( dlg->exec() == QDialog::Accepted ) {
         if ( !editor->saveContact() ) {
            kDebug() << "Unable to save new contact to storage";
            return;
         }
      }
      delete dlg   ;
      delete editor;
   }
   else {
      kDebug() << "Invalid item";
   }
}


/*****************************************************************************
 *                                                                           *
 *                                    Slots                                  *
 *                                                                           *
 ****************************************************************************/

///Called when a new collection is added
void AkonadiBackend::collectionsReceived( const Akonadi::Collection::List&  list)
{
   foreach (const Akonadi::Collection& coll, list) {
      update(coll);
      emit collectionChanged();
   }
}

///Update the contact list even without a new collection
ContactList AkonadiBackend::update_slot()
{
   return m_pContacts;//update(m_Collection);
}
