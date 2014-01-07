/***************************************************************************
 *   Copyright (C) 2012-2014 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Valle <emmanuel.lepage@savoirfairelinux.com >*
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
#ifndef TIP_COLLECTION_H
#define TIP_COLLECTION_H

#include <QtCore/QHash>

#include <canvasobjectmanager.h>

//Qt
class QWidget;

//SFLPhone
class Tip;
class TipManager;
class ConnectionLostTip;
class ConfTip;

class TipCollection {
public:
   //CanvasManager
   static Tip*       canvasObjectToTip(CanvasObjectManager::Object obj);

   //Tutorial mode
   static Tip*        dialPad    ();
   static Tip*        conference ();
   static Tip*        dragAndDrop();

   //Call related
   static Tip*        endCall();
   static Tip*        endBusy();
   static Tip*        ringing();
   static Tip*        removeConference();

   //Account related
   static Tip*        connectionLost();

   //Network related
   static Tip*        networkLost();

   //Manager
   static TipManager* manager();
   static void setManager(TipManager* manager);

   //Widgets
   static QWidget*    canvasWidgetsToTip(CanvasObjectManager::Object obj);

private:
   //Tutorial mode
   static Tip* m_spDialPad;
   static ConfTip*    m_spConf   ;

   //Call related
   static Tip*        m_spEndCall;
   static Tip*        m_spEndBusy;
   static Tip*        m_spRinging;
   static Tip*        m_spRemoveConference;

   //Account related
   static ConnectionLostTip* m_spConnectionLost;

   //Network related
   static Tip*        m_spNetworkLost;

   //Global manager
   static TipManager* m_spManager;
};

#endif
