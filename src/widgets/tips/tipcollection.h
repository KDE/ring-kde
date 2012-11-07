/***************************************************************************
 *   Copyright (C) 2009-2012 by Savoir-Faire Linux                         *
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

class DialPadTip;
class Tip;
class TipManager;

class TipCollection {
public:
   //Tutorial mode
   static DialPadTip* dialPad();
   static Tip*        conference();
   
   //Call related
   static Tip*        endCall();
   static Tip*        rigging();
   
   //Account related
   static Tip*        connectionLost();
   
   //Network related
   static Tip*        networkLost();
   
   //Manager
   static TipManager* manager();
   static void setManager(TipManager* manager);
private:
   //Tutorial mode
   static DialPadTip* m_spDialPad;
   static Tip*        m_spConf;
   
   //Call related
   static Tip*        m_spEndCall;
   static Tip*        m_spRigging;
   
   //Account related
   static Tip*        m_spConnectionLost;
   
   //Network related
   static Tip*        m_spNetworkLost;
   
   //Global manager
   static TipManager* m_spManager;
};

#endif