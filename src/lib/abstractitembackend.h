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
#ifndef ABSTRACTCONTACTBACKEND_H
#define ABSTRACTCONTACTBACKEND_H

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
class Call   ;

///AbstractItemBackendInterface: Allow different way to handle contact without poluting the library
template <class T> class LIB_EXPORT AbstractItemBackendInterface
{
public:
   enum SupportedFeatures {
      NONE     = 0x0      ,
      LOAD     = 0x1 <<  0, /* Load this backend, DO NOT load anything before "load" is called         */
      SAVE     = 0x1 <<  1, /* Save an item                                                            */
      EDIT     = 0x1 <<  2, /* Edit, but **DOT NOT**, save an item)                                    */
      PROBE    = 0x1 <<  3, /* Check if the backend has new items (some backends do this automagically)*/
      ADD      = 0x1 <<  4, /* Add (and save) a new item to the backend                                */
      SAVE_ALL = 0x1 <<  5, /* Save all items at once, this may or may not be faster than "add"        */
      CLEAR    = 0x1 <<  6, /* Clear all items from this backend                                       */
      REMOVE   = 0x1 <<  7, /* Remove a single item                                                    */
      EXPORT   = 0x1 <<  8, /* Export all items, format and output need to be defined by each backends */
      IMPORT   = 0x1 <<  9, /* Import items from an external source, details defined by each backends  */
   };

   explicit AbstractItemBackendInterface() {}
   virtual ~AbstractItemBackendInterface() {}

   virtual bool load() = 0;
   virtual bool reload() = 0;
   virtual bool clear();
   virtual bool save(const T* item) =0;
   virtual bool append(const T* item) =0;
   virtual bool batchSave(const QList<T*> contacts);
   virtual SupportedFeatures  supportedFeatures() const = 0;

   ///Edit 'item', the implementation may be a GUI or something else
   virtual bool        edit       ( T*       item     ) = 0;
   ///Add a new item to the backend
   virtual bool        addNew     ( T*       item     ) = 0;

   ///Add a new phone number to an existing item
   virtual bool addPhoneNumber( T*       item , PhoneNumber* number )=0;


};

// those classes cannot be typedefs because Qt doesn't support template QObjects

class LIB_EXPORT AbstractContactBackend : public QObject, public AbstractItemBackendInterface<Contact>
{
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop
public:
   explicit AbstractContactBackend(QObject* parent = nullptr);
   virtual ~AbstractContactBackend();

Q_SIGNALS:
   void reloaded();
   void newContactAdded(Contact* c);
};

class LIB_EXPORT AbstractHistoryBackend : public QObject, public AbstractItemBackendInterface<Call>
{
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop
public:
   explicit AbstractHistoryBackend(QObject* parent = nullptr);
   virtual ~AbstractHistoryBackend();

Q_SIGNALS:
   void reloaded();
   void newHistoryCallAdded(Call* c);
};


#endif
