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
#include "imconversationmanager.h"

#include "call.h"
#include "callmodel.h"
#include "dbus/callmanager.h"
#include "instantmessagingmodel.h"
#include "private/instantmessagingmodel_p.h"


IMConversationManager* IMConversationManager::m_spInstance = nullptr;

class IMConversationManagerPrivate : public QObject
{
   Q_OBJECT
public:
   friend class Call;
   IMConversationManagerPrivate(IMConversationManager* parent);

   //Attributes
   QHash<QString,InstantMessagingModel*> m_lModels;

private:
   IMConversationManager* q_ptr;

private Q_SLOTS:
   void newMessage(const QString& callId, const QString& from, const QString& message);
};

IMConversationManagerPrivate::IMConversationManagerPrivate(IMConversationManager* parent) : QObject(parent), q_ptr(parent)
{
}

///Signleton
IMConversationManager* IMConversationManager::instance()
{
   if (!m_spInstance) {
      m_spInstance = new IMConversationManager();
   }
   return m_spInstance;
}

IMConversationManager::~IMConversationManager()
{
//    delete d_ptr;
}

///Constructor
IMConversationManager::IMConversationManager() : QObject(nullptr), d_ptr(new IMConversationManagerPrivate(this))
{
   CallManagerInterface& callManager = DBus::CallManager::instance();
   connect(&callManager, SIGNAL(incomingMessage(QString,QString,QString)), d_ptr.data(), SLOT(newMessage(QString,QString,QString)));
}

///Called when a new message is incoming
void IMConversationManagerPrivate::newMessage(const QString& callId, const QString& from, const QString& message)
{
   if (!m_lModels[callId] && CallModel::instance()) {
      Call* call = CallModel::instance()->getCall(callId);
      if (call) {
         qDebug() << "Creating messaging model for call" << callId;
         m_lModels[callId] = new InstantMessagingModel(call);
         emit q_ptr->newMessagingModel(call,m_lModels[callId]);
         m_lModels[callId]->d_ptr->addIncommingMessage(from,message);
      }
   }
   else if (m_lModels[callId]) {
      m_lModels[callId]->d_ptr->addIncommingMessage(from,message);
   }
}

///Singleton
InstantMessagingModel* IMConversationManager::getModel(Call* call) {
   const QString key = call->id();
   if (!d_ptr->m_lModels[key]) {
      d_ptr->m_lModels[key] = new InstantMessagingModel(call);
      emit newMessagingModel(call,d_ptr->m_lModels[key]);
   }
   return d_ptr->m_lModels[key];
}

#include <imconversationmanager.moc>
