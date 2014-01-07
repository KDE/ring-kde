/***************************************************************************
 *   Copyright (C) 2012-2014 by Savoir-Faire Linux                         *
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
#ifndef PLAYER_OVERLAY_H
#define PLAYER_OVERLAY_H

#include <QtGui/QWidget>
#include "ui_playeroverlay.h"
class Call;
class PlayerOverlay : public QWidget, public Ui_PlayerOverlay
{
   Q_OBJECT
public:
   friend class Player;
   explicit PlayerOverlay(Call* call, QWidget* parent = nullptr);
   void setCall(Call* call);

private:
   Call* m_pCall;

private Q_SLOTS:
   void slotDeleteRecording();
};

#endif
