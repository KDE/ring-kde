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
#ifndef CANVASOBJECTMANAGER_H
#define CANVASOBJECTMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QAbstractAnimation>

//Qt
class QTimer;

class CanvasObjectManager : public QObject {
   Q_OBJECT

public:
   /*****************************************************************************************
    *                                                                                       *
    *                            --CANVAS OBJECT MANAGER--                                  *
    *                              ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯                                    *
    *                                                                                       *
    *                                  \_|¯¯¯¯¯¯¯¯¯¯|_/                                     *
    *                     Event(s)-->       System       Object-->                          *
    *                                  /¯|__________|¯\                                     *
    *                                                                                       *
    *                                                                                       *
    * This manager is implemented as a closed loop state machine, each external events      *
    * triggers a static set of operations defined at compile time.                          *
    *                                                                                       *
    *                                                                                       *
    *                                                  |¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯|          *
    * |¯¯¯¯¯|                                          |                         |          *
    * |  |¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯|   //Out_event // |       No objects        |          *
    * |_>|                          |--//Life cycle//->|                         |          *
    *    |     Current objects      | //          //   |_________________________|<¯¯|      *
    *    |                          |                             |            |     |      *
    *    |__________________________|<¯¯¯¯¯¯¯¯¯¯|                 |            |_____|      *
    *               |                           |                 |                         *
    *               |                           |     //                        //          *
    *               |                           |    // Event & Next:In_events //           *
    *     // Event & Next:In_events //          |   //                        //            *
    *    //  Next:prio >= Cur:prio //           |                 |                         *
    *   //                        //            |                \/                         *
    *               |                        |¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯|                   *
    *               |                        |                          |                   *
    *               |_______________________>|       Next object        |                   *
    *                                        |                          |                   *
    *                                        |__________________________|                   *
    *                                                                                       *
    *                                                                                       *
    * Some events can also carry a custom message to be displayed in the notification       *
    *                                                                                       *
    ****************************************************************************************/

   ///@enum Object All supported SFLPhone canvas objects (both elements and widgets)
   enum class Object         : uchar {
      NoObject       = 0, /* Blank canvas                                                  */
      DialInfo       = 1, /* The message explaining how to make a call                     */
      EndCall        = 2, /* Displayed when a call is over                                 */
      Ringing        = 3, /* Displayed when call(s) are ringing                            */
      Network        = 4, /* Displayed when there is a network error or ERRORAUTH          */
      AutoComplete   = 5, /* Displayed when the user is dialling                           */
      DropInfo       = 6, /* Displayed when the user is dropping something into the canvas */
      ConfInfo       = 7, /* Displayed when there is multiple calls and no conferences     */
      AccountDown    = 8, /* Displayed when a previously registered account go down        */
   };

   ///@enum ObjectType if the objects is a background drawing or a front widget
   enum class ObjectType      : uchar {
      OBJECT         = 0, /* The element is a simple drawing                               */
      WIDGET         = 1, /* The elements is a QWidget overlay                             */
   };

   ///@enum ObjectLifeCycle How is this object is supposed to withdraw itself from the canvas
   enum class ObjectLifeCycle : uchar {
      NONE           = 0, /* The element have an undefined life cycle                      */
      TIMED          = 1, /* The element is waiting on a timer tick                        */
      EVENT          = 2, /* The element is waiting for an external event                  */
      STATIC         = 3, /* The element is permanent until overwritten                    */
   };

   ///@enum ObjectState What is the state of an object
   enum class ObjectState     : uchar {
      NO_OBJECT      = 0, /* No object is being displayed                                  */
      TRANSITION_IN  = 1, /* The object is being introduced to the canvas                  */
      TRANSITION_OUT = 2, /* The object is being phased out                                */
      VISIBLE        = 3, /* The item is visible, waiting to be discarded                  */
   };

   ///@enum ObjectPriority Who can overwrite this object
   enum ObjectPriority  : uchar {
      NO_PRIORITY    = 0, /* Cannot discard nothing other and can be discarded by anyone    */
      LOW            = 1, /* The item can be discarded by any other objects                 */
      MEDIUM         = 2, /* The item can be discarded only by other MEDIUM || HIGH objects */
      HIGH           = 3, /* This item can discard any other objects, now                   */
   };

   enum CanvasEvent {
      //Generic events
      NONE                 = 0x00    , /* No events, for item already present on startup    */
      ANY                  = ~0L     , /* Every event match this                            */
      TIMEOUT              = 0x01<<1 , /* Waiting on an (internal) timer event              */

      //Account related events
      NETWORK_ERROR        = 0x01<<2 , /* When an ERRORAUTH or no Internet connection happen*/
      NETWORK_BACK         = 0x01<<3 , /* When the network is back                          */
      UNREGISTERED_ACCOUNT = 0x01<<4 , /* When an account fall down                         */
      REGISTERED_ACCOUNT   = 0x01<<5 , /* When an account become up again                   */

      //Input events
      DRAG_ENTER           = 0x01<<6 , /* When the user drag something into the canvas      */
      DRAG_LEAVE           = 0x01<<7 , /* When the pending-drag leave the canvas            */
      DRAG_MOVE            = 0x01<<8 , /* When the pending-drag position change             */
      DROP                 = 0x01<<9 , /* When a drop event is triggered                    */

      //Call events
      CALL_RINGING         = 0x01<<10, /* When a call is ringing                            */
      CALL_STATE_CHANGED   = 0x01<<11, /* When a phone call state change                    */
      CALL_DIALING_CHANGED = 0x01<<12, /* When the user type something on a dialing call    */
      CALL_COUNT_CHANGED   = 0x01<<13, /* When the number of call change                    */
      CALL_BUSY            = 0x01<<14, /* When a ringing call peer is busy                  */
      NO_CALLS             = 0x01<<15, /* When the last call have been removed from canvas  */
      CALL_ENDED           = 0x01<<16, /* When a call end                                   */
      BUDDY_IN             = 0x01<<17, /* When a buggy log in                               */
      BUDDY_OUT            = 0x01<<18, /* When a buddy log out                              */
      USER_CANCEL          = 0x01<<19, /* When the user press escape                        */
//       LEFT_CONFERENCE   = 0x01<<18, /* Someone left a conference                         */
//       JOIN_CONFERENCE   = 0x01<<19, /* Someone joined a conference                       */
   };

   //Constructor
   explicit CanvasObjectManager(QObject* parent = nullptr);
   virtual ~CanvasObjectManager();

   //Mutator
   bool newEvent   (CanvasEvent events, const QString& message = QString());
   void reset      (                  );

   //Operators
   bool operator <<(CanvasEvent events);

   //Getter
   CanvasObjectManager::Object currentObject() const;
   CanvasObjectManager::Object nextObject   () const;

private:

   ///@struct CanvasElement internal State Machine transition parameters
   struct CanvasElement {
      ObjectType      type              ; /* See ObjectType                                 */
      ObjectLifeCycle lifeCycle         ; /* See ObjectLifeCycle                            */
      ObjectPriority  priority          ; /* See ObjectPriority                             */
      int             inEvent           ; /* All compatible events to trigger this object   */
      int             outEvent          ; /* All compatible events to phase out this object */
      bool            stack             ; /* Use an event counter to phase this object out  */
      bool            systemNotification; /* Send a notification is SFLPhone ain't focused */
      bool            skipAnimation     ; /* Do not trigger out animation when overriding   */
   };

   //Constants
   static const char EVENT_COUNT   = 21;
   static const char ELEMENT_COUNT = 9 ;

   //Attributes
   CanvasObjectManager::Object          m_CurrentObject      ;
   CanvasObjectManager::Object          m_NextObject         ;
   CanvasObjectManager::ObjectState     m_CurrentState       ;
   CanvasObjectManager::ObjectLifeCycle m_CurrentLifeCycle   ;
   bool                                 m_DisableTransition  ;
   QTimer*                              m_pTimer             ;
   static CanvasObjectManager::Object   m_slEvents[ EVENT_COUNT+1 ];
   static const CanvasElement           elements  [ ELEMENT_COUNT ];

   //Helpers
   constexpr static bool haveEvent(CanvasEvent events,CanvasEvent event) {
      return events&event;
   }
   inline bool hasPriority(CanvasObjectManager::Object nextElement);
   static void hInitEvents();
   CanvasObjectManager::Object        eventToObject   ( CanvasEvent event  ) const;
   QList<CanvasObjectManager::Object> eventsToObjects ( CanvasEvent events ) const;
   CanvasEvent                        firstInEvent    ( Object      object ) const;
   CanvasEvent                        eventIndexToFlag( int index          ) const;
   int                                eventFlagToIndex( CanvasEvent events ) const;

   //Private methods
   void initiateOutTransition(bool skipAnimation = false);
   void initiateInTransition(Object nextObj,const QString& message = QString());

   //Tests
   bool testEventToEvent    () const;
   bool testEvenToObject    () const;
   bool testFirstInEvent    () const;
   bool testObjectPriority  ();
   bool testObjectDiscarding();

   //Attributes
   bool m_Minimized;

private Q_SLOTS:
   void slotTimeout();
   void slotTransitionEvents(QAbstractAnimation::Direction direction, QAbstractAnimation::State state);

public Q_SLOTS:
   void slotMinimized(bool isMinimized);

};

#endif
