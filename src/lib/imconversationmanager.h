/****************************************************************************
 *   Copyright (C) 2012-2014 by Savoir-Faire Linux                          *
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
#ifndef IMCONVERSATIONMANAGER_H
#define IMCONVERSATIONMANAGER_H
#include <QtCore/QObject>

#include "typedefs.h"

//SFLPhone
class Call;
class InstantMessagingModel;
class IMConversationManagerPrivate;

///Manager for all IM conversations
class LIB_EXPORT IMConversationManager : public QObject
{
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop
public:

   //Singleton
   static IMConversationManager* instance();

   //Getter
   InstantMessagingModel* getModel(Call* call);
private:
   //Constructor
   explicit IMConversationManager();
   virtual ~IMConversationManager();

   IMConversationManagerPrivate* d_ptr;
   Q_DECLARE_PRIVATE(IMConversationManager)

   //Static attributes
   static IMConversationManager* m_spInstance;


Q_SIGNALS:
   ///Emitted when a new message is available
   void newMessagingModel(Call*,InstantMessagingModel*);
};

#endif