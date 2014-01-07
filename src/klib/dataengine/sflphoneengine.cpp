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

#include "sflphoneengine.h"

//KDE
#include <Plasma/DataContainer>
#include <Plasma/Service>

//SFLPhone
#include "../../lib/call.h"
#include "../../lib/account.h"
#include "../../lib/accountlistmodel.h"
#include "../../lib/contact.h"
#include "../../lib/dbus/metatypes.h"
#include "../../lib/dbus/instancemanager.h"
#include "../../lib/dbus/configurationmanager.h"
#include "../../lib/dbus/callmanager.h"
#include "../../lib/sflphone_const.h"
#include "../../lib/phonenumber.h"
#include "../../lib/phonedirectorymodel.h"
#include "../../klib/akonadibackend.h"
#include "../../klib/helperfunctions.h"
#include "../../klib/kcfg_settings.h"
#include "../../lib/callmodel.h"
#include "../../lib/historymodel.h"
#include "sflphoneservice.h"

//Static
CallModel* SFLPhoneEngine::m_pModel = nullptr;


/*****************************************************************************
 *                                                                           *
 *                               Constructor                                 *
 *                                                                           *
 ****************************************************************************/

///Constructor
SFLPhoneEngine::SFLPhoneEngine(QObject* parent, const QVariantList& args)
    : Plasma::DataEngine(parent, args)
{
   Q_UNUSED(args)
   if (not m_pModel) {
      m_pModel = CallModel::instance();
      //m_pModel->initHistory();
   }

   /*                SOURCE                             SIGNAL                 DESTINATION              SLOT                */
   /**/connect(m_pModel                     , SIGNAL(callStateChanged(Call*))  , this , SLOT(callStateChangedSignal(Call*)) );
   /**/connect(m_pModel                     , SIGNAL(callAdded(Call*))         , this , SLOT(callStateChangedSignal(Call*)) );
   /**/connect(m_pModel                     , SIGNAL(callStateChanged(Call*))  , this , SLOT(callStateChangedSignal(Call*)) );
   /**/connect(AkonadiBackend::instance(), SIGNAL(collectionChanged())      , this , SLOT(updateCollection())            );
   /*                                                                                                                       */
}


/*****************************************************************************
 *                                                                           *
 *                           Dateengine internal                             *
 *                                                                           *
 ****************************************************************************/

///Fill a source only when it is called for the first time, then do it asyncroniously
bool SFLPhoneEngine::sourceRequestEvent(const QString &name)
{
   /*                SOURCE                        CALLBACK         */
   if      ( name == "history"         ) { updateHistory();          }
   else if ( name == "calls"           ) { updateCallList();         }
   else if ( name == "conferences"     ) { updateConferenceList();   }
   else if ( name == "info"            ) { updateInfo();             }
   else if ( name == "accounts"        ) { updateAccounts();         }
   else if ( name == "contacts"        ) { updateContacts();         }
   else if ( name == "bookmark"        ) { updateBookmarkList();     }
   else if ( name.left(7) == "Number:" ) { generateNumberList(name); }
   /*                                                               */
   
   return true;//updateSourceEvent(name);
}

///Not used
bool SFLPhoneEngine::updateSourceEvent(const QString &name)
{
   Q_UNUSED(name)
   return true;
}

///List all default valid sources, more can be requested dynamically
QStringList SFLPhoneEngine::sources() const {
   QStringList toReturn;
   toReturn << "calls" << "history" << "conferences" << "info" << "accounts" << "contacts" << "bookmark";
   return toReturn;
}

///Return the service used for RPC
Plasma::Service* SFLPhoneEngine::serviceForSource(const QString &source)
{
    if (source != "calls")
      return 0;

    SFLPhoneService* service = new SFLPhoneService(this);
    service->setParent(this);
    return service;
}

/*****************************************************************************
 *                                                                           *
 *                                  Getters                                  *
 *                                                                           *
 ****************************************************************************/

///Return the model
CallModel* SFLPhoneEngine::getModel()
{
   return m_pModel;
}


/*****************************************************************************
 *                                                                           *
 *                                Callbacks                                  *
 *                                                                           *
 ****************************************************************************/

///Load/Update history
void SFLPhoneEngine::updateHistory()
{
   CallList list = HistoryModel::getHistory().values();
//    setHistoryCategory(list,HistorySortingMode::Date);

   foreach (Call* oldCall, list) {
      HashStringString current;
      /*             KEY                   VALUE                                                      */
      /**/current[ "peerName"   ] = oldCall->peerName       ()                                         ;
      /**/current[ "peerNumber" ] = oldCall->peerPhoneNumber()->uri()                                  ;
      /**/current[ "length"     ] = (int) oldCall->stopTimeStamp  () - (int) oldCall->startTimeStamp() ;
      /**/current[ "date"       ] = (int) oldCall->stopTimeStamp  ()                                   ;
      /**/current[ "id"         ] = oldCall->id             ()                                         ;
      /*                                                                                              */
      if (oldCall->property("section").isValid())
         current[ "section" ] = oldCall->property("section");
      setData("history", oldCall->id() , current);
   }
}

///Load/Update calllist
void SFLPhoneEngine::updateCallList()
{
   //As of KDE 4.8, an empty source are ignored, adding an invisible entry
   QStringList keys;
   keys << "peerName" << "peerNumber" << "stateName" << "state" << "id";
   QHash<QString,QVariant> fake;
   foreach (const QString& key, keys) {
      fake[key] = "";
   }
   setData("calls", "fake",fake );
   removeAllData("calls");
//    foreach (Call* call, m_pModel->getCalls()) {
//       if ((!call->isConference()) && (call->getState() != CALL_STATE_OVER)) {
//          HashStringString current;
//          /*               KEY                     VALUE              */
//          /**/current[ "peerName"      ] = call->getPeerName        ( );
//          /**/current[ "peerNumber"    ] = call->getPeerPhoneNumber ( );
//          /**/current[ "stateName"     ] = call->toHumanStateName   ( );
//          /**/current[ "state"         ] = call->getState           ( );
//          /**/current[ "id"            ] = call->getCallId          ( );
//          /*                                                          */
//          setData("calls", call->getCallId(), current);
//       }
//    }
}

///Load/Update bookmark list
void SFLPhoneEngine::updateBookmarkList()
{
   removeAllData("bookmark");
   int i=0;
   QVector<PhoneNumber*> cl = PhoneDirectoryModel::instance()->getNumbersByPopularity();
   for (;i < ((cl.size() < 10)?cl.size():10);i++) {
      QHash<QString,QVariant> pop;
      const Contact* cont = cl[i]->contact();
      /*           KEY                          VALUE                     */
      /**/pop["peerName"     ] = (cont)?cont->formattedName():cl[i]->uri() ;
      /**/pop["peerNumber"   ] = cl[i]->uri()                              ;
      /**/pop["section"      ] = "Popular"                                 ;
      /**/pop["listPriority" ] = 1000                                      ;
      /**/pop["id"           ] = i                                         ;
      /*                                                                  */

      setData("bookmark", QString::number(i), pop);
   }

   //TODO Wont work for now
   foreach (const QString& nb, ConfigurationSkeleton::bookmarkList()) {
      i++;
      QHash<QString,QVariant> pop;
      /*             KEY          VALUE */
      /**/pop["peerName"     ] = "TODO"  ;
      /**/pop["peerNumber"   ] = nb      ;
      /**/pop["section"      ] = '1'     ;
      /**/pop["listPriority" ] = 0       ;
      /**/pop["id"           ] = i       ;
      /*                                */
      
      setData("bookmark", QString::number(i), pop);
   }
}

///Load/Update conference list (TODO)
void SFLPhoneEngine::updateConferenceList()
{
   /*foreach (Call* call, m_pModel->getCalls()) {
      if (m_pModel->isConference(call)) {
         CallManagerInterface& callManager = DBus::CallManager::instance();
         currentConferences[call->getConfId()] = callManager.getParticipantList(call->getConfId());
         setData("conferences", call->getConfId(), currentConferences[call->getConfId()]);
      }
   }*/
}

///Update contact collection
void SFLPhoneEngine::updateCollection()
{
   
   typedef QHash<QString,QVariant> SerializedContact;
   ContactList list = AkonadiBackend::instance()->update();
   
   if (!list.size())
      return;
   
   const ContactHash hash = HelperFunctions::toHash(list);
   foreach (const SerializedContact& cont, hash) {
      if (!m_hContacts[hash.key(cont)].size()) {
         m_hContacts[hash.key(cont)] = cont;
      }
   }
   
   removeAllData("contacts");
   int i=0;
   foreach (const SerializedContact& cont, m_hContacts) {
//       cont["section"] = "test";
      setData("contacts", QString::number(i), QVariant(cont));
      i++;
   }
   
   updateBookmarkList();
}

///Dummy implementation of the contact list (TOREMOVE)
void SFLPhoneEngine::updateContacts()
{
   //As of KDE 4.8, an empty source is ignored, adding an invisible entry
   QStringList keys;
   keys << "nickName" << "firstName"      << "secondName" << "formattedName" << "organization" <<
            "Uid"     << "preferredEmail" << "type"       << "group"         << "department";
   
   QHash<QString,QVariant> fake;
   foreach(const QString& key,keys) {
      fake[key].clear();
   }
   setData("contacts", "fake",fake );
}

///Update other information
void SFLPhoneEngine::updateInfo()
{
   setData("info", I18N_NOOP("Current_account"), AccountListModel::currentAccount()->id());
}

///Load/Update account list
void SFLPhoneEngine::updateAccounts()
{
   const QVector<Account*>& list = AccountListModel::instance()->getAccounts();
   foreach(Account* a,list) {
      if (dynamic_cast<Account*>(a)) {
         QHash<QString,QVariant> acc;
         acc[ "id"   ] = a->id()                 ;
         acc[ "alias"] = a->accountDetail(Account::MapField::ALIAS);
         setData("accounts", QString::number(rand()) , acc);
      }
   }
}


/*****************************************************************************
 *                                                                           *
 *                                 Mutators                                  *
 *                                                                           *
 ****************************************************************************/

///Generate a number
void SFLPhoneEngine::generateNumberList(QString name)
{
   QString contactUid = name.right(name.size()-7);
   qDebug() << "LOOKING FOR " << contactUid;
   Contact* cont = AkonadiBackend::instance()->getContactByUid(contactUid);
   if (cont) {
      foreach(PhoneNumber* num,cont->phoneNumbers()) {
         QHash<QString,QVariant> hash;
         hash[ "number" ] = num->uri() ;
         hash[ "type"   ] = num->type();
         setData(name, QString::number(rand()) , hash);
      }
   }
   else {
      kDebug() << "Contact not found";
   }
}

/*****************************************************************************
 *                                                                           *
 *                                   Slots                                   *
 *                                                                           *
 ****************************************************************************/

///When call state change
void SFLPhoneEngine::callStateChangedSignal(Call* call)
{
   Q_UNUSED(call)
   updateCallList();
}

///When incoming call
void SFLPhoneEngine::incomingCallSignal(Call* call)
{
   Q_UNUSED(call)
   updateCallList();
}

///When incoming messge
void SFLPhoneEngine::incomingMessageSignal(const QString& accountId, const QString& message)
{
   Q_UNUSED(accountId)
   Q_UNUSED(message)
   //TODO
}

///When voicemail notify
void SFLPhoneEngine::voiceMailNotifySignal(const QString& accountId, int count)
{
   Q_UNUSED(accountId)
   Q_UNUSED(count)
   //TODO
}

#pragma GCC diagnostic ignored "-Wmissing-declarations"
K_EXPORT_PLASMA_DATAENGINE(sflphone, SFLPhoneEngine)
