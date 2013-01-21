/****************************************************************************
 *   Copyright (C) 2013 by Savoir-Faire Linux                               *
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

#include "macromodel.h"

//Qt
#include <QtCore/QTimer>

//KDE
#include <KAction>

//SFLPhone
#include "../lib/callmanager_interface_singleton.h"

///Init static attributes
MacroModel*  MacroModel::m_pInstance = nullptr;

Macro::Macro(QObject* parent) : QObject(parent),position(0),delay(0)
{
   
}

void Macro::execute() {
   escaped = sequence;
   while (escaped.indexOf("\\n") != -1) {
      escaped = escaped.replace("\\n","\n");
   }
   nextStep();
}

void Macro::nextStep()
{
   if (position < escaped.size()) {
      if (!MacroModel::getInstance()->m_lListeners.size())
         Q_NOREPLY CallManagerInterfaceSingleton::getInstance().playDTMF(QString(escaped[position]));
      else {
         foreach(MacroListener* l,MacroModel::getInstance()->m_lListeners) {
            l->addDTMF(QString(escaped[position]));
         }
      }
      position++;
      QTimer::singleShot(delay?delay:100,this,SLOT(nextStep()));
   }
   else {
      position = 0;
   }
}

MacroModel::MacroModel(QObject* parent) : QObject(parent) {
   
}

///Singleton
MacroModel* MacroModel::getInstance()
{
   if (m_pInstance == nullptr) {
      m_pInstance = new MacroModel(0);
   }
   return m_pInstance;
}

void MacroModel::addMacro(Macro* macro)
{
   KAction* newAction = new KAction(this);
   newAction->setText("MACRO");
   newAction->setIcon(KIcon("view-form-action"));
   connect(newAction, SIGNAL(triggered()), macro , SLOT(execute()) );
   emit addAction(newAction);
}

void MacroModel::addListener(MacroListener* interface)
{
   MacroModel* m = getInstance();
   m->m_lListeners << interface;
}