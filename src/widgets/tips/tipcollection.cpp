/***************************************************************************
 *   Copyright (C) 2012-2013 by Savoir-Faire Linux                         *
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
#include "tipcollection.h"

#include <KLocale>

#include "dialpadtip.h"
#include "riggingtip.h"
#include "connectionlosttip.h"
#include "conftip.h"
#include "removeconferencetip.h"
#include "klib/tipmanager.h"

//Tutorial mode
Tip*               TipCollection::m_spDialPad          = nullptr;
ConfTip*           TipCollection::m_spConf             = nullptr;

//Call related
Tip*               TipCollection::m_spEndCall          = nullptr;
Tip*               TipCollection::m_spEndBusy          = nullptr;
Tip*               TipCollection::m_spRigging          = nullptr;
Tip*               TipCollection::m_spRemoveConference = nullptr;

//Account related
ConnectionLostTip* TipCollection::m_spConnectionLost   = nullptr;

//Network related
Tip*               TipCollection::m_spNetworkLost      = nullptr;

//Global manager
TipManager*        TipCollection::m_spManager          = nullptr;


//Tutorial mode
Tip* TipCollection::dialPad()
{
   if (!m_spDialPad) {
      m_spDialPad = new DialPadTip();
   }
   return m_spDialPad;
}

///Return the conference tip
Tip* TipCollection::conference()
{
   return m_spConf;
}

///Return the drag and drop tip
Tip* TipCollection::dragAndDrop()
{
   if (!m_spConf) {
      m_spConf = new ConfTip();
   }
   return m_spConf;
}

//Call related
///Return end call tip
Tip* TipCollection::endCall()
{
   if (!m_spEndCall) {
      m_spEndCall = new Tip(i18n("Call ended"));
      m_spEndCall->setTimeOut(3000);
   }
   return m_spEndCall;
}

///Return endBusy tip
Tip* TipCollection::endBusy()
{
   if (!m_spEndBusy) {
      m_spEndBusy = new Tip(i18n("Peer is busy"));
      m_spEndBusy->setTimeOut(3000);
   }
   return m_spEndBusy;
}

///Return rigging tip
Tip* TipCollection::rigging()
{
   if (!m_spRigging) {
      m_spRigging = new RiggingTip();
      m_spRigging->setAnimationIn(Tip::TipAnimation::Fade);
      m_spRigging->setAnimationOut(Tip::TipAnimation::Fade);
   }
   return m_spRigging;
}

//Account related
///Return connectionLost tip
Tip* TipCollection::connectionLost()
{
   if (!m_spConnectionLost) {
      m_spConnectionLost = new ConnectionLostTip();
      m_spConnectionLost->setAnimationIn(Tip::TipAnimation::Fade);
      m_spConnectionLost->setAnimationOut(Tip::TipAnimation::Fade);
   }
   return m_spConnectionLost;
}

//Network related
///Return networkLost tip
Tip* TipCollection::networkLost()
{
   if (!m_spNetworkLost) {
      m_spNetworkLost = new Tip("Network lost");
   }
   return m_spNetworkLost;
}

///What happen if you drop a call from a conference on empty space
Tip* TipCollection::removeConference()
{
   if (!m_spRemoveConference) {
      m_spRemoveConference = new RemoveConferenceTip();
      m_spRemoveConference->setAnimationIn(Tip::TipAnimation::Fade);
      m_spRemoveConference->setAnimationOut(Tip::TipAnimation::Fade);
   }
   return m_spRemoveConference;
}

//Global manager
///Return the global tip manager
TipManager* TipCollection::manager()
{
   return m_spManager;
}

///Set the global tip manager
void TipCollection::setManager(TipManager* manager)
{
   m_spManager = manager;
}
