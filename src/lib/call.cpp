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

//Parent
#include "call.h"

//C include
#include <time.h>

//Qt
#include <QtCore/QFile>
#include <QtCore/QTimer>


//SFLPhone library
#include "dbus/callmanager.h"
#include "dbus/configurationmanager.h"
#include "abstractitembackend.h"
#include "contact.h"
#include "uri.h"
#include "account.h"
#include "accountmodel.h"
#include "video/videomodel.h"
#include "historymodel.h"
#include "instantmessagingmodel.h"
#include "useractionmodel.h"
#include "callmodel.h"
#include "numbercategory.h"
#include "phonedirectorymodel.h"
#include "phonenumber.h"
#include "video/videorenderer.h"
#include "tlsmethodmodel.h"
#include "audio/settings.h"
#include "contactmodel.h"

//Track where state changes are performed on finished (over, error, failed) calls
//while not really problematic, it is technically wrong
#define Q_ASSERT_IS_IN_PROGRESS Q_ASSERT(m_CurrentState != Call::State::OVER);
#define FORCE_ERROR_STATE() {qDebug() << "Fatal error on " << this << __FILE__ << __LINE__;\
   d_ptr->changeCurrentState(Call::State::ERROR);}

#define FORCE_ERROR_STATE_P() {qDebug() << "Fatal error on " << this << __FILE__ << __LINE__;\
   changeCurrentState(Call::State::ERROR);}

#include "private/call_p.h"

const TypedStateMachine< TypedStateMachine< Call::State , Call::Action> , Call::State> CallPrivate::actionPerformedStateMap =
{{
//                           ACCEPT                      REFUSE                  TRANSFER                       HOLD                           RECORD              /**/
/*INCOMING     */  {{Call::State::INCOMING      , Call::State::INCOMING    , Call::State::ERROR        , Call::State::INCOMING     ,  Call::State::INCOMING     }},/**/
/*RINGING      */  {{Call::State::ERROR         , Call::State::RINGING     , Call::State::ERROR        , Call::State::ERROR        ,  Call::State::RINGING      }},/**/
/*CURRENT      */  {{Call::State::ERROR         , Call::State::CURRENT     , Call::State::TRANSFERRED  , Call::State::CURRENT      ,  Call::State::CURRENT      }},/**/
/*DIALING      */  {{Call::State::INITIALIZATION, Call::State::OVER        , Call::State::ERROR        , Call::State::ERROR        ,  Call::State::ERROR        }},/**/
/*HOLD         */  {{Call::State::ERROR         , Call::State::HOLD        , Call::State::TRANSF_HOLD  , Call::State::HOLD         ,  Call::State::HOLD         }},/**/
/*FAILURE      */  {{Call::State::ERROR         , Call::State::OVER        , Call::State::ERROR        , Call::State::ERROR        ,  Call::State::ERROR        }},/**/
/*BUSY         */  {{Call::State::ERROR         , Call::State::BUSY        , Call::State::ERROR        , Call::State::ERROR        ,  Call::State::ERROR        }},/**/
/*TRANSFER     */  {{Call::State::TRANSFERRED   , Call::State::TRANSFERRED , Call::State::CURRENT      , Call::State::TRANSFERRED  ,  Call::State::TRANSFERRED  }},/**/
/*TRANSF_HOLD  */  {{Call::State::TRANSF_HOLD   , Call::State::TRANSF_HOLD , Call::State::HOLD         , Call::State::TRANSF_HOLD  ,  Call::State::TRANSF_HOLD  }},/**/
/*OVER         */  {{Call::State::ERROR         , Call::State::ERROR       , Call::State::ERROR        , Call::State::ERROR        ,  Call::State::ERROR        }},/**/
/*ERROR        */  {{Call::State::ERROR         , Call::State::ERROR       , Call::State::ERROR        , Call::State::ERROR        ,  Call::State::ERROR        }},/**/
/*CONF         */  {{Call::State::ERROR         , Call::State::CURRENT     , Call::State::TRANSFERRED  , Call::State::CURRENT      ,  Call::State::CURRENT      }},/**/
/*CONF_HOLD    */  {{Call::State::ERROR         , Call::State::HOLD        , Call::State::TRANSF_HOLD  , Call::State::HOLD         ,  Call::State::HOLD         }},/**/
/*INIT         */  {{Call::State::INITIALIZATION, Call::State::OVER        , Call::State::ERROR        , Call::State::ERROR        ,  Call::State::ERROR        }},/**/
}};//                                                                                                                                                                */

#define CP &CallPrivate
const TypedStateMachine< TypedStateMachine< function , Call::Action > , Call::State > CallPrivate::actionPerformedFunctionMap =
{{
//                      ACCEPT             REFUSE         TRANSFER             HOLD               RECORD            /**/
/*INCOMING       */  {{CP::accept     , CP::refuse   , CP::acceptTransf   , CP::acceptHold  ,  CP::toggleRecord  }},/**/
/*RINGING        */  {{CP::nothing    , CP::hangUp   , CP::nothing        , CP::nothing     ,  CP::toggleRecord  }},/**/
/*CURRENT        */  {{CP::nothing    , CP::hangUp   , CP::nothing        , CP::hold        ,  CP::toggleRecord  }},/**/
/*DIALING        */  {{CP::call       , CP::cancel   , CP::nothing        , CP::nothing     ,  CP::nothing       }},/**/
/*HOLD           */  {{CP::nothing    , CP::hangUp   , CP::nothing        , CP::unhold      ,  CP::toggleRecord  }},/**/
/*FAILURE        */  {{CP::nothing    , CP::remove   , CP::nothing        , CP::nothing     ,  CP::nothing       }},/**/
/*BUSY           */  {{CP::nothing    , CP::hangUp   , CP::nothing        , CP::nothing     ,  CP::nothing       }},/**/
/*TRANSFERT      */  {{CP::transfer   , CP::hangUp   , CP::transfer       , CP::hold        ,  CP::toggleRecord  }},/**/
/*TRANSFERT_HOLD */  {{CP::transfer   , CP::hangUp   , CP::transfer       , CP::unhold      ,  CP::toggleRecord  }},/**/
/*OVER           */  {{CP::nothing    , CP::nothing  , CP::nothing        , CP::nothing     ,  CP::nothing       }},/**/
/*ERROR          */  {{CP::nothing    , CP::remove   , CP::nothing        , CP::nothing     ,  CP::nothing       }},/**/
/*CONF           */  {{CP::nothing    , CP::hangUp   , CP::nothing        , CP::hold        ,  CP::toggleRecord  }},/**/
/*CONF_HOLD      */  {{CP::nothing    , CP::hangUp   , CP::nothing        , CP::unhold      ,  CP::toggleRecord  }},/**/
/*INITIALIZATION */  {{CP::call       , CP::cancel   , CP::nothing        , CP::nothing     ,  CP::nothing       }},/**/
}};//                                                                                                                 */


const TypedStateMachine< TypedStateMachine< Call::State , Call::DaemonState> , Call::State> CallPrivate::stateChangedStateMap =
{{
//                        RINGING                   CURRENT                   BUSY                  HOLD                        HUNGUP                 FAILURE           /**/
/*INCOMING     */ {{Call::State::INCOMING    , Call::State::CURRENT    , Call::State::BUSY   , Call::State::HOLD         ,  Call::State::OVER  ,  Call::State::FAILURE  }},/**/
/*RINGING      */ {{Call::State::RINGING     , Call::State::CURRENT    , Call::State::BUSY   , Call::State::HOLD         ,  Call::State::OVER  ,  Call::State::FAILURE  }},/**/
/*CURRENT      */ {{Call::State::CURRENT     , Call::State::CURRENT    , Call::State::BUSY   , Call::State::HOLD         ,  Call::State::OVER  ,  Call::State::FAILURE  }},/**/
/*DIALING      */ {{Call::State::RINGING     , Call::State::CURRENT    , Call::State::BUSY   , Call::State::HOLD         ,  Call::State::OVER  ,  Call::State::FAILURE  }},/**/
/*HOLD         */ {{Call::State::HOLD        , Call::State::CURRENT    , Call::State::BUSY   , Call::State::HOLD         ,  Call::State::OVER  ,  Call::State::FAILURE  }},/**/
/*FAILURE      */ {{Call::State::FAILURE     , Call::State::FAILURE    , Call::State::BUSY   , Call::State::FAILURE      ,  Call::State::OVER  ,  Call::State::FAILURE  }},/**/
/*BUSY         */ {{Call::State::BUSY        , Call::State::CURRENT    , Call::State::BUSY   , Call::State::BUSY         ,  Call::State::OVER  ,  Call::State::FAILURE  }},/**/
/*TRANSFER     */ {{Call::State::TRANSFERRED , Call::State::TRANSFERRED, Call::State::BUSY   , Call::State::TRANSF_HOLD  ,  Call::State::OVER  ,  Call::State::FAILURE  }},/**/
/*TRANSF_HOLD  */ {{Call::State::TRANSF_HOLD , Call::State::TRANSFERRED, Call::State::BUSY   , Call::State::TRANSF_HOLD  ,  Call::State::OVER  ,  Call::State::FAILURE  }},/**/
/*OVER         */ {{Call::State::OVER        , Call::State::OVER       , Call::State::OVER   , Call::State::OVER         ,  Call::State::OVER  ,  Call::State::OVER     }},/**/
/*ERROR        */ {{Call::State::ERROR       , Call::State::ERROR      , Call::State::ERROR  , Call::State::ERROR        ,  Call::State::ERROR ,  Call::State::ERROR    }},/**/
/*CONF         */ {{Call::State::CURRENT     , Call::State::CURRENT    , Call::State::BUSY   , Call::State::HOLD         ,  Call::State::OVER  ,  Call::State::FAILURE  }},/**/
/*CONF_HOLD    */ {{Call::State::HOLD        , Call::State::CURRENT    , Call::State::BUSY   , Call::State::HOLD         ,  Call::State::OVER  ,  Call::State::FAILURE  }},/**/
/*INIT         */ {{Call::State::RINGING     , Call::State::CURRENT    , Call::State::BUSY   , Call::State::HOLD         ,  Call::State::OVER  ,  Call::State::FAILURE  }},/**/
}};//                                                                                                                                                                        */

const TypedStateMachine< TypedStateMachine< function , Call::DaemonState > , Call::State > CallPrivate::stateChangedFunctionMap =
{{
//                      RINGING                  CURRENT             BUSY              HOLD                    HUNGUP                  FAILURE       /**/
/*INCOMING       */  {{CP::nothing    , CP::start     , CP::startWeird     , CP::startWeird   ,  CP::startStop    , CP::failure }},/**/
/*RINGING        */  {{CP::nothing    , CP::start     , CP::start          , CP::start        ,  CP::startStop    , CP::failure }},/**/
/*CURRENT        */  {{CP::nothing    , CP::nothing   , CP::warning        , CP::nothing      ,  CP::stop         , CP::nothing }},/**/
/*DIALING        */  {{CP::nothing    , CP::warning   , CP::warning        , CP::warning      ,  CP::stop         , CP::warning }},/**/
/*HOLD           */  {{CP::nothing    , CP::nothing   , CP::warning        , CP::nothing      ,  CP::stop         , CP::nothing }},/**/
/*FAILURE        */  {{CP::nothing    , CP::warning   , CP::warning        , CP::warning      ,  CP::stop         , CP::nothing }},/**/
/*BUSY           */  {{CP::nothing    , CP::nothing   , CP::nothing        , CP::warning      ,  CP::stop         , CP::nothing }},/**/
/*TRANSFERT      */  {{CP::nothing    , CP::nothing   , CP::warning        , CP::nothing      ,  CP::stop         , CP::nothing }},/**/
/*TRANSFERT_HOLD */  {{CP::nothing    , CP::nothing   , CP::warning        , CP::nothing      ,  CP::stop         , CP::nothing }},/**/
/*OVER           */  {{CP::nothing    , CP::warning   , CP::warning        , CP::warning      ,  CP::stop         , CP::warning }},/**/
/*ERROR          */  {{CP::error      , CP::error     , CP::error          , CP::error        ,  CP::stop         , CP::error   }},/**/
/*CONF           */  {{CP::nothing    , CP::nothing   , CP::warning        , CP::nothing      ,  CP::stop         , CP::nothing }},/**/
/*CONF_HOLD      */  {{CP::nothing    , CP::nothing   , CP::warning        , CP::nothing      ,  CP::stop         , CP::nothing }},/**/
/*INIT           */  {{CP::nothing    , CP::warning   , CP::warning        , CP::warning      ,  CP::stop         , CP::warning }},/**/
}};//                                                                                                                                */
#undef CP

const TypedStateMachine< Call::LifeCycleState , Call::State > CallPrivate::metaStateMap =
{{
/*               *        Life cycle meta-state              **/
/*INCOMING       */   Call::LifeCycleState::INITIALIZATION ,/**/
/*RINGING        */   Call::LifeCycleState::INITIALIZATION ,/**/
/*CURRENT        */   Call::LifeCycleState::PROGRESS       ,/**/
/*DIALING        */   Call::LifeCycleState::INITIALIZATION ,/**/
/*HOLD           */   Call::LifeCycleState::PROGRESS       ,/**/
/*FAILURE        */   Call::LifeCycleState::FINISHED       ,/**/
/*BUSY           */   Call::LifeCycleState::FINISHED       ,/**/
/*TRANSFERT      */   Call::LifeCycleState::PROGRESS       ,/**/
/*TRANSFERT_HOLD */   Call::LifeCycleState::PROGRESS       ,/**/
/*OVER           */   Call::LifeCycleState::FINISHED       ,/**/
/*ERROR          */   Call::LifeCycleState::FINISHED       ,/**/
/*CONF           */   Call::LifeCycleState::PROGRESS       ,/**/
/*CONF_HOLD      */   Call::LifeCycleState::PROGRESS       ,/**/
/*INIT           */   Call::LifeCycleState::INITIALIZATION ,/**/
}};/*                                                        **/

const TypedStateMachine< TypedStateMachine< bool , Call::LifeCycleState > , Call::State > CallPrivate::metaStateTransitionValidationMap =
{{
/*               *     INITIALIZATION    PROGRESS      FINISHED   **/
/*INCOMING       */  {{     true     ,    false    ,    false }},/**/
/*RINGING        */  {{     true     ,    false    ,    false }},/**/
/*CURRENT        */  {{     true     ,    true     ,    false }},/**/
/*DIALING        */  {{     true     ,    false    ,    false }},/**/
/*HOLD           */  {{     true     ,    true     ,    false }},/**/
/*FAILURE        */  {{     true     ,    true     ,    false }},/**/
/*BUSY           */  {{     true     ,    false    ,    false }},/**/
/*TRANSFERT      */  {{     false    ,    true     ,    false }},/**/
/*TRANSFERT_HOLD */  {{     false    ,    true     ,    false }},/**/
/*OVER           */  {{     true     ,    true     ,    true  }},/**/
/*ERROR          */  {{     true     ,    true     ,    false }},/**/
/*CONF           */  {{     true     ,    false    ,    false }},/**/
/*CONF_HOLD      */  {{     true     ,    false    ,    false }},/**/
/*INIT           */  {{     true     ,    false    ,    false }},/**/
}};/*                                                             **/
/*^^ A call _can_ be created on hold (conference) and as over (peer hang up before pickup)
 the progress->failure one is an implementation bug*/


QDebug LIB_EXPORT operator<<(QDebug dbg, const Call::State& c)
{
   dbg.nospace() << QString(Call::toHumanStateName(c));
   return dbg.space();
}

QDebug LIB_EXPORT operator<<(QDebug dbg, const Call::DaemonState& c)
{
   dbg.nospace() << static_cast<int>(c);
   return dbg.space();
}

QDebug LIB_EXPORT operator<<(QDebug dbg, const Call::Action& c)
{
   switch (c) {
      case Call::Action::ACCEPT:
         dbg.nospace() << "ACCEPT";
      case Call::Action::REFUSE:
         dbg.nospace() << "REFUSE";
      case Call::Action::TRANSFER:
         dbg.nospace() << "TRANSFER";
      case Call::Action::HOLD:
         dbg.nospace() << "HOLD";
      case Call::Action::RECORD:
         dbg.nospace() << "RECORD";
      case Call::Action::__COUNT:
         dbg.nospace() << "COUNT";
   };
   dbg.space();
   dbg.nospace() << '(' << static_cast<int>(c) << ')';
   return dbg.space();
}

CallPrivate::CallPrivate(Call* parent) : QObject(parent),q_ptr(parent),
m_pStopTimeStamp(0),
m_pImModel(nullptr),m_pTimer(nullptr),m_Recording(false),m_Account(nullptr),
m_PeerName(),m_pPeerPhoneNumber(nullptr),m_HistoryConst(HistoryTimeCategoryModel::HistoryConst::Never),
m_CallId(),m_pStartTimeStamp(0),m_pDialNumber(nullptr),m_pTransferNumber(nullptr),
m_History(false),m_Missed(false),m_Direction(Call::Direction::OUTGOING),m_pBackend(nullptr),m_Type(Call::Type::CALL),
m_pUserActionModel(new UserActionModel(parent))
{
}

///Constructor
Call::Call(Call::State startState, const QString& callId, const QString& peerName, PhoneNumber* number, Account* account)
   : QObject(CallModel::instance()),d_ptr(new CallPrivate(this))
{
   Q_ASSERT(!callId.isEmpty());

   d_ptr->m_CurrentState     = startState;
   d_ptr->m_Type             = Call::Type::CALL;
   d_ptr->m_Account          = account;
   d_ptr->m_PeerName         = peerName;
   d_ptr->m_pPeerPhoneNumber = number;
   d_ptr->m_CallId           = callId;

   setObjectName("Call:"+callId);
   d_ptr->changeCurrentState(startState);

   emit changed();
   emit changed(this);
}

///Constructor
Call::Call(const QString& confId, const QString& account)
   : QObject(CallModel::instance()),d_ptr(new CallPrivate(this))
{
   d_ptr->m_CurrentState = Call::State::CONFERENCE;
   d_ptr->m_Account      = AccountModel::instance()->getById(account);
   d_ptr->m_Type         = (!confId.isEmpty())?Call::Type::CONFERENCE:Call::Type::CALL;
   d_ptr->m_CallId       = confId;

   setObjectName("Conf:"+confId);

   if (type() == Call::Type::CONFERENCE) {
      time_t curTime;
      ::time(&curTime);
      d_ptr->setStartTimeStamp(curTime);
      d_ptr->initTimer();
      CallManagerInterface& callManager = DBus::CallManager::instance();
      MapStringString        details    = callManager.getConferenceDetails(id())  ;
      d_ptr->m_CurrentState             = d_ptr->confStatetoCallState(details[ConfDetailsMapFields::CONF_STATE]);
      emit stateChanged();
   }
}

///Destructor
Call::~Call()
{
   if (d_ptr->m_pTimer) delete d_ptr->m_pTimer;
   this->disconnect();

   //m_pTransferNumber and m_pDialNumber are temporary, they are owned by the call
   if ( d_ptr->m_pTransferNumber ) delete d_ptr->m_pTransferNumber;
   if ( d_ptr->m_pDialNumber     ) delete d_ptr->m_pDialNumber;
}

/*****************************************************************************
 *                                                                           *
 *                               Call builder                                *
 *                                                                           *
 ****************************************************************************/

///Build a call from its ID
Call* Call::buildExistingCall(const QString& callId)
{
   CallManagerInterface& callManager = DBus::CallManager::instance();
   MapStringString       details     = callManager.getCallDetails(callId).value();

   //Too noisy
   //qDebug() << "Constructing existing call with details : " << details;

   const QString peerNumber    = details[ Call::DetailsMapFields::PEER_NUMBER ];
   const QString peerName      = details[ Call::DetailsMapFields::PEER_NAME   ];
   const QString account       = details[ Call::DetailsMapFields::ACCOUNT_ID  ];
   Call::State   startState    = startStateFromDaemonCallState(details[Call::DetailsMapFields::STATE], details[Call::DetailsMapFields::TYPE]);
   Account*      acc           = AccountModel::instance()->getById(account);
   PhoneNumber*  nb            = PhoneDirectoryModel::instance()->getNumber(peerNumber,acc);
   Call*         call          = new Call(startState, callId, peerName, nb, acc);
   call->d_ptr->m_Recording    = callManager.getIsRecording(callId);
   call->d_ptr->m_HistoryState = historyStateFromType(details[Call::HistoryMapFields::STATE]);

   if (!details[ Call::DetailsMapFields::TIMESTAMP_START ].isEmpty())
      call->d_ptr->setStartTimeStamp(details[ Call::DetailsMapFields::TIMESTAMP_START ].toInt());
   else {
      time_t curTime;
      ::time(&curTime);
      call->d_ptr->setStartTimeStamp(curTime);
   }

   call->d_ptr->initTimer();

   if (call->peerPhoneNumber()) {
      call->peerPhoneNumber()->addCall(call);
   }

   return call;
} //buildExistingCall

///Build a call from a dialing call (a call that is about to exist)
Call* Call::buildDialingCall(const QString& callId, const QString & peerName, Account* account)
{
   Call* call = new Call(Call::State::DIALING, callId, peerName, nullptr, account);
   call->d_ptr->m_HistoryState = Call::LegacyHistoryState::NONE;
   call->d_ptr->m_Direction = Call::Direction::OUTGOING;
   if (Audio::Settings::instance()->isRoomToneEnabled()) {
      Audio::Settings::instance()->playRoomTone();
   }
   qDebug() << "Created dialing call" << call;
   return call;
}

///Build a call from a dbus event
Call* Call::buildIncomingCall(const QString& callId)
{
   CallManagerInterface & callManager = DBus::CallManager::instance();
   MapStringString details = callManager.getCallDetails(callId).value();

   const QString from          = details[ Call::DetailsMapFields::PEER_NUMBER ];
   const QString account       = details[ Call::DetailsMapFields::ACCOUNT_ID  ];
   const QString peerName      = details[ Call::DetailsMapFields::PEER_NAME   ];
   Account*      acc           = AccountModel::instance()->getById(account);
   PhoneNumber*  nb            = PhoneDirectoryModel::instance()->getNumber(from,acc);
   Call* call                  = new Call(Call::State::INCOMING, callId, peerName, nb, acc);
   call->d_ptr->m_HistoryState = Call::LegacyHistoryState::MISSED;
   call->d_ptr->m_Direction    = Call::Direction::INCOMING;
   if (call->peerPhoneNumber()) {
      call->peerPhoneNumber()->addCall(call);
   }
   return call;
} //buildIncomingCall

///Build a ringing call (from dbus)
Call* Call::buildRingingCall(const QString & callId)
{
   CallManagerInterface& callManager = DBus::CallManager::instance();
   MapStringString details = callManager.getCallDetails(callId).value();

   const QString from          = details[ Call::DetailsMapFields::PEER_NUMBER ];
   const QString account       = details[ Call::DetailsMapFields::ACCOUNT_ID  ];
   const QString peerName      = details[ Call::DetailsMapFields::PEER_NAME   ];
   Account*      acc           = AccountModel::instance()->getById(account);
   PhoneNumber*  nb            = PhoneDirectoryModel::instance()->getNumber(from,acc);
   Call* call                  = new Call(Call::State::RINGING, callId, peerName, nb, acc);
   call->d_ptr->m_HistoryState = LegacyHistoryState::OUTGOING;
   call->d_ptr->m_Direction    = Call::Direction::OUTGOING;

   if (call->peerPhoneNumber()) {
      call->peerPhoneNumber()->addCall(call);
   }
   return call;
} //buildRingingCall


/*****************************************************************************
 *                                                                           *
 *                                  History                                  *
 *                                                                           *
 ****************************************************************************/

///Build a call that is already over
Call* Call::buildHistoryCall(const QMap<QString,QString>& hc)
{
   const QString& callId          = hc[ Call::HistoryMapFields::CALLID          ]          ;
   const QString& name            = hc[ Call::HistoryMapFields::DISPLAY_NAME    ]          ;
   const QString& number          = hc[ Call::HistoryMapFields::PEER_NUMBER     ]          ;
   const QString& type            = hc[ Call::HistoryMapFields::STATE           ]          ;
   const QString& direction       = hc[ Call::HistoryMapFields::DIRECTION       ]          ;
   const bool     missed          = hc[ Call::HistoryMapFields::MISSED          ] == "1";
   time_t         startTimeStamp  = hc[ Call::HistoryMapFields::TIMESTAMP_START ].toUInt() ;
   time_t         stopTimeStamp   = hc[ Call::HistoryMapFields::TIMESTAMP_STOP  ].toUInt() ;
   QString accId                  = hc[ Call::HistoryMapFields::ACCOUNT_ID      ]          ;

   if (accId.isEmpty()) {
      qWarning() << "An history call has an invalid account identifier";
      accId = QString(Account::ProtocolName::IP2IP);
   }

   //Try to assiciate a contact now, the real contact object is probably not
   //loaded yet, but we can get a placeholder for now
//    const QString& contactUsed    = hc[ Call::HistoryMapFields::CONTACT_USED ]; //TODO
   const QString& contactUid     = hc[ Call::HistoryMapFields::CONTACT_UID  ];

   Contact* ct = nullptr;
   if (!hc[ Call::HistoryMapFields::CONTACT_UID].isEmpty())
      ct = ContactModel::instance()->getPlaceHolder(contactUid.toAscii());

   Account*      acc       = AccountModel::instance()->getById(accId);
   PhoneNumber*  nb        = PhoneDirectoryModel::instance()->getNumber(number,ct,acc);

   Call*         call      = new Call(Call::State::OVER, callId, (name == "empty")?QString():name, nb, acc );

   call->d_ptr->m_pStopTimeStamp  = stopTimeStamp ;
   call->d_ptr->m_History         = true;
   call->d_ptr->setStartTimeStamp(startTimeStamp);
   call->d_ptr->m_HistoryState    = historyStateFromType(type);
   call->d_ptr->m_Account         = AccountModel::instance()->getById(accId);

   //BEGIN In ~2015, remove the old logic and clean this
   if (missed || call->d_ptr->m_HistoryState == Call::LegacyHistoryState::MISSED) {
      call->d_ptr->m_Missed = true;
      call->d_ptr->m_HistoryState = Call::LegacyHistoryState::MISSED;
   }
   if (!direction.isEmpty()) {
      if (direction == HistoryStateName::INCOMING) {
         call->d_ptr->m_Direction    = Call::Direction::INCOMING         ;
         call->d_ptr->m_HistoryState = Call::LegacyHistoryState::INCOMING;
      }
      else if (direction == HistoryStateName::OUTGOING) {
         call->d_ptr->m_Direction    = Call::Direction::OUTGOING         ;
         call->d_ptr->m_HistoryState = Call::LegacyHistoryState::OUTGOING;
      }
   }
   else if (call->d_ptr->m_HistoryState == Call::LegacyHistoryState::INCOMING)
      call->d_ptr->m_Direction    = Call::Direction::INCOMING            ;
   else if (call->d_ptr->m_HistoryState == Call::LegacyHistoryState::OUTGOING)
      call->d_ptr->m_Direction    = Call::Direction::OUTGOING            ;
   else //Getting there is a bug. Pick one, even if it is the wrong one
      call->d_ptr->m_Direction    = Call::Direction::OUTGOING            ;
   if (missed)
      call->d_ptr->m_HistoryState = Call::LegacyHistoryState::MISSED;
   //END

   call->setObjectName("History:"+call->d_ptr->m_CallId);

   if (call->peerPhoneNumber()) {
      call->peerPhoneNumber()->addCall(call);

      //Reload the glow and number colors
      connect(call->peerPhoneNumber(),SIGNAL(presentChanged(bool)),call->d_ptr.data(),SLOT(updated()));

      //Change the display name and picture
      connect(call->peerPhoneNumber(),SIGNAL(rebased(PhoneNumber*)),call->d_ptr.data(),SLOT(updated()));
   }

   return call;
}

/// aCall << Call::Action::HOLD
Call* Call::operator<<( Call::Action& c)
{
   performAction(c);
   return this;
}

///Get the history state from the type (see Call.cpp header)
Call::LegacyHistoryState Call::historyStateFromType(const QString& type)
{
   if(type == Call::HistoryStateName::MISSED        )
      return Call::LegacyHistoryState::MISSED   ;
   else if(type == Call::HistoryStateName::OUTGOING )
      return Call::LegacyHistoryState::OUTGOING ;
   else if(type == Call::HistoryStateName::INCOMING )
      return Call::LegacyHistoryState::INCOMING ;
   return  Call::LegacyHistoryState::NONE       ;
}

///Get the start sate from the daemon state
Call::State Call::startStateFromDaemonCallState(const QString& daemonCallState, const QString& daemonCallType)
{
   if(daemonCallState      == Call::DaemonStateInit::CURRENT  )
      return Call::State::CURRENT  ;
   else if(daemonCallState == Call::DaemonStateInit::HOLD     )
      return Call::State::HOLD     ;
   else if(daemonCallState == Call::DaemonStateInit::BUSY     )
      return Call::State::BUSY     ;
   else if(daemonCallState == Call::DaemonStateInit::INACTIVE && daemonCallType == Call::CallDirection::INCOMING )
      return Call::State::INCOMING ;
   else if(daemonCallState == Call::DaemonStateInit::INACTIVE && daemonCallType == Call::CallDirection::OUTGOING )
      return Call::State::RINGING  ;
   else if(daemonCallState == Call::DaemonStateInit::INCOMING )
      return Call::State::INCOMING ;
   else if(daemonCallState == Call::DaemonStateInit::RINGING  )
      return Call::State::RINGING  ;
   else
      return Call::State::FAILURE  ;
} //getStartStateFromDaemonCallState


/*****************************************************************************
 *                                                                           *
 *                                  Getters                                  *
 *                                                                           *
 ****************************************************************************/

///Transfer state from internal to daemon internal syntaz
Call::DaemonState CallPrivate::toDaemonCallState(const QString& stateName)
{
   if(stateName == Call::StateChange::HUNG_UP        )
      return Call::DaemonState::HUNG_UP ;
   if(stateName == Call::StateChange::RINGING        )
      return Call::DaemonState::RINGING ;
   if(stateName == Call::StateChange::CURRENT        )
      return Call::DaemonState::CURRENT ;
   if(stateName == Call::StateChange::UNHOLD_CURRENT )
      return Call::DaemonState::CURRENT ;
   if(stateName == Call::StateChange::HOLD           )
      return Call::DaemonState::HOLD    ;
   if(stateName == Call::StateChange::BUSY           )
      return Call::DaemonState::BUSY    ;
   if(stateName == Call::StateChange::FAILURE        )
      return Call::DaemonState::FAILURE ;

   qDebug() << "stateChanged signal received with unknown state.";
   return Call::DaemonState::FAILURE    ;
} //toDaemonCallState

///Transform a conference call state to a proper call state
Call::State CallPrivate::confStatetoCallState(const QString& stateName)
{
   if      ( stateName == Call::ConferenceStateChange::HOLD   )
      return Call::State::CONFERENCE_HOLD;
   else if ( stateName == Call::ConferenceStateChange::ACTIVE )
      return Call::State::CONFERENCE;
   else
      return Call::State::ERROR; //Well, this may bug a little
}

///Transform a backend state into a translated string
const QString Call::toHumanStateName(const Call::State cur)
{
   switch (cur) {
      case Call::State::INCOMING:
         return tr( "Ringing (in)"      );
         break;
      case Call::State::RINGING:
         return tr( "Ringing (out)"     );
         break;
      case Call::State::CURRENT:
         return tr( "Talking"           );
         break;
      case Call::State::DIALING:
         return tr( "Dialing"           );
         break;
      case Call::State::HOLD:
         return tr( "Hold"              );
         break;
      case Call::State::FAILURE:
         return tr( "Failed"            );
         break;
      case Call::State::BUSY:
         return tr( "Busy"              );
         break;
      case Call::State::TRANSFERRED:
         return tr( "Transfer"          );
         break;
      case Call::State::TRANSF_HOLD:
         return tr( "Transfer hold"     );
         break;
      case Call::State::OVER:
         return tr( "Over"              );
         break;
      case Call::State::ERROR:
         return tr( "Error"             );
         break;
      case Call::State::CONFERENCE:
         return tr( "Conference"        );
         break;
      case Call::State::CONFERENCE_HOLD:
         return tr( "Conference (hold)" );
      case Call::State::__COUNT:
         return tr( "ERROR"             );
      case Call::State::INITIALIZATION:
         return tr( "Initialization"    );
      default:
         return QString::number(static_cast<int>(cur));
   }
}

QString Call::toHumanStateName() const
{
   return toHumanStateName(state());
}

///Get the time (second from 1 jan 1970) when the call ended
time_t Call::stopTimeStamp() const
{
   return d_ptr->m_pStopTimeStamp;
}

///Get the time (second from 1 jan 1970) when the call started
time_t Call::startTimeStamp() const
{
   return d_ptr->m_pStartTimeStamp;
}

///Get the number where the call have been transferred
const QString Call::transferNumber() const
{
   return d_ptr->m_pTransferNumber?d_ptr->m_pTransferNumber->uri():QString();
}

///Get the call / peer number
const QString Call::dialNumber() const
{
   if (d_ptr->m_CurrentState != Call::State::DIALING) return QString();
   if (!d_ptr->m_pDialNumber) {
      d_ptr->m_pDialNumber = new TemporaryPhoneNumber();
   }
   return d_ptr->m_pDialNumber->uri();
}

///Return the call id
const QString Call::id() const
{
   return d_ptr->m_CallId;
}

PhoneNumber* Call::peerPhoneNumber() const
{
   if (d_ptr->m_CurrentState == Call::State::DIALING) {
      if (!d_ptr->m_pTransferNumber) {
         d_ptr->m_pTransferNumber = new TemporaryPhoneNumber(d_ptr->m_pPeerPhoneNumber);
      }
      if (!d_ptr->m_pDialNumber)
         d_ptr->m_pDialNumber = new TemporaryPhoneNumber(d_ptr->m_pPeerPhoneNumber);
      return d_ptr->m_pDialNumber;
   }
   return d_ptr->m_pPeerPhoneNumber?d_ptr->m_pPeerPhoneNumber:const_cast<PhoneNumber*>(PhoneNumber::BLANK());
}

///Get the peer name
const QString Call::peerName() const
{
   return d_ptr->m_PeerName;
}

///Generate the best possible peer name
const QString Call::formattedName() const
{
   if (type() == Call::Type::CONFERENCE)
      return tr("Conference");
   else if (!peerPhoneNumber())
      return "Error";
   else if (peerPhoneNumber()->contact() && !peerPhoneNumber()->contact()->formattedName().isEmpty())
      return peerPhoneNumber()->contact()->formattedName();
   else if (!peerName().isEmpty())
      return d_ptr->m_PeerName;
   else if (peerPhoneNumber())
      return peerPhoneNumber()->uri();
   else
      return tr("Unknown");
}

///If the call have a valid record
bool Call::hasRecording() const
{
   return !recordingPath().isEmpty() && QFile::exists(recordingPath());
}

///Generate an human readable string from the difference between StartTimeStamp and StopTimeStamp (or 'now')
QString Call::length() const
{
   if (d_ptr->m_pStartTimeStamp == d_ptr->m_pStopTimeStamp) return QString(); //Invalid
   int nsec =0;
   if (d_ptr->m_pStopTimeStamp)
      nsec = stopTimeStamp() - startTimeStamp();//If the call is over
   else { //Time to now
      time_t curTime;
      ::time(&curTime);
      nsec = curTime - d_ptr->m_pStartTimeStamp;
   }
   if (nsec/3600)
      return QString("%1:%2:%3 ").arg((nsec%(3600*24))/3600).arg(((nsec%(3600*24))%3600)/60,2,10,QChar('0')).arg(((nsec%(3600*24))%3600)%60,2,10,QChar('0'));
   else
      return QString("%1:%2 ").arg(nsec/60,2,10,QChar('0')).arg(nsec%60,2,10,QChar('0'));
}

///Is this call part of history
bool Call::isHistory()
{
   if (lifeCycleState() == Call::LifeCycleState::FINISHED && !d_ptr->m_History)
      d_ptr->m_History = true;
   return d_ptr->m_History;
}

///Is this call missed
bool Call::isMissed() const
{
   return d_ptr->m_Missed || d_ptr->m_HistoryState == Call::LegacyHistoryState::MISSED;
}

///Is the call incoming or outgoing
Call::Direction Call::direction() const
{
   return d_ptr->m_Direction;
}

///Is the call a conference or something else
Call::Type Call::type() const
{
   return d_ptr->m_Type;
}

///Return the backend used to serialize this call
AbstractHistoryBackend* Call::backend() const
{
   return d_ptr->m_pBackend;
}


///Does this call currently has video
bool Call::hasVideo() const
{
   #ifdef ENABLE_VIDEO
   return VideoModel::instance()->getRenderer(this) != nullptr;
   #else
   return false;
   #endif
}


///Get the current state
Call::State Call::state() const
{
   return d_ptr->m_CurrentState;
}

///Translate the state into its life cycle equivalent
Call::LifeCycleState Call::lifeCycleState() const
{
   return d_ptr->metaStateMap[d_ptr->m_CurrentState];
}

///Get the call recording
bool Call::isRecording() const
{
   return d_ptr->m_Recording;
}

///Get the call account id
Account* Call::account() const
{
   return d_ptr->m_Account;
}

///Get the recording path
const QString Call::recordingPath() const
{
   return d_ptr->m_RecordingPath;
}

///Get the history state
Call::LegacyHistoryState Call::historyState() const
{
   return d_ptr->m_HistoryState;
}

///This function could also be called mayBeSecure or haveChancesToBeEncryptedButWeCantTell.
bool Call::isSecure() const
{

   if (!d_ptr->m_Account) {
      qDebug() << "Account not set, can't check security";
      return false;
   }
   //BUG this doesn't work
   return d_ptr->m_Account && ((d_ptr->m_Account->isTlsEnabled()) || (d_ptr->m_Account->tlsMethod() != TlsMethodModel::Type::DEFAULT));
} //isSecure

///Return the renderer associated with this call or nullptr
VideoRenderer* Call::videoRenderer() const
{
   #ifdef ENABLE_VIDEO
   return VideoModel::instance()->getRenderer(this);
   #else
   return nullptr;
   #endif
}


/*****************************************************************************
 *                                                                           *
 *                                  Setters                                  *
 *                                                                           *
 ****************************************************************************/

///Set the transfer number
void Call::setTransferNumber(const QString& number)
{
   if (!d_ptr->m_pTransferNumber) {
      d_ptr->m_pTransferNumber = new TemporaryPhoneNumber();
   }
   d_ptr->m_pTransferNumber->setUri(number);
}

///Set the call number
void Call::setDialNumber(const QString& number)
{
   //This is not supposed to happen, but this is not a serious issue if it does
   if (d_ptr->m_CurrentState != Call::State::DIALING) {
      qDebug() << "Trying to set a dial number to a non-dialing call, doing nothing";
      return;
   }

   if (!d_ptr->m_pDialNumber) {
      d_ptr->m_pDialNumber = new TemporaryPhoneNumber();
   }

   d_ptr->m_pDialNumber->setUri(number);
   emit dialNumberChanged(d_ptr->m_pDialNumber->uri());
   emit changed();
   emit changed(this);
}

///Set the dial number from a full phone number
void Call::setDialNumber(const PhoneNumber* number)
{
   if (d_ptr->m_CurrentState == Call::State::DIALING && !d_ptr->m_pDialNumber) {
      d_ptr->m_pDialNumber = new TemporaryPhoneNumber(number);
   }
   if (d_ptr->m_pDialNumber && number)
      d_ptr->m_pDialNumber->setUri(number->uri());
   emit dialNumberChanged(d_ptr->m_pDialNumber->uri());
   emit changed();
   emit changed(this);
}

///Set the recording path
void Call::setRecordingPath(const QString& path)
{
   d_ptr->m_RecordingPath = path;
   if (!d_ptr->m_RecordingPath.isEmpty()) {
      CallManagerInterface& callManager = DBus::CallManager::instance();
      connect(&callManager,SIGNAL(recordPlaybackStopped(QString)), d_ptr.data(), SLOT(stopPlayback(QString))  );
      connect(&callManager,SIGNAL(updatePlaybackScale(QString,int,int))  , d_ptr.data(), SLOT(updatePlayback(QString,int,int)));
   }
}

///Set peer name
void Call::setPeerName(const QString& name)
{
   d_ptr->m_PeerName = name;
}

///Set the account (DIALING only, may be ignored)
void Call::setAccount( Account* account)
{
   if (state() == Call::State::DIALING)
      d_ptr->m_Account = account;
}

/// Set the backend to save this call to. It is currently impossible to migrate.
void Call::setBackend(AbstractHistoryBackend* backend)
{
   d_ptr->m_pBackend = backend;
}

/*****************************************************************************
 *                                                                           *
 *                                  Mutator                                  *
 *                                                                           *
 ****************************************************************************/

///The call state just changed (by the daemon)
Call::State CallPrivate::stateChanged(const QString& newStateName)
{
   const Call::State previousState = m_CurrentState;
   if (q_ptr->type() != Call::Type::CONFERENCE) {
      Call::DaemonState dcs = toDaemonCallState(newStateName);
      if (dcs == Call::DaemonState::__COUNT || m_CurrentState == Call::State::__COUNT) {
         qDebug() << "Error: Invalid state change";
         return Call::State::FAILURE;
      }
//       if (previousState == stateChangedStateMap[m_CurrentState][dcs]) {
// #ifndef NDEBUG
//          qDebug() << "Trying to change state with the same state" << previousState;
// #endif
//          return previousState;
//       }

      try {
         //Validate if the transition respect the expected life cycle
         if (!metaStateTransitionValidationMap[stateChangedStateMap[m_CurrentState][dcs]][q_ptr->lifeCycleState()]) {
            qWarning() << "Unexpected state transition from" << q_ptr->state() << "to" << stateChangedStateMap[m_CurrentState][dcs];
            Q_ASSERT(false);
         }
         changeCurrentState(stateChangedStateMap[m_CurrentState][dcs]);
      }
      catch(Call::State& state) {
         qDebug() << "State change failed (stateChangedStateMap)" << state;
         FORCE_ERROR_STATE_P()
         return m_CurrentState;
      }
      catch(Call::DaemonState& state) {
         qDebug() << "State change failed (stateChangedStateMap)" << state;
         FORCE_ERROR_STATE_P()
         return m_CurrentState;
      }
      catch (...) {
         qDebug() << "State change failed (stateChangedStateMap) other";;
         FORCE_ERROR_STATE_P()
         return m_CurrentState;
      }

      CallManagerInterface & callManager = DBus::CallManager::instance();
      MapStringString details = callManager.getCallDetails(m_CallId).value();
      if (details[Call::DetailsMapFields::PEER_NAME] != m_PeerName)
         m_PeerName = details[Call::DetailsMapFields::PEER_NAME];

      try {
         (this->*(stateChangedFunctionMap[previousState][dcs]))();
      }
      catch(Call::State& state) {
         qDebug() << "State change failed (stateChangedFunctionMap)" << state;
         FORCE_ERROR_STATE_P()
         return m_CurrentState;
      }
      catch(Call::DaemonState& state) {
         qDebug() << "State change failed (stateChangedFunctionMap)" << state;
         FORCE_ERROR_STATE_P()
         return m_CurrentState;
      }
      catch (...) {
         qDebug() << "State change failed (stateChangedFunctionMap) other";;
         FORCE_ERROR_STATE_P()
         return m_CurrentState;
      }
   }
   else {
      //Until now, it does not worth using stateChangedStateMap, conferences are quite simple
      //update 2014: Umm... wrong
      m_CurrentState = confStatetoCallState(newStateName); //TODO don't do this
      emit q_ptr->stateChanged();
   }
   if (m_CurrentState != Call::State::DIALING && m_pDialNumber) {
      if (!m_pPeerPhoneNumber)
         m_pPeerPhoneNumber = PhoneDirectoryModel::instance()->fromTemporary(m_pDialNumber);
      delete m_pDialNumber;
      m_pDialNumber = nullptr;
   }
   emit q_ptr->changed();
   emit q_ptr->changed(q_ptr);
   qDebug() << "Calling stateChanged " << newStateName << " -> " << toDaemonCallState(newStateName) << " on call with state " << previousState << ". Become " << m_CurrentState;
   return m_CurrentState;
} //stateChanged

void CallPrivate::performAction(Call::State previousState, Call::Action action)
{
   changeCurrentState(actionPerformedStateMap[previousState][action]);
}

void CallPrivate::performActionCallback(Call::State previousState, Call::Action action)
{
   (this->*(actionPerformedFunctionMap[previousState][action]))();
}

///An account have been performed
Call::State Call::performAction(Call::Action action)
{
   const Call::State previousState = d_ptr->m_CurrentState;

//    if (actionPerformedStateMap[previousState][action] == previousState) {
// #ifndef NDEBUG
//       qDebug() << "Trying to change state with the same state" << previousState;
// #endif
//       return previousState;
//    }

   //update the state
   try {
      d_ptr->performAction(previousState, action);
   }
   catch(Call::State& state) {
      qDebug() << "State change failed (actionPerformedStateMap)" << state;
      FORCE_ERROR_STATE()
      return Call::State::ERROR;
   }
   catch (...) {
      qDebug() << "State change failed (actionPerformedStateMap) other";;
      FORCE_ERROR_STATE()
      return d_ptr->m_CurrentState;
   }

   //execute the action associated with this transition
   try {
      d_ptr->performActionCallback(previousState, action);
   }
   catch(Call::State& state) {
      qDebug() << "State change failed (actionPerformedFunctionMap)" << state;
      FORCE_ERROR_STATE()
      return Call::State::ERROR;
   }
   catch(Call::Action& action) {
      qDebug() << "State change failed (actionPerformedFunctionMap)" << action;
      FORCE_ERROR_STATE()
      return Call::State::ERROR;
   }
   catch (...) {
      qDebug() << "State change failed (actionPerformedFunctionMap) other";;
      FORCE_ERROR_STATE()
      return d_ptr->m_CurrentState;
   }
   qDebug() << "Calling action " << action << " on " << id() << " with state " << previousState << ". Become " << d_ptr->m_CurrentState;
   return d_ptr->m_CurrentState;
} //actionPerformed

///Change the state, do not abuse of this, but it is necessary for error cases
void CallPrivate::changeCurrentState(Call::State newState)
{
   if (newState == Call::State::__COUNT) {
      qDebug() << "Error: Call reach invalid state";
      FORCE_ERROR_STATE_P()
      throw newState;
   }

   m_CurrentState = newState;

   emit q_ptr->stateChanged();
   emit q_ptr->changed();
   emit q_ptr->changed(q_ptr);

   initTimer();

   if (q_ptr->lifeCycleState() == Call::LifeCycleState::FINISHED)
      emit q_ptr->isOver(q_ptr);
}

///Set the start timestamp and update the cache
void CallPrivate::setStartTimeStamp(time_t stamp)
{
   m_pStartTimeStamp = stamp;
   //While the HistoryConst is not directly related to the call concept,
   //It is called to often to ignore
   m_HistoryConst = HistoryTimeCategoryModel::timeToHistoryConst(m_pStartTimeStamp);
}

///Send a text message
void Call::sendTextMessage(const QString& message)
{
   CallManagerInterface& callManager = DBus::CallManager::instance();
   Q_NOREPLY callManager.sendTextMessage(d_ptr->m_CallId,message);
   if (!d_ptr->m_pImModel) {
      d_ptr->m_pImModel = InstantMessagingModelManager::instance()->getModel(this);
   }
   d_ptr->m_pImModel->addOutgoingMessage(message);
}


/*****************************************************************************
 *                                                                           *
 *                              Automate function                            *
 *                                                                           *
 ****************************************************************************/
///@warning DO NOT TOUCH THAT, THEY ARE CALLED FROM AN AUTOMATE, HIGH FRAGILITY

///Do nothing (literally)
void CallPrivate::nothing()
{
   //nop
}

void CallPrivate::error()
{
   if (q_ptr->videoRenderer()) {
      //Well, in this case we have no choice, it still doesn't belong here
      q_ptr->videoRenderer()->stopRendering();
   }
   throw QString("There was an error handling your call, please restart SFLPhone.Is you encounter this problem often, \
   please open SFLPhone-KDE in a terminal and send this last 100 lines before this message in a bug report at \
   https://projects.savoirfairelinux.com/projects/sflphone/issues");
}

///Change history state to failure
void CallPrivate::failure()
{
   m_Missed = true;
   //This is how it always was done
   //The main point is to leave the call in the CallList
   start();
}

///Accept the call
void CallPrivate::accept()
{
   Q_ASSERT_IS_IN_PROGRESS

   CallManagerInterface & callManager = DBus::CallManager::instance();
   qDebug() << "Accepting call. callId : " << m_CallId  << "ConfId:" << m_CallId;
   Q_NOREPLY callManager.accept(m_CallId);
   time_t curTime;
   ::time(&curTime);
   setStartTimeStamp(curTime);
   this->m_HistoryState = Call::LegacyHistoryState::INCOMING;
   m_Direction = Call::Direction::INCOMING;
}

///Refuse the call
void CallPrivate::refuse()
{
   CallManagerInterface & callManager = DBus::CallManager::instance();
   qDebug() << "Refusing call. callId : " << m_CallId  << "ConfId:" << m_CallId;
   const bool ret = callManager.refuse(m_CallId);
   time_t curTime;
   ::time(&curTime);
   setStartTimeStamp(curTime);
   this->m_HistoryState = Call::LegacyHistoryState::MISSED;
   m_Missed = true;

   //If the daemon crashed then re-spawned when a call is ringing, this happen.
   if (!ret)
      FORCE_ERROR_STATE_P()
}

///Accept the transfer
void CallPrivate::acceptTransf()
{
   Q_ASSERT_IS_IN_PROGRESS

   if (!m_pTransferNumber) {
      qDebug() << "Trying to transfer to no one";
      return;
   }
   CallManagerInterface & callManager = DBus::CallManager::instance();
   qDebug() << "Accepting call and transferring it to number : " << m_pTransferNumber->uri() << ". callId : " << m_CallId  << "ConfId:" << m_CallId;
   callManager.accept(m_CallId);
   Q_NOREPLY callManager.transfer(m_CallId, m_pTransferNumber->uri());
}

///Put the call on hold
void CallPrivate::acceptHold()
{
   Q_ASSERT_IS_IN_PROGRESS

   CallManagerInterface & callManager = DBus::CallManager::instance();
   qDebug() << "Accepting call and holding it. callId : " << m_CallId  << "ConfId:" << m_CallId;
   callManager.accept(m_CallId);
   Q_NOREPLY callManager.hold(m_CallId);
   this->m_HistoryState = Call::LegacyHistoryState::INCOMING;
   m_Direction = Call::Direction::INCOMING;
}

///Hang up
void CallPrivate::hangUp()
{
   Q_ASSERT_IS_IN_PROGRESS

   CallManagerInterface & callManager = DBus::CallManager::instance();
   time_t curTime;
   ::time(&curTime);
   m_pStopTimeStamp = curTime;
   qDebug() << "Hanging up call. callId : " << m_CallId << "ConfId:" << m_CallId;
   bool ret;
   if (q_ptr->videoRenderer()) { //TODO remove, cheap hack
      q_ptr->videoRenderer()->stopRendering();
   }
   if (q_ptr->type() != Call::Type::CONFERENCE)
      ret = callManager.hangUp(m_CallId);
   else
      ret = callManager.hangUpConference(m_CallId);
   if (!ret) { //Can happen if the daemon crash and open again
      qDebug() << "Error: Invalid call, the daemon may have crashed";
      changeCurrentState(Call::State::OVER);
   }
   if (m_pTimer)
      m_pTimer->stop();
}

///Remove the call without contacting the daemon
void CallPrivate::remove()
{
   if (q_ptr->lifeCycleState() != Call::LifeCycleState::FINISHED)
      FORCE_ERROR_STATE_P()

   CallManagerInterface & callManager = DBus::CallManager::instance();

   //HACK Call hang up again to make sure the busytone stop, this should
   //return true or false, both are valid, no point to check the result
   if (q_ptr->type() != Call::Type::CONFERENCE)
      callManager.hangUp(m_CallId);
   else
      callManager.hangUpConference(q_ptr->id());

   emit q_ptr->isOver(q_ptr);
   emit q_ptr->stateChanged();
   emit q_ptr->changed();
   emit q_ptr->changed(q_ptr);
}

///Cancel this call
void CallPrivate::cancel()
{
   //This one can be over if the peer server failed to comply with the correct sequence
   CallManagerInterface & callManager = DBus::CallManager::instance();
   qDebug() << "Canceling call. callId : " << m_CallId  << "ConfId:" << q_ptr->id();
   emit q_ptr->dialNumberChanged(QString());
//    Q_NOREPLY callManager.hangUp(m_CallId);
   if (!callManager.hangUp(m_CallId)) {
      qWarning() << "HangUp failed, the call was probably already over";
      changeCurrentState(Call::State::OVER);
   }
}

///Put on hold
void CallPrivate::hold()
{
   Q_ASSERT_IS_IN_PROGRESS

   CallManagerInterface & callManager = DBus::CallManager::instance();
   qDebug() << "Holding call. callId : " << m_CallId << "ConfId:" << q_ptr->id();
   if (q_ptr->type() != Call::Type::CONFERENCE)
      Q_NOREPLY callManager.hold(m_CallId);
   else
      Q_NOREPLY callManager.holdConference(q_ptr->id());
}

///Start the call
void CallPrivate::call()
{
   Q_ASSERT_IS_IN_PROGRESS

   CallManagerInterface& callManager = DBus::CallManager::instance();
   qDebug() << "account = " << m_Account;
   if(!m_Account) {
      qDebug() << "Account is not set, taking the first registered.";
      this->m_Account = AccountModel::currentAccount();
   }
   //Calls to empty URI should not be allowed, sflphoned will go crazy
   if ((!m_pDialNumber) || m_pDialNumber->uri().isEmpty()) {
      qDebug() << "Trying to call an empty URI";
      changeCurrentState(Call::State::FAILURE);
      if (!m_pDialNumber) {
         emit q_ptr->dialNumberChanged(QString());
      }
      else {
         delete m_pDialNumber;
         m_pDialNumber = nullptr;
      }
      q_ptr->setPeerName(tr("Failure"));
      emit q_ptr->stateChanged();
      emit q_ptr->changed();
   }
   //Normal case
   else if(m_Account) {
      qDebug() << "Calling " << q_ptr->peerPhoneNumber()->uri() << " with account " << m_Account << ". callId : " << m_CallId  << "ConfId:" << q_ptr->id();
      callManager.placeCall(m_Account->id(), m_CallId, m_pDialNumber->uri());
      this->m_pPeerPhoneNumber = PhoneDirectoryModel::instance()->getNumber(m_pDialNumber->uri(),q_ptr->account());
      if (ContactModel::instance()->hasBackends()) {
         if (q_ptr->peerPhoneNumber()->contact())
            m_PeerName = q_ptr->peerPhoneNumber()->contact()->formattedName();
      }
      connect(q_ptr->peerPhoneNumber(),SIGNAL(presentChanged(bool)),this,SLOT(updated()));
      time_t curTime;
      ::time(&curTime);
      setStartTimeStamp(curTime);
      this->m_HistoryState = Call::LegacyHistoryState::OUTGOING;
      m_Direction = Call::Direction::OUTGOING;
      if (q_ptr->peerPhoneNumber()) {
         q_ptr->peerPhoneNumber()->addCall(q_ptr);
      }
      if (m_pDialNumber)
         emit q_ptr->dialNumberChanged(QString());
      delete m_pDialNumber;
      m_pDialNumber = nullptr;
   }
   else {
      qDebug() << "Trying to call " << (m_pTransferNumber?QString(m_pTransferNumber->uri()):"ERROR") 
         << " with no account registered . callId : " << m_CallId  << "ConfId:" << q_ptr->id();
      this->m_HistoryState = Call::LegacyHistoryState::NONE;
      throw tr("No account registered!");
   }
}

///Trnasfer the call
void CallPrivate::transfer()
{
   Q_ASSERT_IS_IN_PROGRESS

   if (m_pTransferNumber) {
      CallManagerInterface & callManager = DBus::CallManager::instance();
      qDebug() << "Transferring call to number : " << m_pTransferNumber->uri() << ". callId : " << m_CallId;
      Q_NOREPLY callManager.transfer(m_CallId, m_pTransferNumber->uri());
      time_t curTime;
      ::time(&curTime);
      m_pStopTimeStamp = curTime;
   }
}

///Unhold the call
void CallPrivate::unhold()
{
   Q_ASSERT_IS_IN_PROGRESS

   CallManagerInterface & callManager = DBus::CallManager::instance();
   qDebug() << "Unholding call. callId : " << m_CallId  << "ConfId:" << q_ptr->id();
   if (q_ptr->type() != Call::Type::CONFERENCE)
      Q_NOREPLY callManager.unhold(m_CallId);
   else
      Q_NOREPLY callManager.unholdConference(q_ptr->id());
}

///Record the call
void CallPrivate::toggleRecord()
{
   CallManagerInterface & callManager = DBus::CallManager::instance();
   qDebug() << "Setting record " << !m_Recording << " for call. callId : " << m_CallId  << "ConfId:" << q_ptr->id();

   callManager.toggleRecording(q_ptr->id());
}

///Start the timer
void CallPrivate::start()
{
   qDebug() << "Starting call. callId : " << m_CallId  << "ConfId:" << q_ptr->id();
   time_t curTime;
   ::time(&curTime);
   emit q_ptr->changed();
   emit q_ptr->changed(q_ptr);
   if (m_pDialNumber) {
      if (!m_pPeerPhoneNumber)
         m_pPeerPhoneNumber = PhoneDirectoryModel::instance()->fromTemporary(m_pDialNumber);
      delete m_pDialNumber;
      m_pDialNumber = nullptr;
   }
   setStartTimeStamp(curTime);
}

///Toggle the timer
void CallPrivate::startStop()
{
   qDebug() << "Starting and stoping call. callId : " << m_CallId  << "ConfId:" << q_ptr->id();
   time_t curTime;
   ::time(&curTime);
   setStartTimeStamp(curTime);
   m_pStopTimeStamp  = curTime;
}

///Stop the timer
void CallPrivate::stop()
{
   qDebug() << "Stoping call. callId : " << m_CallId  << "ConfId:" << q_ptr->id();
   if (q_ptr->videoRenderer()) { //TODO remove, cheap hack
      q_ptr->videoRenderer()->stopRendering();
   }
   time_t curTime;
   ::time(&curTime);
   m_pStopTimeStamp = curTime;
}

///Handle error instead of crashing
void CallPrivate::startWeird()
{
   qDebug() << "Starting call. callId : " << m_CallId  << "ConfId:" << q_ptr->id();
   time_t curTime;
   ::time(&curTime);
   setStartTimeStamp(curTime);
   qDebug() << "Warning : call " << m_CallId << " had an unexpected transition of state at its start.";
}

///Print a warning
void CallPrivate::warning()
{
   qWarning() << "Warning : call " << m_CallId << " had an unexpected transition of state.(" << m_CurrentState << ")";
   switch (m_CurrentState) {
      case Call::State::FAILURE        :
      case Call::State::ERROR          :
      case Call::State::__COUNT          :
         //If not stopped, then the counter will keep going
         //Getting here indicate something wrong happened
         //It can be normal, aka, an invalid URI such as '><'
         // or an SFLPhone-KDE bug
         stop();
         break;
      case Call::State::TRANSFERRED    :
      case Call::State::TRANSF_HOLD    :
      case Call::State::DIALING        :
      case Call::State::INITIALIZATION:
      case Call::State::INCOMING       :
      case Call::State::RINGING        :
      case Call::State::CURRENT        :
      case Call::State::HOLD           :
      case Call::State::BUSY           :
      case Call::State::OVER           :
      case Call::State::CONFERENCE     :
      case Call::State::CONFERENCE_HOLD:
      default:
         break;
   }
}

/*****************************************************************************
 *                                                                           *
 *                             Keyboard handling                             *
 *                                                                           *
 ****************************************************************************/

///Input text on the call item
void Call::appendText(const QString& str)
{
   TemporaryPhoneNumber* editNumber = nullptr;

   switch (d_ptr->m_CurrentState) {
   case Call::State::TRANSFERRED :
   case Call::State::TRANSF_HOLD :
      editNumber = d_ptr->m_pTransferNumber;
      break;
   case Call::State::DIALING     :
      editNumber = d_ptr->m_pDialNumber;
      break;
   case Call::State::INITIALIZATION:
   case Call::State::INCOMING:
   case Call::State::RINGING:
   case Call::State::CURRENT:
   case Call::State::HOLD:
   case Call::State::FAILURE:
   case Call::State::BUSY:
   case Call::State::OVER:
   case Call::State::ERROR:
   case Call::State::CONFERENCE:
   case Call::State::CONFERENCE_HOLD:
   case Call::State::__COUNT:
   default:
      qDebug() << "Backspace on call not editable. Doing nothing.";
      return;
   }

   if (editNumber) {
      editNumber->setUri(editNumber->uri()+str);
      if (state() == Call::State::DIALING)
         emit dialNumberChanged(editNumber->uri());
   }
   else
      qDebug() << "TemporaryPhoneNumber not defined";


   emit changed();
   emit changed(this);
}

///Remove the last character
void Call::backspaceItemText()
{
   TemporaryPhoneNumber* editNumber = nullptr;

   switch (d_ptr->m_CurrentState) {
      case Call::State::TRANSFERRED      :
      case Call::State::TRANSF_HOLD      :
         editNumber = d_ptr->m_pTransferNumber;
         break;
      case Call::State::DIALING          :
         editNumber = d_ptr->m_pDialNumber;
         break;
      case Call::State::INITIALIZATION:
      case Call::State::INCOMING:
      case Call::State::RINGING:
      case Call::State::CURRENT:
      case Call::State::HOLD:
      case Call::State::FAILURE:
      case Call::State::BUSY:
      case Call::State::OVER:
      case Call::State::ERROR:
      case Call::State::CONFERENCE:
      case Call::State::CONFERENCE_HOLD:
      case Call::State::__COUNT:
      default                          :
         qDebug() << "Backspace on call not editable. Doing nothing.";
         return;
   }
   if (editNumber) {
      QString text = editNumber->uri();
      const int textSize = text.size();
      if(textSize > 0) {
         editNumber->setUri(text.remove(textSize-1, 1));
         emit changed();
         emit changed(this);
      }
      else {
         d_ptr->changeCurrentState(Call::State::OVER);
      }
   }
   else
      qDebug() << "TemporaryPhoneNumber not defined";
}

///Reset the string a dialing or transfer call
void Call::reset()
{
   TemporaryPhoneNumber* editNumber = nullptr;

   switch (d_ptr->m_CurrentState) {
      case Call::State::TRANSFERRED      :
      case Call::State::TRANSF_HOLD      :
         editNumber = d_ptr->m_pTransferNumber;
         break;
      case Call::State::DIALING          :
         editNumber = d_ptr->m_pDialNumber;
         break;
      case Call::State::INITIALIZATION   :
      case Call::State::INCOMING         :
      case Call::State::RINGING          :
      case Call::State::CURRENT          :
      case Call::State::HOLD             :
      case Call::State::FAILURE          :
      case Call::State::BUSY             :
      case Call::State::OVER             :
      case Call::State::ERROR            :
      case Call::State::CONFERENCE       :
      case Call::State::CONFERENCE_HOLD  :
      case Call::State::__COUNT:
      default                            :
         qDebug() << "Cannot reset" << d_ptr->m_CurrentState << "calls";
         return;
   }
   if (editNumber) {
      editNumber->setUri(QString());
   }
}

/*****************************************************************************
 *                                                                           *
 *                                   SLOTS                                   *
 *                                                                           *
 ****************************************************************************/

void CallPrivate::updated()
{
   emit q_ptr->changed();
   emit q_ptr->changed(q_ptr);
}

///Play the record, if any
void Call::playRecording()
{
   CallManagerInterface& callManager = DBus::CallManager::instance();
   const bool retval = callManager.startRecordedFilePlayback(recordingPath());
   if (retval)
      emit playbackStarted();
}

///Stop the record, if any
void Call::stopRecording()
{
   CallManagerInterface& callManager = DBus::CallManager::instance();
   Q_NOREPLY callManager.stopRecordedFilePlayback(recordingPath());
   emit playbackStopped(); //TODO remove this, it is a workaround for bug #11942
}

///seek the record, if any
void Call::seekRecording(double position)
{
   CallManagerInterface& callManager = DBus::CallManager::instance();
   Q_NOREPLY callManager.recordPlaybackSeek(position);
}

///Daemon record playback stopped
void CallPrivate::stopPlayback(const QString& filePath)
{
   if (filePath == q_ptr->recordingPath()) {
      emit q_ptr->playbackStopped();
   }
}

///Daemon playback position chnaged
void CallPrivate::updatePlayback(const QString& path, int position,int size)
{
   if (path == m_RecordingPath) {
      emit q_ptr->playbackPositionChanged(position,size);
   }
}

UserActionModel* Call::userActionModel() const
{
   return d_ptr->m_pUserActionModel;
}

///Check if creating a timer is necessary
void CallPrivate::initTimer()
{
   if (q_ptr->lifeCycleState() == Call::LifeCycleState::PROGRESS) {
      if (!m_pTimer) {
         m_pTimer = new QTimer(this);
         m_pTimer->setInterval(1000);
         connect(m_pTimer,SIGNAL(timeout()),this,SLOT(updated()));
      }
      if (!m_pTimer->isActive())
         m_pTimer->start();
   }
   else if (m_pTimer && q_ptr->lifeCycleState() != Call::LifeCycleState::PROGRESS) {
      m_pTimer->stop();
      delete m_pTimer;
      m_pTimer = nullptr;
   }
}

///Common source for model data roles
QVariant Call::roleData(int role) const
{
   const Contact* ct = peerPhoneNumber()?peerPhoneNumber()->contact():nullptr;
   switch (role) {
      case Call::Role::Name:
      case Qt::DisplayRole:
         if (type() == Call::Type::CONFERENCE)
            return tr("Conference");
         else if (state() == Call::State::DIALING)
            return dialNumber();
         else if (d_ptr->m_PeerName.isEmpty())
            return ct?ct->formattedName():peerPhoneNumber()?peerPhoneNumber()->uri():dialNumber();
         else
            return formattedName();
         break;
      case Qt::ToolTipRole:
         return tr("Account: ") + (account()?account()->alias():QString());
         break;
      case Qt::EditRole:
         return dialNumber();
      case Call::Role::Number:
         return peerPhoneNumber()->uri();
         break;
      case Call::Role::Direction2:
         return static_cast<int>(d_ptr->m_Direction); //TODO Qt5, use the Q_ENUM
         break;
      case Call::Role::Date:
         return (int)startTimeStamp();
         break;
      case Call::Role::Length:
         return length();
         break;
      case Call::Role::FormattedDate:
         return QDateTime::fromTime_t(startTimeStamp()).toString();
         break;
      case Call::Role::HasRecording:
         return hasRecording();
         break;
      case Call::Role::Historystate:
         return static_cast<int>(historyState());
         break;
      case Call::Role::Filter: {
         QString normStripppedC;
         foreach(QChar char2,QString(static_cast<int>(historyState())+'\n'+roleData(Call::Role::Name).toString()+'\n'+
            roleData(Call::Role::Number).toString()).toLower().normalized(QString::NormalizationForm_KD) ) {
            if (!char2.combiningClass())
               normStripppedC += char2;
         }
         return normStripppedC;
         }
         break;
      case Call::Role::FuzzyDate:
         return (int)d_ptr->m_HistoryConst; //TODO Qt5, use the Q_ENUM
         break;
      case Call::Role::IsBookmark:
         return false;
         break;
      case Call::Role::Security:
         return isSecure();
         break;
      case Call::Role::Department:
         return ct?ct->department():QVariant();
         break;
      case Call::Role::Email:
         return ct?ct->preferredEmail():QVariant();
         break;
      case Call::Role::Organisation:
         return ct?ct->organization():QVariant();
         break;
      case Call::Role::Object:
         return QVariant::fromValue(const_cast<Call*>(this));
         break;
      case Call::Role::PhoneNu:
         return QVariant::fromValue(const_cast<PhoneNumber*>(peerPhoneNumber()));
         break;
      case Call::Role::PhotoPtr:
         return QVariant::fromValue((void*)(ct?ct->photo():nullptr));
         break;
      case Call::Role::CallState:
         return static_cast<int>(state()); //TODO Qt5, use the Q_ENUM
         break;
      case Call::Role::Id:
         return id();
         break;
      case Call::Role::StartTime:
         return (int) d_ptr->m_pStartTimeStamp;
      case Call::Role::StopTime:
         return (int) d_ptr->m_pStopTimeStamp;
      case Call::Role::IsRecording:
         return isRecording();
      case Call::Role::IsPresent:
         return peerPhoneNumber()->isPresent();
      case Call::Role::IsTracked:
         return peerPhoneNumber()->isTracked();
      case Call::Role::SupportPresence:
         return peerPhoneNumber()->supportPresence();
      case Call::Role::CategoryIcon:
         return peerPhoneNumber()->category()->icon(peerPhoneNumber()->isTracked(),peerPhoneNumber()->isPresent());
      case Call::Role::CallCount:
         return peerPhoneNumber()->callCount();
      case Call::Role::TotalSpentTime:
         return peerPhoneNumber()->totalSpentTime();
      case Call::Role::DropState:
         return property("dropState");
         break;
      case Call::Role::Missed:
         return isMissed();
      case Call::Role::CallLifeCycleState:
         return static_cast<int>(lifeCycleState()); //TODO Qt5, use the Q_ENUM
      case Call::Role::DTMFAnimState:
         return property("DTMFAnimState");
         break;
      case Call::Role::LastDTMFidx:
         return property("latestDtmfIdx");
         break;
      case Call::Role::DropPosition:
         return property("dropPosition");
         break;
      default:
         break;
   };
   return QVariant();
}


void Call::playDTMF(const QString& str)
{
   Q_NOREPLY DBus::CallManager::instance().playDTMF(str);
   emit dtmfPlayed(str);
}

#undef Q_ASSERT_IS_IN_PROGRESS
#undef FORCE_ERROR_STATE
#undef FORCE_ERROR_STATE_P

#include <call.moc>
