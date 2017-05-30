/***************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                         *
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
#include "cmd.h"

//Qt
#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>

//KDE
#include <KAboutData>
#include <klocalizedstring.h>

//Ring
#include <call.h>
#include <callmodel.h>
#include <media/text.h>
#include <mainwindow.h>
#include "ringapplication.h"
#include "klib/kcfg_settings.h"

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
   Q_UNUSED(argc)
   Q_UNUSED(argv)

   QCoreApplication* app = QCoreApplication::instance();

   QCommandLineOption call      (QStringList { "place-call" }, i18n("Place a call to a given number"                                              ), QStringLiteral("number" ), QLatin1String(""));
   QCommandLineOption text      (QStringList { "send-text"  }, i18n("Send a text to &lt;number&gt;, use --message to set the content, then hangup"), QStringLiteral("number" ), QLatin1String(""));
   QCommandLineOption message   (QStringList { "message"    }, i18n("Used in combination with --send-text"                                        ), QStringLiteral("content"), QLatin1String(""));
   QCommandLineOption icon      (QStringList { "iconify"    }, i18n("Start in the system tray"                                                    )                                             );
   QCommandLineOption showPhone (QStringList { "phone"      }, i18n("Start with the phone interface"                                              )                                             );
   QCommandLineOption showTimeL (QStringList { "timeline"   }, i18n("Start with the timeline interface"                                           )                                             );

   QCommandLineParser parser;
   parser.addOptions({call,text,message,icon});

   about.setupCommandLine(&parser);

   qDebug() << "APP" << app;

   qDebug() << app->arguments();


   parser.process(*app);

   about.processCommandLine(&parser);

   parser.addVersionOption(    );
   parser.addHelpOption   (    );

   if (parser.isSet(call))
      placeCall(parser.value(call));

   if (parser.isSet(icon))
      iconify();

   if (parser.isSet(text) && parser.isSet(message))
      sendText(parser.value(text),parser.value(message));
}

///Place a call (from the command line)
void Cmd::placeCall(const QString& number)
{
   if (number.isEmpty()) {
      qWarning() << "Example: --place-call 123@example.com";
      return;
   }

   //Wait until the initialization is done
   QTimer::singleShot(0,[number] {
      Call* call = CallModel::instance().dialingCall();
      call->reset();
      call->appendText(number);
      call->performAction(Call::Action::ACCEPT);
   });
}

///Send a text ans hang up (from the command line)
void Cmd::sendText(const QString& number, const QString& text)
{
   Q_UNUSED(number)
   Q_UNUSED(text)
   QTimer::singleShot(0,[number,text] {
      Call* call = CallModel::instance().dialingCall();
      call->reset();
      call->appendText(number);
      call->setProperty("message",text);
      QObject::connect(call,&Call::lifeCycleStateChanged,[text,call](const Call::LifeCycleState st) {
         if (st == Call::LifeCycleState::PROGRESS) {
            call->addOutgoingMedia<Media::Text>()->send({{"text/plain",call->property("message").toString()}});
            call->performAction(Call::Action::REFUSE); //HangUp
         }
      });
      call->performAction(Call::Action::ACCEPT);
   });
}

void Cmd::iconify()
{
   if (RingApplication::instance()) {
      RingApplication::instance()->setIconify(true);
   }
}

void Cmd::phoneInterface()
{
   if (RingApplication::instance()) {
      RingApplication::instance()->setStartPhone(true);
   }
}

void Cmd::timelineInterface()
{
   if (RingApplication::instance()) {
      RingApplication::instance()->setStartTimeline(true);
   }
}

void Cmd::slotActivateActionRequested (const QString&, const QVariant&)
{
}

/**
 * This function is called when a new client try to open. It will stop but this
 * process need to take care of its arguments.
 */
void Cmd::slotActivateRequested (const QStringList& args, const QString& cwd)
{
   Q_UNUSED(cwd)
//TODO manage to share the parseArgs implementation, QCommandLineOption cannot
//    char** l = new char*[args.size()];
//    int i=0;
//    for (const QString& str : args) {
//       l[i] = (char*) malloc(sizeof(char)*(str.toLatin1().size()+1/*for \0*/));
//       strcpy(l[i++],str.toLatin1().data());
//    }
//    parseCmd(args.size(), l);

   enum class Current {
      NONE      , /* No args */
      PLACE_CALL, /* One arg */
      SEND_TEXT , /* One arg */
      MESSAGE   , /* One arg */
      MINIMIZED   /* No args */
   };

   Current current = Current::NONE;
   bool sendMessage = false;
   QStringList messages;
   QString sendTextTo;

   for(const QString& arg : args) {
      if (current != Current::NONE) {
         switch(current) {
            case Current::NONE      :
            case Current::MINIMIZED :
               //Iconify do nothing when the executable is already started
               break;
            case Current::SEND_TEXT :
               sendTextTo = arg;
               break;
            case Current::PLACE_CALL:
               placeCall(arg);
               break;
            case Current::MESSAGE   :
               messages << arg;
               break;
         }
      }
      else {
         if (arg == QLatin1String("--place-call"))
            current = Current::PLACE_CALL;
         else if (arg == QLatin1String("--send-text"))
            sendMessage = true;
         else if (arg == QLatin1String("--message"))
            current = Current::MESSAGE;
         else if (arg == QLatin1String("--iconify"))
         {}//TODO
      }
   }

   if (sendMessage && sendTextTo.size() && messages.size()) {
      foreach (const QString& msg, messages)
         sendText(sendTextTo, msg);
   }

   if (ConfigurationSkeleton::displayOnStart()) {
      MainWindow::app()->show();
      MainWindow::app()->activateWindow();
      MainWindow::app()->raise();
   }
}

void Cmd::slotOpenRequested (const QList<QUrl>&)
{
}

// kate: space-indent on; indent-width 3; replace-tabs on;
