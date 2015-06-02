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
#include "codecs.h"

#include <QtCore/QSortFilterProxyModel>

#include <account.h>
#include <audio/codecmodel.h>

Pages::Codecs::Codecs(QWidget *parent) : PageBase(parent)
{
   setupUi(this);
   connect(this,&PageBase::accountSet,[this]() {
      m_pAudioCodecs->setModel(account()->codecModel()->audioCodecs());
      m_pVideoCodecs->setModel(account()->codecModel()->videoCodecs());
      //TODO fix selectionmodels
   });
}

