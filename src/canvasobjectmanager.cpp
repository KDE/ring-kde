/***************************************************************************
 *   Copyright (C) 2013 by Savoir-Faire Linux                              *
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

#include <QtCore/QDebug>

CanvasObjectManager::Object CanvasObjectManager::m_slEvents[EVENT_COUNT] = { CanvasObjectManager::Object::NoObject };

#define _(NAME) CanvasObjectManager::Object##NAME::
#define E CanvasEvent
const CanvasObjectManager::CanvasElement CanvasObjectManager::elements[ELEMENT_COUNT] = {
   /*                  ObjectType       ObjectLifeCycle        ObjectPriority             IN-EVENT                   OUT-EVENT         STACK   NOTIFY */
   /*NoObject     */ {_(Type)OBJECT , _(LifeCycle)STATIC , _(Priority)NO_PRIORITY, E::NONE                    , E::ANY               , false , false  },
   /*DialInfo     */ {_(Type)OBJECT , _(LifeCycle)STATIC , _(Priority)LOW        , E::NO_CALLS                , E::ANY               , false , false  },
   /*EndCall      */ {_(Type)OBJECT , _(LifeCycle)TIMED  , _(Priority)LOW        , E::CALL_ENDED              , E::ANY               , false , true   },
   /*Ringing      */ {_(Type)OBJECT , _(LifeCycle)EVENT  , _(Priority)MEDIUM     , E::CALL_RINGING            , E::CALL_STATE_CHANGED, true  , true   },
   /*Network      */ {_(Type)OBJECT , _(LifeCycle)EVENT  , _(Priority)MEDIUM     , E::NETWORK_ERROR           , E::ANY               , false , true   },
   /*AutoComplete */ {_(Type)WIDGET , _(LifeCycle)EVENT  , _(Priority)HIGH       , E::CALL_DIALING_CHANGED    , E::CALL_STATE_CHANGED, false , false  },
   /*DropInfo     */ {_(Type)OBJECT , _(LifeCycle)EVENT  , _(Priority)MEDIUM     , E::DRAG_ENTER|E::DRAG_MOVE , E::DRAG_LEAVE|E::DROP, false , false  },
   /*ConfInfo     */ {_(Type)OBJECT , _(LifeCycle)EVENT  , _(Priority)LOW        , E::CALL_COUNT_CHANGED      , E::ANY               , false , false  },
};
#undef _
#undef E

void CanvasObjectManager::hInitEvents() {
   static bool init = false;
   if (!init) {
      for (int i=0;i<EVENT_COUNT-1;i++) {
         for (int j=1;j<ELEMENT_COUNT;j++) {
            if (CanvasObjectManager::elements[j].inEvent & (0x01<<i))
               CanvasObjectManager::m_slEvents[i+1] = static_cast<CanvasObjectManager::Object>(j);
         }
      }
      init = true;
   }
   for(int j=0;j<EVENT_COUNT;j++) {
      qDebug() << "ELEM" << j << (int)m_slEvents[j];
   }
}

CanvasObjectManager::CanvasObjectManager(QObject* parent) : QObject(parent)
{
   hInitEvents();
}

CanvasObjectManager::~CanvasObjectManager()
{
   //Nothing to do
}

bool CanvasObjectManager::newEvent(CanvasEvent events)
{
   bool multiEvent = ((events & ~(events-1))==events)? events : 0;
   if (!multiEvent) {
      int power;
      for (power=ELEMENT_COUNT;power;power--)
         if (events & (0x01<<power)) break;
      if (power) {
         const CanvasObjectManager::Object nextObj = m_slEvents[power];
         if (elements[(int)nextObj].priority >= elements[(int)m_CurrentObject].priority)
            return true;
      }
   }
   //TODO handle many simultaneous events?
   return false;
}

bool CanvasObjectManager::operator<<(CanvasEvent events)
{
   return newEvent(events);
}
