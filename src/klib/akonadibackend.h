/****************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                          *
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

#include <QPointer>

#include <collectioninterface.h>
#include <collectioneditor.h>
#include <collectionmediator.h>
#include "typedefs.h"
#include <akonadi/collectionmodel.h>
#include <kabc/phonenumber.h>
#include <akonadi/item.h>

//Qt
class QObject;

//KDE
namespace KABC {
   class Addressee    ;
   class ContactMethod  ;
}

namespace Akonadi {
   class Session           ;
   class Collection        ;
   class ItemFetchJob;
   class Monitor           ;
}
class KJob;

class ContactMethod;

//Ring
class Person;
template<typename T> class CollectionMediator;

///AkonadiBackend: Implement a backend for Akonadi
class LIB_EXPORT AkonadiBackend : public QObject, public CollectionInterface
{
   Q_OBJECT
public:
   template<typename T>
   explicit AkonadiBackend(CollectionMediator<T>* mediator, const Akonadi::Collection& parentCol);
//    Person* getPersonByPhone ( const QString& phoneNumber ,bool resolveDNS = false, Account* a=nullptr);
   bool     edit       ( Person*       contact , QWidget* parent = 0                           );
   bool     addNewPerson     ( Person*       contact , QWidget* parent = 0                           );
   virtual bool addContactMethod( Person*       contact , ContactMethod* number                           );

   virtual QString name () const override;
   virtual QString category  () const override;
   virtual QVariant icon() const override;
   virtual bool isEnabled() const override;
   virtual bool enable (bool enable) override;
   virtual QByteArray  id() const;

//    virtual bool     edit   ( Person*   contact                                                 ) override;
//    virtual bool     addNew ( Person*   contact                                                 ) override;
//    virtual bool     remove ( Person* c                                                         ) override;
//    virtual bool append(const Person* item);
   virtual ~AkonadiBackend        (                                                                    );

   virtual bool load();
   virtual bool reload();
   virtual bool save(const Person* contact);

   SupportedFeatures supportedFeatures() const;

//    virtual QList<Person*> items() const override;

   Akonadi::Collection collection() const;
private:

   //Attributes
   static Akonadi::Session*              m_pSession   ;
   Akonadi::Monitor*              m_pMonitor   ;
   Akonadi::Collection            m_Coll       ;
   QPointer<Akonadi::ItemFetchJob>   m_pJob;
   bool                           m_isEnabled;
   bool                           m_wasEnabled;
   CollectionMediator<Person>*  m_pMediator;

   //Helper
   KABC::PhoneNumber::Type nameToType(const QString& name);
   Person* addItem(Akonadi::Item item, bool ignoreEmpty = false);
   void fillPerson(Person* c, const KABC::Addressee& addr) const;

   //Parent locator
   static QHash<Akonadi::Collection::Id, AkonadiBackend*> m_hParentLookup;

public Q_SLOTS:
   void update(const Akonadi::Collection& collection);
   void itemsReceived( const Akonadi::Item::List& );
private Q_SLOTS:
   void slotItemAdded(const Akonadi::Item& item, const Akonadi::Collection& coll);
   void slotItemChanged (const Akonadi::Item &item, const QSet< QByteArray > &partIdentifiers);
   void slotItemRemoved (const Akonadi::Item &item);
   void slotJobCompleted(KJob* job);
};

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
   virtual bool append     ( const Person* item ) override;
   virtual bool remove     ( Person*       item ) override;
   virtual bool edit       ( Person*       item ) override;
   virtual bool addNew     ( Person*       item ) override;

   QHash<QString,KABC::Addressee> m_AddrHash   ;
   QHash<QString,Akonadi::Item>   m_ItemHash   ;
   QList<Person*>                m_lBackendPersons;
private:
   virtual QVector<Person*> items() const override;
};
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
///Constructor
template<typename T>
AkonadiBackend::AkonadiBackend(CollectionMediator<T>* mediator, const Akonadi::Collection& parentCol) : QObject(mediator->model()),
   CollectionInterface(new AkonadiEditor(mediator),m_hParentLookup[parentCol.parent()]),m_pJob(nullptr),m_pMediator(mediator),
   m_pMonitor(nullptr),m_isEnabled(false),m_wasEnabled(false)
{
   if (!m_pSession)
      m_pSession = new Akonadi::Session( "Ring::instance" );
   setObjectName(parentCol.name());
   m_Coll = parentCol;
   m_hParentLookup[m_Coll.id()] = this;
} //AkonadiBackend

#endif
