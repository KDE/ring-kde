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
#ifndef HISTORY_MODEL_H
#define HISTORY_MODEL_H
//Base
#include "typedefs.h"
#include <QtCore/QObject>
#include <QtCore/QAbstractItemModel>

//Qt

//SFLPhone
#include "call.h"

//Typedef
typedef QMap<QString, Call*>  CallMap;
typedef QList<Call*>          CallList;

///HistoryModel: History call manager
class LIB_EXPORT HistoryModel : public QAbstractItemModel {
   Q_OBJECT
public:
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

   enum Role {
      Name          = 100,
      Number        = 101,
      Direction     = 102,
      Date          = 103,
      Length        = 104,
      FormattedDate = 105,
      HasRecording  = 106,
      HistoryState  = 107,
      Filter        = 108,
      FuzzyDate     = 109,
      IsBookmark    = 110
   };
   
   //Singleton
   static HistoryModel* self();

   //Getters
   static const CallMap&    getHistory             ();
   static const QStringList getHistoryCallId       ();
   static const QStringList getNumbersByPopularity ();

   //Setters
   static void add(Call* call);
   void setCategoryRole(HistoryModel::Role role) {if (m_Role != role) { m_Role = role;reloadCategories();}}

   //Model implementation
   virtual bool          setData     ( const QModelIndex& index, const QVariant &value, int role   );
   virtual QVariant      data        ( const QModelIndex& index, int role = Qt::DisplayRole        ) const;
   virtual int           rowCount    ( const QModelIndex& parent = QModelIndex()                   ) const;
   virtual Qt::ItemFlags flags       ( const QModelIndex& index                                    ) const;
   virtual int           columnCount ( const QModelIndex& parent = QModelIndex()                   ) const;
   virtual QModelIndex   parent      ( const QModelIndex& index                                    ) const;
   virtual QModelIndex   index       ( int row, int column, const QModelIndex& parent=QModelIndex()) const;
   virtual QVariant      headerData  ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
   
   static HistoryConst timeToHistoryConst(const QDate& date);
   static QString timeToHistoryCategory(const QDate& date);

private:

   //Constructor
   HistoryModel();
   ~HistoryModel();

   bool initHistory ();

   //Mutator
   void addPriv(Call* call);

   //Helpers
   QString category(Call* call) const;
   QVariant commonCallInfo(Call* call, int role) const;

   //Static attributes
   static HistoryModel* m_spInstance;

   //Attributes
   static CallMap m_sHistoryCalls;
   bool m_HistoryInit;

   //Model
   class TopLevelItem : public HistoryTreeBackend,public QObject {
   friend class HistoryModel;
   public:
      virtual QObject* getSelf() {return this;}
   private:
      TopLevelItem(QString name) : HistoryTreeBackend(HistoryTreeBackend::TOP_LEVEL),QObject(nullptr),m_Name(name) {}
      CallList m_lChilds;
      QString m_Name;
   };

   //Model categories
   QList<TopLevelItem*>         m_lCategoryCounter ;
   QHash<QString,TopLevelItem*> m_hCategories      ;
   bool                         m_isContactDateInit;
   int                          m_Role             ;
   bool                         m_ShowAll          ;
   bool                         m_HaveContactModel ;

   static const char* m_slHistoryConstStr[25];

private Q_SLOTS:
   void reloadCategories();

Q_SIGNALS:
   ///Emitted when the history change (new items, cleared)
   void historyChanged          (            );
   ///Emitted when a new item is added to prevent full reload
   void newHistoryCall          ( Call* call );
};

#endif
