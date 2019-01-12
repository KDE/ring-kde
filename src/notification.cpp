/***************************************************************************
 *   Copyright (C) 2015 by Emmanuel Lepage Vallee                          *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
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
#include "notification.h"


// Qt
#include <QtWidgets/QApplication>

// KDE
#include <KNotification>
#include <KLocalizedString>

// LRC
#include <callmodel.h>
#include <accountmodel.h>
#include <call.h>
#include <person.h>
#include <contactmethod.h>
#include <numbercategory.h>
#include <useractionmodel.h>
#include <media/textrecording.h>
#include <media/mimemessage.h>
#include <media/recordingmodel.h>

// Ring-KDE
#include <ringapplication.h>

#define REGISTER_ACTION(list, var, name)  list << name;var = list.size();


/*
 * IncomingCallNotification
 */

class IncomingCallNotification : public KNotification
{
   Q_OBJECT

public:
   explicit IncomingCallNotification(Call* c);

private:
   uint m_Answer      {99};
   //TODO uint m_AnswerVideo {99};
   uint m_HangUp      {99};

   Call* m_pCall;

private Q_SLOTS:
   void stateChanged(Call::LifeCycleState newState, Call::LifeCycleState previousState);
   void actionPerformed(uint actionId);
};

IncomingCallNotification::IncomingCallNotification(Call* call) : KNotification(
   QStringLiteral("incomingCall"), nullptr,
   NotificationFlag::Persistent), m_pCall(call)
{

   const Person* contact = call->peerContactMethod()->contact();
   if (contact) {

      const QPixmap px = (contact->photo()).type() == QVariant::Pixmap ? (contact->photo()).value<QPixmap>():QPixmap();
      setPixmap(px);
   }

   setTitle(i18n("New incoming call"));

   setText(i18n("New call from <b>%1</b>\n<i>(%2)</i>"
      ,call->formattedName()
      ,call->peerContactMethod()->uri()
   ));

   QStringList actions;

   REGISTER_ACTION(actions, m_Answer, i18n( "Answer"  ))
   REGISTER_ACTION(actions, m_HangUp, i18n( "Hang up" ))

   //TODO support answer without video

   setActions(actions);

   addContext(QStringLiteral("contact"), call->peerContactMethod()->category()->name());

   connect(call, &Call::lifeCycleStateChanged, this, &IncomingCallNotification::stateChanged);

   connect(this, SIGNAL(activated(uint)), this , SLOT(actionPerformed(uint)));
}

void IncomingCallNotification::stateChanged(Call::LifeCycleState newState, Call::LifeCycleState previousState)
{
   Q_UNUSED(previousState)

   if (newState != Call::LifeCycleState::INITIALIZATION) {

      close();

      deleteLater();
   }
}

void IncomingCallNotification::actionPerformed(uint actionId)
{
   if (actionId == m_Answer) {
      m_pCall << Call::Action::ACCEPT;
   }
   else if (actionId == m_HangUp) {
      m_pCall << Call::Action::REFUSE;
   }
}


/*
 * Text message notification
 */
class IncomingTextNotification : public KNotification
{
   Q_OBJECT
public:
   explicit IncomingTextNotification(ContactMethod* cm, Media::TextRecording* t);
   void actionPerformed(uint actionId);
};

IncomingTextNotification::IncomingTextNotification(ContactMethod* cm, Media::TextRecording* t) : KNotification(
   QStringLiteral("incomingText"), nullptr)
{
   setTitle(i18n("Message from %1", cm->primaryName()));

   if (auto contact = cm->contact()) {

      const QPixmap px = (contact->photo()).type() == QVariant::Pixmap ? (contact->photo()).value<QPixmap>():QPixmap();
      setPixmap(px);
   }

   setText(t->instantTextMessagingModel()->index(
      t->instantTextMessagingModel()->rowCount()-1, 0
   ).data().toString());
}

void IncomingTextNotification::actionPerformed(uint actionId)
{
   Q_UNUSED(actionId)
}

/*
 * CreateContactNotification
 */

class CreateContactNotification : public KNotification
{
   Q_OBJECT

public:
   explicit CreateContactNotification(ContactMethod* cm);

private:
   uint m_Yes   {99};
   uint m_No    {99};
   uint m_Never {99};

private Q_SLOTS:
   void actionPerformed(uint actionId);
};

CreateContactNotification::CreateContactNotification(ContactMethod* cm) :KNotification(
   QStringLiteral("incomingCall"), nullptr)
{
   setTitle(i18n("Add %1 to contacts?", cm->uri()));

   setText(i18n("Do you wish to add %1 to your addressbook?", cm->primaryName()));

   QStringList actions;

   REGISTER_ACTION(actions, m_Yes  , i18n( "Yes"              ))
   REGISTER_ACTION(actions, m_No   , i18n( "No"               ))
   REGISTER_ACTION(actions, m_Never, i18n( "Do not ask again" ))

   setActions(actions);

   connect(this, SIGNAL(activated(uint)), this , SLOT(actionPerformed(uint)));
}

void CreateContactNotification::actionPerformed(uint actionId)
{
   if (actionId == m_Yes) {
      //TODO do something
   }
   else if (actionId == m_No) {
      //Nothing to do
   }
   else if (actionId == m_Never) {
      //TODO edit the config to disable this notification
   }

   deleteLater();
}




Notification::Notification(QObject* parent) : QObject(parent)
{
   connect(&CallModel::instance(), &CallModel::incomingCall, this, &Notification::incomingCall);
   connect(&AccountModel::instance(), &AccountModel::accountStateChanged, this, &Notification::accountStatus);
   connect(&Media::RecordingModel::instance(), &Media::RecordingModel::mimeMessageInserted, this, &Notification::incomingText);
}

Notification* Notification::instance()
{
   static auto i = new Notification(QApplication::instance());

   return i;
}

void Notification::contactOnline()
{
   //TODO fix the presence model
}

void Notification::accountStatus(Account* a, const Account::RegistrationState state)
{
   Q_UNUSED(a)
   Q_UNUSED(state)
   //TODO
}

void Notification::incomingCall(Call* call)
{
   if (call)
      (new IncomingCallNotification(call))->sendEvent();
}

void Notification::incomingText(Media::MimeMessage* m, Media::TextRecording* t, ContactMethod* cm)
{
   if (m->direction() == Media::Media::Direction::OUT)
      return;

   if (t && !RingApplication::instance()->mayHaveFocus())
      (new IncomingTextNotification(cm, t))->sendEvent();
}

void Notification::createContact()
{
   //TODO
}

#undef REGISTER_ACTION

#include <notification.moc>

// kate: space-indent on; indent-width 3; replace-tabs on;
