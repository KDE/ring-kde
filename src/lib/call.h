/****************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                          *
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


#ifndef CALL_H
#define CALL_H

//Qt
#include <QtCore/QDebug>
class QString;
class QTimer;

//SFLPhone
#include "sflphone_const.h"
#include "typedefs.h"
class AbstractContactBackend;
class Contact;
class Account;
class VideoRenderer;
class InstantMessagingModel;

/**
 * @enum history_state
 * This enum have all the state a call can take in the history
 */
typedef enum
{
  INCOMING,
  OUTGOING,
  MISSED  ,
  NONE
} history_state;


class Call;

typedef  void (Call::*function)();

class LIB_EXPORT HistoryTreeBackend {
public:
    enum Type {
        CALL     = 0,
        NUMBER   = 1,
        TOP_LEVEL= 2,
        BOOKMARK = 3,
    };
    explicit HistoryTreeBackend(HistoryTreeBackend::Type _type);
    virtual ~HistoryTreeBackend();
    HistoryTreeBackend::Type type3() const;
    virtual QObject* getSelf() = 0;
    char dropState();
    void setDropState(const char state);
private:
    HistoryTreeBackend::Type m_Type3;
    char m_DropState;
};

/**
 *  This class represents a call either actual (in the call list
 *  displayed in main window), either past (in the call history).
 *  A call is represented by an automate, with a list of states
 *  (enum call_state) and 2 lists of transition signals
 *  (call_action when the user performs an action on the UI and 
 *  daemon_call_state when the daemon sends a stateChanged signal)
 *  When a transition signal is received, the automate calls a
 *  function then go to a new state according to the previous state
 *  of the call and the signal received.
 *  The functions to call and the new states to go to are placed in
 *  the maps actionPerformedStateMap, actionPerformedFunctionMap, 
 *  stateChangedStateMap and stateChangedFunctionMap.
 *  Those maps are used by actionPerformed and stateChanged functions
 *  to handle the behavior of the automate.
 *  When an actual call goes to the state OVER, it becomes part of
 *  the call history.
 *
 *  It may be better to handle call list and call history separately,
 *  and to use the class Item to handle their display, or a model/view
 *  way. For this it needs to handle the becoming of a call to a past call
 *  keeping the information gathered by the call and needed by the history
 *  call (history state, start time...).
**/
class  LIB_EXPORT Call : public QObject, public HistoryTreeBackend
{
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop
public:
   //Enum
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
      IsBookmark    = 110,
      Security      = 111,
      Department    = 112,
      Email         = 113,
      Organisation  = 114,
      Codec         = 115,
      IsConference  = 116,
      Object        = 117,
      PhotoPtr      = 118,
      CallState     = 119,
      Id            = 120,
      StartTime     = 121,
      StopTime      = 122,
      IsRecording   = 123,
      DropState     = 300,
      DTMFAnimState = 400,
      LastDTMFidx   = 401,
   };
   
   enum class State : unsigned int{
      INCOMING        = 0, /** Ringing incoming call */
      RINGING         = 1, /** Ringing outgoing call */
      CURRENT         = 2, /** Call to which the user can speak and hear */
      DIALING         = 3, /** Call which numbers are being added by the user */
      HOLD            = 4, /** Call is on hold */
      FAILURE         = 5, /** Call has failed */
      BUSY            = 6, /** Call is busy */
      TRANSFERRED     = 7, /** Call is being transferred.  During this state, the user can enter the new number. */
      TRANSF_HOLD     = 8, /** Call is on hold for transfer */
      OVER            = 9, /** Call is over and should not be used */
      ERROR           = 10,/** This state should never be reached */
      CONFERENCE      = 11,/** This call is the current conference*/
      CONFERENCE_HOLD = 12,/** This call is a conference on hold*/
      COUNT,
   };
   
   class StateChange {
   public:
      constexpr static const char* HUNG_UP         = "HUNGUP";
      constexpr static const char* RINGING         = "RINGING";
      constexpr static const char* CURRENT         = "CURRENT";
      constexpr static const char* HOLD            = "HOLD";
      constexpr static const char* BUSY            = "BUSY";
      constexpr static const char* FAILURE         = "FAILURE";
      constexpr static const char* UNHOLD_CURRENT  = "UNHOLD";
   };

   class DaemonStateInit {
   public:
      constexpr static const char* CURRENT    = "CURRENT";
      constexpr static const char* HOLD       = "HOLD";
      constexpr static const char* BUSY       = "BUSY";
      constexpr static const char* INCOMING   = "INCOMING";
      constexpr static const char* RINGING    = "RINGING";
      constexpr static const char* INACTIVE   = "INACTIVE";
   };


   /** @enum daemon_call_state_t 
   * This enum have all the states a call can take for the daemon.
   */
   enum class DaemonState : unsigned int
   {
      /** Ringing outgoing or incoming call */         RINGING,
      /** Call to which the user can speak and hear */ CURRENT,
      /** Call is busy */                              BUSY   ,
      /** Call is on hold */                           HOLD   ,
      /** Call is over  */                             HUNG_UP,
      /** Call has failed */                           FAILURE,
//       /** Call is recording+current  */                RECORD ,
      COUNT,
   };

   /** @enum call_action
   * This enum have all the actions you can make on a call.
   */
   enum class Action : unsigned int
   {
      /** Accept, create or place call or place transfer */ ACCEPT  ,
      /** Red button, refuse or hang up */                  REFUSE  ,
      /** Put into or out of transfer mode*/                TRANSFER,
      /** Hold or unhold the call */                        HOLD    ,
      /** Enable or disable recording */                    RECORD  ,
      COUNT,
   };

   enum DropAction {
      Conference = 100,
      Transfer   = 101,
   };

   //Constructors & Destructors
   Call(QString confId, QString account);
   ~Call();
   static Call* buildDialingCall  (QString callId, const QString & peerName, Account* account = nullptr                                                         );
   static Call* buildIncomingCall (const QString & callId                                                                                                       );
   static Call* buildRingingCall  (const QString & callId                                                                                                       );
   static Call* buildHistoryCall  (const QString & callId, uint startTimeStamp, uint stopTimeStamp, QString account, QString name, QString number, QString type );
   static Call* buildExistingCall (QString callId                                                                                                               );
   static void  setContactBackend (AbstractContactBackend* be                                                                                                   );
   static AbstractContactBackend* contactBackend ();

   //Static getters
   static history_state historyStateFromType            ( QString type                                    );
   static Call::State   startStateFromDaemonCallState   ( QString daemonCallState, QString daemonCallType );
   
   //Getters
   Call::State          state            () const;
   const QString        callId           () const;
   const QString        peerPhoneNumber  () const;
   const QString        peerName         () const;
   history_state        historyState     () const;
   bool                 recording        () const;
   Account*             account          () const;
   bool                 isHistory        () const;
   uint                 stopTimeStamp    () const;
   uint                 startTimeStamp   () const;
   QString              currentCodecName () const;
   bool                 isSecure            () const;
   bool                 isConference        () const;
   bool                 isSelected          () const;
   const QString        confId           () const;
   const QString        transferNumber   () const;
   const QString        callNumber       () const;
   const QString        recordingPath    () const;
   static const QString toHumanStateName    (const Call::State);
   Contact*             contact          ()      ;
   VideoRenderer*       videoRenderer    () const;
   const QString        formattedName    ()      ;
   bool                 hasRecording        () const;
   QString              length           () const;
   QVariant             roleData         (int role) const;

   //Automated function
   Call::State stateChanged(const QString & newState);
   Call::State actionPerformed(Call::Action action);
   
   //Setters
   void setConference     ( bool value            );
   void setConfId         ( QString value         );
   void setTransferNumber ( const QString& number );
   void setCallNumber     ( const QString& number );
   void setRecordingPath  ( const QString& path   );
   void setPeerName       ( const QString& name   );
   void setSelected       ( const bool     value  );
   
   //Mutators
   void appendText(const QString& str);
   void backspaceItemText();
   void changeCurrentState(Call::State newState);
   void sendTextMessage(QString message);

   virtual QObject* getSelf() {return this;}

private:

   //Attributes
   QString                m_Account        ;
   QString                m_CallId         ;
   QString                m_ConfId         ;
   QString                m_PeerPhoneNumber;
   QString                m_PeerName       ;
   QString                m_RecordingPath  ;
   history_state          m_HistoryState   ;
   uint                   m_pStartTimeStamp;
   uint                   m_pStopTimeStamp ;
   QString                m_TransferNumber ;
   QString                m_CallNumber     ;
   bool                   m_isConference   ;
   Call::State            m_CurrentState   ;
   bool                   m_Recording      ;
   static Call*           m_sSelectedCall  ;
   Contact*               m_pContact       ;
   InstantMessagingModel* m_pImModel       ;
   int                    m_LastContactCheck;
   QTimer*                m_pTimer         ;
   static AbstractContactBackend* m_pContactBackend;
   
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
   
   static const char * historyIcons[3];
   
   static const char * callStateIcons[11];

   Call(Call::State startState, const QString& callId, QString peerNumber = "", QString account = "", QString peerName = "");
   
   static Call::DaemonState toDaemonCallState   (const QString& stateName);
   static Call::State       confStatetoCallState(const QString& stateName);
   
   //Automate functions
   // See actionPerformedFunctionMap and stateChangedFunctionMap
   // to know when it is called.
   void nothing      () __attribute__ ((const));
   void error        () __attribute__ ((noreturn));
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
   void setRecord    ();
   void start        ();
   void startStop    ();
   void stop         ();
   void startWeird   ();
   void warning      ();

public Q_SLOTS:
   void playRecording();
   void stopRecording();
   void seekRecording(double position);
   void playDTMF(const QString& str);

private Q_SLOTS:
   void stopPlayback(QString filePath);
   void updatePlayback(int position,int size);
   void updated();

Q_SIGNALS:
   ///Emitted when a call change (state or details)
   void changed();
   void changed(Call* self);
   ///Emitted when the call is over
   void isOver(Call*);
   void playbackPositionChanged(int,int);
   void playbackStopped();
   void playbackStarted();
   void dtmfPlayed(const QString& str);
};

Q_DECLARE_METATYPE(Call*)

QDebug LIB_EXPORT operator<<(QDebug dbg, const Call::State& c       );
QDebug LIB_EXPORT operator<<(QDebug dbg, const Call::DaemonState& c );
QDebug LIB_EXPORT operator<<(QDebug dbg, const Call::Action& c      );

#endif
