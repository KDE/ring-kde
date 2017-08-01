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
#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QtCore/QObject>

#include <account.h>
class Call;
class ContactMethod;
namespace Media {
   class TextRecording;
}

/**
 * This singleton class watch and reatch on events to provide system
 * notifications.
 */
class Notification : public QObject
{
   Q_OBJECT

public:
   explicit Notification(QObject* parent = nullptr);

   void contactOnline();

   void accountStatus(Account* a, const Account::RegistrationState state);

   void incomingText(Media::TextRecording* t, ContactMethod* cm);

   void createContact();

   static Notification* instance();

private Q_SLOTS:
   void incomingCall(Call* c);
};

#endif

// kate: space-indent on; indent-width 3; replace-tabs on;
