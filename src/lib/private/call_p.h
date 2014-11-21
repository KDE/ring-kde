/****************************************************************************
 *   Copyright (C) 2009-2014 by Savoir-Faire Linux                          *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>          *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
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
#ifndef CALL_PRIVATE_H
#define CALL_PRIVATE_H

#include <QtCore/QObject>
#include "call.h"

//Qt
class QTimer;


//SFLPhone
class Account;
class PhoneNumber;
class UserActionModel;
class InstantMessagingModel;

class CallPrivate;
typedef  void (CallPrivate::*function)();

class CallPrivate : public QObject
{
   Q_OBJECT
public:
   friend class CallModel;

   CallPrivate(Call* parent);

   //Attributes
   Account*                 m_Account         ;
   QString                  m_CallId          ;
   PhoneNumber*             m_pPeerPhoneNumber;
   QString                  m_PeerName        ;
   QString                  m_RecordingPath   ;
   Call::LegacyHistoryState m_HistoryState    ;
   time_t                   m_pStartTimeStamp ;
   time_t                   m_pStopTimeStamp  ;
   Call::State              m_CurrentState    ;
   bool                     m_Recording       ;
   InstantMessagingModel*   m_pImModel        ;
   QTimer*                  m_pTimer          ;
   UserActionModel*         m_pUserActionModel;
   bool                     m_History         ;
   bool                     m_Missed          ;
   Call::Direction          m_Direction       ;
   Call::Type               m_Type            ;
   AbstractHistoryBackend*  m_pBackend        ;

   mutable TemporaryPhoneNumber* m_pTransferNumber ;
   mutable TemporaryPhoneNumber* m_pDialNumber     ;

   //Cache
   HistoryTimeCategoryModel::HistoryConst m_HistoryConst;

   //State machine
   /**
    *  actionPerformedStateMap[orig_state][action]
    *  Map of the states to go to when the action action is 
    *  performed on a call in state orig_state.
   **/
   static const TypedStateMachine< TypedStateMachine< Call::State , Call::Action > , Call::State > actionPerformedStateMap;

   /**
    *  actionPerformedFunctionMap[orig_state][action]
    *  Map of the functions to call when the action action is 
    *  performed on a call in state orig_state.
   **/
   static const TypedStateMachine< TypedStateMachine< function , Call::Action > , Call::State > actionPerformedFunctionMap;

   /**
    *  stateChangedStateMap[orig_state][daemon_new_state]
    *  Map of the states to go to when the daemon sends the signal 
    *  callStateChanged with arg daemon_new_state
    *  on a call in state orig_state.
   **/
   static const TypedStateMachine< TypedStateMachine< Call::State , Call::DaemonState > , Call::State > stateChangedStateMap;

   /**
    *  stateChangedFunctionMap[orig_state][daemon_new_state]
    *  Map of the functions to call when the daemon sends the signal 
    *  callStateChanged with arg daemon_new_state
    *  on a call in state orig_state.
   **/
   static const TypedStateMachine< TypedStateMachine< function , Call::DaemonState > , Call::State > stateChangedFunctionMap;

   /**
    * metaStateTransitionValidationMap help validate if a state transition violate the lifecycle logic.
    * it should technically never happen, but this is an easy additional safety to implement
    * and prevent human (developer) errors.
    */
   static const TypedStateMachine< TypedStateMachine< bool , Call::LifeCycleState > , Call::State > metaStateTransitionValidationMap;

   /**
    * Convert the call state into its meta state (life cycle state). The meta state is a flat,
    * forward only progression from creating to archiving of a call.
    */
   static const TypedStateMachine< Call::LifeCycleState , Call::State > metaStateMap;

   static Call::DaemonState toDaemonCallState   (const QString& stateName);
   static Call::State       confStatetoCallState(const QString& stateName);
   Call::State stateChanged(const QString & newState);
   void performAction(Call::State previousState, Call::Action action);
   void performActionCallback(Call::State previousState, Call::Action action);

   //Automate functions
   // See actionPerformedFunctionMap and stateChangedFunctionMap
   // to know when it is called.
   void nothing      () __attribute__ ((const));
   void error        () __attribute__ ((noreturn));
   void failure      ();
   void accept       ();
   void refuse       ();
   void acceptTransf ();
   void acceptHold   ();
   void hangUp       ();
   void cancel       ();
   void hold         ();
   void call         ();
   void transfer     ();
   void unhold       ();
   void switchRecord ();
   void toggleRecord ();
   void start        ();
   void startStop    ();
   void stop         ();
   void startWeird   ();
   void warning      ();
   void remove       ();

   //Helpers
   void changeCurrentState(Call::State newState);
   void setStartTimeStamp(time_t stamp);
   void initTimer();

private:
   Call* q_ptr;

private Q_SLOTS:
   void stopPlayback(const QString& filePath);
   void updatePlayback(const QString& path,int position,int size);
   void updated();
};

#endif