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
#ifndef CONTACTBYNAMEPROXYMODEL_H
#define CONTACTBYNAMEPROXYMODEL_H

#include <QtGui/QSortFilterProxyModel>
#include <QtCore/QHash>

//SFLPhone
#include "../lib/typedefs.h"
#include "../lib/contact.h"
class ContactBackend;


class LIB_EXPORT ContactByNameProxyModel :  public QAbstractItemModel
{
   Q_OBJECT
public:
   friend class ContactBackend;
   ContactByNameProxyModel(ContactBackend* parent,int role = Qt::DisplayRole, bool showAll = false);
   
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
   class TopLevelItem : public ContactTreeBackend,public QObject {
   friend class ContactByNameProxyModel;
   public:
      virtual QObject* getSelf() {return this;}
   private:
      TopLevelItem(QString name) : ContactTreeBackend(ContactTreeBackend::TOP_LEVEL),QObject(nullptr),m_Name(name) {}
      int counter;
      int idx;
      QList<Contact*> m_lChilds;
      QString m_Name;
   };
   virtual ~ContactByNameProxyModel();
   ContactBackend* m_pModel;
   QList<TopLevelItem*> m_lCategoryCounter;
   QHash<QString,TopLevelItem*> m_hCategories;
   int m_Role;
   bool m_ShowAll;
   const static char* m_slHistoryConstStr[25];
   bool m_isContactDateInit;
   QHash<Contact*, QDateTime> m_hContactByDate;
   
   QModelIndex getContactIndex(Contact* ct) const;
   
   enum HistoryConst {
         Today             = 0  ,
         Yesterday         = 1  ,
         Two_days_ago      = 2  ,
         Three_days_ago    = 3  ,
         Four_days_ago     = 4  ,
         Five_days_ago     = 5  ,
         Six_days_ago      = 6  ,
         Last_week         = 7  ,
         Two_weeks_ago     = 8  ,
         Three_weeks_ago   = 9  ,
         Last_month        = 10 ,
         Two_months_ago    = 11 ,
         Three_months_ago  = 12 ,
         Four_months_ago   = 13 ,
         Five_months_ago   = 14 ,
         Six_months_ago    = 15 ,
         Seven_months_ago  = 16 ,
         Eight_months_ago  = 17 ,
         Nine_months_ago   = 18 ,
         Ten_months_ago    = 19 ,
         Eleven_months_ago = 20 ,
         Twelve_months_ago = 21 ,
         Last_year         = 22 ,
         Very_long_time_ago= 23 ,
         Never             = 24
      };
   
   //Helpers
   QString category(Contact* ct) const;
   QString timeToHistoryCategory(const QDate& date) const;
   QHash<Contact*, QDateTime> getContactListByTime() const;
   HistoryConst timeToHistoryConst(const QDate& date) const;

private Q_SLOTS:
   void reloadCategories();
};

#endif
