/***************************************************************************
 *   Copyright (C)  2013 by Savoir-Faire Linux                             *
 *   Author : Emmanuel Lepage Valle <emmanuel.lepage@savoirfairelinux.com >*
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
#include "callviewtoolbar.h"

//Qt
#include <QtGui/QTreeView>
#include <QtGui/QHBoxLayout>

//KDE
#include <KDebug>

//SFLPhone
#include "sflphone.h"
#include "actioncollection.h"
#include "extendedaction.h"
#include <klib/tipmanager.h>
#include <lib/call.h>
#include <lib/useractionmodel.h>
#include <lib/callmodel.h>

CallViewToolbar::CallViewToolbar(QTreeView* parent) : OverlayToolbar(parent),m_pParent(parent)
{
   addAction( ActionCollection::instance()->holdAction()     ,static_cast<int>(UserActionModel::Action::HOLD)     );
   addAction( ActionCollection::instance()->unholdAction()   ,static_cast<int>(UserActionModel::Action::UNHOLD)   );
   addAction( ActionCollection::instance()->muteCaptureAction()     ,static_cast<int>(UserActionModel::Action::MUTE)     );
   addAction( ActionCollection::instance()->pickupAction()   ,static_cast<int>(UserActionModel::Action::PICKUP)   );
   addAction( ActionCollection::instance()->hangupAction()   ,static_cast<int>(UserActionModel::Action::HANGUP)   );
   addAction( ActionCollection::instance()->transferAction() ,static_cast<int>(UserActionModel::Action::TRANSFER) );
   addAction( ActionCollection::instance()->recordAction()   ,static_cast<int>(UserActionModel::Action::RECORD)   );
   addAction( ActionCollection::instance()->refuseAction()   ,static_cast<int>(UserActionModel::Action::REFUSE)   );
   addAction( ActionCollection::instance()->acceptAction()   ,static_cast<int>(UserActionModel::Action::ACCEPT)   );
}

CallViewToolbar::~CallViewToolbar()
{
   
}

void CallViewToolbar::updateState()
{
   QModelIndex index = m_pParent->selectionModel()->currentIndex();
   const int rowcount = CallModel::instance()->rowCount();
   if ((!m_pParent->selectionModel()->hasSelection() || !index.isValid()) && rowcount) {
      m_pParent->selectionModel()->setCurrentIndex(CallModel::instance()->index(0,0),QItemSelectionModel::SelectCurrent);
      index = m_pParent->selectionModel()->currentIndex();
   }
   if (rowcount && index.isValid() && (index.row() < rowcount || index.parent().isValid())) {
      Call* call = qvariant_cast<Call*>(index.data(Call::Role::Object));
      setVisible(true);
      TipManager* manager = qvariant_cast<TipManager*>(parentWidget()->property("tipManager"));
      manager->setBottomMargin(53);
      char act_counter = 0;
      for (int i = 0;i<enum_class_size<UserActionModel::Action>();i++) {
         try {
            if (call && call->userActionModel()) {
               actionButton(i)->setVisible(call->userActionModel()->isActionEnabled(static_cast<UserActionModel::Action>(i)));
               act_counter += call->userActionModel()->isActionEnabled( static_cast<UserActionModel::Action>(i));
            }
         }
         catch (Call::State& state) {
            qDebug() << "OverlayToolbar is out of bound (state)" << state;
         }
         catch (UserActionModel::Action& btn) {
            kDebug() << "OverlayToolbar is out of bound (Action)" << (int)btn;
         }
         catch (...) {
            kDebug() << "OverlayToolbar is out of bound (Other)";
         }
      }
      if (!act_counter)
         setVisible(false);
   }
   else {
      setVisible(false);
      TipManager* manager = qvariant_cast<TipManager*>(parentWidget()->property("tipManager"));
      manager->setBottomMargin(0);
   }
   //Now set the top margin, this doesn't really belong anywhere, so why not here
   const int rows = CallModel::instance()->rowCount(QModelIndex());
   QModelIndex last = CallModel::instance()->index(rows-1,0);
   if (CallModel::instance()->rowCount(last) > 0)
      last = CallModel::instance()->index(CallModel::instance()->rowCount(last)-1,0,last);
   const QRect topMargin =  m_pParent->visualRect(last);
   TipManager* manager = qvariant_cast<TipManager*>(parentWidget()->property("tipManager"));
   manager->setTopMargin(topMargin.y()+topMargin.height());
} //updateState
