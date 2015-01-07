/****************************************************************************
 *   Copyright (C) 2014-2015 by Savoir-Faire Linux                          *
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
#ifndef CONTACTMODEL_H
#define CONTACTMODEL_H

#include <QObject>
#include <QHash>
#include <QStringList>
#include <QVariant>
#include <QtCore/QAbstractItemModel>

#include "typedefs.h"
#include "contact.h"
#include "commonbackendmanagerinterface.h"

//SFLPhone
class Contact;
class Account;
class AbstractContactBackend;
class ContactModelPrivate;

//Typedef
typedef QVector<Contact*> ContactList;

///ContactModel: Allow different way to handle contact without poluting the library
class LIB_EXPORT ContactModel :
   public QAbstractItemModel, public CommonBackendManagerInterface<AbstractContactBackend> {
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop
public:
   enum Role {
      Organization      = 100,
      Group             = 101,
      Department        = 102,
      PreferredEmail    = 103,
      FormattedLastUsed = 104,
      IndexedLastUsed   = 105,
      DatedLastUsed     = 106,
      Active            = 107,
      Filter            = 200, //All roles, all at once
      DropState         = 300, //State for drag and drop
   };

   //Properties
   Q_PROPERTY(bool hasBackends   READ hasBackends  )

   explicit ContactModel(QObject* parent = nullptr);
   virtual ~ContactModel();

   //Mutator
   bool addContact(Contact* c);
   void disableContact(Contact* c);
   void addBackend(AbstractContactBackend* backend, LoadOptions = LoadOptions::NONE);

   //Getters
   Contact* getContactByUid   ( const QByteArray& uid );
   Contact* getPlaceHolder(const QByteArray& uid );
   bool     hasBackends       () const;
   const ContactList contacts() const;
   virtual const QVector<AbstractContactBackend*> enabledBackends() const override;
   virtual bool hasEnabledBackends  () const override;
   virtual const QVector<AbstractContactBackend*> backends() const override;
   virtual bool enableBackend(AbstractContactBackend* backend, bool enabled) override;
   virtual CommonItemBackendModel* backendModel() const override;

   //Model implementation
   virtual bool          setData     ( const QModelIndex& index, const QVariant &value, int role   ) override;
   virtual QVariant      data        ( const QModelIndex& index, int role = Qt::DisplayRole        ) const override;
   virtual int           rowCount    ( const QModelIndex& parent = QModelIndex()                   ) const override;
   virtual Qt::ItemFlags flags       ( const QModelIndex& index                                    ) const override;
   virtual int           columnCount ( const QModelIndex& parent = QModelIndex()                   ) const override;
   virtual QModelIndex   parent      ( const QModelIndex& index                                    ) const override;
   virtual QModelIndex   index       ( int row, int column, const QModelIndex& parent=QModelIndex()) const override;
   virtual QVariant      headerData  ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;

   //Singleton
   static ContactModel* instance();

private:
   QScopedPointer<ContactModelPrivate> d_ptr;
   Q_DECLARE_PRIVATE(ContactModel)

   //Singleton
   static ContactModel* m_spInstance;

public Q_SLOTS:
   bool addNewContact(Contact* c, AbstractContactBackend* backend = nullptr);

Q_SIGNALS:
   void reloaded();
   void newContactAdded(Contact* c);
   void newBackendAdded(AbstractContactBackend* backend);
};


#endif
