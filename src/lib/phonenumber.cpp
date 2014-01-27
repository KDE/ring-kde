/****************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                          *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
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
#include "phonenumber.h"
#include "phonedirectorymodel.h"
#include "contact.h"
#include "account.h"
#include "call.h"
#include "dbus/presencemanager.h"
#include "numbercategorymodel.h"
#include "numbercategory.h"

QHash<int,Call*> PhoneNumber::m_shMostUsed = QHash<int,Call*>();

const PhoneNumber* PhoneNumber::m_spBlank = nullptr;
const PhoneNumber* PhoneNumber::BLANK()
{
   if (!m_spBlank) {
      m_spBlank = new PhoneNumber("",NumberCategoryModel::other());
      const_cast<PhoneNumber*>(m_spBlank)->m_Type = PhoneNumber::Type::BLANK;
   }
   return m_spBlank;
}

///Constructor
PhoneNumber::PhoneNumber(const QString& number, NumberCategory* cat, Type st) : QObject(PhoneDirectoryModel::instance()),
   m_Uri(stripUri(number)),m_pCategory(cat),m_Tracked(false),m_Present(false),m_LastUsed(0),
   m_Type(st),m_PopularityIndex(-1),m_pContact(nullptr),m_pAccount(nullptr),
   m_LastWeekCount(0),m_LastTrimCount(0),m_HaveCalled(false),m_IsBookmark(false),m_TotalSeconds(0)
{
   setObjectName(m_Uri);
   m_hasType = cat != NumberCategoryModel::other();
   if (m_hasType) {
      NumberCategoryModel::instance()->registerNumber(this);
   }
}

PhoneNumber::~PhoneNumber()
{
}

///Return if this number presence is being tracked
bool PhoneNumber::isTracked() const
{
   //If the number doesn't support it, ignore the flag
   return supportPresence() && m_Tracked;
}

///Is this number present
bool PhoneNumber::isPresent() const
{
   return m_Tracked && m_Present;
}

///This number presence status string
QString PhoneNumber::presenceMessage() const
{
   return m_PresentMessage;
}

///Return the number
QString PhoneNumber::uri() const {
   return m_Uri ;
}

///Return the phone number type
NumberCategory* PhoneNumber::category() const {
   return m_pCategory ;
}

///Return this number associated account, if any
Account* PhoneNumber::account() const
{
   return m_pAccount;
}

///Return this number associated contact, if any
Contact* PhoneNumber::contact() const
{
   return m_pContact;
}

///Return when this number was last used
time_t PhoneNumber::lastUsed() const
{
   return m_LastUsed;
}

///Set this number default account
void PhoneNumber::setAccount(Account* account)
{
   m_pAccount = account;
   if (m_pAccount)
      connect (m_pAccount,SIGNAL(destroyed(QObject*)),this,SLOT(accountDestroyed(QObject*)));
   emit changed();
}

///Set this number contact
void PhoneNumber::setContact(Contact* contact)
{
   m_pContact = contact;
   if (contact && m_Type != PhoneNumber::Type::TEMPORARY)
      PhoneDirectoryModel::instance()->indexNumber(this,m_hNames.keys()+QStringList(contact->formattedName()));
   emit changed();
}

void PhoneNumber::setCategory(NumberCategory* cat)
{
   if (cat == m_pCategory) return;
   if (m_hasType)
      NumberCategoryModel::instance()->unregisterNumber(this);
   m_hasType = cat != NumberCategoryModel::other();
   m_pCategory = cat;
   if (m_hasType)
      NumberCategoryModel::instance()->registerNumber(this);
   emit changed();
}

void PhoneNumber::setBookmarked(bool bookmarked )
{
   m_IsBookmark = bookmarked;
}

///Force an Uid on this number (instead of hash)
void PhoneNumber::setUid(const QString& uri)
{
   m_Uid = uri;
}

///Attempt to change the number type
bool PhoneNumber::setType(PhoneNumber::Type t)
{
   if (m_Type == PhoneNumber::Type::BLANK)
      return false;
   if (account() && t == PhoneNumber::Type::ACCOUNT) {
      if (account()->supportPresenceSubscribe()) {
         m_Tracked = true; //The daemon will init the tracker itself
         emit trackedChanged(true);
      }
      m_Type = t;
      return true;
   }
   return false;
}

///Set if this number is tracking presence information
void PhoneNumber::setTracked(bool track)
{
   if (track != m_Tracked) { //Subscribe only once
      //You can't subscribe without account
      if (track && !m_pAccount) return;
      m_Tracked = track;
      DBus::PresenceManager::instance().subscribeBuddy(m_pAccount->id(),fullUri(),track);
      emit changed();
      emit trackedChanged(track);
   }
}

///Allow phonedirectorymodel to change presence status
void PhoneNumber::setPresent(bool present)
{
   if (m_Present != present) {
      m_Present = present;
      emit presentChanged(present);
   }
}

void PhoneNumber::setPresenceMessage(const QString& message)
{
   if (m_PresentMessage != message) {
      m_PresentMessage = message;
      emit presenceMessageChanged(message);
   }
}

///Return the current type of the number
PhoneNumber::Type PhoneNumber::type() const
{
   return m_Type;
}

///Return the number of calls from this number
int PhoneNumber::callCount() const
{
   return m_lCalls.size();
}

uint PhoneNumber::weekCount() const
{
   return m_LastWeekCount;
}

uint PhoneNumber::trimCount() const
{
   return m_LastTrimCount;
}

bool PhoneNumber::haveCalled() const
{
   return m_HaveCalled;
}

///Best bet for this person real name
QString PhoneNumber::primaryName() const
{
   if (m_pContact)
      return m_pContact->formattedName();
   else if (m_hNames.size() == 1)
      return m_hNames.constBegin().key();
   else {
      QString toReturn = tr("Unknown");
      QHash<QString,int>::const_iterator i = m_hNames.constBegin();
      int max = 0;
      while (i != m_hNames.end()) {
         if (i.value() > max) {
            max      = i.value();
            toReturn = i.key  ();
         }
         i++;
      }
      return toReturn;
   }
}

///Is this number bookmarked
bool PhoneNumber::isBookmarked() const
{
   return m_IsBookmark;
}

///If this number could (theoretically) support presence status
bool PhoneNumber::supportPresence() const
{
   //Without an account, presence is impossible
   if (!m_pAccount)
      return false;
   //The account also have to support it
   if (!m_pAccount->supportPresenceSubscribe())
       return false;

   //In the end, it all come down to this, is the number tracked
   return true;
}

///Proxy accessor to the category icon
QVariant PhoneNumber::icon() const
{
   return category()->icon(isTracked(),isPresent());
}

///The number of seconds spent with the URI (from history)
int PhoneNumber::totalSpentTime() const
{
   return m_TotalSeconds;
}

///Return this number unique identifier (hash)
QString PhoneNumber::uid() const
{
   return m_Uid.isEmpty()?toHash():m_Uid;
}

///Return all calls from this number
QList<Call*> PhoneNumber::calls() const
{
   return m_lCalls;
}

///Return the phonenumber position in the popularity index
int PhoneNumber::popularityIndex() const
{
   return m_PopularityIndex;
}

QHash<QString,int> PhoneNumber::alternativeNames() const
{
   return m_hNames;
}

///Add a call to the call list, notify listener
void PhoneNumber::addCall(Call* call)
{
   if (!call) return;
   m_Type = PhoneNumber::Type::USED;
   m_lCalls << call;
   m_TotalSeconds += call->stopTimeStamp() - call->startTimeStamp();
   time_t now;
   ::time ( &now );
   if (now - 3600*24*7 < call->stopTimeStamp())
      m_LastWeekCount++;
   if (now - 3600*24*7*15 < call->stopTimeStamp())
      m_LastTrimCount++;

   if (call->historyState() == Call::LegacyHistoryState::OUTGOING || call->direction() == Call::Direction::OUTGOING)
      m_HaveCalled = true;

   emit callAdded(call);
   if (call->startTimeStamp() > m_LastUsed)
      m_LastUsed = call->startTimeStamp();
   emit changed();
}

///Generate an unique representation of this number
QString PhoneNumber::toHash() const
{
   return QString("%1///%2///%3").arg(uri()).arg(account()?account()->id():QString()).arg(contact()?contact()->uid():QString());
}


///Return the domaine of an URI (<sip:12345@exemple.com>)
QString PhoneNumber::hostname() const
{
   if (m_Uri.indexOf('@') != -1) {
      return m_Uri.split('@')[1].left(m_Uri.split('@')[1].size());
   }
   return QString();
}

QString PhoneNumber::fullUri() const
{
   return QString("<sip:%1>").arg(m_Uri);
}


///Strip out <sip:****> from the URI
QString PhoneNumber::stripUri(const QString& uri)
{
   int start(0),end(uri.size()-1); //Other type of comparaisons were too slow
   if (uri.size() > 0 && uri[0] == '<' && uri[4] == ':')
      start = 5;
   if (end && uri[end] == '>')
      end--;
   return uri.mid(start,end-start+1);
}

///Increment name counter and update indexes
void PhoneNumber::incrementAlternativeName(const QString& name)
{
   const bool needReIndexing = !m_hNames[name];
   m_hNames[name]++;
   if (needReIndexing && m_Type != PhoneNumber::Type::TEMPORARY)
      PhoneDirectoryModel::instance()->indexNumber(this,m_hNames.keys()+(m_pContact?(QStringList(m_pContact->formattedName())):QStringList()));
}

void PhoneNumber::accountDestroyed(QObject* o)
{
   if (o == m_pAccount)
      m_pAccount = nullptr;
}

/************************************************************************************
 *                                                                                  *
 *                             Temporary phone number                               *
 *                                                                                  *
 ***********************************************************************************/

void TemporaryPhoneNumber::setUri(const QString& uri)
{
   m_Uri = uri;
   emit changed();
}

///Constructor
TemporaryPhoneNumber::TemporaryPhoneNumber(const PhoneNumber* number) :
   PhoneNumber(QString(),NumberCategoryModel::other(),PhoneNumber::Type::TEMPORARY)
{
   if (number) {
      setContact(number->contact());
      setAccount(number->account());
   }
}
