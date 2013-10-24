/****************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                          *
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
#include "callmodel.h"

//Qt
#include <QtCore/QDebug>
#include <QtGui/QDragEnterEvent>

//SFLPhone library
#include "call.h"
#include "phonedirectorymodel.h"
#include "phonenumber.h"
#include "accountlistmodel.h"
#include "dbus/metatypes.h"
#include "dbus/callmanager.h"
#include "dbus/configurationmanager.h"
#include "dbus/instancemanager.h"
#include "sflphone_const.h"
#include "typedefs.h"
#include "abstractcontactbackend.h"
#include "dbus/videomanager.h"
#include "historymodel.h"
#include "visitors/phonenumberselector.h"

//Define
///InternalStruct: internal representation of a call
struct InternalStruct {
   InternalStruct() : m_pParent(nullptr),call_real(nullptr),conference(false){}
   Call*                  call_real  ;
   QModelIndex            index      ;
   QList<InternalStruct*> m_lChildren;
   bool                   conference ;
   InternalStruct*        m_pParent  ;
};

//Static member
CallModel*   CallModel::m_spInstance = nullptr;


/*****************************************************************************
 *                                                                           *
 *                               Constructor                                 *
 *                                                                           *
 ****************************************************************************/

///Singleton
CallModel* CallModel::instance() {
   if (!m_spInstance) {
      m_spInstance = new CallModel();
      m_spInstance->init();
   }
   return m_spInstance;
}

///Retrieve current and older calls from the daemon, fill history, model and enable drag n' drop
CallModel::CallModel() : QAbstractItemModel(QCoreApplication::instance())
{
   setObjectName("CallModel");
} //CallModel

///Constructor (there fix an initializationn loop)
void CallModel::init()
{
   static bool dbusInit = false;
   initRoles();
   if (!dbusInit) {
      CallManagerInterface& callManager = DBus::CallManager::instance();
      #ifdef ENABLE_VIDEO
      VideoInterface& interface = DBus::VideoManager::instance();
      #endif

      //SLOTS
      /*             SENDER                          SIGNAL                     RECEIVER                    SLOT                   */
      /**/connect(&callManager, SIGNAL(callStateChanged(QString,QString))       , this , SLOT(slotCallStateChanged(QString,QString))   );
      /**/connect(&callManager, SIGNAL(incomingCall(QString,QString,QString))   , this , SLOT(slotIncomingCall(QString,QString))       );
      /**/connect(&callManager, SIGNAL(conferenceCreated(QString))              , this , SLOT(slotIncomingConference(QString))         );
      /**/connect(&callManager, SIGNAL(conferenceChanged(QString,QString))      , this , SLOT(slotChangingConference(QString,QString)) );
      /**/connect(&callManager, SIGNAL(conferenceRemoved(QString))              , this , SLOT(slotConferenceRemoved(QString))          );
      /**/connect(&callManager, SIGNAL(voiceMailNotify(QString,int))            , this , SLOT(slotVoiceMailNotify(QString,int))        );
      /**/connect(&callManager, SIGNAL(volumeChanged(QString,double))           , this , SLOT(slotVolumeChanged(QString,double))       );
      /**/connect(&callManager, SIGNAL(recordPlaybackFilepath(QString,QString)) , this , SLOT(slotNewRecordingAvail(QString,QString))  );
      /**/connect(&callManager, SIGNAL(recordingStateChanged(QString,bool))     , this,  SLOT(slotRecordStateChanged(QString,bool)));
      #ifdef ENABLE_VIDEO
      /**/connect(&interface  , SIGNAL(startedDecoding(QString,QString,int,int)), this , SLOT(slotStartedDecoding(QString,QString))    );
      /**/connect(&interface  , SIGNAL(stoppedDecoding(QString,QString))        , this , SLOT(slotStoppedDecoding(QString,QString))    );
      #endif
      /*                                                                                                                           */

      connect(HistoryModel::instance(),SIGNAL(newHistoryCall(Call*)),this,SLOT(slotAddPrivateCall(Call*)));

      dbusInit = true;

      HistoryModel::getHistory();
//       foreach(Call* call,){
//          addCall(call,nullptr);
//       }
   }
   static bool m_sInstanceInit = false;
   if (!m_sInstanceInit)
      registerCommTypes();
   m_sInstanceInit = true;

   CallManagerInterface& callManager = DBus::CallManager::instance();
   const QStringList callList = callManager.getCallList();
   foreach (const QString& callId, callList) {
      Call* tmpCall = Call::buildExistingCall(callId);
      addCall(tmpCall);
   }

   const QStringList confList = callManager.getConferenceList();
   foreach (const QString& confId, confList) {
      Call* conf = addConference(confId);
      emit conferenceCreated(conf);
   }
}

///Destructor
CallModel::~CallModel()
{
   foreach (Call* call,  m_sPrivateCallList_call.keys())
      delete call;
   foreach (InternalStruct* s,  m_sPrivateCallList_call.values())
      delete s;
   m_sPrivateCallList_call.clear  ();
   m_sPrivateCallList_callId.clear();
   m_spInstance = nullptr;
}

void CallModel::initRoles()
{
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
}


/*****************************************************************************
 *                                                                           *
 *                         Access related functions                          *
 *                                                                           *
 ****************************************************************************/

///Return the active call count
int CallModel::size()
{
   return m_lInternalModel.size();
}

///Return the action call list
 CallList CallModel::getCallList()
{
   CallList callList;
   #pragma GCC diagnostic ignored "-Wshadow"
   foreach(InternalStruct* internalS, m_lInternalModel) {
      callList.push_back(internalS->call_real);
      if (internalS->m_lChildren.size()) {
         #pragma GCC diagnostic ignored "-Wshadow"
         foreach(InternalStruct* childInt,internalS->m_lChildren) {
            callList.push_back(childInt->call_real);
         }
      }
   }
   return callList;
} //getCallList

///Return all conferences
CallList CallModel::getConferenceList()
{
   CallList confList;

   //That way it can not be invalid
   const QStringList confListS = DBus::CallManager::instance().getConferenceList();
   foreach (const QString& confId, confListS) {
      InternalStruct* internalS = m_sPrivateCallList_callId[confId];
      if (!internalS) {
         qDebug() << "Warning: Conference not found, creating it, this should not happen";
         Call* conf = addConference(confId);
         confList << conf;
         emit conferenceCreated(conf);
      }
      else
         confList << internalS->call_real;
   }
   return confList;
} //getConferenceList

bool CallModel::hasConference() const
{
   foreach(const InternalStruct* s, m_lInternalModel) {
      if (s->m_lChildren.size())
         return true;
   }
   return false;
}

bool CallModel::isValid()
{
   return DBus::CallManager::instance().isValid();
}


/*****************************************************************************
 *                                                                           *
 *                            Call related code                              *
 *                                                                           *
 ****************************************************************************/

///Get the call associated with this index
Call* CallModel::getCall( const QModelIndex& idx              ) const
{
   if (idx.data(Call::Role::Object).canConvert<Call*>())
      return qvariant_cast<Call*>(idx.data(Call::Role::Object));
   return nullptr;
}

///Get the call associated with this ID
Call* CallModel::getCall( const QString& callId ) const
{
   if (m_sPrivateCallList_callId[callId]) {
      return m_sPrivateCallList_callId[callId]->call_real;
   }
   return nullptr;
}

///Add a call in the model structure, the call must exist before being added to the model
Call* CallModel::addCall(Call* call, Call* parentCall)
{
   if (!call || call->state() == Call::State::OVER || (parentCall && parentCall->state() == Call::State::OVER))
      return new Call(QString(),QString()); //Invalid, but better than managing NULL everywhere

   InternalStruct* aNewStruct = new InternalStruct;
   aNewStruct->call_real  = call;
   aNewStruct->conference = false;

   m_sPrivateCallList_call  [ call              ] = aNewStruct;
   if (call->state() != Call::State::OVER)
      m_lInternalModel << aNewStruct;
   m_sPrivateCallList_callId[ call->id() ] = aNewStruct;

   if (call->state() != Call::State::OVER)
      emit callAdded(call,parentCall);
   const QModelIndex idx = index(m_lInternalModel.size()-1,0,QModelIndex());
   emit dataChanged(idx, idx);
   connect(call,SIGNAL(changed(Call*)),this,SLOT(slotCallChanged(Call*)));
   connect(call,SIGNAL(dtmfPlayed(QString)),this,SLOT(slotDTMFPlayed(QString)));
   emit layoutChanged();
   return call;
} //addCall

///Create a new dialing call from peer name and the account ID
Call* CallModel::addDialingCall(const QString& peerName, Account* account)
{
   Account* acc = (account)?account:AccountListModel::currentAccount();
   return (!acc)?nullptr:addCall(Call::buildDialingCall(QString::number(qrand()), peerName, acc));
}  //addDialingCall

///Create a new incoming call when the daemon is being called
Call* CallModel::addIncomingCall(const QString& callId)
{
   Call* call = addCall(Call::buildIncomingCall(callId));
   //Call without account is not possible
   if (dynamic_cast<Account*>(call->account())) {
      if (call->account()->isAutoAnswer()) {
         call->performAction(Call::Action::ACCEPT);
      }
   }
   else {
      qDebug() << "Incoming call from an invalid account";
      throw tr("Invalid account");
   }
   return call;
}

///Create a ringing call
Call* CallModel::addRingingCall(const QString& callId)
{
   return addCall(Call::buildRingingCall(callId));
}

///Remove a call and update the internal structure
void CallModel::removeCall(Call* call)
{
   InternalStruct* internal = m_sPrivateCallList_call[call];

   if (!internal) {
      qDebug() << "Cannot remove call: call not found";
      return;
   }

   if (m_sPrivateCallList_call[call] != nullptr) {
      m_lInternalModel.removeAll(m_sPrivateCallList_call[call]);
      m_sPrivateCallList_call.remove(call);
   }

   if (m_sPrivateCallList_callId[m_sPrivateCallList_callId.key(internal)] == internal) {
      m_sPrivateCallList_callId.remove(m_sPrivateCallList_callId.key(internal));
   }

   m_lInternalModel.removeAll(internal);

   //Restore calls to the main list if they are not rey over
   if (internal->m_lChildren.size()) {
      foreach(InternalStruct* child,internal->m_lChildren) {
         if (child->call_real->state() != Call::State::OVER)
            m_lInternalModel << child;
      }
   }

   //The daemon often fail to emit the right signal, cleanup manually
   foreach(InternalStruct* topLevel, m_lInternalModel) {
      if (topLevel->call_real->isConference() && !topLevel->m_lChildren.size())
         removeConference(topLevel->call_real);
   }
   emit layoutChanged();
} //removeCall


QModelIndex CallModel::getIndex(Call* call)
{
   InternalStruct* internal = m_sPrivateCallList_call[call];
   int idx = m_lInternalModel.indexOf(internal);
   if (idx != -1) {
      return index(idx,0);
   }
   else {
      foreach(InternalStruct* str,m_lInternalModel) {
         idx = str->m_lChildren.indexOf(internal);
         if (idx != -1)
            return index(idx,0,index(m_lInternalModel.indexOf(str),0));
      }
   }
   return QModelIndex();
}

///Transfer "toTransfer" to "target" and wait to see it it succeeded
void CallModel::attendedTransfer(Call* toTransfer, Call* target)
{
   if ((!toTransfer) || (!target)) return;
   Q_NOREPLY DBus::CallManager::instance().attendedTransfer(toTransfer->id(),target->id());

   //TODO [Daemon] Implement this correctly
   toTransfer->changeCurrentState(Call::State::OVER);
   target->changeCurrentState(Call::State::OVER);
} //attendedTransfer

///Transfer this call to  "target" number
void CallModel::transfer(Call* toTransfer, const PhoneNumber* target)
{
   qDebug() << "Transferring call " << toTransfer->id() << "to" << target->uri();
   toTransfer->setTransferNumber ( target->uri()            );
   toTransfer->performAction     ( Call::Action::TRANSFER   );
   toTransfer->changeCurrentState( Call::State::TRANSFERRED );
   toTransfer->performAction     ( Call::Action::ACCEPT     );
   toTransfer->changeCurrentState( Call::State::OVER        );
   emit toTransfer->isOver(toTransfer);
} //transfer

/*****************************************************************************
 *                                                                           *
 *                         Conference related code                           *
 *                                                                           *
 ****************************************************************************/

///Add a new conference, get the call list and update the interface as needed
Call* CallModel::addConference(const QString& confID)
{
   qDebug() << "Notified of a new conference " << confID;
   CallManagerInterface& callManager = DBus::CallManager::instance();
   const QStringList callList = callManager.getParticipantList(confID);
   qDebug() << "Paticiapants are:" << callList;

   if (!callList.size()) {
      qDebug() << "This conference (" + confID + ") contain no call";
      return nullptr;
   }

   if (!m_sPrivateCallList_callId[callList[0]]) {
      qDebug() << "Invalid call";
      return nullptr;
   }

   Call* newConf = nullptr;
   if (m_sPrivateCallList_callId[callList[0]]->call_real->account())
      newConf =  new Call(confID, m_sPrivateCallList_callId[callList[0]]->call_real->account()->id());

   if (newConf) {
      InternalStruct* aNewStruct = new InternalStruct;
      aNewStruct->call_real  = newConf;
      aNewStruct->conference = true;

      m_sPrivateCallList_call[newConf]  = aNewStruct;
      m_sPrivateCallList_callId[confID] = aNewStruct;
      m_lInternalModel << aNewStruct;

      foreach(const QString& callId,callList) {
         InternalStruct* callInt = m_sPrivateCallList_callId[callId];
         if (callInt) {
            if (callInt->m_pParent && callInt->m_pParent != aNewStruct)
               callInt->m_pParent->m_lChildren.removeAll(callInt);
            m_lInternalModel.removeAll(callInt);
            callInt->m_pParent = aNewStruct;
            if (aNewStruct->m_lChildren.indexOf(callInt) == -1)
               aNewStruct->m_lChildren << callInt;
         }
         else {
            qDebug() << "References to unknown call";
         }
      }
      const QModelIndex idx = index(m_lInternalModel.size()-1,0,QModelIndex());
      emit dataChanged(idx, idx);
      emit layoutChanged();
      connect(newConf,SIGNAL(changed(Call*)),this,SLOT(slotCallChanged(Call*)));
   }

   return newConf;
} //addConference

///Join two call to create a conference, the conference will be created later (see addConference)
bool CallModel::createConferenceFromCall(Call* call1, Call* call2)
{
  if (!call1 || !call2) return false;
  qDebug() << "Joining call: " << call1->id() << " and " << call2->id();
  Q_NOREPLY DBus::CallManager::instance().joinParticipant(call1->id(),call2->id());
  return true;
} //createConferenceFromCall

///Add a new participant to a conference
bool CallModel::addParticipant(Call* call2, Call* conference)
{
   if (conference->isConference()) {
      Q_NOREPLY DBus::CallManager::instance().addParticipant(call2->id(), conference->confId());
      return true;
   }
   else {
      qDebug() << "This is not a conference";
      return false;
   }
} //addParticipant

///Remove a participant from a conference
bool CallModel::detachParticipant(Call* call)
{
   Q_NOREPLY DBus::CallManager::instance().detachParticipant(call->id());
   return true;
}

///Merge two conferences
bool CallModel::mergeConferences(Call* conf1, Call* conf2)
{
   Q_NOREPLY DBus::CallManager::instance().joinConference(conf1->confId(),conf2->confId());
   return true;
}

///Executed when the daemon signal a modification in an existing conference. Update the call list and update the TreeView
bool CallModel::changeConference(const QString& confId, const QString& state)
{
   Q_UNUSED(state)
   qDebug() << "Conf changed";

   if (!m_sPrivateCallList_callId[confId]) {
      qDebug() << "The conference does not exist";
      return false;
   }

   if (!m_sPrivateCallList_callId[confId]->index.isValid()) {
      qDebug() << "The conference item does not exist";
      return false;
   }
   return true;
} //changeConference

///Remove a conference from the model and the TreeView
void CallModel::removeConference(const QString &confId)
{
   if (m_sPrivateCallList_callId[confId])
      qDebug() << "Ending conversation containing " << m_sPrivateCallList_callId[confId]->m_lChildren.size() << " participants";
   removeConference(getCall(confId));
}

///Remove a conference using it's call object
void CallModel::removeConference(Call* call)
{
   const InternalStruct* internal = m_sPrivateCallList_call[call];

   if (!internal) {
      qDebug() << "Cannot remove conference: call not found";
      return;
   }
   removeCall(call);
}


/*****************************************************************************
 *                                                                           *
 *                                  Model                                    *
 *                                                                           *
 ****************************************************************************/

///This model doesn't support direct write, only the dragState hack
bool CallModel::setData( const QModelIndex& idx, const QVariant &value, int role)
{
   if (idx.isValid()) {
      if (role == Call::Role::DropState) {
         Call* call = getCall(idx);
         if (call)
            call->setProperty("dropState",value.toInt());
         emit dataChanged(idx,idx);
      }
      else if (role == Qt::EditRole) {
         const QString number = value.toString();
         Call* call = getCall(idx);
         if (call && number != call->dialNumber()) {
            call->setDialNumber(number);
            emit dataChanged(idx,idx);
            return true;
         }
      }
      else if (role == Call::Role::DTMFAnimState) {
         Call* call = getCall(idx);
         if (call) {
            call->setProperty("DTMFAnimState",value.toInt());
            emit dataChanged(idx,idx);
            return true;
         }
      }
      else if (role == Call::Role::DropPosition) {
         Call* call = getCall(idx);
         if (call) {
            call->setProperty("dropPosition",value.toInt());
            emit dataChanged(idx,idx);
            return true;
         }
      }
   }
   return false;
}

///Get information relative to the index
QVariant CallModel::data( const QModelIndex& idx, int role) const
{
   if (!idx.isValid())
      return QVariant();
   Call* call = nullptr;
   if (!idx.parent().isValid() && m_lInternalModel.size() > idx.row() && m_lInternalModel[idx.row()])
      call = m_lInternalModel[idx.row()]->call_real;
   else if (idx.parent().isValid() && m_lInternalModel.size() > idx.parent().row()) {
      InternalStruct* intList = m_lInternalModel[idx.parent().row()];
      if (intList->conference == true && intList->m_lChildren.size() > idx.row() && intList->m_lChildren[idx.row()])
         call = intList->m_lChildren[idx.row()]->call_real;
   }
   return call?call->roleData((Call::Role)role):QVariant();
}

///Header data
QVariant CallModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   Q_UNUSED(section)
   if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
      return QVariant(tr("Calls"));
   return QVariant();
}

///The number of conference and stand alone calls
int CallModel::rowCount( const QModelIndex& parentIdx ) const
{
   if (!parentIdx.isValid()) {
      return m_lInternalModel.size();
   }
   else if (!parentIdx.parent().isValid()) {
      return m_lInternalModel[parentIdx.row()]->m_lChildren.size();
   }
   return 0;
}

///Make everything selectable and drag-able
Qt::ItemFlags CallModel::flags( const QModelIndex& idx ) const
{
   if (!idx.isValid())
      return Qt::NoItemFlags;
   return Qt::ItemIsEnabled|Qt::ItemIsSelectable 
      | Qt::ItemIsDragEnabled 
      | ((!idx.data(Call::Role::IsConference).toBool())?(Qt::ItemIsDropEnabled):Qt::ItemIsEnabled)
      | ((idx.data(Call::Role::CallState) == static_cast<int>(Call::State::DIALING))?Qt::ItemIsEditable:Qt::NoItemFlags);
}

///Return valid payload types
int CallModel::acceptedPayloadTypes()
{
   return DropPayloadType::CALL | DropPayloadType::HISTORY | DropPayloadType::CONTACT | DropPayloadType::NUMBER | DropPayloadType::TEXT;
}

///There is always 1 column
int CallModel::columnCount ( const QModelIndex& parentIdx) const
{
   Q_UNUSED(parentIdx)
   return 1;
}

///Return the conference if 'index' is part of one
QModelIndex CallModel::parent( const QModelIndex& idx) const
{
   if (!idx.isValid())
      return QModelIndex();
   const InternalStruct* modelItem = (InternalStruct*)idx.internalPointer();
   if (modelItem && modelItem->m_pParent) {
      const int rowidx = m_lInternalModel.indexOf(modelItem->m_pParent);
      if (rowidx != -1) {
         return CallModel::index(rowidx,0,QModelIndex());
      }
   }
   return QModelIndex();
}

///Get the call index at row,column (active call only)
QModelIndex CallModel::index( int row, int column, const QModelIndex& parentIdx) const
{
   if (row >= 0 && !parentIdx.isValid() && m_lInternalModel.size() > row) {
      return createIndex(row,column,m_lInternalModel[row]);
   }
   else if (row >= 0 && parentIdx.isValid() && m_lInternalModel[parentIdx.row()]->m_lChildren.size() > row) {
      return createIndex(row,column,m_lInternalModel[parentIdx.row()]->m_lChildren[row]);
   }
   return QModelIndex();
}

QStringList CallModel::mimeTypes() const
{
   static QStringList mimes;
   if (!mimes.size()) {
      mimes << MIME_PLAIN_TEXT << MIME_PHONENUMBER << MIME_CALLID << "text/html";
   }
   return mimes;
}

QMimeData* CallModel::mimeData(const QModelIndexList& indexes) const
{
   QMimeData* mData = new QMimeData();
   foreach (const QModelIndex &idx, indexes) {
      if (idx.isValid()) {
         QString text = data(idx, Call::Role::Number).toString();
         mData->setData(MIME_PLAIN_TEXT , text.toUtf8());
         mData->setData(MIME_PHONENUMBER, text.toUtf8());
         qDebug() << "Setting mime" << idx.data(Call::Role::Id).toString();
         mData->setData(MIME_CALLID  , idx.data(Call::Role::Id).toString().toUtf8());
         return mData;
      }
   }
   return mData;
}

bool CallModel::isPartOf(const QModelIndex& confIdx, Call* call)
{
   if (!confIdx.isValid() || !call) return false;

   for (int i=0;i<confIdx.model()->rowCount(confIdx);i++) { //TODO use model one directly
      if (confIdx.child(i,0).data(Call::Role::Id) == call->id()) {
         return true;
      }
   }
   return false;
}

bool CallModel::dropMimeData(const QMimeData* mimedata, Qt::DropAction action, int row, int column, const QModelIndex& parentIdx )
{
   Q_UNUSED(action)
   if (mimedata->hasFormat(MIME_CALLID)) {
      const QByteArray encodedCallId = mimedata->data( MIME_CALLID    );
      const QModelIndex targetIdx    = index   ( row,column,parentIdx );
      Call* call                     = getCall ( encodedCallId        );
      Call* target                   = getCall ( targetIdx            );

      //Call or conference dropped on itself -> cannot transfer or merge, so exit now
      if (target == call) {
         qDebug() << "Call/Conf dropped on itself (doing nothing)";
         return false;
      }
      else if (!call) {
         qDebug() << "Call not found";
         return false;
      }

      switch (mimedata->property("dropAction").toInt()) {
         case Call::DropAction::Conference:
            //Call or conference dropped on part of itself -> cannot merge conference with itself
            if (isPartOf(targetIdx,call) || isPartOf(targetIdx.parent(),call) || (call && targetIdx.parent().data(Call::Role::Id) == encodedCallId)) {
               qDebug() << "Call/Conf dropped on its own conference (doing nothing)";
               return false;
            }
            //Conference dropped on a conference -> merge both conferences
            else if (call && target && call->isConference() && target->isConference()) {
               qDebug() << "Merge conferences" << call->confId() << "and" << target->confId();
               mergeConferences(call,target);
               return true;
            }
            //Conference dropped on a call part of a conference -> merge both conferences
            else if (call && call->isConference() && targetIdx.parent().isValid()) {
               qDebug() << "Merge conferences" << call->confId() << "and" << targetIdx.parent().data(Call::Role::Id).toString();
               mergeConferences(call,getCall(targetIdx.parent()));
               return true;
            }
            //Drop a call on a conference -> add it to the conference
            else if (target && (targetIdx.parent().isValid() || target->isConference())) {
               Call* conf = target->isConference()?target:qvariant_cast<Call*>(targetIdx.parent().data(Call::Role::Object));
               if (conf) {
                  qDebug() << "Adding call " << call->id() << "to conference" << conf->confId();
                  addParticipant(call,conf);
               return true;
               }
            }
            //Conference dropped on a call
            else if (target && call && rowCount(getIndex(call))) {
               qDebug() << "Conference dropped on a call: adding call to conference";
               addParticipant(target,call);
               return true;
            }
            //Call dropped on a call
            else if (call && target && !targetIdx.parent().isValid()) {
               qDebug() << "Call dropped on a call: creating a conference";
               createConferenceFromCall(call,target);
               return true;
            }
            break;
         case Call::DropAction::Transfer:
            qDebug() << "Performing an attended transfer";
            attendedTransfer(call,target);
            break;
         default:
            break;
      }
   }
   else if (mimedata->hasFormat(MIME_PHONENUMBER)) {
      const QByteArray encodedPhoneNumber = mimedata->data( MIME_PHONENUMBER );
      Call* target = getCall(index(row,column,parentIdx));
      qDebug() << "Phone number" << encodedPhoneNumber << "on call" << target;
      Call* newCall = addDialingCall(QString(),target->account());
      newCall->setDialNumber(encodedPhoneNumber);
      newCall->performAction(Call::Action::ACCEPT);
      createConferenceFromCall(newCall,target);
   }
   else if (mimedata->hasFormat(MIME_CONTACT)) {
      const QByteArray encodedContact = mimedata->data(MIME_CONTACT);
      Call* target = getCall(index(row,column,parentIdx));
      qDebug() << "Contact" << encodedContact << "on call" << target;
      if (PhoneNumberSelector::defaultVisitor()) {
         const PhoneNumber* number = PhoneNumberSelector::defaultVisitor()->getNumber(
            Call::contactBackend()->getContactByUid(encodedContact));
         if (!number->uri().isEmpty()) {
            Call* newCall = addDialingCall();
            newCall->setDialNumber(number);
            newCall->performAction(Call::Action::ACCEPT);
            createConferenceFromCall(newCall,target);
         }
         else {
            qDebug() << "Contact not found";
         }
      }
      else
         qDebug() << "There is nothing to handle contact";
   }
   return false;
}


/*****************************************************************************
 *                                                                           *
 *                                   Slots                                   *
 *                                                                           *
 ****************************************************************************/

///When a call state change
void CallModel::slotCallStateChanged(const QString& callID, const QString& state)
{
   //This code is part of the CallModel interface too
   qDebug() << "Call State Changed for call  " << callID << " . New state : " << state;
   InternalStruct* internal = m_sPrivateCallList_callId[callID];
   Call* call = nullptr;
   Call::State previousState = Call::State::RINGING;
   if(!internal) {
      qDebug() << "Call not found";
      if(state == Call::StateChange::RINGING) {
         call = addRingingCall(callID);
      }
      else {
         qDebug() << "Call doesn't exist in this client. Might have been initialized by another client instance before this one started.";
         return;
      }
   }
   else {
      call = internal->call_real;
      previousState = call->state();
      qDebug() << "Call found" << call << call->state();
      call->stateChanged(state);
      if (state == Call::StateChange::HUNG_UP)
         removeCall(call);
   }

   if (call->state() == Call::State::OVER) {
      HistoryModel::instance()->add(call);
   }

   emit callStateChanged(call,previousState);

} //slotCallStateChanged

///When a new call is incoming
void CallModel::slotIncomingCall(const QString& accountID, const QString& callID)
{
   Q_UNUSED(accountID)
   qDebug() << "Signal : Incoming Call ! ID = " << callID;
   emit incomingCall(addIncomingCall(callID));
}

///When a new conference is incoming
void CallModel::slotIncomingConference(const QString& confID)
{
   if (!getCall(confID)) {
      Call* conf = addConference(confID);
      qDebug() << "Adding conference" << conf << confID;
      emit conferenceCreated(conf);
   }
}

///When a conference change
void CallModel::slotChangingConference(const QString &confID, const QString& state)
{
   InternalStruct* confInt = m_sPrivateCallList_callId[confID];
   if (!confInt) {
      qDebug() << "Error: conference not found";
      return;
   }
   Call* conf = confInt->call_real;
   qDebug() << "Changing conference state" << conf << confID;
   if (conf && dynamic_cast<Call*>(conf)) { //Prevent a race condition between call and conference
      if (!changeConference(confID, state)) {
         qDebug() << "Changing conference failed";
         return;
      }
      conf->stateChanged(state);
      CallManagerInterface& callManager = DBus::CallManager::instance();
      QStringList participants = callManager.getParticipantList(confID);

      foreach(InternalStruct* child,confInt->m_lChildren) {
         if (participants.indexOf(child->call_real->id()) == -1 && child->call_real->state() != Call::State::OVER)
            m_lInternalModel << child;
      }
      confInt->m_lChildren.clear();
      foreach(const QString& callId,participants) {
         InternalStruct* callInt = m_sPrivateCallList_callId[callId];
         if (callInt) {
            if (callInt->m_pParent && callInt->m_pParent != confInt)
               callInt->m_pParent->m_lChildren.removeAll(callInt);
            m_lInternalModel.removeAll(callInt);
            callInt->m_pParent = confInt;
            confInt->m_lChildren << callInt;
         }
         else {
            qDebug() << "Participants not found";
         }
      }

      //The daemon often fail to emit the right signal, cleanup manually
      foreach(InternalStruct* topLevel, m_lInternalModel) {
         if (topLevel->call_real->isConference() && !topLevel->m_lChildren.size())
            removeConference(topLevel->call_real);
      }

      const QModelIndex idx = index(m_lInternalModel.indexOf(confInt),0,QModelIndex());
      emit dataChanged(idx, idx);
      emit layoutChanged();
      emit conferenceChanged(conf);
   }
   else {
      qDebug() << "Trying to affect a conference that does not exist (anymore)";
   }
} //slotChangingConference

///When a conference is removed
void CallModel::slotConferenceRemoved(const QString &confId)
{
   Call* conf = getCall(confId);
   emit aboutToRemoveConference(conf);
   removeConference(confId);
   emit conferenceRemoved(conf);
}

///When a new voice mail is available
void CallModel::slotVoiceMailNotify(const QString &accountID, int count)
{
   qDebug() << "Signal : VoiceMail Notify ! " << count << " new voice mails for account " << accountID;
   emit voiceMailNotify(accountID,count);
}

///When the daemon change the volume
void CallModel::slotVolumeChanged(const QString & device, double value)
{
   emit volumeChanged(device,value);
}

///Make the call aware it has a recording
void CallModel::slotNewRecordingAvail( const QString& callId, const QString& filePath)
{
   getCall(callId)->setRecordingPath(filePath);
}

#ifdef ENABLE_VIDEO
///Updating call state when video is added
void CallModel::slotStartedDecoding(const QString& callId, const QString& shmKey)
{
   Q_UNUSED(callId)
   Q_UNUSED(shmKey)
}

///Updating call state when video is removed
void CallModel::slotStoppedDecoding(const QString& callId, const QString& shmKey)
{
   Q_UNUSED(callId)
   Q_UNUSED(shmKey)
}
#endif

///Update model if the data change
void CallModel::slotCallChanged(Call* call)
{
   //Transfer is "local" state, it doesn't require the daemon, so it need to be
   //handled "manually" instead of relying on the backend signals
   if (call->state() == Call::State::TRANSFERRED) {
      emit callStateChanged(call, Call::State::TRANSFERRED);
   }
   InternalStruct* callInt = m_sPrivateCallList_call[call];
   if (callInt) {
      const int idxOf = m_lInternalModel.indexOf(callInt);
      if (idxOf != -1) {
         const QModelIndex idx = index(idxOf,0,QModelIndex());
         emit dataChanged(idx,idx);
      }
   }
}

///Add call slot
void CallModel::slotAddPrivateCall(Call* call) {
   addCall(call,nullptr);
}

///Notice views that a dtmf have been played
void CallModel::slotDTMFPlayed( const QString& str )
{
   Call* call = qobject_cast<Call*>(QObject::sender());
   if (str.size()==1) {
      int idx = 0;
      char s = str.toLower().toAscii()[0];
      if (s >= '1' && s <= '9'     ) idx = s - '1'     ;
      else if (s >= 'a' && s <= 'v') idx = (s - 'a')/3 ;
      else if (s >= 'w' && s <= 'z') idx = 8           ;
      else if (s == '0'            ) idx = 10          ;
      else if (s == '*'            ) idx = 9           ;
      else if (s == '#'            ) idx = 11          ;
      else                           idx = -1          ;
      call->setProperty("latestDtmfIdx",idx);
   }
   const QModelIndex& idx = getIndex(call);
   setData(idx,50, Call::Role::DTMFAnimState);
}

///Called when a recording state change
void CallModel::slotRecordStateChanged (const QString& callId, bool state)
{
   Call* call = getCall(callId);
   if (call) {
      call->m_Recording = state;
      emit call->changed();
      emit call->changed(call);
   }
}
