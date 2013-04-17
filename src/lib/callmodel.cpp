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
#include <callmodel.h>

//Qt
#include <QtCore/QHash>
#include <QtCore/QDebug>
#include <QtGui/QDragEnterEvent>

//SFLPhone library
#include "call.h"
#include "accountlist.h"
#include "dbus/metatypes.h"
#include "callmanager_interface_singleton.h"
#include "configurationmanager_interface_singleton.h"
#include "instance_interface_singleton.h"
#include "sflphone_const.h"
#include "typedefs.h"
#include "contactbackend.h"

//System
#include "unistd.h"

//Define

//Static member
 bool         CallModel::m_sInstanceInit        = false     ;
 bool         CallModelBase::m_sCallInit            = false     ;
 CallMap      CallModelBase::m_lConfList            = CallMap() ;
 CallModel*   CallModel::m_spInstance           = nullptr   ;

 typename CallModelBase::InternalCall   CallModelBase::m_sPrivateCallList_call   ;
 typename CallModelBase::InternalCallId CallModelBase::m_sPrivateCallList_callId ;
 typename CallModelBase::InternalIndex  CallModelBase::m_sPrivateCallList_index  ;

//Parent
#include "video_interface_singleton.h"
#include "historymodel.h"

bool CallModelBase::dbusInit = false;
CallMap CallModelBase::m_sActiveCalls;

///Constructor
CallModelBase::CallModelBase(QObject* parent) : QAbstractItemModel(parent)
{
   if (!dbusInit) {
      CallManagerInterface& callManager = CallManagerInterfaceSingleton::getInstance();
      #ifdef ENABLE_VIDEO
      VideoInterface& interface = VideoInterfaceSingleton::getInstance();
      #endif

      //SLOTS
      /*             SENDER                          SIGNAL                     RECEIVER                    SLOT                   */
      /**/connect(&callManager, SIGNAL(callStateChanged(QString,QString))       , this , SLOT(callStateChanged(QString,QString))   );
      /**/connect(&callManager, SIGNAL(incomingCall(QString,QString,QString))   , this , SLOT(incomingCall(QString,QString))       );
      /**/connect(&callManager, SIGNAL(conferenceCreated(QString))              , this , SLOT(incomingConference(QString))         );
      /**/connect(&callManager, SIGNAL(conferenceChanged(QString,QString))      , this , SLOT(changingConference(QString,QString)) );
      /**/connect(&callManager, SIGNAL(conferenceRemoved(QString))              , this , SLOT(conferenceRemovedSlot(QString))      );
      /**/connect(&callManager, SIGNAL(voiceMailNotify(QString,int))            , this , SLOT(voiceMailNotifySlot(QString,int))    );
      /**/connect(&callManager, SIGNAL(volumeChanged(QString,double))           , this , SLOT(volumeChangedSlot(QString,double))   );
      /**/connect(&callManager, SIGNAL(recordPlaybackFilepath(QString,QString)) , this , SLOT(newRecordingAvail(QString,QString))  );
      #ifdef ENABLE_VIDEO
      /**/connect(&interface  , SIGNAL(startedDecoding(QString,QString,int,int)), this , SLOT(startedDecoding(QString,QString))    );
      /**/connect(&interface  , SIGNAL(stoppedDecoding(QString,QString))        , this , SLOT(stoppedDecoding(QString,QString))    );
      #endif
      /*                                                                                                                           */

      connect(HistoryModel::self(),SIGNAL(newHistoryCall(Call*)),this,SLOT(addPrivateCall(Call*)));

      dbusInit = true;

      foreach(Call* call,HistoryModel::getHistory()){
         addCall(call,0);
      }
   }
}

///Destructor
CallModelBase::~CallModelBase()
{
   //if (m_spAccountList) delete m_spAccountList;
}

///When a call state change
void CallModelBase::callStateChanged(const QString &callID, const QString &state)
{
   //This code is part of the CallModel interface too
   qDebug() << "Call State Changed for call  " << callID << " . New state : " << state;
   Call* call = findCallByCallId(callID);
   if(!call) {
      qDebug() << "Call not found";
      if(state == CALL_STATE_CHANGE_RINGING) {
         call = addRingingCall(callID);
      }
      else {
         qDebug() << "Call doesn't exist in this client. Might have been initialized by another client instance before this one started.";
         return;
      }
   }
   else {
      qDebug() << "Call found" << call;
      call->stateChanged(state);
   }

   if (call->getCurrentState() == CALL_STATE_OVER) {
      HistoryModel::add(call);
   }

   emit callStateChanged(call);

}


/*****************************************************************************
 *                                                                           *
 *                                   Slots                                   *
 *                                                                           *
 ****************************************************************************/

///When a new call is incoming
void CallModelBase::incomingCall(const QString & accountID, const QString & callID)
{
   Q_UNUSED(accountID)
   qDebug() << "Signal : Incoming Call ! ID = " << callID;
   Call* call = addIncomingCall(callID);

   emit incomingCall(call);
}

///When a new conference is incoming
void CallModelBase::incomingConference(const QString &confID)
{
   Call* conf = addConference(confID);
   qDebug() << "Adding conference" << conf << confID;
   emit conferenceCreated(conf);
}

///When a conference change
void CallModelBase::changingConference(const QString &confID, const QString &state)
{
   InternalStruct* confInt = m_sPrivateCallList_callId[confID];
   Call* conf = confInt->call_real;
   qDebug() << "Changing conference state" << conf << confID;
   if (conf && dynamic_cast<Call*>(conf)) { //Prevent a race condition between call and conference
      changeConference(confID, state);
      conf->stateChanged(state);
      CallManagerInterface& callManager = CallManagerInterfaceSingleton::getInstance();
      QStringList participants = callManager.getParticipantList(confID);
      foreach(QString callId,participants) {
         InternalStruct* callInt = m_sPrivateCallList_callId[callId];
         if (callInt->m_pParent != confInt)
            callInt->m_pParent->m_lChildren.removeAll(callInt);
         m_lInternalModel.removeAll(callInt);
         callInt->m_pParent = confInt;
         if (confInt->m_lChildren.indexOf(callInt) == -1)
            confInt->m_lChildren << callInt;
      }

      const QModelIndex idx = index(m_lInternalModel.indexOf(confInt),0,QModelIndex());
      emit dataChanged(idx, idx);
      emit layoutChanged();
      emit conferenceChanged(conf);
   }
   else {
      qDebug() << "Trying to affect a conference that does not exist (anymore)";
   }
}

///When a conference is removed
void CallModelBase::conferenceRemovedSlot(const QString &confId)
{
   Call* conf = getCall(confId);
   emit aboutToRemoveConference(conf);
   removeConference(confId);
   emit conferenceRemoved(conf);
}

///When a new voice mail is available
void CallModelBase::voiceMailNotifySlot(const QString &accountID, int count)
{
   qDebug() << "Signal : VoiceMail Notify ! " << count << " new voice mails for account " << accountID;
   emit voiceMailNotify(accountID,count);
}

///When the daemon change the volume
void CallModelBase::volumeChangedSlot(const QString & device, double value)
{
   emit volumeChanged(device,value);
}

///Add a call to the model (reimplemented in .hpp)
Call* CallModelBase::addCall(Call* call, Call* parent)
{
   if (call->getCurrentState() != CALL_STATE_OVER)
      emit callAdded(call,parent);

   connect(call, SIGNAL(isOver(Call*)), this, SLOT(removeActiveCall(Call*)));
   return call;
}

///Emit conference created signal
Call* CallModelBase::addConferenceS(Call* conf)
{
   emit conferenceCreated(conf);
   return conf;
}

///Remove it from active calls
void CallModelBase::removeActiveCall(Call* call)
{
   Q_UNUSED(call);
   //There is a race condition
   //m_sActiveCalls[call->getCallId()] = nullptr;
}

///Make the call aware it has a recording
void CallModelBase::newRecordingAvail( const QString& callId, const QString& filePath)
{
   Call* call = getCall(callId);
   call->setRecordingPath(filePath);
}

#ifdef ENABLE_VIDEO
///Updating call state when video is added
void CallModelBase::startedDecoding(const QString& callId, const QString& shmKey  )
{
   Q_UNUSED(callId)
   Q_UNUSED(shmKey)
}

///Updating call state when video is removed
void CallModelBase::stoppedDecoding(const QString& callId, const QString& shmKey)
{
   Q_UNUSED(callId)
   Q_UNUSED(shmKey)
}
#endif

///Update model if the data change
void CallModelBase::callChanged(Call* call)
{
   InternalStruct* callInt = m_sPrivateCallList_call[call];
   if (callInt) {
      int idxOf = m_lInternalModel.indexOf(callInt);
      if (idxOf != -1) {
         const QModelIndex idx = index(idxOf,0,QModelIndex());
         emit dataChanged(idx,idx);
      }
   }
}

/*****************************************************************************
 *                                                                           *
 *                                  Getter                                   *
 *                                                                           *
 ****************************************************************************/

///Return a list of registered accounts
// AccountList* CallModelBase::getAccountList()
// {
//    if (m_spAccountList == NULL) {
//       m_spAccountList = new AccountList(true);
//    }
//    return m_spAccountList;
// }

bool CallModelBase::isValid()
{
   CallManagerInterface& callManager = CallManagerInterfaceSingleton::getInstance();
   return (callManager.isValid());
}


/*****************************************************************************
 *                                                                           *
 *                                  Setters                                  *
 *                                                                           *
 ****************************************************************************/

///Add call slot
void CallModelBase::addPrivateCall(Call* call) {
   addCall(call,0);
}

//More code in CallModel.hpp







































/*****************************************************************************
 *                                                                           *
 *                               Constructor                                 *
 *                                                                           *
 ****************************************************************************/

///Retrieve current and older calls from the daemon, fill history and the calls TreeView and enable drag n' drop
 CallModel::CallModel() : CallModelBase(0)
{
   m_spInstance = this;
   init();
}

///Static destructor
 void CallModel::destroy()
{
   foreach (Call* call,  m_sPrivateCallList_call.keys()) {
      delete call;
   }
   foreach (InternalStruct* s,  m_sPrivateCallList_call.values()) {
      delete s;
   }
   m_sPrivateCallList_call.clear  ();
   m_sPrivateCallList_callId.clear();
//    m_sPrivateCallList_widget.clear();
   m_sPrivateCallList_index.clear ();
}

///Destructor
 CallModel::~CallModel()
{
   
}

///Open the connection to the daemon and register this client
bool CallModel::init()
{
   if (!m_sInstanceInit)
      registerCommTypes();
   m_sInstanceInit = true;
   return true;
} //init

///Fill the call list
///@warning This solution wont scale to multiple call or history model implementation. Some static addCall + foreach for each call would be needed if this case ever become unavoidable
bool CallModel::initCall()
{
   if (!m_sCallInit) {
      CallManagerInterface& callManager = CallManagerInterfaceSingleton::getInstance();
      const QStringList callList = callManager.getCallList();
      foreach (const QString& callId, callList) {
         Call* tmpCall = Call::buildExistingCall(callId);
         m_sActiveCalls[tmpCall->getCallId()] = tmpCall;
         addCall(tmpCall);
      }

      const QStringList confList = callManager.getConferenceList();
      foreach (const QString& confId, confList) {
         CallModelBase::addConferenceS(addConference(confId));
      }
   }
   m_sCallInit = true;
   return true;
} //initCall


/*****************************************************************************
 *                                                                           *
 *                         Access related functions                          *
 *                                                                           *
 ****************************************************************************/

///Return the active call count
 int CallModel::size()
{
   return m_sActiveCalls.size();
}

///Return a call corresponding to this ID or NULL
 Call* CallModel::findCallByCallId(const QString& callId)
{
   return m_sActiveCalls[callId];
}

///Return the action call list
 CallList CallModel::getCallList()
{
   CallList callList;
   foreach(Call* call, m_sActiveCalls) {
      if (dynamic_cast<Call*>(call) && call->getState() != CALL_STATE_OVER) //Prevent a race
         callList.push_back(call);
   }
   return callList;
}

///Return all conferences
 CallList CallModel::getConferenceList()
{
   CallList confList;

   //That way it can not be invalid
   const QStringList confListS = CallManagerInterfaceSingleton::getInstance().getConferenceList();
   foreach (const QString& confId, confListS) {
      if (m_lConfList[confId] != nullptr)
         confList << m_lConfList[confId];
      else
         confList << addConference(confId);
   }
   return confList;
}


/*****************************************************************************
 *                                                                           *
 *                            Call related code                              *
 *                                                                           *
 ****************************************************************************/

///Add a call in the model structure, the call must exist before being added to the model
 Call* CallModel::addCall(Call* call, Call* parent)
{
   Q_UNUSED(parent)
   if (!call)
      return new Call("",""); //Invalid, but better than managing NULL everywhere

   InternalStruct* aNewStruct = new InternalStruct;
   aNewStruct->call_real  = call;
   aNewStruct->conference = false;

   m_sPrivateCallList_call  [ call              ] = aNewStruct;
   m_lInternalModel << aNewStruct;
   m_sPrivateCallList_callId[ call->getCallId() ] = aNewStruct;

   //setCurrentItem(callItem);
   CallModelBase::addCall(call,parent);
   const QModelIndex idx = index(m_lInternalModel.size()-1,0,QModelIndex());
   emit dataChanged(idx, idx);
   connect(call,SIGNAL(changed(Call*)),this,SLOT(callChanged(Call*)));
   return call;
}

///Common set of instruction shared by all call adder
 Call* CallModel::addCallCommon(Call* call)
{
   m_sActiveCalls[call->getCallId()] = call;
   addCall(call);
   return call;
} //addCallCommon

///Create a new dialing call from peer name and the account ID
 Call* CallModel::addDialingCall(const QString& peerName, Account* account)
{
   Account* acc = (account)?account:AccountList::getCurrentAccount();
   if (acc) {
      Call* call = Call::buildDialingCall(generateCallId(), peerName, acc->getAccountId());
      return addCallCommon(call);
   }
   else {
      return nullptr;
   }
}  //addDialingCall

///Create a new incoming call when the daemon is being called
 Call* CallModel::addIncomingCall(const QString& callId)
{
   Call* call = Call::buildIncomingCall(callId);
   Call* call2 = addCallCommon(call);
   //Call without account is not possible
   if (dynamic_cast<Account*>(call2->getAccount())) {
      if (call2 && call2->getAccount()->isAutoAnswer()) {
         call2->actionPerformed(CALL_ACTION_ACCEPT);
      }
   }
   else {
      qDebug() << "Incoming call from an invalid account";
      throw "Invalid account";
   }
   return call2;
}

///Create a ringing call
 Call* CallModel::addRingingCall(const QString& callId)
{
   Call* call = Call::buildRingingCall(callId);
   return addCallCommon(call);
}

///Generate a new random call unique identifier (callId)
 QString CallModel::generateCallId()
{
   int id = qrand();
   return QString::number(id);
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

//    if (m_sPrivateCallList_widget[m_sPrivateCallList_widget.key(internal)] == internal) {
//       m_sPrivateCallList_widget.remove(m_sPrivateCallList_widget.key(internal));
//    }

   if (m_sPrivateCallList_index[m_sPrivateCallList_index.key(internal)] == internal) {
      m_sPrivateCallList_index.remove(m_sPrivateCallList_index.key(internal));
   }
} //removeCall

///Transfer "toTransfer" to "target" and wait to see it it succeeded
 void CallModel::attendedTransfer(Call* toTransfer, Call* target)
{
   CallManagerInterface& callManager = CallManagerInterfaceSingleton::getInstance();
   Q_NOREPLY callManager.attendedTransfer(toTransfer->getCallId(),target->getCallId());

   //TODO [Daemon] Implement this correctly
   toTransfer->changeCurrentState(CALL_STATE_OVER);
   target->changeCurrentState(CALL_STATE_OVER);
} //attendedTransfer

///Transfer this call to  "target" number
 void CallModel::transfer(Call* toTransfer, QString target)
{
   qDebug() << "Transferring call " << toTransfer->getCallId() << "to" << target;
   toTransfer->setTransferNumber ( target                 );
   toTransfer->changeCurrentState( CALL_STATE_TRANSFERRED );
   toTransfer->actionPerformed   ( CALL_ACTION_TRANSFER   );
   toTransfer->changeCurrentState( CALL_STATE_OVER        );
} //transfer

/*****************************************************************************
 *                                                                           *
 *                         Conference related code                           *
 *                                                                           *
 ****************************************************************************/

///Add a new conference, get the call list and update the interface as needed
Call* CallModel::addConference(const QString & confID)
{
   qDebug() << "Notified of a new conference " << confID;
   CallManagerInterface& callManager = CallManagerInterfaceSingleton::getInstance();
   const QStringList callList = callManager.getParticipantList(confID);
   qDebug() << "Paticiapants are:" << callList;

   if (!callList.size()) {
      qDebug() << "This conference (" + confID + ") contain no call";
      return 0;
   }

   if (!m_sPrivateCallList_callId[callList[0]]) {
      qDebug() << "Invalid call";
      return 0;
   }

   Call* newConf = nullptr;
   if (m_sPrivateCallList_callId[callList[0]]->call_real->getAccount())
      newConf =  new Call(confID, m_sPrivateCallList_callId[callList[0]]->call_real->getAccount()->getAccountId());

   if (newConf) {
      InternalStruct* aNewStruct = new InternalStruct;
      aNewStruct->call_real  = newConf;
      aNewStruct->conference = true;

      m_sPrivateCallList_call[newConf]  = aNewStruct;
      m_sPrivateCallList_callId[confID] = aNewStruct;
      m_lInternalModel << aNewStruct;

      m_lConfList[newConf->getConfId()] = newConf   ;
      foreach(QString callId,callList) {
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
      connect(newConf,SIGNAL(changed(Call*)),this,SLOT(callChanged(Call*)));
   }
   
   return newConf;
} //addConference

///Join two call to create a conference, the conference will be created later (see addConference)
bool CallModel::createConferenceFromCall(Call* call1, Call* call2)
{
  qDebug() << "Joining call: " << call1->getCallId() << " and " << call2->getCallId();
  CallManagerInterface &callManager = CallManagerInterfaceSingleton::getInstance();
  Q_NOREPLY callManager.joinParticipant(call1->getCallId(),call2->getCallId());
  return true;
} //createConferenceFromCall

///Add a new participant to a conference
bool CallModel::addParticipant(Call* call2, Call* conference)
{
   if (conference->isConference()) {
      CallManagerInterface& callManager = CallManagerInterfaceSingleton::getInstance();
      Q_NOREPLY callManager.addParticipant(call2->getCallId(), conference->getConfId());
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
   CallManagerInterface& callManager = CallManagerInterfaceSingleton::getInstance();
   Q_NOREPLY callManager.detachParticipant(call->getCallId());
   return true;
}

///Merge two conferences
bool CallModel::mergeConferences(Call* conf1, Call* conf2)
{
   CallManagerInterface& callManager = CallManagerInterfaceSingleton::getInstance();
   Q_NOREPLY callManager.joinConference(conf1->getConfId(),conf2->getConfId());
   return true;
}

///Executed when the daemon signal a modification in an existing conference. Update the call list and update the TreeView
bool CallModel::changeConference(const QString& confId, const QString& state)
{
   qDebug() << "Conf changed";
   Q_UNUSED(state)

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
   InternalStruct* internal = m_sPrivateCallList_call[call];

   if (!internal) {
      qDebug() << "Cannot remove conference: call not found";
      return;
   }
   removeCall(call);

   m_lInternalModel.removeAll(internal);
   m_lConfList[call->getConfId()] = nullptr;
}


/*****************************************************************************
 *                                                                           *
 *                             Magic Dispatcher                              *
 *                                                                           *
 ****************************************************************************/

///Get a call from it's widget
//  Call* CallModel::getCall         ( const CallWidget widget     ) const
// {
//    if (m_sPrivateCallList_widget[widget]) {
//       return m_sPrivateCallList_widget[widget]->call_real;
//    }
//    return nullptr;
// }

///Get a call list from a conference
//  QList<Call*> CallModel::getCalls ( const CallWidget widget     ) const
// {
//    QList<Call*> toReturn;
//    if (m_sPrivateCallList_widget[widget] && m_sPrivateCallList_widget[widget]->conference) {
//       foreach (InternalStruct* child, m_sPrivateCallList_widget[widget]->children) {
//          toReturn << child.call_real;
//       }
//    }
//    return toReturn;
// }

///Get a list of every call
 QList<Call*> CallModel::getCalls (                             )
{
   QList<Call*> toReturn;
   foreach (InternalStruct* child, m_sPrivateCallList_call) {
      toReturn << child->call_real;
   }
   return toReturn;
}

///Is the call associated with that widget a conference
//  bool CallModel::isConference     ( const CallWidget widget      ) const
// {
//    if (m_sPrivateCallList_widget[widget]) {
//       return m_sPrivateCallList_widget[widget]->conference;
//    }
//    return false;
// }

///Is that call a conference
 bool CallModel::isConference     ( const Call* call             ) const
{
   if (m_sPrivateCallList_call[(Call*)call]) {
      return m_sPrivateCallList_call[(Call*)call]->conference;
   }
   return false;
}

///Do nothing, provided for API consistency
 Call* CallModel::getCall         ( const Call* call             ) const
{ 
   return (Call*)call;
}

///Return the calls from the "call" conference
 QList<Call*> CallModel::getCalls ( const Call* call             ) const
{ 
   QList<Call*> toReturn;
   if (m_sPrivateCallList_call[(Call*)call] && m_sPrivateCallList_call[(Call*)call]->conference) {
      foreach (InternalStruct* child, m_sPrivateCallList_call[(Call*)call]->m_lChildren) {
         toReturn << child->call_real;
      }
   }
   return toReturn;
}

///Is the call associated with that Index a conference             
 bool CallModel::isConference     ( const QModelIndex& idx              ) const
{ 
   if (m_sPrivateCallList_index[idx]) {
      return m_sPrivateCallList_index[idx]->conference;
   }
   return false;
}

///Get the call associated with this index                         
 Call* CallModel::getCall         ( const QModelIndex& idx              ) const
{ 
   if (m_sPrivateCallList_index[idx]) {
      return m_sPrivateCallList_index[idx]->call_real;
   }
   qDebug() << "Call not found";
   return nullptr;
}

///Get the call associated with that conference index              
 QList<Call*> CallModel::getCalls ( const QModelIndex& idx              ) const
{ 
   QList<Call*> toReturn;
   if (m_sPrivateCallList_index[idx] && m_sPrivateCallList_index[idx]->conference) {
      foreach (InternalStruct* child, m_sPrivateCallList_index[idx]->m_lChildren) {
         toReturn << child->call_real;
      }
   }
   return toReturn;
}

///Is the call associated with that ID a conference                
 bool CallModel::isConference     ( const QString& callId        ) const
{ 
   if (m_sPrivateCallList_callId[callId]) {
      return m_sPrivateCallList_callId[callId]->conference;
   }
   return false;
}

///Get the call associated with this ID                            
 Call* CallModel::getCall         ( const QString& callId        ) const
{ 
   if (m_sPrivateCallList_callId[callId]) {
      return m_sPrivateCallList_callId[callId]->call_real;
   }
   return nullptr;
}

///Get the calls associated with this ID
 QList<Call*> CallModel::getCalls ( const QString& callId        ) const
{
   QList<Call*> toReturn;
   if (m_sPrivateCallList_callId[callId] && m_sPrivateCallList_callId[callId]->conference) {
      foreach (InternalStruct* child, m_sPrivateCallList_callId[callId]->m_lChildren) {
         toReturn << child->call_real;
      }
   }
   return toReturn;
}

///Get the index associated with this call
// QModelIndex CallModel::getIndex        ( const Call* call             ) const
// {
//    if (m_sPrivateCallList_call[(Call*)call]) {
//       return m_sPrivateCallList_call[(Call*)call]->index;
//    }
//    return QModelIndex();
// }

///Get the index associated with this index (dummy implementation) 
// QModelIndex CallModel::getIndex        ( const QModelIndex& idx              ) const
// {
//    if (m_sPrivateCallList_index[idx]) {
//       return m_sPrivateCallList_index[idx]->index;
//    }
//    return QModelIndex();
// }

///Get the index associated with this call                         
//  Index CallModel::getIndex        ( const CallWidget widget      ) const
// {
//    if (m_sPrivateCallList_widget[widget]) {
//       return m_sPrivateCallList_widget[widget]->index;
//    }
//    return nullptr;
// }

///Get the index associated with this ID                           
// QModelIndex CallModel::getIndex        ( const QString& callId        ) const
// {
//    if (m_sPrivateCallList_callId[callId]) {
//       return m_sPrivateCallList_callId[callId]->index;
//    }
//    return QModelIndex();
// }

///Get the widget associated with this call                        
//  CallWidget CallModel::getWidget  ( const Call* call             ) const
// {
//    if (m_sPrivateCallList_call[call]) {
//       return m_sPrivateCallList_call[call]->call;
//    }
//    return nullptr;
// }
// 
// ///Get the widget associated with this ID                          
//  CallWidget CallModel::getWidget  ( const Index idx              ) const
// {
//    if (m_sPrivateCallList_index[idx]) {
//       return m_sPrivateCallList_index[idx]->call;
//    }
//    return nullptr;
// }
// 
// ///Get the widget associated with this widget (dummy)              
//  CallWidget CallModel::getWidget  ( const CallWidget widget      ) const
// {
//    if (m_sPrivateCallList_widget[widget]) {
//       return m_sPrivateCallList_widget[widget]->call;
//    }
//    return nullptr;
// }
// 
// ///Get the widget associated with this ID                          
//  CallWidget CallModel::getWidget  ( const QString& widget        ) const
// {
//    if (m_sPrivateCallList_widget[widget]) {
//       return m_sPrivateCallList_widget[widget]->call;
//    }
//    return nullptr;
// }

///Common set of instruction shared by all gui updater
bool CallModel::updateCommon(Call* call)
{
   if (!m_sPrivateCallList_call[call] && dynamic_cast<Call*>(call)) {
      m_sPrivateCallList_call   [ call              ]             = new InternalStruct            ;
      m_sPrivateCallList_call   [ call              ]->call_real  = call                          ;
      m_sPrivateCallList_call   [ call              ]->conference = false                         ;
      m_sPrivateCallList_callId [ call->getCallId() ]             = m_sPrivateCallList_call[call] ;
      m_lInternalModel << m_sPrivateCallList_call   [ call ];
      const QModelIndex idx = index(m_lInternalModel.size()-1,0,QModelIndex());
      emit dataChanged(idx, idx);
   }
   else
      return false;
   return true;
}

///Update the widget associated with this call                     
//  bool CallModel::updateWidget     (Call* call, CallWidget value )
// {
//    if (!updateCommon(call)) return false;
//    m_sPrivateCallList_call[call]->call = value                         ;
//    m_sPrivateCallList_widget[value]    = m_sPrivateCallList_call[call] ;
//    return true;
// }

///Update the index associated with this call
bool CallModel::updateIndex      (Call* call, const QModelIndex& value      )
{
   updateCommon(call);
   if (!m_sPrivateCallList_call[call])
      return false;
   m_sPrivateCallList_call[call]->index = value                         ;
   m_sPrivateCallList_index[value]      = m_sPrivateCallList_call[call] ;
   return true;
}


/*****************************************************************************
 *                                                                           *
 *                                  Model                                    *
 *                                                                           *
 ****************************************************************************/

bool CallModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

QVariant CallModel::data( const QModelIndex& index, int role) const
{
   if (!index.isValid())
      return QVariant();
   Call* call = nullptr;
   if (!index.parent().isValid()) {
      call = m_lInternalModel[index.row()]->call_real;
   }
   else if (index.parent().isValid()) {
      call = m_lInternalModel[index.parent().row()]->m_lChildren[index.row()]->call_real;
   }
   return call?call->getRoleData((Call::Role)role):QVariant();
}

QVariant CallModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   Q_UNUSED(section)
   if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
      return QVariant("Calls");
   return QVariant();
}

int CallModel::rowCount( const QModelIndex& parent ) const
{
   if (!parent.isValid()) {
      return m_lInternalModel.size();
   }
   else if (!parent.parent().isValid()) {
      return m_lInternalModel[parent.row()]->m_lChildren.size();
   }
   return 0;
}

Qt::ItemFlags CallModel::flags( const QModelIndex& index ) const
{
   if (!index.isValid())
      return 0;
   return Qt::ItemIsEnabled | ((!index.data(Call::Role::IsConference).toBool())?Qt::ItemIsSelectable:Qt::ItemIsEnabled);
}

int CallModel::columnCount ( const QModelIndex& parent) const
{
   Q_UNUSED(parent)
   return 1;
}

QModelIndex CallModel::parent( const QModelIndex& index) const
{
   if (!index.isValid())
      return QModelIndex();
   InternalStruct* modelItem = (InternalStruct*)index.internalPointer();
   if (modelItem && modelItem->m_pParent) {
      int idx = m_lInternalModel.indexOf(modelItem->m_pParent);
      if (idx != -1) {
         return CallModel::index(idx,0,QModelIndex());
      }
   }
   return QModelIndex();
}

QModelIndex CallModel::index( int row, int column, const QModelIndex& parent) const
{
   if (!parent.isValid() && m_lInternalModel.size() > row) {
      return createIndex(row,column,m_lInternalModel[row]);
   }
   else if (parent.isValid() && m_lInternalModel[parent.row()]->m_lChildren.size() > row) {
      return createIndex(row,column,m_lInternalModel[parent.row()]->m_lChildren[row]);
   }
   return QModelIndex();
}