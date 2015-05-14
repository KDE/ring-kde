/***************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                         *
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
#include "cmd.h"

//Qt
#include <QApplication>

//KDE
#include <KAboutData>
#include <klocalizedstring.h>

//Ring
#include <call.h>
#include <callmodel.h>
#include <media/text.h>
#include <ring.h>
#include <QCommandLineParser>
#include <QCommandLineOption>

//Static definition
Cmd* Cmd::m_spSelf = nullptr;

///(Private) constructor
Cmd::Cmd(QObject* parent) : QObject(parent)
{
}

///Signleton
Cmd* Cmd::instance() {
   if (!m_spSelf) {
      m_spSelf = new Cmd();
   }
   return m_spSelf;
}

///Setup command line options before passing them to the KUniqueApplication
void Cmd::parseCmd(int argc, char **argv, KAboutData& about)
{
   QApplication app(argc, argv);
   QCommandLineParser parser;
   KAboutData::setApplicationData(about);
   parser.addVersionOption();
   parser.addHelpOption();
   //PORTING SCRIPT: adapt aboutdata variable if necessary
   about.setupCommandLine(&parser);
   parser.process(app);
   about.processCommandLine(&parser);

   parser.addOption(QCommandLineOption(QStringList() << QLatin1String("call"), i18n("Place a call to a given number"                                        ), QLatin1String("number"), QLatin1String("")));
   parser.addOption(QCommandLineOption(QStringList() << QLatin1String("text"), i18n("Send a text to &lt;number&gt;, use --message to set the content, then hangup"), QLatin1String("number"), QLatin1String("")));
   parser.addOption(QCommandLineOption(QStringList() << QLatin1String("message"), i18n("Used in combination with --send-text"                                   ), QLatin1String("content"), QLatin1String("")));

//    QCommandLineParser::parsedArgs();

//    KUniqueApplication::addCmdLineOptions();
}

///Place a call (from the command line)
void Cmd::placeCall(const QString& number)
{
   Call* call = CallModel::instance()->dialingCall();
   call->reset();
   call->appendText(number);
   call->performAction(Call::Action::ACCEPT);
}

///Send a text ans hang up (from the command line)
void Cmd::sendText(const QString& number, const QString& text)
{
   Call* call = CallModel::instance()->dialingCall();
   call->reset();
   call->appendText(number);
   call->setProperty("message",text);
   connect(call,SIGNAL(changed(Call*)),instance(),SLOT(textMessagePickup(Call*)));
   call->performAction(Call::Action::ACCEPT);
}

///Send the message now that the call is ready
void Cmd::textMessagePickup(Call* call)
{
   if (call->state() == Call::State::CURRENT) {
      call->addOutgoingMedia<Media::Text>()->send(call->property("message").toString());
      disconnect(call,SIGNAL(changed(Call*)),instance(),SLOT(textMessagePickup(Call*)));
      call->performAction(Call::Action::REFUSE); //HangUp
   }
}
