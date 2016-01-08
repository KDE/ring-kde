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

#include <QtCore/QPointer>

#include <collectioninterface.h>
#include <collectioneditor.h>
#include <collectionmediator.h>
#include "typedefs.h"
#include <kcontacts/phonenumber.h>
#include <AkonadiCore/Item>

//Qt
class QObject;

//KDE
namespace KContacts {
   class Addressee    ;
   class ContactMethod  ;
}

namespace Akonadi {
   class Session        ;
   class Collection     ;
   class ItemFetchJob   ;
   class Monitor        ;
   class EntityTreeModel;
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
   // Constructor
   explicit AkonadiBackend(CollectionMediator<Person>* mediator, Akonadi::Collection* parentCol);
   virtual ~AkonadiBackend(                                                                    );

   // Mutator
   bool addContactMethod( Person*       contact , ContactMethod* number                         );

   // CollectionInterface override
   virtual QString    name      (                       ) const override;
   virtual QString    category  (                       ) const override;
   virtual QVariant   icon      (                       ) const override;
   virtual bool       isEnabled (                       ) const override;
   virtual bool       enable    ( bool enable           )       override;
   virtual QByteArray id        (                       ) const override;
   virtual bool       load      (                       )       override;
   virtual bool       reload    (                       )       override;
   virtual bool       save      ( const Person* contact );
   virtual FlagPack<SupportedFeatures> supportedFeatures() const override;

   //Helper
   static KContacts::PhoneNumber::Type nameToType(const QString& name);
   static void initCollections();

private:

   //Attributes
   static Akonadi::Session*          m_spSession  ;
   static Akonadi::EntityTreeModel*  m_spModel    ;

   Akonadi::Monitor*                 m_pMonitor   ;
   Akonadi::Collection               m_Coll       ;
   QPointer<Akonadi::ItemFetchJob>   m_pJob       ;
   bool                              m_isEnabled  ;
   bool                              m_wasEnabled ;
   CollectionMediator<Person>*       m_pMediator  ;

   //Helper
   Person* addItem             (Akonadi::Item item       , bool  ignoreEmpty = false       );
   static void fillPerson      (Person* c                , const KContacts::Addressee& addr);
   static void digg            (QAbstractItemModel* model, const QModelIndex& idx          );
   static void slotRowsInserted(const QModelIndex& parent, int   start            , int end);

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

#endif
