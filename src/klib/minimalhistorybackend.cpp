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

//KDE
#include <KStandardDirs>

//SFLPhone
#include "../lib/call.h"

MinimalHistoryBackend::MinimalHistoryBackend(QObject* parent) : AbstractHistoryBackend(parent)
{
   setObjectName("MinimalHistoryBackend");
}

MinimalHistoryBackend::~MinimalHistoryBackend()
{
   
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
      qWarning() << "History doesn't exist or is not writable";
   return false;
}

bool MinimalHistoryBackend::reload()
{
   return false;
}

bool MinimalHistoryBackend::save(const Call* call)
{
   Q_UNUSED(call)
   return false;
}

AbstractHistoryBackend::SupportedFeatures MinimalHistoryBackend::supportedFeatures() const
{
   return (AbstractHistoryBackend::SupportedFeatures) (
      AbstractHistoryBackend::SupportedFeatures::NONE |
      AbstractHistoryBackend::SupportedFeatures::LOAD |
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