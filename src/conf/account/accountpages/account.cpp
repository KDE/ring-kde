/***************************************************************************
 *   Copyright (C) 2014-2015 by Savoir-Faire Linux                         *
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

#include "account.h"
// #include ""
#include <KConfigDialogManager>
#include "accountserializationadapter.h"

Pages::Account::Account(const QString& accountId, QWidget *parent) : PageBase(parent)
{
   setupUi(this);

   AccountSerializationAdapter* adapter = new AccountSerializationAdapter(accountId);

   m_pCurrentManager = new KConfigDialogManager(this, adapter);

   m_pCurrentManager->addWidget(this);

   connect(m_pCurrentManager, SIGNAL(settingsChanged()), this, SLOT(slotUpdateButtons()));
   connect(m_pCurrentManager, SIGNAL(widgetModified()), this, SLOT(slotUpdateButtons()));
}

void Pages::Account::updateWidgets()
{
   qDebug() << "Update widgets";
   m_pCurrentManager->updateWidgets();
}

void Pages::Account::updateSettings()
{
   qDebug() << "Update settings";
   m_pCurrentManager->updateSettings();
}

KConfigDialogManager* Pages::Account::manager() const
{
   return m_pCurrentManager;
}

void Pages::Account::slotUpdateButtons()
{
   qDebug() << "Update buttons";
   emit updateButtons();
}