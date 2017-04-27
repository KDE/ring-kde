/***************************************************************************
 *   Copyright (C) 2014-2015 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
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
#include "ringtone.h"

#include <account.h>
#include <ringtonemodel.h>

// KDE
#include <KLocalizedString>

Pages::RingTone::RingTone(QWidget *parent) : PageBase(parent)
{
   setupUi(this);
   m_pAddFile->setPlaceholderText(i18n("Select a new ringtone"));

   connect(m_pAddFile, &KUrlRequester::urlSelected, this, &Pages::RingTone::urlSelected);
   connect(this,&PageBase::accountSet,[this]() {
      m_pRingtones->setModel(&RingtoneModel::instance());
      m_pRingtones->setSelectionModel(RingtoneModel::instance().selectionModel(account()));

      if (m_pRingtones->horizontalHeader()) {
         m_pRingtones->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
         for (int i =1;i<RingtoneModel::instance().columnCount();i++)
            m_pRingtones->horizontalHeader()->setSectionResizeMode(i,QHeaderView::ResizeToContents);
      }
   });
}

void Pages::RingTone::urlSelected(const QUrl& url)
{
   RingtoneModel::instance().add(url, account());
   setChanged();
}

void Pages::RingTone::play()
{
   RingtoneModel::instance().play(
      RingtoneModel::instance().selectionModel(account())->currentIndex()
   );
}

// kate: space-indent on; indent-width 3; replace-tabs on;
