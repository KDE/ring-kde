/****************************************************************************
 *   Copyright (C) 2012-2013 by Savoir-Faire Linux                          *
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
#ifndef PRESENCEMODEL_H
#define PRESENCEMODEL_H

#include <QtCore/QString>
#include <QtCore/QAbstractListModel>
#include "typedefs.h"

class Account;
class Contact;

///PresenceModel:
class LIB_EXPORT PresenceModel : public QAbstractListModel {
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop
public:
   //Roles
   enum Role {
      CONTACT = 100,
      URI     = 101,
      MESSAGE = 102,
      PRESENT = 103,
   };

   //Constructor
   explicit PresenceModel(QObject* parent = nullptr);

   //Abstract model member
   QVariant      data    (const QModelIndex& index, int role = Qt::DisplayRole     ) const;
   int           rowCount(const QModelIndex& parent = QModelIndex()                ) const;
   Qt::ItemFlags flags   (const QModelIndex& index                                 ) const;
   virtual bool  setData (const QModelIndex& index, const QVariant &value, int role)      ;

   void reset();

private:
   ///@struct TrackedContact presence internal representation
   struct TrackedContact {
      Contact* contact;
      QString  uri    ;
      Account* account;
      QString  message;
      bool     present;
   };

   //Attributes
   QList<TrackedContact*> m_lTracker;

private Q_SLOTS:
    void slotIncomingNotifications(QString uri, bool status, QString message);
    void slotRequest(QString uri);
};
Q_DECLARE_METATYPE(PresenceModel*)

#endif
