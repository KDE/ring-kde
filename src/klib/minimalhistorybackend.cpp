/************************************************************************************
 *   Copyright (C) 2014-2015 by Savoir-Faire Linux                                       *
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
#include <QtWidgets/QApplication>
#include <QtCore/QStandardPaths>

//KDE

#include <kmessagebox.h>
#include <klocalizedstring.h>
#include <QStandardPaths>

//Ring
#include "call.h"
#include "account.h"
#include "contactmethod.h"
#include "historymodel.h"



MinimalHistoryBackend::~MinimalHistoryBackend()
{

}

bool MinimalHistoryEditor::save(const Call* item)
{
   Q_UNUSED(item)
   return false;
}

bool MinimalHistoryEditor::append(const Call* item)
{
   Q_UNUSED(item)
   return false;
}

bool MinimalHistoryEditor::remove(Call* item)
{
   Q_UNUSED(item)
   return false;
}

bool MinimalHistoryEditor::edit( Call* item)
{
   Q_UNUSED(item)
   return false;
}

bool MinimalHistoryEditor::addNew( Call* item)
{
   Q_UNUSED(item)
   return false;
}

QVector<Call*> MinimalHistoryEditor::items() const
{
   return QVector<Call*>();
}

QString MinimalHistoryBackend::name () const
{
   return QObject::tr("Minimal history backend");
}

QString MinimalHistoryBackend::category () const
{
   return QObject::tr("History");
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
   QFile file(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QLatin1Char('/') +"history.ini");
   if ( file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
      QMap<QString,QString> hc;
      while (!file.atEnd()) {
         QByteArray line = file.readLine().trimmed();

         //The item is complete
         if ((line.isEmpty() || !line.size()) && hc.size()) {
            Call* pastCall = Call::buildHistoryCall(hc);
            if (pastCall->peerName().isEmpty()) {
               pastCall->setPeerName(QObject::tr("Unknown"));
            }
            pastCall->setRecordingPath(hc[ Call::HistoryMapFields::RECORDING_PATH ]);
            pastCall->setBackend(this);
            m_pMediator->addItem(pastCall);
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


// bool MinimalHistoryBackend::append(const Call* call)
// {
//    if (call->backend() == this  || call->id().isEmpty()) return false;
//    //TODO support \r and \n\r end of line
//    QFile file(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QLatin1Char('/') + "")+"history.ini";
//    if ( file.open(QIODevice::Append | QIODevice::Text) ) {
//       const QString direction = (call->direction()==Call::Direction::INCOMING)?
//          Call::HistoryStateName::INCOMING : Call::HistoryStateName::OUTGOING;
//       QTextStream streamFileOut(&file);
//       const Account* a = call->account();
//       streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::CALLID          ).arg(call->id()                     );
//       streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::TIMESTAMP_START ).arg(call->startTimeStamp()         );
//       streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::TIMESTAMP_STOP  ).arg(call->stopTimeStamp()          );
//       streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::ACCOUNT_ID      ).arg(a?QString(a->id()):""          );
//       streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::DISPLAY_NAME    ).arg(call->peerName()               );
//       streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::PEER_NUMBER     ).arg(call->peerContactMethod()->uri() );
//       streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::DIRECTION       ).arg(direction                      );
//       streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::MISSED          ).arg(call->isMissed()               );
//       streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::RECORDING_PATH  ).arg(call->recordingPath()          );
//       streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::CONTACT_USED    ).arg(false                          );//TODO
//       if (call->peerContactMethod()->contact()) {
//          streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::CONTACT_UID  ).arg(
//             QString(call->peerContactMethod()->contact()->uid())
//          );
//       }
//       streamFileOut << "\n";
//       streamFileOut.flush();
//       file.close();
//       return true;
//    }
//    else
//       qWarning() << "Unable to save history";
//    return false;
// }

/** Rewrite the file from scratch
 * @todo Eventually check if it is necessary, it will be faster
 */
// bool MinimalHistoryBackend::save(const Call* call)
// {
//    Q_UNUSED(call)
//    if (call->backend() != this)
//       append(call);
// 
//    //TODO, need to regenerate everything
//    /*QFile file(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QLatin1Char('/') + "")+"history.ini";
//    if ( file.open(QIODevice::WriteOnly | QIODevice::Text) ) {
//       foreach(const Call* call, HistoryModel::instance()->getHistoryCalls()) {
//          qDebug() << "HERE" << call->id();
//       }
//       return true;
//    }*/
//    return false;
// }

CollectionInterface::SupportedFeatures MinimalHistoryBackend::supportedFeatures() const
{
   return (CollectionInterface::SupportedFeatures) (
      CollectionInterface::SupportedFeatures::NONE  |
      CollectionInterface::SupportedFeatures::LOAD  |
      CollectionInterface::SupportedFeatures::CLEAR |
//       CollectionInterface::SupportedFeatures::REMOVE|
      CollectionInterface::SupportedFeatures::ADD   );
}

///Edit 'item', the implementation may be a GUI or somehting else
// bool MinimalHistoryBackend::edit( Call* call)
// {
//    Q_UNUSED(call)
//    return false;
// }


// bool MinimalHistoryBackend::remove ( Call* c )
// {
//    Q_UNUSED(c)
//    qDebug() << "Removing item is not yet supported";
//    return true;
// }

///Add a new item to the backend
// bool MinimalHistoryBackend::addNew( Call* call)
// {
//    Q_UNUSED(call)
//    return true;
// }

///Add a new phone number to an existing item
// bool MinimalHistoryBackend::addContactMethod( Call* call , ContactMethod* number )
// {
//    Q_UNUSED(call)
//    Q_UNUSED(number)
//    return false;
// }

bool MinimalHistoryBackend::clear()
{
   const int ret = KMessageBox::questionYesNo(static_cast<QApplication*>(QApplication::instance())->activeWindow(), i18n("Are you sure you want to clear history?"), i18n("Clear history"));
   if (ret == KMessageBox::Yes) {
      QFile::remove(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QLatin1Char('/') + "")+"history.ini";
      return true;
   }
   return false;
}

QByteArray MinimalHistoryBackend::id() const
{
   return "mhb";
}

// QList<Call*> MinimalHistoryBackend::items() const
// {
//    return QList<Call*>(); //TODO
// }

