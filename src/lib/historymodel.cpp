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
#include "phonenumber.h"
#include "callmodel.h"
#include "historytimecategorymodel.h"

/*****************************************************************************
 *                                                                           *
 *                             Private classes                               *
 *                                                                           *
 ****************************************************************************/

HistoryModel* HistoryModel::m_spInstance    = nullptr;
CallMap       HistoryModel::m_sHistoryCalls          ;

HistoryModel::TopLevelItem::TopLevelItem(const QString& name, int index) : 
   CategorizedCompositeNode(CategorizedCompositeNode::Type::TOP_LEVEL),QObject(nullptr),m_Index(index),m_NameStr(name)
{}

HistoryModel::TopLevelItem::~TopLevelItem() {
   m_spInstance->m_lCategoryCounter.removeAll(this);
}

QObject* HistoryModel::TopLevelItem::getSelf() const
{
   return const_cast<HistoryModel::TopLevelItem*>(this);
}


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
               hc[ Call::HistoryMapFields::CALLID          ]         ,
               hc[ Call::HistoryMapFields::TIMESTAMP_START ].toUInt(),
               hc[ Call::HistoryMapFields::TIMESTAMP_STOP  ].toUInt(),
               hc[ Call::HistoryMapFields::ACCOUNT_ID      ]         ,
               hc[ Call::HistoryMapFields::DISPLAY_NAME    ]         ,
               hc[ Call::HistoryMapFields::PEER_NUMBER     ]         ,
               hc[ Call::HistoryMapFields::STATE           ]
      );
      if (pastCall->peerName().isEmpty()) {
         pastCall->setPeerName(tr("Unknown"));
      }
      pastCall->setRecordingPath(hc[ Call::HistoryMapFields::RECORDING_PATH ]);
      add(pastCall);
   }
   m_HistoryInit = true;
   m_spInstance = this;
   reloadCategories();
   m_lMimes << MIME_PLAIN_TEXT << MIME_PHONENUMBER << MIME_HISTORYID;
   QHash<int, QByteArray> roles = roleNames();
   roles.insert(Call::Role::Name          ,QByteArray("name"));
   roles.insert(Call::Role::Number        ,QByteArray("number"));
   roles.insert(Call::Role::Direction     ,QByteArray("direction"));
   roles.insert(Call::Role::Date          ,QByteArray("date"));
   roles.insert(Call::Role::Length        ,QByteArray("length"));
   roles.insert(Call::Role::FormattedDate ,QByteArray("formattedDate"));
   roles.insert(Call::Role::HasRecording  ,QByteArray("hasRecording"));
   roles.insert(Call::Role::Historystate  ,QByteArray("historyState"));
   roles.insert(Call::Role::Filter        ,QByteArray("filter"));
   roles.insert(Call::Role::FuzzyDate     ,QByteArray("fuzzyDate"));
   roles.insert(Call::Role::IsBookmark    ,QByteArray("isBookmark"));
   roles.insert(Call::Role::Security      ,QByteArray("security"));
   roles.insert(Call::Role::Department    ,QByteArray("department"));
   roles.insert(Call::Role::Email         ,QByteArray("email"));
   roles.insert(Call::Role::Organisation  ,QByteArray("organisation"));
   roles.insert(Call::Role::Codec         ,QByteArray("codec"));
   roles.insert(Call::Role::IsConference  ,QByteArray("isConference"));
   roles.insert(Call::Role::Object        ,QByteArray("object"));
   roles.insert(Call::Role::PhotoPtr      ,QByteArray("photoPtr"));
   roles.insert(Call::Role::CallState     ,QByteArray("callState"));
   roles.insert(Call::Role::Id            ,QByteArray("id"));
   roles.insert(Call::Role::StartTime     ,QByteArray("startTime"));
   roles.insert(Call::Role::StopTime      ,QByteArray("stopTime"));
   roles.insert(Call::Role::DropState     ,QByteArray("dropState"));
   roles.insert(Call::Role::DTMFAnimState ,QByteArray("dTMFAnimState"));
   roles.insert(Call::Role::LastDTMFidx   ,QByteArray("lastDTMFidx"));
   roles.insert(Call::Role::IsRecording   ,QByteArray("isRecording"));
   setRoleNames(roles);
} //initHistory

///Destructor
HistoryModel::~HistoryModel()
{
   for (int i=0; i<m_lCategoryCounter.size();i++) {
      delete m_lCategoryCounter[i];
   }
   while(m_lCategoryCounter.size()) {
      TopLevelItem* item = m_lCategoryCounter[0];
      m_lCategoryCounter.removeAt(0);
      delete item;
   }
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

HistoryModel::TopLevelItem* HistoryModel::getCategory(const Call* call)
{
   TopLevelItem* category = nullptr;
   QString name;
   int index = -1;
   if (m_Role == Call::Role::FuzzyDate) {
      index = call->roleData(Call::Role::FuzzyDate).toInt();
      name = HistoryTimeCategoryModel::indexToName(index);
      category = m_hCategories[index];
   }
   else {
      name = call->roleData(m_Role).toString();
      category = m_hCategoryByName[name];
   }
   if (!category) {
      category = new TopLevelItem(name,index);
      category->modelRow = m_lCategoryCounter.size();
      m_lCategoryCounter << category;
      m_hCategories    [index] = category;
      m_hCategoryByName[name ] = category;
   }
   return category;
}

///Add to history
void HistoryModel::add(Call* call)
{
   if (!call || call->state() != Call::State::OVER)
      return;

   if (!m_HaveContactModel && call->contactBackend()) {
      connect(((QObject*)call->contactBackend()),SIGNAL(collectionChanged()),this,SLOT(reloadCategories()));
      m_HaveContactModel = true;
   }

   emit newHistoryCall(call);
   getCategory(call)->m_lChildren << call;
   m_sHistoryCalls[call->startTimeStamp()] = call;
   emit historyChanged();
//    emit layoutChanged(); //Cause segfault
}

///Return the history list
const CallMap& HistoryModel::getHistory()
{
   instance();
   return m_sHistoryCalls;
}


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
   m_hCategoryByName.clear();
   foreach(TopLevelItem* item, m_lCategoryCounter) {
      delete item;
   }
   m_lCategoryCounter.clear();
   m_isContactDateInit = false;
   foreach(Call* call, getHistory()) {
      TopLevelItem* category = getCategory(call);
      if (category) {
         category->m_lChildren << call;
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
      CategorizedCompositeNode* modelItem = (CategorizedCompositeNode*)idx.internalPointer();
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

   CategorizedCompositeNode* modelItem = static_cast<CategorizedCompositeNode*>(idx.internalPointer());
   switch (modelItem->type()) {
      case CategorizedCompositeNode::Type::TOP_LEVEL:
      switch (role) {
         case Qt::DisplayRole:
            return static_cast<TopLevelItem*>(modelItem)->m_NameStr;
         case Call::Role::FuzzyDate:
         case Call::Role::Date:
            return m_lCategoryCounter.size() - static_cast<TopLevelItem*>(modelItem)->m_Index;
         default:
            break;
      }
      break;
   case CategorizedCompositeNode::Type::CALL:
      if (role == Call::Role::DropState)
         return QVariant(modelItem->dropState());
      else {
         const int parRow = idx.parent().row();
         const TopLevelItem* parTli = m_lCategoryCounter[parRow];
         if (m_lCategoryCounter.size() > parRow && parRow >= 0 && parTli && parTli->m_lChildren.size() > idx.row())
            return parTli->m_lChildren[idx.row()]->roleData((Call::Role)role);
      }
      break;
   case CategorizedCompositeNode::Type::NUMBER:
   case CategorizedCompositeNode::Type::BOOKMARK:
   case CategorizedCompositeNode::Type::CONTACT:
   default:
      break;
   };
   return QVariant();
}

QVariant HistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   Q_UNUSED(section)
   if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
      return QVariant(tr("History"));
   if (role == Qt::InitialSortOrderRole)
      return QVariant(Qt::DescendingOrder);
   return QVariant();
}

int HistoryModel::rowCount( const QModelIndex& parentIdx ) const
{
   if ((!parentIdx.isValid()) || (!parentIdx.internalPointer())) {
      return m_lCategoryCounter.size();
   }
   else {
      CategorizedCompositeNode* node = static_cast<CategorizedCompositeNode*>(parentIdx.internalPointer());
      switch(node->type()) {
         case CategorizedCompositeNode::Type::TOP_LEVEL:
            return ((TopLevelItem*)node)->m_lChildren.size();
         case CategorizedCompositeNode::Type::CALL:
         case CategorizedCompositeNode::Type::NUMBER:
         case CategorizedCompositeNode::Type::BOOKMARK:
         case CategorizedCompositeNode::Type::CONTACT:
         default:
            return 0;
      };
   }
}

Qt::ItemFlags HistoryModel::flags( const QModelIndex& idx ) const
{
   if (!idx.isValid())
      return Qt::NoItemFlags;
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
   CategorizedCompositeNode* modelItem = static_cast<CategorizedCompositeNode*>(idx.internalPointer());
   if (modelItem && modelItem->type() == CategorizedCompositeNode::Type::CALL) {
      const Call* call = (Call*)((CategorizedCompositeNode*)(idx.internalPointer()))->getSelf();
      TopLevelItem* tli = const_cast<HistoryModel*>(this)->getCategory(call);
      if (tli)
         return HistoryModel::index(tli->modelRow,0);
   }
   return QModelIndex();
}

QModelIndex HistoryModel::index( int row, int column, const QModelIndex& parentIdx) const
{
   if (!parentIdx.isValid()) {
      if (row >= 0 && m_lCategoryCounter.size() > row) {
         return createIndex(row,column,(void*)m_lCategoryCounter[row]);
      }
   }
   else {
      CategorizedCompositeNode* node = static_cast<CategorizedCompositeNode*>(parentIdx.internalPointer());
      switch(node->type()) {
         case CategorizedCompositeNode::Type::TOP_LEVEL:
            if (((TopLevelItem*)node)->m_lChildren.size() > row)
               return createIndex(row,column,(void*)static_cast<CategorizedCompositeNode*>(((TopLevelItem*)node)->m_lChildren[row]));
            break;
         case CategorizedCompositeNode::Type::CALL:
         case CategorizedCompositeNode::Type::NUMBER:
         case CategorizedCompositeNode::Type::BOOKMARK:
         case CategorizedCompositeNode::Type::CONTACT:
            break;
      };
   }
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
         CategorizedCompositeNode* node = static_cast<CategorizedCompositeNode*>(idx.internalPointer());
         if (node->type() == CategorizedCompositeNode::Type::CALL)
            mimeData2->setData(MIME_HISTORYID  , static_cast<Call*>(node->getSelf())->id().toUtf8());
         return mimeData2;
      }
   }
   return mimeData2;
}


bool HistoryModel::dropMimeData(const QMimeData *mime, Qt::DropAction action, int row, int column, const QModelIndex &parentIdx)
{
   Q_UNUSED(row)
   Q_UNUSED(column)
   Q_UNUSED(action)
   setData(parentIdx,-1,Call::Role::DropState);
   QByteArray encodedPhoneNumber = mime->data( MIME_PHONENUMBER );
   QByteArray encodedContact     = mime->data( MIME_CONTACT     );

   if (parentIdx.isValid() && mime->hasFormat( MIME_CALLID)) {
      QByteArray encodedCallId      = mime->data( MIME_CALLID      );
      Call* call = CallModel::instance()->getCall(encodedCallId);
      if (call) {
         const QModelIndex& idx = index(row,column,parentIdx);
         if (idx.isValid()) {
            const Call* target = (Call*)((CategorizedCompositeNode*)(idx.internalPointer()))->getSelf();
            if (target) {
               CallModel::instance()->transfer(call,target->peerPhoneNumber());
               return true;
            }
         }
      }
   }
   return false;
}

///Return valid payload types
int HistoryModel::acceptedPayloadTypes()
{
   return CallModel::DropPayloadType::CALL;
}

void HistoryModel::setCategoryRole(Call::Role role) 
{
   if (m_Role != role) {
      m_Role = role;
      reloadCategories();
   }
}
