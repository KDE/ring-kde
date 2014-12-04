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

//Private
#include "private/phonedirectorymodel_p.h"

QHash<int,Call*> PhoneNumber::m_shMostUsed = QHash<int,Call*>();

const PhoneNumber* PhoneNumber::m_spBlank = nullptr;

class PrivatePhoneNumber {
public:
   PrivatePhoneNumber(const URI& number, NumberCategory* cat, PhoneNumber::Type st);
   NumberCategory*    m_pCategory        ;
   bool               m_Present          ;
   QString            m_PresentMessage   ;
   bool               m_Tracked          ;
   Contact*           m_pContact         ;
   Account*           m_pAccount         ;
   time_t             m_LastUsed         ;
   QList<Call*>       m_lCalls           ;
   int                m_PopularityIndex  ;
   QString            m_MostCommonName   ;
   QHash<QString,int> m_hNames           ;
   bool               m_hasType          ;
   uint               m_LastWeekCount    ;
   uint               m_LastTrimCount    ;
   bool               m_HaveCalled       ;
   int                m_Index            ;
   bool               m_IsBookmark       ;
   int                m_TotalSeconds     ;
   QString            m_Uid              ;
   QString            m_PrimaryName_cache;
   URI                m_Uri              ;
   PhoneNumber::Type  m_Type             ;
   QList<URI>         m_lOtherURIs       ;

   //Parents
   QList<PhoneNumber*> m_lParents;

   //Emit proxies
   void callAdded(Call* call);
   void changed  (          );
   void presentChanged(bool);
   void presenceMessageChanged(const QString&);
   void trackedChanged(bool);
   void primaryNameChanged(const QString& name);
   void rebased(PhoneNumber* other);
};

void PrivatePhoneNumber::callAdded(Call* call)
{
   foreach (PhoneNumber* n, m_lParents)
      emit n->callAdded(call);
}

void PrivatePhoneNumber::changed()
{
   foreach (PhoneNumber* n, m_lParents)
      emit n->changed();
}

void PrivatePhoneNumber::presentChanged(bool s)
{
   foreach (PhoneNumber* n, m_lParents)
      emit n->presentChanged(s);
}

void PrivatePhoneNumber::presenceMessageChanged(const QString& status)
{
   foreach (PhoneNumber* n, m_lParents)
      emit n->presenceMessageChanged(status);
}

void PrivatePhoneNumber::trackedChanged(bool t)
{
   foreach (PhoneNumber* n, m_lParents)
      emit n->trackedChanged(t);
}

void PrivatePhoneNumber::primaryNameChanged(const QString& name)
{
   foreach (PhoneNumber* n, m_lParents)
      emit n->primaryNameChanged(name);
}

void PrivatePhoneNumber::rebased(PhoneNumber* other)
{
   foreach (PhoneNumber* n, m_lParents)
      emit n->rebased(other);
}


const PhoneNumber* PhoneNumber::BLANK()
{
   if (!m_spBlank) {
      m_spBlank = new PhoneNumber(QString(),NumberCategoryModel::other());
      const_cast<PhoneNumber*>(m_spBlank)->d_ptr->m_Type = PhoneNumber::Type::BLANK;
   }
   return m_spBlank;
}

PrivatePhoneNumber::PrivatePhoneNumber(const URI& uri, NumberCategory* cat, PhoneNumber::Type st) :
   m_Uri(uri),m_pCategory(cat),m_Tracked(false),m_Present(false),m_LastUsed(0),
   m_Type(st),m_PopularityIndex(-1),m_pContact(nullptr),m_pAccount(nullptr),
   m_LastWeekCount(0),m_LastTrimCount(0),m_HaveCalled(false),m_IsBookmark(false),m_TotalSeconds(0),
   m_Index(-1)
{}

///Constructor
PhoneNumber::PhoneNumber(const URI& number, NumberCategory* cat, Type st) : QObject(PhoneDirectoryModel::instance()),
d_ptr(new PrivatePhoneNumber(number,cat,st))
{
   setObjectName(d_ptr->m_Uri);
   d_ptr->m_hasType = cat != NumberCategoryModel::other();
   if (d_ptr->m_hasType) {
      NumberCategoryModel::instance()->registerNumber(this);
   }
   d_ptr->m_lParents << this;
}

PhoneNumber::~PhoneNumber()
{
   d_ptr->m_lParents.removeAll(this);
   if (!d_ptr->m_lParents.size())
      delete d_ptr;
}

///Return if this number presence is being tracked
bool PhoneNumber::isTracked() const
{
   //If the number doesn't support it, ignore the flag
   return supportPresence() && d_ptr->m_Tracked;
}

///Is this number present
bool PhoneNumber::isPresent() const
{
   return d_ptr->m_Tracked && d_ptr->m_Present;
}

///This number presence status string
QString PhoneNumber::presenceMessage() const
{
   return d_ptr->m_PresentMessage;
}

///Return the number
URI PhoneNumber::uri() const {
   return d_ptr->m_Uri ;
}

///This phone number has a type
bool PhoneNumber::hasType() const
{
   return d_ptr->m_hasType;
}

///Protected getter to get the number index
int PhoneNumber::index() const
{
   return d_ptr->m_Index;
}

///Return the phone number type
NumberCategory* PhoneNumber::category() const {
   return d_ptr->m_pCategory ;
}

///Return this number associated account, if any
Account* PhoneNumber::account() const
{
   return d_ptr->m_pAccount;
}

///Return this number associated contact, if any
Contact* PhoneNumber::contact() const
{
   return d_ptr->m_pContact;
}

///Return when this number was last used
time_t PhoneNumber::lastUsed() const
{
   return d_ptr->m_LastUsed;
}

///Set this number default account
void PhoneNumber::setAccount(Account* account)
{
   d_ptr->m_pAccount = account;
   if (d_ptr->m_pAccount)
      connect (d_ptr->m_pAccount,SIGNAL(destroyed(QObject*)),this,SLOT(accountDestroyed(QObject*)));
   d_ptr->changed();
}

///Set this number contact
void PhoneNumber::setContact(Contact* contact)
{
   d_ptr->m_pContact = contact;
   if (contact && d_ptr->m_Type != PhoneNumber::Type::TEMPORARY) {
      PhoneDirectoryModel::instance()->d_ptr->indexNumber(this,d_ptr->m_hNames.keys()+QStringList(contact->formattedName()));
      d_ptr->m_PrimaryName_cache = contact->formattedName();
      d_ptr->primaryNameChanged(d_ptr->m_PrimaryName_cache);
      connect(contact,SIGNAL(rebased(Contact*)),this,SLOT(contactRebased(Contact*)));
   }
   d_ptr->changed();
}

///Protected setter to set if there is a type
void PhoneNumber::setHasType(bool value)
{
   d_ptr->m_hasType = value;
}

///Protected setter to set the PhoneDirectoryModel index
void PhoneNumber::setIndex(int value)
{
   d_ptr->m_Index = value;
}

///Protected setter to change the popularity index
void PhoneNumber::setPopularityIndex(int value)
{
   d_ptr->m_PopularityIndex = value;
}

void PhoneNumber::setCategory(NumberCategory* cat)
{
   if (cat == d_ptr->m_pCategory) return;
   if (d_ptr->m_hasType)
      NumberCategoryModel::instance()->unregisterNumber(this);
   d_ptr->m_hasType = cat != NumberCategoryModel::other();
   d_ptr->m_pCategory = cat;
   if (d_ptr->m_hasType)
      NumberCategoryModel::instance()->registerNumber(this);
   d_ptr->changed();
}

void PhoneNumber::setBookmarked(bool bookmarked )
{
   d_ptr->m_IsBookmark = bookmarked;
}

///Force an Uid on this number (instead of hash)
void PhoneNumber::setUid(const QString& uri)
{
   d_ptr->m_Uid = uri;
}

///Attempt to change the number type
bool PhoneNumber::setType(PhoneNumber::Type t)
{
   if (d_ptr->m_Type == PhoneNumber::Type::BLANK)
      return false;
   if (account() && t == PhoneNumber::Type::ACCOUNT) {
      if (account()->supportPresenceSubscribe()) {
         d_ptr->m_Tracked = true; //The daemon will init the tracker itself
         d_ptr->trackedChanged(true);
      }
      d_ptr->m_Type = t;
      return true;
   }
   return false;
}

///Set if this number is tracking presence information
void PhoneNumber::setTracked(bool track)
{
   if (track != d_ptr->m_Tracked) { //Subscribe only once
      //You can't subscribe without account
      if (track && !d_ptr->m_pAccount) return;
      d_ptr->m_Tracked = track;
      DBus::PresenceManager::instance().subscribeBuddy(d_ptr->m_pAccount->id(),uri().fullUri(),track);
      d_ptr->changed();
      d_ptr->trackedChanged(track);
   }
}

///Allow phonedirectorymodel to change presence status
void PhoneNumber::setPresent(bool present)
{
   if (d_ptr->m_Present != present) {
      d_ptr->m_Present = present;
      d_ptr->presentChanged(present);
   }
}

void PhoneNumber::setPresenceMessage(const QString& message)
{
   if (d_ptr->m_PresentMessage != message) {
      d_ptr->m_PresentMessage = message;
      d_ptr->presenceMessageChanged(message);
   }
}

///Return the current type of the number
PhoneNumber::Type PhoneNumber::type() const
{
   return d_ptr->m_Type;
}

///Return the number of calls from this number
int PhoneNumber::callCount() const
{
   return d_ptr->m_lCalls.size();
}

uint PhoneNumber::weekCount() const
{
   return d_ptr->m_LastWeekCount;
}

uint PhoneNumber::trimCount() const
{
   return d_ptr->m_LastTrimCount;
}

bool PhoneNumber::haveCalled() const
{
   return d_ptr->m_HaveCalled;
}

///Best bet for this person real name
QString PhoneNumber::primaryName() const
{
   //Compute the primary name
   if (d_ptr->m_PrimaryName_cache.isEmpty()) {
      QString ret;
      if (d_ptr->m_hNames.size() == 1)
         ret =  d_ptr->m_hNames.constBegin().key();
      else {
         QString toReturn = tr("Unknown");
         int max = 0;
         for (QHash<QString,int>::const_iterator i = d_ptr->m_hNames.begin(); i != d_ptr->m_hNames.end(); ++i) {
            if (i.value() > max) {
               max      = i.value();
               toReturn = i.key  ();
            }
         }
         ret = toReturn;
      }
      const_cast<PhoneNumber*>(this)->d_ptr->m_PrimaryName_cache = ret;
      const_cast<PhoneNumber*>(this)->d_ptr->primaryNameChanged(d_ptr->m_PrimaryName_cache);
   }
   //Fallback: Use the URI
   if (d_ptr->m_PrimaryName_cache.isEmpty()) {
      return uri();
   }

   //Return the cached primaryname
   return d_ptr->m_PrimaryName_cache;
}

///Is this number bookmarked
bool PhoneNumber::isBookmarked() const
{
   return d_ptr->m_IsBookmark;
}

///If this number could (theoretically) support presence status
bool PhoneNumber::supportPresence() const
{
   //Without an account, presence is impossible
   if (!d_ptr->m_pAccount)
      return false;
   //The account also have to support it
   if (!d_ptr->m_pAccount->supportPresenceSubscribe())
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
   return d_ptr->m_TotalSeconds;
}

///Return this number unique identifier (hash)
QString PhoneNumber::uid() const
{
   return d_ptr->m_Uid.isEmpty()?toHash():d_ptr->m_Uid;
}

///Return all calls from this number
QList<Call*> PhoneNumber::calls() const
{
   return d_ptr->m_lCalls;
}

///Return the phonenumber position in the popularity index
int PhoneNumber::popularityIndex() const
{
   return d_ptr->m_PopularityIndex;
}

QHash<QString,int> PhoneNumber::alternativeNames() const
{
   return d_ptr->m_hNames;
}

///Add a call to the call list, notify listener
void PhoneNumber::addCall(Call* call)
{
   if (!call) return;
   d_ptr->m_Type = PhoneNumber::Type::USED;
   d_ptr->m_lCalls << call;
   d_ptr->m_TotalSeconds += call->stopTimeStamp() - call->startTimeStamp();
   time_t now;
   ::time ( &now );
   if (now - 3600*24*7 < call->stopTimeStamp())
      d_ptr->m_LastWeekCount++;
   if (now - 3600*24*7*15 < call->stopTimeStamp())
      d_ptr->m_LastTrimCount++;

   if (call->historyState() == Call::LegacyHistoryState::OUTGOING || call->direction() == Call::Direction::OUTGOING)
      d_ptr->m_HaveCalled = true;

   d_ptr->callAdded(call);
   if (call->startTimeStamp() > d_ptr->m_LastUsed)
      d_ptr->m_LastUsed = call->startTimeStamp();
   d_ptr->changed();
}

///Generate an unique representation of this number
QString PhoneNumber::toHash() const
{
   return QString("%1///%2///%3").arg(uri()).arg(account()?account()->id():QString()).arg(contact()?contact()->uid():QString());
}



///Increment name counter and update indexes
void PhoneNumber::incrementAlternativeName(const QString& name)
{
   const bool needReIndexing = !d_ptr->m_hNames[name];
   d_ptr->m_hNames[name]++;
   if (needReIndexing && d_ptr->m_Type != PhoneNumber::Type::TEMPORARY) {
      PhoneDirectoryModel::instance()->d_ptr->indexNumber(this,d_ptr->m_hNames.keys()+(d_ptr->m_pContact?(QStringList(d_ptr->m_pContact->formattedName())):QStringList()));
      //Invalid m_PrimaryName_cache
      if (!d_ptr->m_pContact)
         d_ptr->m_PrimaryName_cache.clear();
   }
}

void PhoneNumber::accountDestroyed(QObject* o)
{
   if (o == d_ptr->m_pAccount)
      d_ptr->m_pAccount = nullptr;
}

/**
 * When the PhoneNumber contact is merged with another one, the phone number
 * data might be replaced, like the preferred name.
 */
void PhoneNumber::contactRebased(Contact* other)
{
   d_ptr->m_PrimaryName_cache = other->formattedName();
   d_ptr->primaryNameChanged(d_ptr->m_PrimaryName_cache);
   d_ptr->changed();

   //It is a "partial" rebase, so the PhoneNumber data stay the same
   d_ptr->rebased(this);
}

/**
 * Merge two phone number to share the same data. This avoid having to change
 * pointers all over the place. The PhoneNumber objects remain intact, the
 * PhoneDirectoryModel will replace the old references, but existing ones will
 * keep working.
 */
bool PhoneNumber::merge(PhoneNumber* other)
{

   if ((!other) || other == this || other->d_ptr == d_ptr)
      return false;

   //This is invalid, those are different numbers
   if (account() && other->account() && account() != other->account())
      return false;

   //TODO Check if the merge is valid

   //TODO Merge the alternative names

   //TODO Handle presence

   PrivatePhoneNumber* currentD = d_ptr;

   //Replace the D-Pointer
   this->d_ptr= other->d_ptr;
   d_ptr->m_lParents << this;

   //In case the URI is different, take the longest and most precise
   //TODO keep a log of all URI used
   if (currentD->m_Uri.size() > other->d_ptr->m_Uri.size()) {
      other->d_ptr->m_lOtherURIs << other->d_ptr->m_Uri;
      other->d_ptr->m_Uri = currentD->m_Uri;
   }
   else
      other->d_ptr->m_lOtherURIs << currentD->m_Uri;

   emit changed();
   emit rebased(other);

   currentD->m_lParents.removeAll(this);
   if (!currentD->m_lParents.size())
      delete currentD;
   return true;
}

bool PhoneNumber::operator==(PhoneNumber* other)
{
   return other && this->d_ptr== other->d_ptr;
}

bool PhoneNumber::operator==(const PhoneNumber* other) const
{
   return other && this->d_ptr== other->d_ptr;
}

bool PhoneNumber::operator==(PhoneNumber& other)
{
   return &other && this->d_ptr== other.d_ptr;
}

bool PhoneNumber::operator==(const PhoneNumber& other) const
{
   return &other && this->d_ptr== other.d_ptr;
}

/************************************************************************************
 *                                                                                  *
 *                             Temporary phone number                               *
 *                                                                                  *
 ***********************************************************************************/

void TemporaryPhoneNumber::setUri(const QString& uri)
{
   d_ptr->m_Uri = uri;
   d_ptr->changed();
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
