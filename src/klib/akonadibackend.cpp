/****************************************************************************
 *   Copyright (C) 2011-2015 by Savoir-Faire Linux                          *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                    *
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
#include <QtCore/QPointer>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QPushButton>

//KDE
#include <KJob>
#include <KLocalizedString>
#include <AkonadiCore/Control>
#include <AkonadiCore/CollectionFilterProxyModel>
// #include <AkonadiCore/Kmime/MessageModel>
#include <AkonadiCore/RecursiveItemFetchJob>
#include <AkonadiCore/ItemFetchJob>
#include <AkonadiCore/ItemFetchScope>
#include <AkonadiCore/CollectionFetchJob>
#include <AkonadiCore/CollectionFetchScope>
#include <Akonadi/Contact/ContactEditor>
#include <Akonadi/Contact/ContactEditorDialog>
#include <AkonadiCore/Session>
#include <AkonadiCore/Monitor>
#include <AkonadiCore/ChangeRecorder>
#include <AkonadiCore/ItemDeleteJob>
#include <AkonadiCore/EntityDisplayAttribute>
#include <AkonadiCore/EntityTreeModel>
#include <kcontacts/addressee.h>
#include <kcontacts/addresseelist.h>
#include <kcontacts/contactgroup.h>

//Ring library
#include "person.h"
#include "accountmodel.h"
#include "account.h"
#include "call.h"
#include "callmodel.h"
#include "contactmethod.h"
#include "individualdirectory.h"
#include "numbercategorymodel.h"
#include "collectioninterface.h"
#include "numbercategory.h"
#include "persondirectory.h"
#include "interfaces/itemmodelstateserializeri.h"
#include "globalinstances.h"

// Ring-KDE
#include "kcfg_settings.h"

Akonadi::Session*          AkonadiBackend::m_spSession = nullptr;
Akonadi::EntityTreeModel*  AkonadiBackend::m_spModel   = nullptr;
QHash<Akonadi::Collection::Id, AkonadiBackend*> AkonadiBackend::m_hParentLookup;

class AkonadiEditor : public CollectionEditor<Person>
{
public:
   AkonadiEditor(CollectionMediator<Person>* m) : CollectionEditor<Person>(m) {}
   ~AkonadiEditor() {
      m_lBackendPersons.clear();
      m_ItemHash.clear();
      m_AddrHash.clear();
   }
   virtual bool save       ( const Person* item ) override;
   virtual bool addExisting( const Person* item ) override;
   virtual bool remove     ( const Person* item ) override;
   virtual bool edit       ( Person*       item ) override;
   virtual bool addNew     ( Person*       item ) override;

   QHash<QString,KContacts::Addressee> m_AddrHash       ;
   QHash<QString,Akonadi::Item>        m_ItemHash       ;
   QVector<Person*>                    m_lBackendPersons;
private:
   virtual QVector<Person*> items() const override;
};

void AkonadiBackend::digg(QAbstractItemModel* model, const QModelIndex& idx)
{
   if (!model)
      return;

   if (idx.isValid()) {
      auto col = idx.data( Akonadi::EntityTreeModel::CollectionRole ).value<Akonadi::Collection>();

      if (col.isValid()) {
         auto col2 = Session::instance()->personDirectory()->addCollection<AkonadiBackend,Akonadi::Collection*>(&col);
         if (col2 && col2->isEnabled())
            col2->load();
      }
   }

   for (int i = 0;i < model->rowCount(idx);i++) {
      QModelIndex current = model->index(i,0,idx);
      digg(model, current);
   }
}

void AkonadiBackend::slotRowsInserted(const QModelIndex& parent, int start, int end)
{
   for (int i = start; i <= end; i++) {
      digg(m_spModel, m_spModel->index(i,0, parent));
   }
}

// Watch and digg the EntityTreeModel to get the collection list
void AkonadiBackend::initCollections()
{
   if (!m_spSession)
      m_spSession = new Akonadi::Session("ring-kde");

   auto changeRecorder = new Akonadi::ChangeRecorder();

   changeRecorder->setCollectionMonitored( Akonadi::Collection::root() );
   changeRecorder->setMimeTypeMonitored( "text/directory" );
   changeRecorder->setSession( m_spSession );

   m_spModel = new Akonadi::EntityTreeModel( changeRecorder, changeRecorder );
   m_spModel->setItemPopulationStrategy( Akonadi::EntityTreeModel::NoItemPopulation );

   QObject::connect(m_spModel, &QAbstractItemModel::rowsInserted, &slotRowsInserted);

   digg(m_spModel, QModelIndex());
}

///Constructor
AkonadiBackend::AkonadiBackend(CollectionMediator<Person>* mediator, Akonadi::Collection* parentCol) : QObject(mediator->model()),
   CollectionInterface(new AkonadiEditor(mediator),m_hParentLookup[parentCol->parentCollection().id()]),m_pJob{},m_pMediator(mediator),
  m_pMonitor(nullptr),m_isEnabled(false),m_wasEnabled(false)
{
   if (!m_spSession)
      m_spSession = new Akonadi::Session( "Ring::instance" );
   setObjectName(parentCol->name());
   m_Coll = *parentCol;
   m_hParentLookup[m_Coll.id()] = this;
} //AkonadiBackend



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

bool AkonadiEditor::addExisting(const Person* item)
{
   Q_UNUSED(item)
   return false;
}

bool AkonadiEditor::remove(const Person* c)
{
   if (!c)
      return false;
   Akonadi::Item item = m_ItemHash[c->uid()];
   Akonadi::ItemDeleteJob *job = new Akonadi::ItemDeleteJob( item );
   job->exec();
   return true;
}

void applyNativeEdit()
{
   
}

bool AkonadiEditor::edit( Person* contact)
{
   Akonadi::Item item = m_ItemHash[contact->uid()];
   if (!(item.hasPayload<KContacts::Addressee>() && item.payload<KContacts::Addressee>().uid() == contact->uid())) {
      qDebug() << "Person not found";
      return false ;
   }

   if ( item.isValid() ) {
      QPointer<Akonadi::ContactEditorDialog> editor = new Akonadi::ContactEditorDialog( Akonadi::ContactEditorDialog::EditMode );
      editor->editor()->loadContact(item);

      if ( editor->exec() == QDialog::Accepted ) {
         if ( !contact->save() ) {
            delete editor;
            qDebug() << "Unable to save new contact to storage";
            return false;
         }
      }
      delete editor;
      return true;
   }
   return false;
}

bool AkonadiEditor::addNew( Person* contact)
{
   KContacts::Addressee newPerson;
   newPerson.setNickName       ( contact->nickName     () );
   newPerson.setFormattedName  ( contact->formattedName() );
   newPerson.setGivenName      ( contact->firstName    () );
   newPerson.setFamilyName     ( contact->secondName   () );
   newPerson.setOrganization   ( contact->organization () );
   newPerson.setDepartment     ( contact->department   () );
//    newPerson.setPreferredEmail ( contact->preferredEmail()  );//TODO

   foreach (ContactMethod* nb, contact->phoneNumbers()) {
      KContacts::PhoneNumber pn;
      pn.setType(AkonadiBackend::nameToType(nb->category()->name()));

      pn.setNumber(nb->uri());
      newPerson.insertPhoneNumber(pn);
   }

   //aPerson->setContactMethods   (newNumbers           );//TODO

   QPointer<Akonadi::ContactEditorDialog> editor = new Akonadi::ContactEditorDialog( Akonadi::ContactEditorDialog::CreateMode );

   editor->editor()->setContactTemplate(newPerson);

   if ( editor->exec() == QDialog::Accepted ) {
      if ( !contact->save() ) {
         delete editor;
         qDebug() << "Unable to save new contact to storage";
         return false;
      }
   }
   delete editor;
   return true;
}

QVector<Person*> AkonadiEditor::items() const
{
   return m_lBackendPersons;
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
   return i18n("Contact");
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
   try {
      return GlobalInstances::itemModelStateSerializer().isChecked(this);
   } catch (...) {
      return true;
   }
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
   m_wasEnabled = false;
   return true;
}

bool AkonadiBackend::enable (bool enable)
{
   if (enable && (!m_wasEnabled)) {
      return load();
   }
   else if (m_wasEnabled && enable) {
      foreach(Person* contact, items<Person>()) {
         activate(contact);
         emit contact->changed();
      }
      m_wasEnabled = false;
      m_isEnabled = true;
   }
   else if (m_isEnabled && !enable) {
      foreach(Person* contact, items<Person>()) {
         deactivate(contact);
         emit contact->changed();
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

FlagPack<CollectionInterface::SupportedFeatures> AkonadiBackend::supportedFeatures() const
{
   return (
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

///Convert string to akonadi KContacts::PhoneNumber
KContacts::PhoneNumber::Type AkonadiBackend::nameToType(const QString& name)
{
   if      (name == "Home"   ) return KContacts::PhoneNumber::Home ;
   else if (name == "Work"   ) return KContacts::PhoneNumber::Work ;
   else if (name == "Msg"    ) return KContacts::PhoneNumber::Msg  ;
   else if (name == "Pref"   ) return KContacts::PhoneNumber::Pref ;
   else if (name == "Voice"  ) return KContacts::PhoneNumber::Voice;
   else if (name == "Fax"    ) return KContacts::PhoneNumber::Fax  ;
   else if (name == "Cell"   ) return KContacts::PhoneNumber::Cell ;
   else if (name == "Video"  ) return KContacts::PhoneNumber::Video;
   else if (name == "Bbs"    ) return KContacts::PhoneNumber::Bbs  ;
   else if (name == "Modem"  ) return KContacts::PhoneNumber::Modem;
   else if (name == "Car"    ) return KContacts::PhoneNumber::Car  ;
   else if (name == "Isdn"   ) return KContacts::PhoneNumber::Isdn ;
   else if (name == "Pcs"    ) return KContacts::PhoneNumber::Pcs  ;
   else if (name == "Pager"  ) return KContacts::PhoneNumber::Pager;
   return KContacts::PhoneNumber::Home;
}

void AkonadiBackend::fillPerson(Person* c, const KContacts::Addressee& addr)
{
   if (!c) {
      qDebug() << "Contact not found";
      return;
   }

   c->setNickName       ( addr.nickName      () );
   c->setFormattedName  ( addr.formattedName () );
   c->setFirstName      ( addr.givenName     () );
   c->setFamilyName     ( addr.familyName    () );
   c->setOrganization   ( addr.organization  () );
   c->setPreferredEmail ( addr.preferredEmail() );
   c->setDepartment     ( addr.department    () );
   c->setUid            ( addr.uid().toUtf8  () );

   const KContacts::PhoneNumber::List numbers = addr.phoneNumbers();
   QVector<ContactMethod*> newNumbers;
   foreach (const KContacts::PhoneNumber& number, numbers) {
      ContactMethod* cm = Session::instance()->individualDirectory()->getNumber(number.number(),c,nullptr,number.typeLabel());

      newNumbers << cm;
   }

   c->setContactMethods   ( newNumbers );
}

Person* AkonadiBackend::addItem(Akonadi::Item item, bool ignoreEmpty)
{
   Q_UNUSED(ignoreEmpty)
   Person* aPerson = nullptr;
   if ( item.hasPayload<KContacts::Addressee>() ) {
      m_pMonitor->setItemMonitored(item,true);
      KContacts::Addressee tmp = item.payload<KContacts::Addressee>();
      const KContacts::PhoneNumber::List numbers = tmp.phoneNumbers();
      const QString uid = tmp.uid();
      if (numbers.size() ) {
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
         Session::instance()->personDirectory()->addPerson(c);
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

   Akonadi::RecursiveItemFetchJob *job = new Akonadi::RecursiveItemFetchJob( collection, QStringList() << KContacts::Addressee::mimeType() << KContacts::ContactGroup::mimeType());
   job->fetchScope().fetchFullPayload();
   connect(job, SIGNAL(result(KJob*)), this, SLOT(slotJobCompleted(KJob*)));
   job->start();

} //update

///Save a contact
bool AkonadiBackend::save(const Person* contact)
{
   Akonadi::Item item = static_cast<AkonadiEditor*>(editor<Person>())->m_ItemHash[contact->uid()];

   if (!(item.hasPayload<KContacts::Addressee>() && item.payload<KContacts::Addressee>().uid() == contact->uid())) {
      qDebug() << "Person not found";
      return false;
   }

   auto payload = item.payload<KContacts::Addressee> ();

   payload.setNickName       ( contact->nickName     () );
   payload.setFormattedName  ( contact->formattedName() );
   payload.setGivenName      ( contact->firstName    () );
   payload.setFamilyName     ( contact->secondName   () );
   payload.setOrganization   ( contact->organization () );
   payload.setDepartment     ( contact->department   () );

   foreach (ContactMethod* nb, contact->phoneNumbers()) {
      KContacts::PhoneNumber pn;
      pn.setType(nameToType(nb->category()->name()));

      pn.setNumber(nb->uri());
      payload.insertPhoneNumber(pn);
   }
   //TODO save the contact
   return false;
}

///Add a new phone number to an existing contact
bool AkonadiBackend::addContactMethod(Person* contact, ContactMethod* number)
{
   Akonadi::Item item = static_cast<AkonadiEditor*>(editor<Person>())->m_ItemHash[contact->uid()];
   if (!(item.hasPayload<KContacts::Addressee>() && item.payload<KContacts::Addressee>().uid() == contact->uid())) {
      qDebug() << "Person not found";
      return false;
   }
   if ( item.isValid() ) {
      KContacts::Addressee payload = item.payload<KContacts::Addressee>();
      payload.insertPhoneNumber(KContacts::PhoneNumber(number->uri(),nameToType(number->category()->name())));
      item.setPayload<KContacts::Addressee>(payload);
      QPointer<Akonadi::ContactEditorDialog> editor = new Akonadi::ContactEditorDialog( Akonadi::ContactEditorDialog::EditMode, (QWidget*)nullptr );
      editor->editor()->loadContact(item);

      if ( editor->exec() == QDialog::Accepted ) {
         if ( !contact->save() ) {
            delete editor;
            qDebug() << "Unable to save new contact to storage";
            return false;
         }
      }

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
   foreach (const Akonadi::Item& item, list) {
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
   if (item.hasPayload<KContacts::Addressee>()) {
      KContacts::Addressee tmp = item.payload<KContacts::Addressee>();
      Person* c = Session::instance()->personDirectory()->getPersonByUid(tmp.uid().toUtf8());
      if (c)
         fillPerson(c,tmp);
   }
}

///Callback when a contact is removed
void AkonadiBackend::slotItemRemoved(const Akonadi::Item &item)
{
   Person* c = Session::instance()->personDirectory()->getPersonByUid(item.remoteId().toUtf8());

   if (c)
      deactivate(c);
   else
      qDebug() << "A contact was deleted, but Ring-KDE can't find it";
   //Session::instance()->personDirectory()->disablePerson(c);
}

// kate: space-indent on; indent-width 3; replace-tabs on;
