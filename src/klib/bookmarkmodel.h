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
#ifndef BOOKMARKMODEL_H
#define BOOKMARKMODEL_H

#include <QtGui/QSortFilterProxyModel>
#include <QtCore/QHash>

//SFLPhone
#include "../lib/typedefs.h"
#include "../lib/contact.h"
#include "../lib/call.h"
class ContactBackend;
class NumberTreeBackend;

class LIB_EXPORT BookmarkModel :  public QAbstractItemModel
{
   Q_OBJECT
public:
   static BookmarkModel* getInstance() {
      if (!m_pSelf)
         m_pSelf = new BookmarkModel(nullptr);
      return m_pSelf;
   }

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

private:
   BookmarkModel(QObject* parent) : QAbstractItemModel(parent){
      setObjectName("BookmarkModel");
      reloadCategories();
   }
   virtual ~BookmarkModel() {}
   class TopLevelItem : public HistoryTreeBackend, public QObject {
   friend class BookmarkModel;
   public:
      virtual QObject* getSelf() {return this;}
   private:
      TopLevelItem(QString name) : HistoryTreeBackend(HistoryTreeBackend::TOP_LEVEL),QObject(nullptr),m_Name(name) {}
      QList<NumberTreeBackend*> m_lChilds;
      QString m_Name;
   };
   ContactBackend* m_pModel;
   QList<TopLevelItem*> m_lCategoryCounter;
   QHash<QString,TopLevelItem*> m_hCategories;
   const static char* m_slHistoryConstStr[25];
   bool m_isContactDateInit;
   QHash<Contact*, QDateTime> m_hContactByDate;
   
   QModelIndex getContactIndex(Contact* ct) const;
   
   //Helpers
//    QString category(Contact* ct) const;
//    QHash<Contact*, QDateTime> /*getContactListByTime*/() const;
   QVariant commonCallInfo(NumberTreeBackend* call, int role = Qt::DisplayRole) const;
   QString category(NumberTreeBackend* number) const;
   
   static BookmarkModel* m_pSelf;

private Q_SLOTS:
   void reloadCategories();
};

#endif
