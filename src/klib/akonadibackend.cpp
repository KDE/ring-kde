/****************************************************************************
 *   Copyright (C) 2011-2015 by Savoir-Faire Linux                          *
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
#include <QTimer>
#include <QObject>
#include <QPointer>

//KDE
#include <QDebug>
#include <KJob>
#include <QDialog>
#include <akonadi/control.h>
#include <akonadi/collectionfilterproxymodel.h>
#include <akonadi/kmime/messagemodel.h>
#include <akonadi/recursiveitemfetchjob.h>
#include <akonadi/itemfetchjob.h>
#include <akonadi/itemfetchscope.h>
#include <akonadi/collectionfetchjob.h>
#include <akonadi/collectionfetchscope.h>
#include <akonadi/contact/contacteditor.h>
#include <akonadi/contact/contacteditordialog.h>
#include <akonadi/session.h>
#include <akonadi/monitor.h>
#include <akonadi/itemdeletejob.h>
#include <akonadi/entitydisplayattribute.h>
#include <kabc/addressee.h>
#include <kabc/addresseelist.h>
#include <kabc/contactgroup.h>
#include <QAbstractItemModel>
#include <KConfigGroup>

//Ring library
#include "person.h"
#include "accountmodel.h"
#include "account.h"
#include "call.h"
#include "callmodel.h"
#include "contactmethod.h"
#include "phonedirectorymodel.h"
#include "numbercategorymodel.h"
#include "collectioninterface.h"
#include "numbercategory.h"
#include "personmodel.h"
#include "kcfg_settings.h"

Akonadi::Session* AkonadiBackend::m_pSession = nullptr;
QHash<Akonadi::Collection::Id, AkonadiBackend*> AkonadiBackend::m_hParentLookup;





///Destructor
AkonadiBackend::~AkonadiBackend()
{
   if (m_pJob)
      delete m_pJob;
   if (m_pMonitor)
      delete m_pMonitor;
}

bool AkonadiEditor::save(const Person* item)
{
   Q_UNUSED(item)
   return false;
}

bool AkonadiEditor::append(const Person* item)
{
   Q_UNUSED(item)
   return false;
}

bool AkonadiEditor::remove(Person* c)
{
   if (!c)
      return false;
   Akonadi::Item item = m_ItemHash[c->uid()];
   Akonadi::ItemDeleteJob *job = new Akonadi::ItemDeleteJob( item );
   job->exec();
   c->setActive(false);
   return true;
}

bool AkonadiEditor::edit( Person* item)
{
   Q_UNUSED(item)
   return false;
}

bool AkonadiEditor::addNew( Person* item)
{
   Q_UNUSED(item)
   return false;
}

QVector<Person*> AkonadiEditor::items() const
{
   return QVector<Person*>();
}

QString AkonadiBackend::name () const
{
   QString name;
   Akonadi::EntityDisplayAttribute* attr = m_Coll.attribute<Akonadi::EntityDisplayAttribute>();
   if (attr)
      name = attr->displayName().trimmed();
   return name.isEmpty()?m_Coll.name():name;
}

QString AkonadiBackend::category() const
{
   return tr("Contact");
}

QVariant AkonadiBackend::icon() const
{
   Akonadi::EntityDisplayAttribute* attr = m_Coll.attribute<Akonadi::EntityDisplayAttribute>();
   if (attr)
      return QVariant(attr->icon());
   return QVariant();
}

bool AkonadiBackend::isEnabled() const
{
   return m_isEnabled;
}

bool AkonadiBackend::load()
{
   Akonadi::ItemFetchScope scope;
   scope.fetchFullPayload(true);

   // fetching all collections recursively, starting at the root collection
   m_pJob = new Akonadi::ItemFetchJob( m_Coll, this );
   m_pJob->setFetchScope(scope);
//    m_pJob->fetchScope().setContentMimeTypes( QStringList() << "text/x-vcard" );
   connect( m_pJob, SIGNAL(itemsReceived(Akonadi::Item::List)), this, SLOT(itemsReceived(Akonadi::Item::List)) );

   //Configure change monitor
   m_pMonitor = new Akonadi::Monitor(this);
   m_pMonitor->fetchCollectionStatistics(false);
   m_pMonitor->setItemFetchScope(scope);
   connect(m_pMonitor,SIGNAL(itemAdded(Akonadi::Item,Akonadi::Collection)),this,SLOT(slotItemAdded(Akonadi::Item,Akonadi::Collection)));
   connect(m_pMonitor,SIGNAL(itemChanged(Akonadi::Item,QSet<QByteArray>)),this,SLOT(slotItemChanged(Akonadi::Item,QSet<QByteArray>)));
   connect(m_pMonitor,SIGNAL(itemRemoved(Akonadi::Item)),this,SLOT(slotItemRemoved(Akonadi::Item)));


   m_pMonitor->setCollectionMonitored(m_Coll,true);
   m_isEnabled = true; //FIXME does it make sense to merge loaded and enabled?
   return true;
}

bool AkonadiBackend::enable (bool enable)
{
   if (enable && (!m_wasEnabled)) {
      return load();
   }
   else if (m_wasEnabled && enable) {
      foreach(Person* contact, static_cast<AkonadiEditor*>(editor<Person>())->m_lBackendPersons) {
         contact->setActive(true);
      }
      m_wasEnabled = false;
      m_isEnabled = true;
   }
   else if (isEnabled()) {
      foreach(Person* contact, static_cast<AkonadiEditor*>(editor<Person>())->m_lBackendPersons) {
         contact->setActive(false);
      }
      m_isEnabled = false;
      m_wasEnabled = true;
   }
   return false;
}

bool AkonadiBackend::reload()
{
   //TODO
   return false;
}

QByteArray AkonadiBackend::id() const
{
   return QString::number(m_Coll.id()).toLatin1();
}

CollectionInterface::SupportedFeatures AkonadiBackend::supportedFeatures() const
{
   return (CollectionInterface::SupportedFeatures) (
      CollectionInterface::SupportedFeatures::NONE        |
      CollectionInterface::SupportedFeatures::LOAD        |
      CollectionInterface::SupportedFeatures::SAVE        |
      CollectionInterface::SupportedFeatures::EDIT        |
      CollectionInterface::SupportedFeatures::REMOVE      |
      CollectionInterface::SupportedFeatures::ADD         |
      CollectionInterface::SupportedFeatures::MANAGEABLE  |
      CollectionInterface::SupportedFeatures::DISABLEABLE |
      CollectionInterface::SupportedFeatures::ENABLEABLE
   );
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

void AkonadiBackend::fillPerson(Person* c, const KABC::Addressee& addr) const
{
   c->setNickName       (addr.nickName()       );
   c->setFormattedName  (addr.formattedName()  );
   c->setFirstName      (addr.givenName()      );
   c->setFamilyName     (addr.familyName()     );
   c->setOrganization   (addr.organization()   );
   c->setPreferredEmail (addr.preferredEmail() );
   c->setDepartment     (addr.department()     );
   c->setUid            (addr.uid().toUtf8()   );

   const KABC::PhoneNumber::List numbers = addr.phoneNumbers();
   Person::ContactMethods newNumbers(c);
   foreach (const KABC::PhoneNumber& number, numbers) {
      newNumbers << PhoneDirectoryModel::instance()->getNumber(number.number(),c,nullptr,number.typeLabel());
      QString number2 = number.number();
      if (number2.left (5) == "<sip:")
         number2 = number2.remove(0,5);
      if (number2.right(1) == ">"    )
         number2 = number2.remove(number2.size()-2,1);
   }
   c->setContactMethods   (newNumbers           );
}

Person* AkonadiBackend::addItem(Akonadi::Item item, bool ignoreEmpty)
{
   Person* aPerson = nullptr;
   if ( item.hasPayload<KABC::Addressee>() ) {
      m_pMonitor->setItemMonitored(item,true);
      KABC::Addressee tmp = item.payload<KABC::Addressee>();
      const KABC::PhoneNumber::List numbers = tmp.phoneNumbers();
      const QString uid = tmp.uid();
      if (numbers.size() || !ignoreEmpty) {
         aPerson   = new Person(this);

         //This need to be done first because of the phone numbers indexes
         fillPerson(aPerson,tmp);

         if (!tmp.photo().data().isNull())
            aPerson->setPhoto(QPixmap::fromImage( tmp.photo().data()).scaled(QSize(48,48)));

         static_cast<AkonadiEditor*>(editor<Person>())->m_AddrHash[ uid ] = tmp ;
         static_cast<AkonadiEditor*>(editor<Person>())->m_ItemHash[ uid ] = item;
         static_cast<AkonadiEditor*>(editor<Person>())->m_lBackendPersons << aPerson;
      }
   }
   return aPerson;
}


/*****************************************************************************
 *                                                                           *
 *                                  Mutator                                  *
 *                                                                           *
 ****************************************************************************/

void AkonadiBackend::slotJobCompleted(KJob* job)
{
   if (job->error()) {
      qDebug() << "An Akonadi job failed";
      return;
   }
   Akonadi::RecursiveItemFetchJob* akojob = qobject_cast<Akonadi::RecursiveItemFetchJob*>(job);
   if (akojob) {
      const bool onlyWithNumber =  ConfigurationSkeleton::hidePersonWithoutPhone();
      const Akonadi::Item::List items = akojob->items();
      foreach ( const Akonadi::Item &item, items ) {
         Person* c = addItem(item,onlyWithNumber);
         PersonModel::instance()->addPerson(c);
      }
   }
}

///Update the contact list when a new Akonadi collection is added
void AkonadiBackend::update(const Akonadi::Collection& collection)
{
   if ( !collection.isValid() ) {
      qDebug() << "The current collection is not valid";
      return;
   }

   Akonadi::RecursiveItemFetchJob *job = new Akonadi::RecursiveItemFetchJob( collection, QStringList() << KABC::Addressee::mimeType() << KABC::ContactGroup::mimeType());
   job->fetchScope().fetchFullPayload();
   connect(job, SIGNAL(result(KJob*)), this, SLOT(slotJobCompleted(KJob*)));
   job->start();
//    return m_PersonByUid.values();
} //update

// bool AkonadiBackend::remove(Person* c)
// {
//    
// }

///Edit backend value using an updated frontend contact
bool AkonadiBackend::edit(Person* contact,QWidget* parent)
{
   Akonadi::Item item = static_cast<AkonadiEditor*>(editor<Person>())->m_ItemHash[contact->uid()];
   if (!(item.hasPayload<KABC::Addressee>() && item.payload<KABC::Addressee>().uid() == contact->uid())) {
      qDebug() << "Person not found";
      return false ;
   }

   if ( item.isValid() ) {
      QPointer<Akonadi::ContactEditor> editor = new Akonadi::ContactEditor( Akonadi::ContactEditor::EditMode, parent );
      editor->loadContact(item);
      QPointer<QDialog> dlg = new QDialog(parent);
      QVBoxLayout *mainLayout = new QVBoxLayout;
      dlg->setLayout(mainLayout);
      mainLayout->addWidget(editor);
      if ( dlg->exec() == QDialog::Accepted ) {
         if ( !editor->saveContact() ) {
            delete dlg;
            qDebug() << "Unable to save new contact to storage";
            return false;
         }
      }
      delete editor;
      delete dlg   ;
      return true;
   }
   return false;
} //editPerson

///Save a contact
bool AkonadiBackend::save(const Person* contact)
{
   Akonadi::Item item = static_cast<AkonadiEditor*>(editor<Person>())->m_ItemHash[contact->uid()];
   if (!(item.hasPayload<KABC::Addressee>() && item.payload<KABC::Addressee>().uid() == contact->uid())) {
      qDebug() << "Person not found";
      return false;
   }
   KABC::Addressee payload = item.payload<KABC::Addressee>();
   payload.setNickName       ( contact->nickName()        );
   payload.setFormattedName  ( contact->formattedName()   );
   payload.setGivenName      ( contact->firstName()       );
   payload.setFamilyName     ( contact->secondName()      );
   payload.setOrganization   ( contact->organization()    );
   payload.setDepartment     ( contact->department()      );

   foreach (ContactMethod* nb, contact->phoneNumbers()) {
      KABC::PhoneNumber pn;
      pn.setType(nameToType(nb->category()->name()));

      pn.setNumber(nb->uri());
      payload.insertPhoneNumber(pn);
   }
   //TODO save the contact
   return false;
}


// bool AkonadiBackend::append(const Person* item)
// {
//    Q_UNUSED(item)
//    return false;
// }

///Add a new contact
bool AkonadiBackend::addNewPerson(Person* contact,QWidget* parent)
{
   KABC::Addressee newPerson;
   newPerson.setNickName       ( contact->nickName()        );
   newPerson.setFormattedName  ( contact->formattedName()   );
   newPerson.setGivenName      ( contact->firstName()       );
   newPerson.setFamilyName     ( contact->secondName()      );
   newPerson.setOrganization   ( contact->organization()    );
   newPerson.setDepartment     ( contact->department()      );
   //newPerson.setPreferredEmail ( contact->getPreferredEmail()  );//TODO

   foreach (ContactMethod* nb, contact->phoneNumbers()) {
      KABC::PhoneNumber pn;
      pn.setType(nameToType(nb->category()->name()));

      pn.setNumber(nb->uri());
      newPerson.insertPhoneNumber(pn);
   }

   //aPerson->setContactMethods   (newNumbers           );//TODO

   QPointer<Akonadi::ContactEditor> editor = new Akonadi::ContactEditor( Akonadi::ContactEditor::CreateMode, parent );

   editor->setContactTemplate(newPerson);

   QPointer<QDialog> dlg = new QDialog(parent);
   QVBoxLayout *mainLayout = new QVBoxLayout;
   dlg->setLayout(mainLayout);
   mainLayout->addWidget(editor);
   if ( dlg->exec() == QDialog::Accepted ) {
      if ( !editor->saveContact() ) {
         delete dlg;
         qDebug() << "Unable to save new contact to storage";
         return false;
      }
   }
   delete dlg;
   return true;
} //addNewPerson

///Implement virtual pure method
// bool AkonadiBackend::edit(Person* contact)
// {
//    return edit(contact,nullptr);
// }

///Implement virtual pure method
// bool AkonadiBackend::addNew(Person* contact)
// {
//    return addNewPerson(contact,nullptr);
// }

///Add a new phone number to an existing contact
bool AkonadiBackend::addContactMethod(Person* contact, ContactMethod* number)
{
   Akonadi::Item item = static_cast<AkonadiEditor*>(editor<Person>())->m_ItemHash[contact->uid()];
   if (!(item.hasPayload<KABC::Addressee>() && item.payload<KABC::Addressee>().uid() == contact->uid())) {
      qDebug() << "Person not found";
      return false;
   }
   if ( item.isValid() ) {
      KABC::Addressee payload = item.payload<KABC::Addressee>();
      payload.insertPhoneNumber(KABC::PhoneNumber(number->uri(),nameToType(number->category()->name())));
      item.setPayload<KABC::Addressee>(payload);
      QPointer<Akonadi::ContactEditor> editor = new Akonadi::ContactEditor( Akonadi::ContactEditor::EditMode, (QWidget*)nullptr );
      editor->loadContact(item);

      QPointer<QDialog> dlg = new QDialog(nullptr);
      QVBoxLayout *mainLayout = new QVBoxLayout;
      dlg->setLayout(mainLayout);
      mainLayout->addWidget(editor);
      if ( dlg->exec() == QDialog::Accepted ) {
         if ( !editor->saveContact() ) {
            delete dlg;
            qDebug() << "Unable to save new contact to storage";
            return false;
         }
      }
      delete dlg   ;
      delete editor;
      return true;
   }
   else {
      qDebug() << "Invalid item";
      return false;
   }
}


/*****************************************************************************
 *                                                                           *
 *                                    Slots                                  *
 *                                                                           *
 ****************************************************************************/

///Called when a new collection is added
void AkonadiBackend::itemsReceived( const Akonadi::Item::List& list)
{
//    QList<int> disabledColl = ConfigurationSkeleton::disabledCollectionList();
   foreach (const Akonadi::Item& item, list) {
//       if (disabledColl.indexOf(coll.id()) == -1) {
//          update(coll);
//          emit reloaded();
//       }
      slotItemAdded(item,m_Coll);
   }
}

///Callback when a new item is added
void AkonadiBackend::slotItemAdded(const Akonadi::Item& item,const Akonadi::Collection& coll)
{
   Q_UNUSED(coll)
   Person* c = addItem(item,ConfigurationSkeleton::hidePersonWithoutPhone());
   if (c) { //Not all items will have an addressee payload
      m_pMediator->addItem(c);
   }
}

///Callback when an item change
void AkonadiBackend::slotItemChanged(const Akonadi::Item &item, const QSet< QByteArray > &part)
{
   Q_UNUSED(part)
   if (item.hasPayload<KABC::Addressee>()) {
      KABC::Addressee tmp = item.payload<KABC::Addressee>();
      Person* c = PersonModel::instance()->getPersonByUid(tmp.uid().toUtf8());
      if (c)
         fillPerson(c,tmp);
   }
}

///Callback when a contact is removed
void AkonadiBackend::slotItemRemoved(const Akonadi::Item &item)
{
   Person* c = PersonModel::instance()->getPersonByUid(item.remoteId().toUtf8());
   PersonModel::instance()->disablePerson(c);
}

Akonadi::Collection AkonadiBackend::collection() const
{
   return m_Coll;
}

// QList<Person*> AkonadiBackend::items() const
// {
//    return m_lBackendPersons;
// }
