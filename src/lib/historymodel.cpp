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
#include "historymodel.h"

//C include
#include <time.h>

//SFLPhone lib
#include "dbus/callmanager.h"
#include "dbus/configurationmanager.h"
#include "call.h"
#include "contact.h"

const QString HistoryModel::m_slHistoryConstStr[25] = {
      QString("Today")                                 ,//0
      QString("Yesterday")                             ,//1
      QDate::currentDate().addDays(-2).toString("dddd"),//2
      QDate::currentDate().addDays(-3).toString("dddd"),//3
      QDate::currentDate().addDays(-4).toString("dddd"),//4
      QDate::currentDate().addDays(-5).toString("dddd"),//5
      QDate::currentDate().addDays(-6).toString("dddd"),//6
      QString("Last week")                             ,//7
      QString("Two weeks ago")                         ,//8
      QString("Three weeks ago")                       ,//9
      QString("Last month")                            ,//10
      QString("Two months ago")                        ,//11
      QString("Three months ago")                      ,//12
      QString("Four months ago")                       ,//13
      QString("Five months ago")                       ,//14
      QString("Six months ago")                        ,//15
      QString("Seven months ago")                      ,//16
      QString("Eight months ago")                      ,//17
      QString("Nine months ago")                       ,//18
      QString("Ten months ago")                        ,//19
      QString("Eleven months ago")                     ,//20
      QString("Twelve months ago")                     ,//21
      QString("Last year")                             ,//22
      QString("Very long time ago")                    ,//23
      QString("Never")                                  //24
};


/*****************************************************************************
 *                                                                           *
 *                             Private classes                               *
 *                                                                           *
 ****************************************************************************/

///SortableCallSource: helper class to make sorting possible
class SortableCallSource {
public:
   SortableCallSource(Call* call=0) : count(0),callInfo(call) {}
   uint count;
   Call* callInfo;
   bool operator<(SortableCallSource other) {
      return (other.count > count);
   }
};

inline bool operator< (const SortableCallSource & s1, const SortableCallSource & s2)
{
    return  s1.count < s2.count;
}

HistoryModel* HistoryModel::m_spInstance    = nullptr;
CallMap       HistoryModel::m_sHistoryCalls          ;

HistoryModel::TopLevelItem::TopLevelItem(int name) : 
   HistoryTreeBackend(HistoryTreeBackend::TOP_LEVEL),QObject(nullptr),m_Name(name),m_NameStr(m_slHistoryConstStr[name])
{}


/*****************************************************************************
 *                                                                           *
 *                                 Constructor                               *
 *                                                                           *
 ****************************************************************************/

///Constructor
HistoryModel::HistoryModel():QAbstractItemModel(QCoreApplication::instance()),m_HistoryInit(false),m_Role(Call::Role::FuzzyDate),m_HaveContactModel(false)
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   const QVector< QMap<QString, QString> > history = configurationManager.getHistory();
   foreach (const MapStringString& hc, history) {
      Call* pastCall = Call::buildHistoryCall(
               hc[ CALLID_KEY          ]         ,
               hc[ TIMESTAMP_START_KEY ].toUInt(),
               hc[ TIMESTAMP_STOP_KEY  ].toUInt(),
               hc[ ACCOUNT_ID_KEY      ]         ,
               hc[ DISPLAY_NAME_KEY    ]         ,
               hc[ PEER_NUMBER_KEY     ]         ,
               hc[ STATE_KEY           ]
      );
      if (pastCall->peerName().isEmpty()) {
         pastCall->setPeerName("Unknown");
      }
      pastCall->setRecordingPath(hc[ RECORDING_PATH_KEY ]);
      add(pastCall);
   }
   m_HistoryInit = true;
   m_spInstance = this;
   reloadCategories();
   m_lMimes << MIME_PLAIN_TEXT << MIME_PHONENUMBER << MIME_HISTORYID;
} //initHistory

///Destructor
HistoryModel::~HistoryModel()
{
   m_spInstance = nullptr;
}

///Singleton
HistoryModel* HistoryModel::instance()
{
   if (!m_spInstance)
      m_spInstance = new HistoryModel();
   return m_spInstance;
}


/*****************************************************************************
 *                                                                           *
 *                           History related code                            *
 *                                                                           *
 ****************************************************************************/

///Add to history
void HistoryModel::add(Call* call)
{
   if (!call || call->state() != Call::State::OVER)
      return;

   m_sHistoryCalls[call->startTimeStamp()] = call;
   if (!m_HaveContactModel && call->contactBackend()) {
      connect(((QObject*)call->contactBackend()),SIGNAL(collectionChanged()),this,SLOT(reloadCategories()));
      m_HaveContactModel = true;
   }

   emit newHistoryCall(call);
   const int cat = call->roleData(Call::Role::FuzzyDate).toInt();
   if (!m_hCategories[cat]) { 
      TopLevelItem* item = new TopLevelItem(cat);
      m_hCategories[cat] = item;
      m_lCategoryCounter << item;
      emit dataChanged(index(rowCount()-1,0),index(rowCount()-1,0));
   }
   m_hCategories[cat]->m_lChilds << call;
   emit historyChanged();
//    emit layoutChanged();
}

///Return the history list
const CallMap& HistoryModel::getHistory()
{
   instance();
   return m_sHistoryCalls;
}

///Return a list of all previous calls
const QStringList HistoryModel::getHistoryCallId()
{
   instance();
   QStringList toReturn;
   foreach(Call* call, m_sHistoryCalls) {
      toReturn << call->callId();
   }
   return toReturn;
}

///Sort all history call by popularity and return the result (most popular first)
const QStringList HistoryModel::getNumbersByPopularity()
{
   instance();
   QHash<QString,SortableCallSource*> hc;
   foreach (Call* call, getHistory()) {
      if (!hc[call->peerPhoneNumber()]) {
         hc[call->peerPhoneNumber()] = new SortableCallSource(call);
      }
      hc[call->peerPhoneNumber()]->count++;
   }
   QList<SortableCallSource> userList;
   foreach (SortableCallSource* i,hc) {
      userList << *i;
   }
   qSort(userList);
   QStringList cl;
   for (int i=userList.size()-1;i >=0 ;i--) {
      cl << userList[i].callInfo->peerPhoneNumber();
   }
   foreach (SortableCallSource* i,hc) {
      delete i;
   }

   return cl;
} //getNumbersByPopularity


/*****************************************************************************
 *                                                                           *
 *                              Model related                                *
 *                                                                           *
 ****************************************************************************/

void HistoryModel::reloadCategories()
{
   if (!m_HistoryInit)
      return;
   beginResetModel();
   m_hCategories.clear();
   foreach(TopLevelItem* item, m_lCategoryCounter) {
      delete item;
   }
   m_lCategoryCounter.clear();
   m_isContactDateInit = false;
   foreach(Call* call, getHistory()) {
      const int val = call->roleData(Call::Role::FuzzyDate).toInt();
      if (!m_hCategories[val]) {
         TopLevelItem* item = new TopLevelItem(val);
         m_hCategories[val] = item;
         m_lCategoryCounter << item;
      }
      TopLevelItem* item = m_hCategories[val];
      if (item) {
         item->m_lChilds << call;
      }
      else
         qDebug() << "ERROR count";
   }
   endResetModel();
   emit layoutChanged();
   emit dataChanged(index(0,0),index(rowCount()-1,0));
}

bool HistoryModel::setData( const QModelIndex& idx, const QVariant &value, int role)
{
   if (idx.isValid() && idx.parent().isValid()) {
      HistoryTreeBackend* modelItem = (HistoryTreeBackend*)idx.internalPointer();
      if (role == Call::Role::DropState) {
         modelItem->setDropState(value.toInt());
         emit dataChanged(idx, idx);
      }
   }
   return false;
}

QVariant HistoryModel::data( const QModelIndex& idx, int role) const
{
   if (!idx.isValid())
      return QVariant();

   HistoryTreeBackend* modelItem = static_cast<HistoryTreeBackend*>(idx.internalPointer());
   switch (modelItem->type3()) {
      case HistoryTreeBackend::Type::TOP_LEVEL:
      switch (role) {
         case Qt::DisplayRole:
            return ((TopLevelItem*)modelItem)->m_NameStr;
         case Call::Role::FuzzyDate:
         case Call::Role::Date:
            return 999 - ((TopLevelItem*)modelItem)->m_Name;
         default:
            break;
      }
      break;
   case HistoryTreeBackend::Type::CALL:
      if (role == Call::Role::DropState)
         return QVariant(modelItem->dropState());
      else if (m_lCategoryCounter.size() >= idx.parent().row() 
         && m_lCategoryCounter[idx.parent().row()] 
         && m_lCategoryCounter[idx.parent().row()]->m_lChilds.size() >= idx.row())
         return m_lCategoryCounter[idx.parent().row()]->m_lChilds[idx.row()]->roleData((Call::Role)role);
      break;
   case HistoryTreeBackend::Type::NUMBER:
   case HistoryTreeBackend::Type::BOOKMARK:
   default:
      break;
   };
   return QVariant();
}

QVariant HistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   Q_UNUSED(section)
   if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
      return QVariant("History");
   if (role == Qt::InitialSortOrderRole)
      return QVariant(Qt::DescendingOrder);
   return QVariant();
}

int HistoryModel::rowCount( const QModelIndex& parentIdx ) const
{
   if (!parentIdx.isValid() || !parentIdx.internalPointer()) {
      return m_lCategoryCounter.size();
   }
   else if (!parentIdx.parent().isValid()) {
      return m_lCategoryCounter[parentIdx.row()]->m_lChilds.size();
   }
//    else if (parent.parent().isValid() && !parent.parent().parent().isValid()) {
//       return m_lCategoryCounter[parent.parent().row()]->m_lChilds[parent.row()]->getPhoneNumbers().size();
//    }
   return 0;
}

Qt::ItemFlags HistoryModel::flags( const QModelIndex& idx ) const
{
   if (!idx.isValid())
      return 0;
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable | (idx.parent().isValid()?Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled:Qt::ItemIsEnabled);
}

int HistoryModel::columnCount ( const QModelIndex& parentIdx) const
{
   Q_UNUSED(parentIdx)
   return 1;
}

QModelIndex HistoryModel::parent( const QModelIndex& idx) const
{
   if (!idx.isValid() || !idx.internalPointer()) {
      return QModelIndex();
   }
   HistoryTreeBackend* modelItem = static_cast<HistoryTreeBackend*>(idx.internalPointer());
   if (modelItem && (long long)modelItem > 100 && modelItem->type3() == HistoryTreeBackend::Type::CALL) {
      const Call* call = (Call*)((HistoryTreeBackend*)(idx.internalPointer()))->getSelf();
      const int val = call->roleData(Call::Role::FuzzyDate).toInt();
      if (m_hCategories[val])
         return HistoryModel::index(m_lCategoryCounter.indexOf(m_hCategories[val]),0);
   }
//    else if (modelItem && modelItem->type3() == HistoryTreeBackend::Type::NUMBER) {
//       Contact* ct = (Contact*)modelItem->getSelf();
//       QString val = category(ct);
//       if (m_hCategories[val]) {
//          return HistoryModel::index(
//             (m_hCategories[val]->m_lChilds.indexOf(ct)),
//             0,
//             HistoryModel::index(m_lCategoryCounter.indexOf(m_hCategories[val]),0));
//       }
//    }
   else if (modelItem && modelItem->type3() == HistoryTreeBackend::Type::TOP_LEVEL) {
      return QModelIndex();
   }
   return QModelIndex();
}

QModelIndex HistoryModel::index( int row, int column, const QModelIndex& parentIdx) const
{
   if (!parentIdx.isValid()) {
      return createIndex(row,column,m_lCategoryCounter[row]);
   }
   else if (!parentIdx.parent().isValid() && column < m_lCategoryCounter[parentIdx.row()]->m_lChilds.size() ) {
      return createIndex(row,column,(void*)dynamic_cast<HistoryTreeBackend*>(m_lCategoryCounter[parentIdx.row()]->m_lChilds[row]));
   }
//    else if (parent.parent().isValid()) {
//       return createIndex(row,column,(void*)&m_lCategoryCounter[parent.parent().row()]->m_lChilds[parent.row()]->getPhoneNumbers());
//    }
   return QModelIndex();
}

QStringList HistoryModel::mimeTypes() const
{
   return m_lMimes;
}

QMimeData* HistoryModel::mimeData(const QModelIndexList &indexes) const
{
   QMimeData *mimeData2 = new QMimeData();
   foreach (const QModelIndex &idx, indexes) {
      if (idx.isValid()) {
         QString text = data(idx, Call::Role::Number).toString();
         mimeData2->setData(MIME_PLAIN_TEXT , text.toUtf8());
         mimeData2->setData(MIME_PHONENUMBER, text.toUtf8());
         mimeData2->setData(MIME_HISTORYID  , ((Call*)idx.internalPointer())->callId().toUtf8());
         return mimeData2;
      }
   }
   return mimeData2;
}


bool HistoryModel::dropMimeData(const QMimeData *mime, Qt::DropAction action, int row, int column, const QModelIndex &parentIdx)
{
   Q_UNUSED(row)
   Q_UNUSED(column)
//    QModelIndex idx = index(row,column,parent);
   qDebug() << "DROPPED" << action << parentIdx.data(Qt::DisplayRole) << parentIdx.isValid() << parentIdx.data(Qt::DisplayRole);
   setData(parentIdx,-1,Call::Role::DropState);
   QByteArray encodedCallId      = mime->data( MIME_CALLID      );
   QByteArray encodedPhoneNumber = mime->data( MIME_PHONENUMBER );
   QByteArray encodedContact     = mime->data( MIME_CONTACT     );

//    if (data->hasFormat( MIME_CALLID) && !QString(encodedCallId).isEmpty()) {
//       qDebug() << "CallId dropped"<< QString(encodedCallId);
//       Call* call = SFLPhone::model()->getCall(data->data(MIME_CALLID));
//       if (dynamic_cast<Call*>(call)) {
//          call->changeCurrentState(CALL_STATE_TRANSFERRED);
//          SFLPhone::model()->transfer(call, m_pItemCall->getPeerPhoneNumber());
//       }
//    }
//    else if (!QString(encodedPhoneNumber).isEmpty()) {
//       qDebug() << "PhoneNumber dropped"<< QString(encodedPhoneNumber);
//       phoneNumberToCall(parent, index, data, action);
//    }
//    else if (!QString(encodedContact).isEmpty()) {
//       qDebug() << "Contact dropped"<< QString(encodedContact);
//       contactToCall(parent, index, data, action);
//    }
   return false;
}

// QString HistoryModel::category(const Call* call) const
// {
//    int cat = call->roleData((Call::Role)m_Role).toInt();
// //    if (cat.size() && !m_ShowAll)
// //       cat = cat[0].toUpper();
//    return m_slHistoryConstStr[cat];
// }

HistoryTreeBackend::HistoryTreeBackend(HistoryTreeBackend::Type _type) : m_Type3(_type),m_DropState(0)
{
   
}

HistoryTreeBackend::Type HistoryTreeBackend::type3() const
{
   return m_Type3;
}

QString HistoryModel::timeToHistoryCategory(const time_t time)
{
   int period = (int)HistoryModel::timeToHistoryConst(time);
   if (period >= 0 && period <= 24)
      return m_slHistoryConstStr[period];
   else
      return m_slHistoryConstStr[24];
}

HistoryModel::HistoryConst HistoryModel::timeToHistoryConst(const time_t time)
{
   time_t currentTime;
   ::time(&currentTime);
   if (currentTime - time <= 3600*24) //The future case would be a bug, but it have to be handled anyway or it will appear in "very long time ago"
      return HistoryConst::Today;

   //Check for last week
   if (currentTime-(6)*3600*24 < time) {
      for (int i=1;i<7;i++) {
         if (currentTime-(i)*3600*24 < time)
            return (HistoryModel::HistoryConst)i; //Yesterday to Six_days_ago
      }
   }
   //Check for last month
   if (currentTime - ((3)*7*24*3600) < time) {
      for (int i=1;i<4;i++) {
         if (currentTime - ((i)*7*24*3600) < time)
            return (HistoryModel::HistoryConst)(i+((int)HistoryModel::HistoryConst::Last_week)-1); //Last_week to Three_weeks_ago
      }
   }
   //Check for last year
   if (currentTime-(12)*30.4f*24*3600 < time) {
      for (int i=1;i<12;i++) {
         if (currentTime-(i+1)*30.4f*24*3600 < time) //Not exact, but faster
            return (HistoryModel::HistoryConst)(i+((int)HistoryModel::HistoryConst::Last_month)-1); //Last_month to Twelve_months ago
      }
   }
   //if (QDate::currentDate().addYears(-1)  >= date && QDate::currentDate().addYears(-2)  < date)
   if (currentTime-365*24*3600 < time)
      return HistoryConst::Last_year;

   //Every other senario
   return HistoryModel::HistoryConst::Very_long_time_ago;
}

void HistoryModel::setCategoryRole(Call::Role role) 
{
   if (m_Role != role) {
      m_Role = role;reloadCategories();
   }
}
