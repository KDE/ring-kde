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
#include "player.h"

#include <KIcon>
#include "playeroverlay.h"
#include "lib/call.h"

Player::Player(QWidget* parent) : QWidget(parent),m_pParent(static_cast<PlayerOverlay*>(parent))
{
   setupUi(this);
   m_pPlayPause->setIcon( KIcon( "media-playback-start" ));
   m_pStop->setIcon( KIcon( "media-playback-stop"  ));
   connect( m_pParent->m_pCall , SIGNAL(playbackPositionChanged(int,int)) , this               , SLOT(slotUpdateSlider(int,int)));
   connect( m_pPlayPause       , SIGNAL(clicked())                        , m_pParent->m_pCall , SLOT(playRecording()));
   connect( m_pStop            , SIGNAL(clicked())                        , m_pParent->m_pCall , SLOT(stopRecording()));
   connect( m_pSlider          , SIGNAL(sliderPressed())                  , this               , SLOT(slotDisconnectSlider()));
   connect( m_pSlider          , SIGNAL(sliderReleased())                 , this               , SLOT(slotConnectSlider()));
//    setAttribute( Qt::WA_TranslucentBackground, true );
}

void Player::slotHide()
{
   setVisible(false);
   emit sigHide(true);
}

void Player::slotDisconnectSlider()
{
   disconnect(m_pParent->m_pCall,SIGNAL(playbackPositionChanged(int,int)),this,SLOT(slotUpdateSlider(int,int)));
}

void Player::slotConnectSlider()
{
   m_pParent->m_pCall->seekRecording(((double)m_pSlider->value())/((double)m_pSlider->maximum()) * 100);
   connect(m_pParent->m_pCall,SIGNAL(playbackPositionChanged(int,int)),this,SLOT(slotUpdateSlider(int,int)));
}

void Player::slotUpdateSlider(int pos, int size)
{
   m_pLeft->setText(QString("%1").arg((size/1000-pos/1000)/60,2,10,QChar('0'))+':'+QString("%1").arg((size/1000-pos/1000)%60,2,10,QChar('0')));
   m_pElapsed->setText(QString("%1").arg((pos/1000)/60,2,10,QChar('0'))+':'+QString("%1").arg((pos/1000)%60,2,10,QChar('0')));
   m_pSlider->setMaximum(size);
   m_pSlider->setValue(pos);
}

void Player::play()
{
   m_pParent->m_pCall->playRecording();
}
