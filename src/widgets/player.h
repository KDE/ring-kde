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
#ifndef PLAYER_H
#define PLAYER_H

#include <QtGui/QWidget>

#include "ui_player.h"

class PlayerOverlay;

class Player : public QWidget, public Ui_Player
{
   Q_OBJECT
public:
   explicit Player(QWidget* parent = nullptr);

private:
   PlayerOverlay* m_pParent;

private Q_SLOTS:
   void slotHide();
   void slotDisconnectSlider();
   void slotConnectSlider();
   void slotUpdateSlider(int,int);
   void play();

Q_SIGNALS:
   void sigHide(bool);
};

#endif
