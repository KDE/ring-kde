/***************************************************************************
 *   Copyright (C) 2015 by Emmanuel Lepage Vallee                          *
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

// Ring-KDE
#include <mainwindow.h>
#include <view.h>

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
   uint m_AnswerVideo {99};
   uint m_HangUp      {99};

   Call* m_pCall;

private Q_SLOTS:
   void stateChanged(Call::LifeCycleState newState, Call::LifeCycleState previousState);
   void actionPerformed(uint actionId);
};

IncomingCallNotification::IncomingCallNotification(Call* call) : KNotification(
   QStringLiteral("incomingCall"), MainWindow::view(), NotificationFlag::Persistent  ),
   m_pCall(call)
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
   QStringLiteral("incomingCall"), MainWindow::view())
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

void Notification::incomingText()
{
   //TODO
}

void Notification::createContact()
{
   //TODO
}

#undef REGISTER_ACTION

#include <notification.moc>
