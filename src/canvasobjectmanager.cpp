/***************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/
#include "canvasobjectmanager.h"

//Qt
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtGui/QApplication>

//X11
#ifdef Q_WS_X11
// #include <xcb/xcb.h>
// #include <xcb/xcb_atom.h>
#endif

//KDE
#include <KNotification>
#include <KLocale>

//SFLPhone
#include <klib/tipmanager.h>
#include <widgets/tips/tipcollection.h>
#include "sflphone.h"


CanvasObjectManager::Object CanvasObjectManager::m_slEvents[EVENT_COUNT+1] = { CanvasObjectManager::Object::NoObject };

#define OBJ_DEF(obj) elements[static_cast<int>(obj)]
#define _(NAME) CanvasObjectManager::Object##NAME::
#define E CanvasEvent
const CanvasObjectManager::CanvasElement CanvasObjectManager::elements[ELEMENT_COUNT] = {
   /*                  ObjectType       ObjectLifeCycle        ObjectPriority             IN-EVENT                   OUT-EVENT         STACK   NOTIFY */
   /*0 NoObject     */ {_(Type)OBJECT , _(LifeCycle)STATIC , _(Priority)NO_PRIORITY, E::NONE                    , E::ANY               , false , false  , false},
   /*1 DialInfo     */ {_(Type)OBJECT , _(LifeCycle)STATIC , _(Priority)LOW        , E::NO_CALLS                , E::ANY               , false , false  , false},
   /*2 EndCall      */ {_(Type)OBJECT , _(LifeCycle)TIMED  , _(Priority)LOW        , E::CALL_ENDED              , E::ANY               , false , true   , false},
   /*3 Ringing      */ {_(Type)OBJECT , _(LifeCycle)EVENT  , _(Priority)MEDIUM     , E::CALL_RINGING            , E::CALL_STATE_CHANGED
                                                                                                                 | E::CALL_ENDED
                                                                                                                 | E::CALL_BUSY        , true  , false  , false},
   /*4 Network      */ {_(Type)OBJECT , _(LifeCycle)EVENT  , _(Priority)MEDIUM     , E::NETWORK_ERROR           , E::NETWORK_BACK
                                                                                                                 |E::REGISTERED_ACCOUNT
                                                                                                                 |E::CALL_RINGING      , false , true   , false},
   /*5 AutoComplete */ {_(Type)WIDGET , _(LifeCycle)EVENT  , _(Priority)HIGH       , E::CALL_DIALING_CHANGED    , E::CALL_STATE_CHANGED
                                                                                                                 | E::CALL_ENDED
                                                                                                                 | E::CALL_RINGING
                                                                                                                 | E::CALL_BUSY
                                                                                                                 | E::USER_CANCEL      , false , false  , true },
   /*6 DropInfo     */ {_(Type)OBJECT , _(LifeCycle)EVENT  , _(Priority)MEDIUM     , E::DRAG_ENTER|E::DRAG_MOVE , E::DRAG_LEAVE|E::DROP, false , false  , false},
   /*7 ConfInfo     */ {_(Type)OBJECT , _(LifeCycle)EVENT  , _(Priority)LOW        , E::CALL_COUNT_CHANGED      , E::ANY               , false , false  , false},
   /*8 AccountDown  */ {_(Type)OBJECT , _(LifeCycle)EVENT  , _(Priority)HIGH       , E::UNREGISTERED_ACCOUNT    , E::ANY               , false , true   , false},
};
#undef _
#undef E

void CanvasObjectManager::hInitEvents() {
   static bool init = false;
   if (!init) {
      for (int i=0;i<EVENT_COUNT-1;i++) {
         for (int j=1;j<ELEMENT_COUNT;j++) {
            if (OBJ_DEF(j).inEvent & (0x01<<i))
               CanvasObjectManager::m_slEvents[i+1] = static_cast<CanvasObjectManager::Object>(j);
         }
      }
      init = true;
   }
}

CanvasObjectManager::CanvasObjectManager(QObject* parent) : QObject(parent),m_DisableTransition(false),
m_CurrentObject(CanvasObjectManager::Object::NoObject),m_NextObject(CanvasObjectManager::Object::NoObject)
,m_pTimer(nullptr),m_CurrentState(ObjectState::NO_OBJECT),m_Minimized(false),m_CurrentLifeCycle(ObjectLifeCycle::NONE)
{
   hInitEvents();

//    m_DisableTransition = true;
   connect(TipCollection::manager(),SIGNAL(transitionStarted(QAbstractAnimation::Direction,QAbstractAnimation::State)),
      this,SLOT(slotTransitionEvents(QAbstractAnimation::Direction,QAbstractAnimation::State)));


   //TODO remove
//    testEventToEvent    ();
//    testEvenToObject    ();
//    testFirstInEvent    ();
//    testObjectPriority  ();
//    testObjectDiscarding();
}

CanvasObjectManager::~CanvasObjectManager()
{
   //Nothing to do
}

///This function take a *SINGLE* flag as parameter, return corresponding object
CanvasObjectManager::Object CanvasObjectManager::eventToObject(CanvasEvent event) const
{
   const int power = eventFlagToIndex(event);
   return m_slEvents[power];
}

///This function take a *MULTIPLE* flag as parameter, return corresponding object
QList<CanvasObjectManager::Object> CanvasObjectManager::eventsToObjects(CanvasEvent events) const
{
   Q_UNUSED(events);
   //TODO
   return QList<CanvasObjectManager::Object>();
}

///Get the first event triggering an object (for testing purpose only)
CanvasObjectManager::CanvasEvent CanvasObjectManager::firstInEvent( Object object ) const
{
   if (!OBJ_DEF(object).inEvent)
      return CanvasObjectManager::CanvasEvent::NONE;

   for (int power=0;power<EVENT_COUNT-1;power++) {
      if (m_slEvents[power+1] == object)
         return static_cast<CanvasObjectManager::CanvasEvent>(0x01<<power);
   }
   return CanvasObjectManager::CanvasEvent::NONE;
}


CanvasObjectManager::CanvasEvent CanvasObjectManager::eventIndexToFlag(int index) const
{
   if (!index || index > EVENT_COUNT-1)
      return CanvasEvent::NONE;
   return index==1?CanvasEvent::ANY:static_cast<CanvasEvent>(0x01<<(index-1));
}

int CanvasObjectManager::eventFlagToIndex(CanvasObjectManager::CanvasEvent events) const
{
   if (!events)
      return 0;
   else if (events == CanvasEvent::ANY)
      return 1;
   else {
      for (int power = EVENT_COUNT;power;power--)
         if (events & (0x01<<power)) return power+1;
   }
   return 0;
}

void CanvasObjectManager::initiateOutTransition(bool skipAnimation)
{
//    qDebug() << "IN OUT" << (int)m_NextObject << (int)m_CurrentObject;
   if (m_NextObject != m_CurrentObject) {
      if (m_DisableTransition || skipAnimation) {
         if (m_pTimer && m_pTimer->isActive())
            m_pTimer->stop();
         m_CurrentObject = m_NextObject;
         TipCollection::manager()->hideCurrentTip(true);
         m_NextObject    = CanvasObjectManager::Object::NoObject;
      }
      else {
         //TODO
         if (m_CurrentObject != Object::NoObject) {
            switch (OBJ_DEF(m_CurrentObject).type) {
               case ObjectType::OBJECT: {
                  //If hideCurrentTip return true, then act as if the transition was over
//                   qDebug() << "\n\n\nMMMMM" << OBJ_DEF(m_CurrentObject).skipAnimation << (int)m_NextObject;
                  if (TipCollection::manager()->hideCurrentTip(OBJ_DEF(m_NextObject).skipAnimation)) {
      //                qDebug() << "PROBLEM";
                     m_CurrentObject = m_NextObject;
                     m_NextObject    = CanvasObjectManager::Object::NoObject;
                  }
                  } break;
               case ObjectType::WIDGET: {
//                   qDebug() << "OUT WIDGET";
                  QWidget* w = TipCollection::canvasWidgetsToTip(m_CurrentObject);
//                   qDebug() << "\n\n\nFOO" << w;
                  if (w) {
                     w->setVisible(false);
                     m_CurrentObject = m_NextObject;
                     m_NextObject    = CanvasObjectManager::Object::NoObject;
                  }
                  } break;
   //             qDebug() << "SHOULD HIDE TIP" << (int)m_NextObject;
            };
         }
      }
   }
}

void CanvasObjectManager::initiateInTransition(Object nextObj,const QString& message)
{
//    qDebug() << "HERE" << (int)m_CurrentState << m_DisableTransition << (int)nextObj;
   if (nextObj != m_CurrentObject) {
      if (m_DisableTransition || m_CurrentState == ObjectState::NO_OBJECT) {
         if (m_pTimer && m_pTimer->isActive())
            m_pTimer->stop();
         m_CurrentObject = nextObj;
         nextObj = CanvasObjectManager::Object::NoObject;
         switch (OBJ_DEF(m_CurrentObject).type) {
            case ObjectType::OBJECT: {
               Tip* currentTip = TipCollection::canvasObjectToTip(m_CurrentObject);
               if (currentTip) {
                  if (OBJ_DEF(m_CurrentObject).lifeCycle == ObjectLifeCycle::TIMED && currentTip->timeout()) {
                     if (!m_pTimer) {
                        m_pTimer = new QTimer(this);
                        m_pTimer->setSingleShot(true);
                        connect(m_pTimer,SIGNAL(timeout()),this,SLOT(slotTimeout()));
                     }
                     if (!message.isEmpty()) {
                        currentTip->setText(message);
                     }
                     m_pTimer->setInterval(currentTip->timeout());
                     m_pTimer->start();
                  }
                  //If the window is minimized, use system notification
                  const QString m = message.isEmpty()?currentTip->text():message;
                  if (m_Minimized && !m.isEmpty())
                     KNotification::event(KNotification::Notification, i18n("SFLPhone"), m);
                  //If the window is not focused and the object request notification, use system
                  //disabled as it is not implemented by many top tier window managers
#ifdef Q_WS_X11
// Doesn't work on most window managers
//                   static xcb_connection_t *c = nullptr;
//                   if (c || (c = xcb_connect(nullptr,nullptr))) {
//                      xcb_window_t win = SFLPhone::app()->winId();
//                      xcb_generic_error_t *error;
//                      xcb_intern_atom_cookie_t cookie = xcb_intern_atom(c, 0, strlen("_NET_WM_STATE_FOCUSED"), "_NET_WM_STATE_FOCUSED");
//                      xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(c, cookie, &error);
//                      static xcb_atom_t _NET_WM_STATE_FOCUSED = reply->atom;
//                      xcb_get_property_cookie_t propC = xcb_get_property_unchecked(c, false, win,_NET_WM_STATE_FOCUSED, XCB_GET_PROPERTY_TYPE_ANY, 0, 1);
//                      xcb_get_property_reply_t *replyP = xcb_get_property_reply(c, propC, NULL);
//                      if (replyP) {
//                         bool isFocused = *((bool*)xcb_get_property_value(replyP));
//                         qDebug() << "\n\n\nClient is focused" <<isFocused;
//                         free(replyP);
//                      }
//                      free(reply);
//                      qDebug() << "\n\nNOTIF" << QApplication::activeWindow() << OBJ_DEF(m_CurrentObject).systemNotification;
//                      if (!QApplication::activeWindow() && OBJ_DEF(m_CurrentObject).systemNotification) {
//                         KNotification::event(KNotification::Notification, i18n("SFLPhone"), message.isEmpty()?currentTip->text():message);
//                      }
//                   }
#endif
               }
               TipCollection::manager()->setCurrentTip(currentTip);


               } break;
            case ObjectType::WIDGET:
               QWidget* w = TipCollection::canvasWidgetsToTip(m_CurrentObject);
               if (w) {
                  w->setVisible(true);
               }
               break;
         };
      }
      else {
//          qDebug() << "CURRENT IS VISIBLE" <<(int)m_CurrentObject << "next" << (int)nextObj;
         m_NextObject = nextObj;
         initiateOutTransition();
      }
   }
}

bool CanvasObjectManager::newEvent(CanvasEvent events, const QString& message)
{
//    qDebug() << "NEW EVENT" << events << message << (int)m_CurrentState << (int)m_CurrentObject << (OBJ_DEF(m_CurrentObject).outEvent & events);

   const bool singleEvent = (events & (events - 1)) == 0;
   CanvasObjectManager::Object nextObj = eventToObject(events);

   //First, notify the current object of the new event, it may be discarded
   if (m_CurrentObject != CanvasObjectManager::Object::NoObject && OBJ_DEF(m_CurrentObject).outEvent & events) {
//       qDebug() << "OUT (dsfsdf)";
      initiateOutTransition(singleEvent && OBJ_DEF(nextObj).skipAnimation);
   }

   //Detect if there is multiple flags set
   if (singleEvent) {
//       qDebug() << "IS SINGLE" << (OBJ_DEF(nextObj).priority >= OBJ_DEF(m_CurrentObject).priority) << (int)nextObj << (int)m_CurrentObject << (int) m_CurrentState;
      if (OBJ_DEF(nextObj).priority >= OBJ_DEF(m_CurrentObject).priority || m_CurrentState == ObjectState::TRANSITION_OUT) {
//          qDebug() << "In IF";
         initiateInTransition(nextObj,message);
         return true;
      }
   }
   else {
      QList<CanvasObjectManager::Object> nextObjs = CanvasObjectManager::eventsToObjects(events);
      CanvasObjectManager::Object highestPriorityNextObj = CanvasObjectManager::Object::NoObject;
      foreach(const CanvasObjectManager::Object& nextObj, nextObjs) {
         if (OBJ_DEF(nextObj).priority > OBJ_DEF(highestPriorityNextObj).priority)
            highestPriorityNextObj = nextObj;
      }
      if (highestPriorityNextObj != CanvasObjectManager::Object::NoObject) {
         initiateInTransition(highestPriorityNextObj,message);
         return true;
//          qDebug() << "ret";
      }
   }
//    qDebug() << "ret";
   return false;
}

void CanvasObjectManager::reset()
{
   if (m_CurrentObject != CanvasObjectManager::Object::NoObject) {
      //TODO
   }
   if (m_pTimer && m_pTimer->isActive())
      m_pTimer->stop();
   m_CurrentObject = CanvasObjectManager::Object::NoObject;
   TipCollection::manager()->hideCurrentTip(true);
}

bool CanvasObjectManager::operator<<(CanvasEvent events)
{
   return newEvent(events);
}

CanvasObjectManager::Object CanvasObjectManager::currentObject() const
{
   return m_CurrentObject;
}

CanvasObjectManager::Object CanvasObjectManager::nextObject   () const
{
   return m_NextObject;
}



void CanvasObjectManager::slotTimeout()
{
   //Hide the canvas object, stopping the countdown should be handled elsewhere
   initiateOutTransition();
}

void CanvasObjectManager::slotTransitionEvents(QAbstractAnimation::Direction direction, QAbstractAnimation::State state)
{
//    qDebug() << "IN TRANSITION EVENT" << direction << state;
   switch(state) {
      case QAbstractAnimation::Stopped:
         if (m_CurrentObject == Object::NoObject) {
            m_CurrentState = ObjectState::NO_OBJECT;
//             qDebug() << "HERE4" << (int)m_NextObject;
            if (m_NextObject != Object::NoObject) {
               initiateInTransition(m_NextObject);
               m_NextObject = Object::NoObject;
            }
         }
         else
            switch (direction) {
               case QAbstractAnimation::Forward:
                  m_CurrentState = ObjectState::VISIBLE;
//                   qDebug() << "HERE2";
                  break;
               case QAbstractAnimation::Backward:
                  m_CurrentState = ObjectState::NO_OBJECT;
//                   qDebug() << "HERE3" << (int) m_NextObject;
                  initiateInTransition(m_NextObject);
                  m_NextObject = Object::NoObject;
                  break;
            };
         break;
      case QAbstractAnimation::Paused:
         //TODO
         break;
      case QAbstractAnimation::Running:
         switch (direction) {
            case QAbstractAnimation::Forward:
               m_CurrentState = ObjectState::TRANSITION_IN;
               break;
            case QAbstractAnimation::Backward:
               m_CurrentState = ObjectState::TRANSITION_OUT;
               break;
         };
         break;
   };
}

void CanvasObjectManager::slotMinimized(bool isMinimized)
{
   m_Minimized = isMinimized;
}









bool CanvasObjectManager::testEventToEvent() const
{
   /* This test is to validate the event table is correctly generated
    * from the structured elements transition table ("elements"). Each
    * event can be mapped to ONE AND ONLY ONE canvas object
    *
    * INPUT:  element structure
    * OUTPUT: events
    */

   hInitEvents();

   bool success = true;

   for (int j=0;j<ELEMENT_COUNT;j++) {
      CanvasObjectManager::Object obj = static_cast<CanvasObjectManager::Object>(j);
      const CanvasElement& element = OBJ_DEF(j);
      for (int power=0;power<EVENT_COUNT;power++) {
         if (element.inEvent & (0x01<<power)) {
            if (m_slEvents[power+1] != obj) {
               qDebug() << "testEventToEvent failed, event:" << power << "should map to:"
                  << j << "but map to" << (int)m_slEvents[power+1];
               success = false;
            }
         }
      }
   }
   qDebug() << "\n\n\ntestEventToEvent" << success;
   return success;
}

bool CanvasObjectManager::testEvenToObject() const
{
   /* This test validate the translation between events and object.
    * 
    * INPUT: events
    * OUTPUT: objects
    */

   hInitEvents();

   bool success = true;

   //0 is none and 1 any, so those need to be tested separately
   for (int index=2;index<EVENT_COUNT;index++) {
      CanvasObjectManager::CanvasEvent ev = eventIndexToFlag(index);
      if (eventToObject(ev) != m_slEvents[index]) {
         qDebug() << "testEvenToObject failed, power map to " << (int) m_slEvents[index]
            << "but eventToObject return" << (int)eventToObject(ev);
         success = false;
      }
   }

   qDebug() << "\n\n\ntestEvenToObject" << success;

   return success;
}


bool CanvasObjectManager::testFirstInEvent() const
{
   bool success = true;
   for (int j=0;j<ELEMENT_COUNT;j++) {
      CanvasObjectManager::CanvasEvent ev = firstInEvent(static_cast<CanvasObjectManager::Object>(j));
      if ((!(OBJ_DEF(j).inEvent & ev)) && OBJ_DEF(j).inEvent) {
         qDebug() << "testFirstInEvent failed" << j << "event is " << ev << " valid events:" << OBJ_DEF(j).inEvent;
         success = false;
      }
   }
   qDebug() << "\n\n\ntestFirstInEvent" << success;
   return success;
}

bool CanvasObjectManager::testObjectPriority()
{
   /* This test validate every possible transition to see if the event
    * always end up changing the currentObject to something of higher priority
    * 
    */
   m_DisableTransition = true;
   bool success = true;
   for (int j=0;j<ELEMENT_COUNT;j++) {
      CanvasObjectManager::Object obj = static_cast<CanvasObjectManager::Object>(j);
      for (int i=0;i<ELEMENT_COUNT;i++) {
         CanvasObjectManager::Object next = static_cast<CanvasObjectManager::Object>(i);
         reset();
         initiateInTransition(obj);
         if(obj!=m_CurrentObject) {
            qDebug() << "Current object mismatch";
            success = false;
         }
         Q_ASSERT(obj == m_CurrentObject);
         const bool overrideObj = newEvent(firstInEvent(next));
         if ((OBJ_DEF(next).priority >= OBJ_DEF(obj).priority) != overrideObj && !(obj != CanvasObjectManager::Object::NoObject && OBJ_DEF(obj).outEvent & firstInEvent(next))) {
            qDebug() << "PRE" << OBJ_DEF(obj).outEvent << firstInEvent(next) << (OBJ_DEF(obj).outEvent&&firstInEvent(next));
            qDebug() << "testObjectPriority failed" << (int)next << ((overrideObj)?"override":"doesn't override")
               << (int)obj << "current priority:" << OBJ_DEF(obj).priority << "next priority" << OBJ_DEF(next).priority << "..." << (int)m_CurrentObject;
            success = false;
         }
      }
   }
   qDebug() << "\n\n\ntestObjectPriority" << success;
   return success;
}

bool CanvasObjectManager::testObjectDiscarding()
{
   /* This test validate if every objects is discarded by the right events
    * 
    * 
    */
   m_DisableTransition = true;
   bool success = true;

   //First, lets test "ANY"
   for (int j=0;j<ELEMENT_COUNT;j++) {
      if (OBJ_DEF(j).outEvent == CanvasEvent::ANY) {
         for (int index=1;index<EVENT_COUNT;index++) {
            const CanvasEvent event = eventIndexToFlag(index);
            CanvasObjectManager::Object next = static_cast<CanvasObjectManager::Object>(j);
            reset();
            initiateInTransition(next);
            newEvent(static_cast<CanvasEvent>(event));
            if ((next == currentObject() && m_slEvents[index] != next)
               && !(next == currentObject() && currentObject() == CanvasObjectManager::Object::NoObject )) {
               qDebug() << "testObjectDiscarding failed with ANY, was" << j << "is" << (int)m_CurrentObject << event << (event == CanvasEvent::ANY);
               success = false;
            }
         }
      }
   }

   for (int j=0;j<ELEMENT_COUNT;j++) {
      if (OBJ_DEF(j).outEvent != CanvasEvent::ANY) {
         for (int index=2;index<EVENT_COUNT;index++) {
            CanvasObjectManager::Object next = static_cast<CanvasObjectManager::Object>(j);
            reset();
            initiateInTransition(next);
            const CanvasEvent event = eventIndexToFlag(index);
            newEvent(static_cast<CanvasEvent>(event));
            if ( OBJ_DEF(next).outEvent & event && next == currentObject() ) {
               qDebug() << "ntestObjectDiscarding failed, event #" << index << "should discard" << (int)next;
               success = false;
            }
         }
      }
   }

   qDebug() << "\n\n\ntestObjectDiscarding"<<success;
   return false;
}

#undef OBJ_DEF
