/****************************************************************************
 *   Copyright (C) 2012-2015 by Savoir-Faire Linux                          *
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
#ifndef INSTANTMESSAGINGMODELPRIVATE_H
#define INSTANTMESSAGINGMODELPRIVATE_H

#include <QtCore/QAbstractItemModel>

class InstantMessagingModel;
class Call;

class InstantMessagingModelPrivate : public QObject
{
   Q_OBJECT
public:
   explicit InstantMessagingModelPrivate(InstantMessagingModel* parent);

   //Private members
   void addIncommingMessage(const QString& from, const QString& message);
   void addOutgoingMessage(const QString& message);

   //Interal struct
   struct InternalIM {
      QString from;
      QString message;
   };

   //Attributes
   QList<InternalIM>           m_lMessages;
   QHash<QModelIndex,QVariant> m_lImages  ;
   Call*                       m_pCall    ;
private:
   InstantMessagingModel* q_ptr;
};

#endif
