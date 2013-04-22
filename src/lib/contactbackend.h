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

#ifndef CONTACT_BACKEND_H
#define CONTACT_BACKEND_H

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

//Typedef
typedef QList<Contact*> ContactList;

///ContactBackend: Allow different way to handle contact without poluting the library
class LIB_EXPORT ContactBackend : public QAbstractItemModel {
   Q_OBJECT
public:
   enum Role {
      Organization      = 100,
      Group             = 101,
      Department        = 102,
      PreferredEmail    = 103,
      FormattedLastUsed = 104,
      IndexedLastUsed   = 105,
      DatedLastUsed     = 106,
      Filter            = 200, //All roles, all at once
      DropState         = 300, //State for drag and drop
   };
   
   explicit ContactBackend(QObject* parent = nullptr);
   virtual ~ContactBackend();

   ///Get a contact using a phone number
   ///@param resolveDNS interpret the number as is (false) or parse it to extract the domain and number (true)
   virtual Contact*    getContactByPhone ( const QString& phoneNumber , bool resolveDNS = false, Account* a = nullptr) = 0;

   ///Return a contact (or nullptr) according to the contact unique identifier
   virtual Contact*    getContactByUid   ( const QString& uid         ) = 0;
   ///Edit 'contact', the implementation may be a GUI or somehting else
   virtual void        editContact       ( Contact*       contact     ) = 0;
   ///Add a new contact to the backend
   virtual void        addNewContact     ( Contact*       contact     ) = 0;
   
   virtual const ContactList& getContactList() const = 0;

   ///Add a new phone number to an existing contact
   virtual void addPhoneNumber( Contact*       contact , QString  number, QString type )=0;

   //Model implementation
   virtual bool          setData     ( const QModelIndex& index, const QVariant &value, int role   );
   virtual QVariant      data        ( const QModelIndex& index, int role = Qt::DisplayRole        ) const;
   virtual int           rowCount    ( const QModelIndex& parent = QModelIndex()                   ) const;
   virtual Qt::ItemFlags flags       ( const QModelIndex& index                                    ) const;
   virtual int           columnCount ( const QModelIndex& parent = QModelIndex()                   ) const;
   virtual QModelIndex   parent      ( const QModelIndex& index                                    ) const;
   virtual QModelIndex   index       ( int row, int column, const QModelIndex& parent=QModelIndex()) const;
   virtual QVariant      headerData  ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
   
   int getUpdateCount() {return m_UpdatesCounter;}
protected:
   virtual ContactList update_slot       (                            ) = 0;

   //Helper
   QString getUserFromPhone    (QString phoneNumber);
   QString getHostNameFromPhone(QString phoneNumber);

   //Attributes
   QHash<QString,Contact*>        m_ContactByPhone ;
   QHash<QString,Contact*>        m_ContactByUid   ;
   int m_UpdatesCounter;
public Q_SLOTS:
   ContactList update();

private Q_SLOTS:
   void slotReloadModel();

Q_SIGNALS:
   void collectionChanged();
};

#endif
