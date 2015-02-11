/***************************************************************************
 *   Copyright (C) 2012-2015 by Savoir-Faire Linux                         *
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
#include "immanager.h"
#include "instantmessagingmodel.h"
#include "imconversationmanager.h"
#include "call.h"
#include "../delegates/imdelegate.h"
#include <klocalizedstring.h>

///Constructor
IMManager::IMManager(QWidget* parent) : QTabWidget(parent)
{
   setVisible(false);
   setTabsClosable(true);
   connect(IMConversationManager::instance(),SIGNAL(newMessagingModel(Call*,InstantMessagingModel*)),this,SLOT(newConversation(Call*,InstantMessagingModel*)));
   connect(this,SIGNAL(tabCloseRequested(int)),this,SLOT(closeRequest(int)));
}

///Destructor
void IMManager::newConversation(Call* call, InstantMessagingModel* model)
{
   IMTab* newTab = new IMTab(model,this);
   m_lTabs[call] = newTab;
   setVisible(true);
   QString name = call->formattedName();
   addTab(newTab,name);
}

///Close a tab
void IMManager::closeRequest(int index)
{
   QWidget* wdg = widget(index);
   Call* call = m_lTabs.key(qobject_cast<IMTab*>(wdg));
   if (call) {
      m_lTabs[call] = nullptr;
      delete wdg;
      if (!count())
         setVisible(false);
   }
}
