/****************************************************************************
 *   Copyright (C) 2012-2014 by Savoir-Faire Linux                          *
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
#include "lastusednumbermodel.h"
#include "abstractitembackend.h"
#include "visitors/itemmodelstateserializationvisitor.h"

/*****************************************************************************
 *                                                                           *
 *                             Private classes                               *
 *                                                                           *
 ****************************************************************************/

class HistoryTopLevelItem;

class HistoryModelPrivate : public QObject
{
   Q_OBJECT
public:
   HistoryModelPrivate(HistoryModel* parent);

   //Model
   class HistoryItem : public CategorizedCompositeNode {
   public:
      explicit HistoryItem(Call* call);
      virtual ~HistoryItem();
      virtual QObject* getSelf() const;
      Call* call() const;
      int m_Index;
      HistoryTopLevelItem* m_pParent;
      HistoryItemNode* m_pNode;
   private:
      Call* m_pCall;
   };

   //Helpers
   HistoryTopLevelItem* getCategory(const Call* call);

   //Attributes
   static CallMap m_sHistoryCalls;
   QVector<AbstractHistoryBackend*> m_lBackends;

   //Model categories
   QList<HistoryTopLevelItem*>         m_lCategoryCounter ;
   QHash<int,HistoryTopLevelItem*>     m_hCategories      ;
   QHash<QString,HistoryTopLevelItem*> m_hCategoryByName  ;
   int                          m_Role             ;
   QStringList                  m_lMimes           ;

private:
   HistoryModel* q_ptr;

public Q_SLOTS:
   void reloadCategories();
   void slotChanged(const QModelIndex& idx);
};

class HistoryTopLevelItem : public CategorizedCompositeNode,public QObject {
   friend class HistoryModel;
   friend class HistoryModelPrivate;
public:
   virtual QObject* getSelf() const;
   virtual ~HistoryTopLevelItem();
   int m_Index;
   int m_AbsoluteIndex;
   QVector<HistoryModelPrivate::HistoryItem*> m_lChildren;
private:
   explicit HistoryTopLevelItem(const QString& name, int index);
   QString m_NameStr;
   int modelRow;
};

class HistoryItemNode : public QObject //TODO remove this once Qt4 support is dropped
{
   Q_OBJECT
public:
   HistoryItemNode(HistoryModel* m, Call* c, HistoryModelPrivate::HistoryItem* backend);
   Call* m_pCall;
private:
   HistoryModelPrivate::HistoryItem* m_pBackend;
   HistoryModel* m_pModel;
private Q_SLOTS:
   void slotNumberChanged();
Q_SIGNALS:
   void changed(const QModelIndex& idx);
};

HistoryItemNode::HistoryItemNode(HistoryModel* m, Call* c, HistoryModelPrivate::HistoryItem* backend) :
m_pCall(c),m_pBackend(backend),m_pModel(m){
   connect(c,SIGNAL(changed()),this,SLOT(slotNumberChanged()));
}

void HistoryItemNode::slotNumberChanged()
{
   emit changed(m_pModel->index(m_pBackend->m_Index,0,m_pModel->index(m_pBackend->m_pParent->m_AbsoluteIndex,0)));
}

HistoryModel* HistoryModel::m_spInstance    = nullptr;
CallMap       HistoryModelPrivate::m_sHistoryCalls          ;

HistoryTopLevelItem::HistoryTopLevelItem(const QString& name, int index) : 
   CategorizedCompositeNode(CategorizedCompositeNode::Type::TOP_LEVEL),QObject(nullptr),m_Index(index),m_NameStr(name),
   m_AbsoluteIndex(-1),modelRow(-1)
{}

HistoryTopLevelItem::~HistoryTopLevelItem() {
   HistoryModel::m_spInstance->d_ptr->m_lCategoryCounter.removeAll(this);
   while(m_lChildren.size()) {
      HistoryModelPrivate::HistoryItem* item = m_lChildren[0];
      m_lChildren.remove(0);
      delete item;
   }
}

QObject* HistoryTopLevelItem::getSelf() const
{
   return const_cast<HistoryTopLevelItem*>(this);
}

HistoryModelPrivate::HistoryItem::HistoryItem(Call* call) : CategorizedCompositeNode(CategorizedCompositeNode::Type::CALL),m_pCall(call),
m_Index(0),m_pParent(nullptr),m_pNode(nullptr)
{
   
}

HistoryModelPrivate::HistoryItem::~HistoryItem()
{
   delete m_pNode;
}


QObject* HistoryModelPrivate::HistoryItem::getSelf() const
{
   return const_cast<Call*>(m_pCall);
}

Call* HistoryModelPrivate::HistoryItem::call() const
{
   return m_pCall;
}


/*****************************************************************************
 *                                                                           *
 *                                 Constructor                               *
 *                                                                           *
 ****************************************************************************/

HistoryModelPrivate::HistoryModelPrivate(HistoryModel* parent) : QObject(parent), q_ptr(parent),
m_Role(Call::Role::FuzzyDate)
{
}

///Constructor
HistoryModel::HistoryModel():QAbstractItemModel(QCoreApplication::instance()),d_ptr(new HistoryModelPrivate(this))
{
   m_spInstance  = this;
   d_ptr->m_lMimes << MIME_PLAIN_TEXT << MIME_PHONENUMBER << MIME_HISTORYID;
   QHash<int, QByteArray> roles = roleNames();
   roles.insert(Call::Role::Name          ,QByteArray("name"          ));
   roles.insert(Call::Role::Number        ,QByteArray("number"        ));
   roles.insert(Call::Role::Direction2    ,QByteArray("direction"     ));
   roles.insert(Call::Role::Date          ,QByteArray("date"          ));
   roles.insert(Call::Role::Length        ,QByteArray("length"        ));
   roles.insert(Call::Role::FormattedDate ,QByteArray("formattedDate" ));
   roles.insert(Call::Role::HasRecording  ,QByteArray("hasRecording"  ));
   roles.insert(Call::Role::Historystate  ,QByteArray("historyState"  ));
   roles.insert(Call::Role::Filter        ,QByteArray("filter"        ));
   roles.insert(Call::Role::FuzzyDate     ,QByteArray("fuzzyDate"     ));
   roles.insert(Call::Role::IsBookmark    ,QByteArray("isBookmark"    ));
   roles.insert(Call::Role::Security      ,QByteArray("security"      ));
   roles.insert(Call::Role::Department    ,QByteArray("department"    ));
   roles.insert(Call::Role::Email         ,QByteArray("email"         ));
   roles.insert(Call::Role::Organisation  ,QByteArray("organisation"  ));
   roles.insert(Call::Role::Object        ,QByteArray("object"        ));
   roles.insert(Call::Role::PhotoPtr      ,QByteArray("photoPtr"      ));
   roles.insert(Call::Role::CallState     ,QByteArray("callState"     ));
   roles.insert(Call::Role::Id            ,QByteArray("id"            ));
   roles.insert(Call::Role::StartTime     ,QByteArray("startTime"     ));
   roles.insert(Call::Role::StopTime      ,QByteArray("stopTime"      ));
   roles.insert(Call::Role::DropState     ,QByteArray("dropState"     ));
   roles.insert(Call::Role::DTMFAnimState ,QByteArray("dTMFAnimState" ));
   roles.insert(Call::Role::LastDTMFidx   ,QByteArray("lastDTMFidx"   ));
   roles.insert(Call::Role::IsRecording   ,QByteArray("isRecording"   ));
   setRoleNames(roles);
} //initHistory

///Destructor
HistoryModel::~HistoryModel()
{
   for (int i=0; i<d_ptr->m_lCategoryCounter.size();i++) {
      delete d_ptr->m_lCategoryCounter[i];
   }
   while(d_ptr->m_lCategoryCounter.size()) {
      HistoryTopLevelItem* item = d_ptr->m_lCategoryCounter[0];
      d_ptr->m_lCategoryCounter.removeAt(0);

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
///Get the top level item based on a call
HistoryTopLevelItem* HistoryModelPrivate::getCategory(const Call* call)
{
   HistoryTopLevelItem* category = nullptr;
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
      category = new HistoryTopLevelItem(name,index);
      category->modelRow = m_lCategoryCounter.size();
//       emit layoutAboutToBeChanged(); //Not necessary
//       beginInsertRows(QModelIndex(),m_lCategoryCounter.size(),m_lCategoryCounter.size());
      category->m_AbsoluteIndex = m_lCategoryCounter.size();
      m_lCategoryCounter << category;
      m_hCategories    [index] = category;
      m_hCategoryByName[name ] = category;
//       endInsertRows();
//       emit layoutChanged();
   }
   return category;
}


const CallMap HistoryModel::getHistoryCalls() const
{
   return d_ptr->m_sHistoryCalls;
}

///Add to history
void HistoryModel::add(Call* call)
{
   if (!call || call->lifeCycleState() != Call::LifeCycleState::FINISHED || !call->startTimeStamp()) {
      return;
   }

//    if (!m_HaveContactModel && call->contactBackend()) {
//       connect(((QObject*)call->contactBackend()),SIGNAL(collectionChanged()),this,SLOT(reloadCategories()));
//       m_HaveContactModel = true;
//    }//TODO implement reordering

   emit newHistoryCall(call);
   emit layoutAboutToBeChanged();
   HistoryTopLevelItem* tl = d_ptr->getCategory(call);
   const QModelIndex& parentIdx = index(tl->modelRow,0);
   beginInsertRows(parentIdx,tl->m_lChildren.size(),tl->m_lChildren.size());
   HistoryModelPrivate::HistoryItem* item = new HistoryModelPrivate::HistoryItem(call);
   item->m_pParent = tl;
   item->m_pNode = new HistoryItemNode(this,call,item);
   connect(item->m_pNode,SIGNAL(changed(QModelIndex)),d_ptr.data(),SLOT(slotChanged(QModelIndex)));
   item->m_Index = tl->m_lChildren.size();
   tl->m_lChildren << item;

   //Try to prevent startTimeStamp() collisions, it technically doesn't work as time_t are signed
   //we don't care
   d_ptr->m_sHistoryCalls[(call->startTimeStamp() << 10)+qrand()%1024] = call;
   endInsertRows();
   emit layoutChanged();
   LastUsedNumberModel::instance()->addCall(call);
   emit historyChanged();

   // Loop until it find a compatible backend
   //HACK only support a single active history backend
   if (!call->backend()) {
      foreach (AbstractHistoryBackend* backend, d_ptr->m_lBackends) {
         if (backend->supportedFeatures() & AbstractHistoryBackend::ADD) {
            if (backend->append(call)) {
               call->setBackend(backend);
               break;
            }
         }
      }
   }
}

///Set if the history has a limit
void HistoryModel::setHistoryLimited(bool isLimited)
{
   if (!isLimited)
      DBus::ConfigurationManager::instance().setHistoryLimit(0);
}

///Set the number of days before history items are discarded
void HistoryModel::setHistoryLimit(int numberOfDays)
{
   DBus::ConfigurationManager::instance().setHistoryLimit(numberOfDays);
}

///Is history items are being deleted after "historyLimit()" days
bool HistoryModel::isHistoryLimited() const
{
   return DBus::ConfigurationManager::instance().getHistoryLimit() != 0;
}

///Number of days before items are discarded (0 = never)
int HistoryModel::historyLimit() const
{
   return DBus::ConfigurationManager::instance().getHistoryLimit();
}


/*****************************************************************************
 *                                                                           *
 *                              Model related                                *
 *                                                                           *
 ****************************************************************************/

void HistoryModelPrivate::reloadCategories()
{
   q_ptr->beginResetModel();
   m_hCategories.clear();
   m_hCategoryByName.clear();
   foreach(HistoryTopLevelItem* item, m_lCategoryCounter) {
      delete item;
   }
   m_lCategoryCounter.clear();
   foreach(Call* call, m_sHistoryCalls) {
      HistoryTopLevelItem* category = getCategory(call);
      if (category) {
         HistoryItem* item = new HistoryItem(call);
         item->m_Index = category->m_lChildren.size();
         item->m_pNode = new HistoryItemNode(q_ptr,call,item);
         connect(item->m_pNode,SIGNAL(changed(QModelIndex)),this,SLOT(slotChanged(QModelIndex)));
         item->m_pParent = category;
         category->m_lChildren << item;
      }
      else
         qDebug() << "ERROR count";
   }
   q_ptr->endResetModel();
   emit q_ptr->layoutAboutToBeChanged();
   emit q_ptr->layoutChanged();
   emit q_ptr->dataChanged(q_ptr->index(0,0),q_ptr->index(q_ptr->rowCount()-1,0));
}

void HistoryModelPrivate::slotChanged(const QModelIndex& idx)
{
   emit q_ptr->dataChanged(idx,idx);
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
            return static_cast<HistoryTopLevelItem*>(modelItem)->m_NameStr;
         case Call::Role::FuzzyDate:
         case Call::Role::Date:
            return d_ptr->m_lCategoryCounter.size() - static_cast<HistoryTopLevelItem*>(modelItem)->m_Index;
         default:
            break;
      }
      break;
   case CategorizedCompositeNode::Type::CALL:
      if (role == Call::Role::DropState)
         return QVariant(modelItem->dropState());
      else {
         const int parRow = idx.parent().row();
         const HistoryTopLevelItem* parTli = d_ptr->m_lCategoryCounter[parRow];
         if (d_ptr->m_lCategoryCounter.size() > parRow && parRow >= 0 && parTli && parTli->m_lChildren.size() > idx.row())
            return parTli->m_lChildren[idx.row()]->call()->roleData((Call::Role)role);
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
      return d_ptr->m_lCategoryCounter.size();
   }
   else {
      CategorizedCompositeNode* node = static_cast<CategorizedCompositeNode*>(parentIdx.internalPointer());
      switch(node->type()) {
         case CategorizedCompositeNode::Type::TOP_LEVEL:
            return ((HistoryTopLevelItem*)node)->m_lChildren.size();
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
      HistoryTopLevelItem* tli = d_ptr->getCategory(call);
      if (tli)
         return HistoryModel::index(tli->modelRow,0);
   }
   return QModelIndex();
}

QModelIndex HistoryModel::index( int row, int column, const QModelIndex& parentIdx) const
{
   if (!parentIdx.isValid()) {
      if (row >= 0 && d_ptr->m_lCategoryCounter.size() > row) {
         return createIndex(row,column,(void*)d_ptr->m_lCategoryCounter[row]);
      }
   }
   else {
      CategorizedCompositeNode* node = static_cast<CategorizedCompositeNode*>(parentIdx.internalPointer());
      switch(node->type()) {
         case CategorizedCompositeNode::Type::TOP_LEVEL:
            if (((HistoryTopLevelItem*)node)->m_lChildren.size() > row)
               return createIndex(row,column,(void*)static_cast<CategorizedCompositeNode*>(((HistoryTopLevelItem*)node)->m_lChildren[row]));
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

///Called when dynamically adding calls, otherwise the proxy filter will segfault
bool HistoryModel::insertRows( int row, int count, const QModelIndex & parent)
{
   if (parent.isValid()) {
      beginInsertRows(parent,row,row+count-1);
      endInsertRows();
      return true;
   }
   return false;
}

QStringList HistoryModel::mimeTypes() const
{
   return d_ptr->m_lMimes;
}

QMimeData* HistoryModel::mimeData(const QModelIndexList &indexes) const
{
   QMimeData *mimeData2 = new QMimeData();
   foreach (const QModelIndex &idx, indexes) {
      if (idx.isValid()) {
         const QString text = data(idx, Call::Role::Number).toString();
         mimeData2->setData(MIME_PLAIN_TEXT , text.toUtf8());
         const Call* call = (Call*)((CategorizedCompositeNode*)(idx.internalPointer()))->getSelf();
         mimeData2->setData(MIME_PHONENUMBER, call->peerPhoneNumber()->toHash().toUtf8());
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


bool HistoryModel::hasBackends() const
{
   return d_ptr->m_lBackends.size();
}

bool HistoryModel::hasEnabledBackends() const
{
   return d_ptr->m_lBackends.size();
}

void HistoryModel::addBackend(AbstractHistoryBackend* backend, LoadOptions options)
{
   d_ptr->m_lBackends << backend;
   connect(backend,SIGNAL(newHistoryCallAdded(Call*)),this,SLOT(add(Call*)));
   if (options & LoadOptions::FORCE_ENABLED || ItemModelStateSerializationVisitor::instance()->isChecked(backend))
      backend->load();
   emit newBackendAdded(backend);
}

///Call all backends that support clearing
void HistoryModel::clearAllBackends() const
{
   foreach (AbstractHistoryBackend* backend, d_ptr->m_lBackends) {
      if (backend->supportedFeatures() & AbstractHistoryBackend::ADD) {
         backend->clear();
      }
   }

   //TODO Remove this
   //Clear the daemon history backend as apparently the Gnome client wont
   //Use its native backend anytime soon
   DBus::ConfigurationManager::instance().clearHistory();
}


bool HistoryModel::enableBackend(AbstractHistoryBackend* backend, bool enabled)
{
   Q_UNUSED(backend)
   Q_UNUSED(enabled)
   return false;//TODO
}

CommonItemBackendModel* HistoryModel::backendModel() const
{
   return nullptr; //TODO
}

const QVector<AbstractHistoryBackend*> HistoryModel::backends() const
{
   return d_ptr->m_lBackends;
}

const QVector<AbstractHistoryBackend*> HistoryModel::enabledBackends() const
{
   return d_ptr->m_lBackends;
}

///Return valid payload types
int HistoryModel::acceptedPayloadTypes() const
{
   return CallModel::DropPayloadType::CALL;
}

void HistoryModel::setCategoryRole(Call::Role role) 
{
   if (d_ptr->m_Role != role) {
      d_ptr->m_Role = role;
      d_ptr->reloadCategories();
   }
}

#include <historymodel.moc>
