/***************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                         *
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
#include "presence.h"
#include <lib/presencestatusmodel.h>

Presence::Presence(QWidget* parent) : QWidget(parent)
{
   setupUi(this);
   m_pMessage->setVisible(false);
   m_pPresentRB->setVisible(false);
   m_pAbsentRB->setVisible(false);
   m_pView->setModel(PresenceStatusModel::instance());

   //Toggle between custom and pre-defined status
   connect(m_pCustomMessageCK             ,SIGNAL(toggled(bool)),                          PresenceStatusModel::instance(),SLOT(setUseCustomStatus(bool)));
   connect(PresenceStatusModel::instance(),SIGNAL(useCustomStatusChanged(bool)),           m_pCustomMessageCK,             SLOT(setChecked(bool)));

   //Toggle between present and absent
   connect(m_pPresentRB                   ,SIGNAL(toggled(bool)),                          PresenceStatusModel::instance(),SLOT(setCustomStatus(bool)));
   connect(PresenceStatusModel::instance(),SIGNAL(customStatusChanged(bool)),              m_pPresentRB,                   SLOT(setChecked(bool)));

   //Select current pre-defined status
   connect(m_pView->selectionModel()      ,SIGNAL(currentChanged(QModelIndex,QModelIndex)),PresenceStatusModel::instance(),SLOT(setCurrentIndex(QModelIndex)));

   //Change custom text status
   connect(m_pMessage                     ,SIGNAL(textChanged()),                          this,                           SLOT(slotTextChanged()));
   connect(PresenceStatusModel::instance(),SIGNAL(customMessageChanged(QString)),          this,                           SLOT(slotReplaceText(QString)));
}

void Presence::slotTextChanged()
{
   const int pos = m_pMessage->textCursor().position();
   PresenceStatusModel::instance()->setCustomMessage(m_pMessage->toPlainText());
   m_pMessage->textCursor().setPosition(pos);
}

void Presence::slotReplaceText(const QString& newText)
{
   if (newText != m_pMessage->toPlainText()) {
      const int pos = m_pMessage->textCursor().position();
      m_pMessage->setPlainText(newText);
      m_pMessage->textCursor().setPosition(pos);
   }
}
