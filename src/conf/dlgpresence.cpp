/****************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                          *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                    *
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

// Qt
#include <QtGui/QIcon>

//KDE
#include <KLocalizedString>

//Ring
#include <presencestatusmodel.h>
#include "tip/tipmanager.h"
#include "tip/tip.h"

DlgPresence::DlgPresence(QWidget *parent) : QWidget(parent),m_Changed(false)
{
   setupUi(this);
   m_pView->setModel(&PresenceStatusModel::instance());
   m_pUp->setIcon     ( QIcon::fromTheme( QStringLiteral("go-up")       ) );
   m_pDown->setIcon   ( QIcon::fromTheme( QStringLiteral("go-down")     ) );
   m_pAdd->setIcon    ( QIcon::fromTheme( QStringLiteral("list-add")    ) );
   m_pRemove->setIcon ( QIcon::fromTheme( QStringLiteral("list-remove") ) );
   connect(m_pAdd   , &QAbstractButton::clicked,&PresenceStatusModel::instance() ,&PresenceStatusModel::addRow       );
   connect(m_pUp    , &QAbstractButton::clicked,this                            ,&DlgPresence::slotMoveUp   );
   connect(m_pDown  , &QAbstractButton::clicked,this                            ,&DlgPresence::slotMoveDown );
   connect(m_pRemove, &QAbstractButton::clicked,this                            ,&DlgPresence::slotRemoveRow);
   connect(&PresenceStatusModel::instance(),&QAbstractItemModel::dataChanged,this,&DlgPresence::slotChanged);

   if (m_pView->horizontalHeader()) {
      /*m_pView->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
      m_pView->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
      for (int i=2;i<PresenceStatusModel::instance().columnCount();i++) {
         m_pView->horizontalHeader()->setSectionResizeMode(i,QHeaderView::ResizeToContents);
      }*/
   }

   //Add an info tip in the account list
   m_pTipManager = new TipManager(m_pView);
   m_pTip = new Tip(i18n("In this table, it is possible to manage different presence states. "
   "The \"Message\" and \"Present\" values will be exported to the server for every accounts that support it. "
   "The other fields are designed to make presence status management easier. Please note that some SIP registrar "
   "have incomplete presence status (publishing) support."),this);
   m_pTip->setMaximumWidth(510);
   m_pTipManager->setCurrentTip(m_pTip);
}

DlgPresence::~DlgPresence()
{
   delete m_pTipManager;
   delete m_pTip;
}

void DlgPresence::updateWidgets()
{
   
}

void DlgPresence::updateSettings()
{
   PresenceStatusModel::instance().save();
   m_Changed = false;
   emit updateButtons();
}

bool DlgPresence::hasChanged()
{
   return m_Changed;
}

void DlgPresence::slotChanged()
{
   m_Changed = true;
   emit updateButtons();
}

void DlgPresence::slotRemoveRow()
{
   PresenceStatusModel::instance().removeRow(m_pView->currentIndex());
}


void DlgPresence::slotMoveUp()
{
   PresenceStatusModel::instance().moveUp(m_pView->currentIndex());
}

void DlgPresence::slotMoveDown()
{
   PresenceStatusModel::instance().moveDown(m_pView->currentIndex());
}

// kate: space-indent on; indent-width 3; replace-tabs on;
