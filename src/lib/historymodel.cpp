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
#include "callmanager_interface_singleton.h"
#include "configurationmanager_interface_singleton.h"
#include "call.h"
#include "contact.h"

const char* HistoryModel::m_slHistoryConstStr[25] = {
      "Today"                                                    ,//0
      "Yesterday"                                                ,//1
      QDate::currentDate().addDays(-2).toString("dddd").toAscii().constData(),//2
      QDate::currentDate().addDays(-3).toString("dddd").toAscii().constData(),//3
      QDate::currentDate().addDays(-4).toString("dddd").toAscii().constData(),//4
      QDate::currentDate().addDays(-5).toString("dddd").toAscii().constData(),//5
      QDate::currentDate().addDays(-6).toString("dddd").toAscii().constData(),//6
      "Last week"                                                ,//7
      "Two weeks ago"                                            ,//8
      "Three weeks ago"                                          ,//9
      "Last month"                                               ,//10
      "Two months ago"                                           ,//11
      "Three months ago"                                         ,//12
      "Four months ago"                                          ,//13
      "Five months ago"                                          ,//14
      "Six months ago"                                           ,//15
      "Seven months ago"                                         ,//16
      "Eight months ago"                                         ,//17
      "Nine months ago"                                          ,//18
      "Ten months ago"                                           ,//19
      "Eleven months ago"                                        ,//20
      "Twelve months ago"                                        ,//21
      "Last year"                                                ,//22
      "Very long time ago"                                       ,//23
      "Never"                                                     //24
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


/*****************************************************************************
 *                                                                           *
 *                                 Constructor                               *
 *                                                                           *
 ****************************************************************************/

///Constructor
HistoryModel::HistoryModel():QAbstractItemModel(nullptr),m_HistoryInit(false),m_Role(Call::Role::FuzzyDate),m_HaveContactModel(false)
{
   ConfigurationManagerInterface& configurationManager = ConfigurationManagerInterfaceSingleton::getInstance();
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
      if (pastCall->getPeerName().isEmpty()) {
         pastCall->setPeerName("Unknown");
      }
      pastCall->setRecordingPath(hc[ RECORDING_PATH_KEY ]);
      addPriv(pastCall);
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
HistoryModel* HistoryModel::self()
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
   self()->addPriv(call);
}

///Add to history
void HistoryModel::addPriv(Call* call)
{
   if (call) {
      m_sHistoryCalls[call->getStartTimeStamp()] = call;
   }
   if (!m_HaveContactModel && call->getContactBackend()) {
      connect(((QObject*)call->getContactBackend()),SIGNAL(collectionChanged()),this,SLOT(reloadCategories()));
      m_HaveContactModel = true;
   }
   emit newHistoryCall(call);
   QString cat = category(call);
   if (!m_hCategories[cat]) { 
      TopLevelItem* item = new TopLevelItem(cat);
      m_hCategories[cat] = item;
      m_lCategoryCounter << item;
      emit dataChanged(index(rowCount()-1,0),index(rowCount()-1,0));
   }
   m_hCategories[cat]->m_lChilds << call;
   emit historyChanged();
}

///Return the history list
const CallMap& HistoryModel::getHistory()
{
   self();
   return m_sHistoryCalls;
}

///Return a list of all previous calls
const QStringList HistoryModel::getHistoryCallId()
{
   self();
   QStringList toReturn;
   foreach(Call* call, m_sHistoryCalls) {
      toReturn << call->getCallId();
   }
   return toReturn;
}

///Sort all history call by popularity and return the result (most popular first)
const QStringList HistoryModel::getNumbersByPopularity()
{
   self();
   QHash<QString,SortableCallSource*> hc;
   foreach (Call* call, getHistory()) {
      if (!hc[call->getPeerPhoneNumber()]) {
         hc[call->getPeerPhoneNumber()] = new SortableCallSource(call);
      }
      hc[call->getPeerPhoneNumber()]->count++;
   }
   QList<SortableCallSource> userList;
   foreach (SortableCallSource* i,hc) {
      userList << *i;
   }
   qSort(userList);
   QStringList cl;
   for (int i=userList.size()-1;i >=0 ;i--) {
      cl << userList[i].callInfo->getPeerPhoneNumber();
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
      QString val = category(call);
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

bool HistoryModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   if (index.isValid() && index.parent().isValid()) {
      HistoryTreeBackend* modelItem = (HistoryTreeBackend*)index.internalPointer();
      if (role == Call::Role::DropState) {
         modelItem->setDropState(value.toInt());
         emit dataChanged(index, index);
      }
      else if (role == Call::Role::DropString) {
         modelItem->setDropString(value.toString());
         emit dataChanged(index, index);
      }
   }
   return false;
}

QVariant HistoryModel::data( const QModelIndex& index, int role) const
{
   if (!index.isValid())
      return QVariant();
   
   HistoryTreeBackend* modelItem = (HistoryTreeBackend*)index.internalPointer();
   switch (modelItem->type3()) {
      case HistoryTreeBackend::Type::TOP_LEVEL:
      switch (role) {
         case Qt::DisplayRole:
            return ((TopLevelItem*)modelItem)->m_Name;
      }
      break;
   case HistoryTreeBackend::Type::CALL:
      if (role == Call::Role::DropState)
         return QVariant(modelItem->dropState());
      else if (role == Call::Role::DropString)
         return QVariant(modelItem->dropString());
      else if (m_lCategoryCounter.size() >= index.parent().row() && m_lCategoryCounter[index.parent().row()])
         return m_lCategoryCounter[index.parent().row()]->m_lChilds[index.row()]->getRoleData((Call::Role)role);
      break;
   case HistoryTreeBackend::Type::NUMBER:
   case HistoryTreeBackend::Type::BOOKMARK:
      break;
   };
   return QVariant();
}

QVariant HistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   Q_UNUSED(section)
   if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
      return QVariant("Contacts");
   return QVariant();
}

int HistoryModel::rowCount( const QModelIndex& parent ) const
{
   if (!parent.isValid() || !parent.internalPointer()) {
      return m_lCategoryCounter.size();
   }
   else if (!parent.parent().isValid()) {
      return m_lCategoryCounter[parent.row()]->m_lChilds.size();
   }
//    else if (parent.parent().isValid() && !parent.parent().parent().isValid()) {
//       return m_lCategoryCounter[parent.parent().row()]->m_lChilds[parent.row()]->getPhoneNumbers().size();
//    }
   return 0;
}

Qt::ItemFlags HistoryModel::flags( const QModelIndex& index ) const
{
   if (!index.isValid())
      return 0;
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable | (index.parent().isValid()?Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled:Qt::ItemIsEnabled);
}

int HistoryModel::columnCount ( const QModelIndex& parent) const
{
   Q_UNUSED(parent)
   return 1;
}

QModelIndex HistoryModel::parent( const QModelIndex& index) const
{
   if (!index.isValid() || !index.internalPointer()) {
      return QModelIndex();
   }
   HistoryTreeBackend* modelItem = static_cast<HistoryTreeBackend*>(index.internalPointer());
   if (modelItem && (long long)modelItem > 100 && modelItem->type3() == HistoryTreeBackend::Type::CALL) {
      Call* call = (Call*)((HistoryTreeBackend*)(index.internalPointer()))->getSelf();
      QString val = category(call);
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

QModelIndex HistoryModel::index( int row, int column, const QModelIndex& parent) const
{
   if (!parent.isValid()) {
      return createIndex(row,column,m_lCategoryCounter[row]);
   }
   else if (!parent.parent().isValid() && column < m_lCategoryCounter[parent.row()]->m_lChilds.size() ) {
      return createIndex(row,column,(void*)dynamic_cast<HistoryTreeBackend*>(m_lCategoryCounter[parent.row()]->m_lChilds[row]));
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
   QMimeData *mimeData = new QMimeData();
   foreach (const QModelIndex &index, indexes) {
      if (index.isValid()) {
         QString text = data(index, Call::Role::Number).toString();
         mimeData->setData(MIME_PLAIN_TEXT , text.toUtf8());
         mimeData->setData(MIME_PHONENUMBER, text.toUtf8());
         mimeData->setData(MIME_HISTORYID  , ((Call*)index.internalPointer())->getCallId().toUtf8());
         return mimeData;
      }
   }
   return mimeData;
}


bool HistoryModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
//    QModelIndex idx = index(row,column,parent);
   qDebug() << "DROPPED" << action << parent.data(Qt::DisplayRole) << parent.isValid() << parent.data(Qt::DisplayRole);
   setData(parent,-1,Call::Role::DropState);
   QByteArray encodedCallId      = data->data( MIME_CALLID      );
   QByteArray encodedPhoneNumber = data->data( MIME_PHONENUMBER );
   QByteArray encodedContact     = data->data( MIME_CONTACT     );

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

// QVariant HistoryModel::commonCallInfo(Call* call, int role) const
// {
//    if (!call)
//       return QVariant();
//    QVariant cat;
//    Contact* ct = call->getContact();
//    switch (role) {
//       case Qt::DisplayRole:
//       case Call::Role::Name:
//          cat = ct?ct->getFormattedName():call->getPeerName();
//          break;
//       case Call::Role::Number:
//          cat = call->getPeerPhoneNumber();
//          break;
//       case Call::Role::Direction:
//          cat = call->getHistoryState();
//          break;
//       case Call::Role::Date:
//          cat = call->getStartTimeStamp();
//          break;
//       case Call::Role::Length:
//          cat = call->getLength();
//          break;
//       case Call::Role::FormattedDate:
//          cat = QDateTime::fromTime_t(call->getStartTimeStamp().toUInt()).toString();
//          break;
//       case Call::Role::HasRecording:
//          cat = call->hasRecording();
//          break;
//       case Call::Role::HistoryState:
//          cat = call->getHistoryState();
//          break;
//       case Call::Role::Filter:
//          cat = call->getHistoryState()+'\n'+commonCallInfo(call,Call::Role::Name).toString()+'\n'+commonCallInfo(call,Call::Role::Number).toString();
//          break;
//       case Call::Role::FuzzyDate:
//          cat = timeToHistoryCategory(QDateTime::fromTime_t(call->getStartTimeStamp().toUInt()).date());
//          break;
//       case Call::Role::IsBookmark:
//          cat = false;
//          break;
//       case Call::Role::Object:
//          return QVariant::fromValue(call);
//          break;
// //       case Call::Role::PhotoPtr:
// //          return QVariant::fromValue((void*)(ct?ct->getPhoto():nullptr));
// //          break;
//    }
//    return cat;
// }

QString HistoryModel::category(Call* call) const
{
   QString cat = call->getRoleData((Call::Role)m_Role).toString();
//    if (cat.size() && !m_ShowAll)
//       cat = cat[0].toUpper();
   return cat;
}

HistoryTreeBackend::HistoryTreeBackend(HistoryTreeBackend::Type _type) : m_Type3(_type),m_DropState(0)
{
   
}

HistoryTreeBackend::Type HistoryTreeBackend::type3() const
{
   return m_Type3;
}

QString HistoryModel::timeToHistoryCategory(const QDate& date)
{
   int period = HistoryModel::timeToHistoryConst(date);
   if (period >= 0 && period <= 24)
      return QString(m_slHistoryConstStr[HistoryModel::timeToHistoryConst(date)]);
   else
      return QString(m_slHistoryConstStr[24]);
}

HistoryModel::HistoryConst HistoryModel::timeToHistoryConst(const QDate& date)
{ //TODO this is slow and it often used, it is possible to optimize this directly on the integer range
   //m_spEvHandler->update();
   if (QDate::currentDate()  == date || QDate::currentDate()  < date) //The future case would be a bug, but it have to be handled anyway or it will appear in "very long time ago"
      return HistoryConst::Today;

   //Check for last week
   for (int i=1;i<7;i++) {
      if (QDate::currentDate().addDays(-i)  == date)
         return (HistoryModel::HistoryConst)i; //Yesterday to Six_days_ago
   }

   //Check for last month
   for (int i=1;i<4;i++) {
      if (QDate::currentDate().addDays(-(i*7))  >= date && QDate::currentDate().addDays(-(i*7) -7)  < date)
         return (HistoryModel::HistoryConst)(i+Last_week-1); //Last_week to Three_weeks_ago
   }

   //Check for last year
   for (int i=1;i<12;i++) {
      if (QDate::currentDate().addMonths(-i)  >= date && QDate::currentDate().addMonths((-i) - 1)  < date)
         return (HistoryModel::HistoryConst)(i+Last_month-1); //Last_month to Twelve_months ago
   }

   if (QDate::currentDate().addYears(-1)  >= date && QDate::currentDate().addYears(-2)  < date)
      return HistoryConst::Last_year;

   //Every other senario
   return HistoryModel::HistoryConst::Very_long_time_ago;
}