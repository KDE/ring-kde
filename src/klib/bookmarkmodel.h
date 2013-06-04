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
#include <QtCore/QStringList>

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
   //Singleton
   static BookmarkModel* getInstance();

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

private:
   //Private constructor
   explicit BookmarkModel(QObject* parent);
   virtual ~BookmarkModel() {}
      
   ///Top level bookmark item
   class TopLevelItem : public HistoryTreeBackend, public QObject {
      friend class BookmarkModel;
      public:
         virtual QObject* getSelf() {return this;}
      private:
         explicit TopLevelItem(QString name) : HistoryTreeBackend(HistoryTreeBackend::TOP_LEVEL),QObject(nullptr),m_Name(name) {}
         QList<NumberTreeBackend*> m_lChilds;
         QString m_Name;
   };

   //Attributes
   ContactBackend*              m_pModel               ;
   QList<TopLevelItem*>         m_lCategoryCounter     ;
   QHash<QString,TopLevelItem*> m_hCategories          ;
   const static char*           m_slHistoryConstStr[25];
   bool                         m_isContactDateInit    ;
   QHash<Contact*, QDateTime>   m_hContactByDate       ;
   QStringList                  m_lMimes               ;

   //Getters
   QModelIndex getContactIndex(Contact* ct) const;

   //Helpers
   QVariant commonCallInfo(NumberTreeBackend* call, int role = Qt::DisplayRole) const;
   QString category(NumberTreeBackend* number) const;

   //Singleton
   static BookmarkModel* m_pSelf;

public Q_SLOTS:
   void reloadCategories();
};

#endif
