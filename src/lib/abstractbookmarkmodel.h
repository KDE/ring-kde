/****************************************************************************
 *   Copyright (C) 2013 by Savoir-Faire Linux                               *
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
#ifndef ABSTRACTBOOKMARKMODEL_H
#define ABSTRACTBOOKMARKMODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QHash>
#include <QtCore/QStringList>
#include <QtCore/QDateTime>

//SFLPhone
#include "../lib/typedefs.h"
#include "../lib/contact.h"
#include "../lib/call.h"
class ContactBackend;
class NumberTreeBackend;

class LIB_EXPORT AbstractBookmarkModel :  public QAbstractItemModel
{
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop
public:
   friend class NumberTreeBackend;
   //Constructor
   virtual ~AbstractBookmarkModel() {}
   explicit AbstractBookmarkModel(QObject* parent);

   //Setters
   void setRole(int role);
   void setShowAll(bool showAll);

   //Model implementation
   virtual bool          setData     ( const QModelIndex& index, const QVariant &value, int role   );
   virtual QVariant      data        ( const QModelIndex& index, int role = Qt::DisplayRole        ) const;
   virtual int           rowCount    ( const QModelIndex& parent = QModelIndex()                   ) const;
   virtual Qt::ItemFlags flags       ( const QModelIndex& index                                    ) const;
   virtual int           columnCount ( const QModelIndex& parent = QModelIndex()                   ) const;
   virtual QModelIndex   parent      ( const QModelIndex& index                                    ) const;
   virtual QModelIndex   index       ( int row, int column, const QModelIndex& parent=QModelIndex()) const;
   virtual QVariant      headerData  ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
   virtual QStringList   mimeTypes   (                                                             ) const;
   virtual QMimeData*    mimeData    ( const QModelIndexList &indexes                              ) const;
   virtual bool          removeRows  ( int row, int count, const QModelIndex & parent = QModelIndex() );

   //Management
   void         remove(const QModelIndex& idx                         );
   virtual void addBookmark   (PhoneNumber* number, bool trackPresence = false) = 0;
   virtual void removeBookmark(PhoneNumber* number                            ) = 0;

   //Presence
   void reloadPresence();

   //Getters
   int acceptedPayloadTypes();

protected:
   virtual bool                  displayFrequentlyUsed() const;
   virtual QVector<PhoneNumber*> bookmarkList         () const;

   ///@struct Subscription presence internal representation
   struct Subscription {
      PhoneNumber*       number ;
      Account*           account;
      QString            message;
      bool               present;
      NumberTreeBackend* view   ;
      class Status {
      public:
         constexpr static const char* ONLINE  = "Online" ;
         constexpr static const char* OFFLINE = "Offline";
      };
   };

   //Attributes
   QList<Subscription*> m_lTracker;

   //Helpers
   static QVector<PhoneNumber*> serialisedToList(const QStringList& list);

private:
   //Private constructor

   ///Top level bookmark item
   class TopLevelItem : public CategorizedCompositeNode {
      friend class AbstractBookmarkModel;
      public:
         virtual QObject* getSelf() const {return nullptr;}
      private:
         explicit TopLevelItem(QString name);
         QList<NumberTreeBackend*> m_lChildren;
         QString m_Name;
         int m_Row;
   };

   //Attributes
   ContactBackend*              m_pModel               ;
   QList<TopLevelItem*>         m_lCategoryCounter     ;
   QHash<QString,TopLevelItem*> m_hCategories          ;
   const static char*           m_slHistoryConstStr[25];
   QHash<Contact*, QDateTime>   m_hContactByDate       ;
   QStringList                  m_lMimes               ;

   //Getters
   QModelIndex getContactIndex(Contact* ct) const;

   //Helpers
   QVariant commonCallInfo(NumberTreeBackend* call, int role = Qt::DisplayRole) const;
   QString category(NumberTreeBackend* number) const;

private Q_SLOTS:
//    void slotIncomingNotifications(const QString& uri, bool status, const QString& message);
   void slotRequest(const QString& uri);

public Q_SLOTS:
   void reloadCategories();
};

#endif
