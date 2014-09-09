/************************************************************************************
 *   Copyright (C) 2014 by Savoir-Faire Linux                                       *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>         *
 *                                                                                  *
 *   This library is free software; you can redistribute it and/or                  *
 *   modify it under the terms of the GNU Lesser General Public                     *
 *   License as published by the Free Software Foundation; either                   *
 *   version 2.1 of the License, or (at your option) any later version.             *
 *                                                                                  *
 *   This library is distributed in the hope that it will be useful,                *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of                 *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU              *
 *   Lesser General Public License for more details.                                *
 *                                                                                  *
 *   You should have received a copy of the GNU Lesser General Public               *
 *   License along with this library; if not, write to the Free Software            *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA *
 ***********************************************************************************/
#include "minimalhistorybackend.h"

//Qt
#include <QtCore/QFile>
#include <QtCore/QHash>
#include <QtGui/QApplication>

//KDE
#include <KStandardDirs>
#include <KMessageBox>
#include <KLocale>

//SFLPhone
#include "../lib/call.h"
#include "../lib/account.h"
#include "../lib/phonenumber.h"
#include "../lib/historymodel.h"

MinimalHistoryBackend::MinimalHistoryBackend(QObject* parent) : AbstractHistoryBackend(nullptr,parent)
{
   setObjectName("MinimalHistoryBackend");
}

MinimalHistoryBackend::~MinimalHistoryBackend()
{
   
}

QString MinimalHistoryBackend::name () const
{
   return "Minimal history backend";
}

QVariant MinimalHistoryBackend::icon() const
{
   return QVariant();
}

bool MinimalHistoryBackend::isEnabled() const
{
   return true;
}

bool MinimalHistoryBackend::load()
{
   QFile file(KStandardDirs::locateLocal("appdata","")+"history.ini");
   if ( file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
      QMap<QString,QString> hc;
      while (!file.atEnd()) {
         QByteArray line = file.readLine().trimmed();

         //The item is complete
         if ((line.isEmpty() || !line.size()) && hc.size()) {
            Call* pastCall = Call::buildHistoryCall(hc);
            if (pastCall->peerName().isEmpty()) {
               pastCall->setPeerName(tr("Unknown"));
            }
            pastCall->setRecordingPath(hc[ Call::HistoryMapFields::RECORDING_PATH ]);
            pastCall->setBackend(this);
            emit newHistoryCallAdded(pastCall);
            hc.clear();
         }
         // Add to the current set
         else {
            const int idx = line.indexOf("=");
            if (idx >= 0)
               hc[line.left(idx)] = line.right(line.size()-idx-1);
         }
      }
      return true;
   }
   else
      qWarning() << "History doesn't exist or is not readable";
   return false;
}

bool MinimalHistoryBackend::reload()
{
   return false;
}


bool MinimalHistoryBackend::append(const Call* call)
{
   if (call->backend() == this  || call->id().isEmpty()) return false;
   //TODO support \r and \n\r end of line
   QFile file(KStandardDirs::locateLocal("appdata","")+"history.ini");
   if ( file.open(QIODevice::Append | QIODevice::Text) ) {
      const QString direction = (call->direction()==Call::Direction::INCOMING)?
         Call::HistoryStateName::INCOMING : Call::HistoryStateName::OUTGOING;
      QTextStream streamFileOut(&file);
      const Account* a = call->account();
      streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::CALLID          ).arg(call->id()                     );
      streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::TIMESTAMP_START ).arg(call->startTimeStamp()         );
      streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::TIMESTAMP_STOP  ).arg(call->stopTimeStamp()          );
      streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::ACCOUNT_ID      ).arg(a?a->id():""                   );
      streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::DISPLAY_NAME    ).arg(call->peerName()               );
      streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::PEER_NUMBER     ).arg(call->peerPhoneNumber()->uri() );
      streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::DIRECTION       ).arg(direction                      );
      streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::MISSED          ).arg(call->isMissed()               );
      streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::RECORDING_PATH  ).arg(call->recordingPath()          );
      streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::CONTACT_USED    ).arg(false                          );//TODO
      if (call->peerPhoneNumber()->contact()) {
         streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::CONTACT_UID  ).arg(
            QString(call->peerPhoneNumber()->contact()->uid())
         );
      }
      streamFileOut << "\n";
      streamFileOut.flush();
      file.close();
      return true;
   }
   else
      qWarning() << "Unable to save history";
   return false;
}

/** Rewrite the file from scratch
 * @todo Eventually check if it is necessary, it will be faster
 */
bool MinimalHistoryBackend::save(const Call* call)
{
   Q_UNUSED(call)
   if (call->backend() != this)
      append(call);

   //TODO, need to regenerate everything
   /*QFile file(KStandardDirs::locateLocal("appdata","")+"history.ini");
   if ( file.open(QIODevice::WriteOnly | QIODevice::Text) ) {
      foreach(const Call* call, HistoryModel::instance()->getHistoryCalls()) {
         qDebug() << "HERE" << call->id();
      }
      return true;
   }*/
   return false;
}

AbstractHistoryBackend::SupportedFeatures MinimalHistoryBackend::supportedFeatures() const
{
   return (AbstractHistoryBackend::SupportedFeatures) (
      AbstractHistoryBackend::SupportedFeatures::NONE |
      AbstractHistoryBackend::SupportedFeatures::LOAD |
      AbstractHistoryBackend::SupportedFeatures::CLEAR|
      AbstractHistoryBackend::SupportedFeatures::ADD  );
}

///Edit 'item', the implementation may be a GUI or somehting else
bool MinimalHistoryBackend::edit( Call* call)
{
   Q_UNUSED(call)
   return false;
}
///Add a new item to the backend
bool MinimalHistoryBackend::addNew( Call* call)
{
   Q_UNUSED(call)
   return true;
}

///Add a new phone number to an existing item
bool MinimalHistoryBackend::addPhoneNumber( Call* call , PhoneNumber* number )
{
   Q_UNUSED(call)
   Q_UNUSED(number)
   return false;
}

bool MinimalHistoryBackend::clear()
{
   const int ret = KMessageBox::questionYesNo(static_cast<QApplication*>(QApplication::instance())->activeWindow(), i18n("Are you sure you want to clear history?"), i18n("Clear history"));
   if (ret == KMessageBox::Yes) {
      QFile::remove(KStandardDirs::locateLocal("appdata","")+"history.ini");
      return true;
   }
   return false;
}

QByteArray MinimalHistoryBackend::id() const
{
   return "mhb";
}

QList<Call*> MinimalHistoryBackend::items() const
{
   return QList<Call*>(); //TODO
}

