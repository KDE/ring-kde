/***************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                         *
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
#include "sflphonecmd.h"

//KDE
#include <KCmdLineArgs>
#include <KUniqueApplication>
#include <KLocale>

//SFLPhone
#include <lib/call.h>
#include <lib/callmodel.h>
#include <sflphone.h>

//Static definition
SFLPhoneCmd* SFLPhoneCmd::m_spSelf = nullptr;

///(Private) constructor
SFLPhoneCmd::SFLPhoneCmd(QObject* parent) : QObject(parent)
{
}

///Signleton
SFLPhoneCmd* SFLPhoneCmd::instance() {
   if (!m_spSelf) {
      m_spSelf = new SFLPhoneCmd();
   }
   return m_spSelf;
}

///Setup command line options before passing them to the KUniqueApplication
void SFLPhoneCmd::parseCmd(int argc, char **argv, KAboutData& about)
{
      KCmdLineArgs::init(argc, argv, &about);
      KCmdLineOptions options;
      options.add("place-call <number>", ki18n("Place a call to a given number"                                        ),"");
      options.add("send-text <number>" , ki18n("Send a text to &lt;number&gt;, use --message to set the content, then hangup"),"");
      options.add("message <content>"  , ki18n("Used in combination with --send-text"                                   ),"");
      KCmdLineArgs::addCmdLineOptions(options);

      KCmdLineArgs::parsedArgs();

      KUniqueApplication::addCmdLineOptions();
}

///Place a call (from the command line)
void SFLPhoneCmd::placeCall(const QString& number)
{
   Call* call = CallModel::instance()->dialingCall();
   call->reset();
   call->appendText(number);
   call->performAction(Call::Action::ACCEPT);
}

///Send a text ans hang up (from the command line)
void SFLPhoneCmd::sendText(const QString& number, const QString& text)
{
   Call* call = CallModel::instance()->dialingCall();
   call->reset();
   call->appendText(number);
   call->setProperty("message",text);
   connect(call,SIGNAL(changed(Call*)),instance(),SLOT(textMessagePickup(Call*)));
   call->performAction(Call::Action::ACCEPT);
}

///Send the message now that the call is ready
void SFLPhoneCmd::textMessagePickup(Call* call)
{
   if (call->state() == Call::State::CURRENT) {
      call->sendTextMessage(call->property("message").toString());
      disconnect(call,SIGNAL(changed(Call*)),instance(),SLOT(textMessagePickup(Call*)));
      call->performAction(Call::Action::REFUSE); //HangUp
   }
}
