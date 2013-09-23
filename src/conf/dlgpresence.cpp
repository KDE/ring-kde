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
#include "dlgpresence.h"
#include <lib/presencestatusmodel.h>

#include <KIcon>

DlgPresence::DlgPresence(QWidget *parent) : QWidget(parent)
{
   setupUi(this);
   m_pView->setModel(PresenceStatusModel::instance());
   m_pUp->setIcon     ( KIcon( "go-up"       ) );
   m_pDown->setIcon   ( KIcon( "go-down"     ) );
   m_pAdd->setIcon    ( KIcon( "list-add"    ) );
   m_pRemove->setIcon ( KIcon( "list-remove" ) );
   connect(m_pAdd   , SIGNAL(clicked()),PresenceStatusModel::instance() ,SLOT(addRow())       );
   connect(m_pUp    , SIGNAL(clicked()),this                            ,SLOT(slotMoveUp())   );
   connect(m_pDown  , SIGNAL(clicked()),this                            ,SLOT(slotMoveDown()) );
   connect(m_pRemove, SIGNAL(clicked()),this                            ,SLOT(slotRemoveRow()));
   
   m_pView->horizontalHeader()->setResizeMode(0,QHeaderView::ResizeToContents);
   m_pView->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);
   for (int i=2;i<PresenceStatusModel::instance()->columnCount();i++) {
      m_pView->horizontalHeader()->setResizeMode(i,QHeaderView::ResizeToContents);
   }
}

void DlgPresence::updateWidgets()
{
   
}

void DlgPresence::updateSettings()
{
   PresenceStatusModel::instance()->save();
}

bool DlgPresence::hasChanged()
{
   return false;
}

void DlgPresence::slotRemoveRow()
{
   PresenceStatusModel::instance()->removeRow(m_pView->currentIndex());
}


void DlgPresence::slotMoveUp()
{
   PresenceStatusModel::instance()->moveUp(m_pView->currentIndex());
}

void DlgPresence::slotMoveDown()
{
   PresenceStatusModel::instance()->moveDown(m_pView->currentIndex());
}
