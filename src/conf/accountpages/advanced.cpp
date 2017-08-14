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
#include "advanced.h"

#include <account.h>

Pages::Advanced::Advanced(QWidget *parent) : PageBase(parent)
{
   setupUi(this);

   connect(this,&PageBase::accountSet,[this]() {
      m_pDTMF_GB->setVisible(account()->protocol() == Account::Protocol::SIP);
   });

   connect(lrcfg_hasActiveCallLimit, &QCheckBox::toggled, this, [this](bool v) {
      if (!v)
         lrcfg_activeCallLimit->setValue(-1);
      else if (lrcfg_activeCallLimit->value() == -1)
         lrcfg_activeCallLimit->setValue(1);
   });
}

// kate: space-indent on; indent-width 3; replace-tabs on;

