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
HistoryModel::HistoryModel():QAbstractItemModel(nullptr),m_HistoryInit(false),m_Role(Qt::DisplayRole),m_HaveContactModel(false)
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
   qDebug() << "\n\nBEGIN LAOD MODEL";
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

         emit dataChanged(index(0,0),index(rowCount()-1,0));
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
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
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
      return commonCallInfo(m_lCategoryCounter[index.parent().row()]->m_lChilds[index.row()],role);
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
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
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

QVariant HistoryModel::commonCallInfo(Call* call, int role) const
{
   if (!call)
      return QVariant();
   QVariant cat;
   Contact* ct = nullptr;//call->getContact();
   switch (role) {
      case Qt::DisplayRole:
      case Name:
         cat = ct?ct->getFormattedName():call->getPeerName();
         break;
      case Number:
         cat = call->getPeerPhoneNumber();
         break;
      case Direction:
         cat = call->getHistoryState();
         break;
      case Date:
         cat = call->getStartTimeStamp();
         break;
      case Length:
         cat = call->getLength();
         break;
   }
   return cat;
}

QString HistoryModel::category(Call* call) const
{
   QString cat = commonCallInfo(call,m_Role).toString();
//    if (cat.size() && !m_ShowAll)
//       cat = cat[0].toUpper();
   return cat;
}

HistoryTreeBackend::HistoryTreeBackend(HistoryTreeBackend::Type _type) : m_Type3(_type)
{
   
}

HistoryTreeBackend::Type HistoryTreeBackend::type3() const
{
   return m_Type3;
}