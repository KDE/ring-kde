/****************************************************************************
 *   Copyright (C) 2014 by Savoir-Faire Linux                               *
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

//SFLPhone
class Contact;
class Account;
class AbstractContactBackend;

//Typedef
typedef QVector<Contact*> ContactList;

///ContactModel: Allow different way to handle contact without poluting the library
class LIB_EXPORT ContactModel : public QAbstractItemModel {
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

   //Getters
   Contact* getContactByUid   ( const QString& uid );
   bool     hasBackends       () const;
   const ContactList contacts() const;

   //Model implementation
   virtual bool          setData     ( const QModelIndex& index, const QVariant &value, int role   )  __attribute__ ((const));
   virtual QVariant      data        ( const QModelIndex& index, int role = Qt::DisplayRole        ) const;
   virtual int           rowCount    ( const QModelIndex& parent = QModelIndex()                   ) const;
   virtual Qt::ItemFlags flags       ( const QModelIndex& index                                    ) const;
   virtual int           columnCount ( const QModelIndex& parent = QModelIndex()                   ) const;
   virtual QModelIndex   parent      ( const QModelIndex& index                                    ) const;
   virtual QModelIndex   index       ( int row, int column, const QModelIndex& parent=QModelIndex()) const;
   virtual QVariant      headerData  ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

   //Singleton
   static ContactModel* instance();

protected:
//    virtual ContactList update_slot() = 0;

   //Helper
//    QString getUserFromPhone    (const QString &phoneNumber);

   //Attributes
   QHash<QString,Contact*>        m_ContactByUid   ;
   int m_UpdatesCounter;

private:
   //Attributes
   static ContactModel* m_spInstance;
   QList<AbstractContactBackend*> m_lBackends;

   //Indexes
   QHash<QByteArray,Contact*> m_hContactsByUid;
   QVector<Contact*> m_lContacts;

// public Q_SLOTS:
//    ContactList update();

// private Q_SLOTS:
//    void slotReloadModel();

Q_SIGNALS:
   void collectionChanged();
   void newContactAdded(Contact* c);
};

#endif
