/***************************************************************************
 *   Copyright (C) 2012-2015 by Savoir-Faire Linux                         *
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

#include <QtGui/QIcon>
#include "playeroverlay.h"
#include "media/recording.h"
#include "media/avrecording.h"

Player::Player(QWidget* parent) : QWidget(parent),m_pParent(static_cast<PlayerOverlay*>(parent))
{
   setupUi(this);
   m_pPlayPause->setIcon( QIcon::fromTheme( QStringLiteral("media-playback-start") ));
   m_pStop->setIcon( QIcon::fromTheme( QStringLiteral("media-playback-stop")  ));
   slotConnectSlider();
      void formattedTimeElapsedChanged(const QString& formattedValue);
   ///Emitted when the formatted duration string change
   void formattedDurationChanged   (const QString& formattedValue);
   ///Emitted when the formatted time left string change
   void formattedTimeLeftChanged   (const QString& formattedValue);
   m_pSlider->setMaximum(10000);
   connect( m_pPlayPause       , &QAbstractButton::clicked                        , m_pParent->m_pRecording , &Media::AVRecording::play);
   connect( m_pStop            , &QAbstractButton::clicked                        , m_pParent->m_pRecording , &Media::AVRecording::stop);
   connect( m_pSlider          , SIGNAL(sliderPressed())                  , this               , SLOT(slotDisconnectSlider()));
   connect( m_pSlider          , SIGNAL(sliderReleased())                 , this               , SLOT(slotConnectSlider()));
}

void Player::slotHide()
{
   setVisible(false);
   emit sigHide(true);
}

void Player::slotDisconnectSlider()
{
   disconnect(m_pParent->m_pRecording ,&Media::AVRecording::playbackPositionChanged     ,this,&Player::slotUpdateSlider           );
   disconnect(m_pParent->m_pRecording ,&Media::AVRecording::formattedTimeElapsedChanged ,this,&Player::formattedTimeElapsedChanged);
   disconnect(m_pParent->m_pRecording ,&Media::AVRecording::formattedTimeLeftChanged    ,this,&Player::formattedTimeLeftChanged   );
}

void Player::slotConnectSlider()
{
   m_pParent->m_pRecording->seek(((double)m_pSlider->value())/((double)m_pSlider->maximum()) * 100);
   connect(m_pParent->m_pRecording ,&Media::AVRecording::playbackPositionChanged     ,this,&Player::slotUpdateSlider           );
   connect(m_pParent->m_pRecording ,&Media::AVRecording::formattedTimeElapsedChanged ,this,&Player::formattedTimeElapsedChanged);
   connect(m_pParent->m_pRecording ,&Media::AVRecording::formattedTimeLeftChanged    ,this,&Player::formattedTimeLeftChanged   );
}

void Player::slotUpdateSlider(double pos)
{
   m_pSlider->setValue(pos*10000);
}

void Player::formattedTimeElapsedChanged(const QString& formattedValue)
{
   m_pElapsed->setText(formattedValue);
}

void Player::formattedTimeLeftChanged(const QString& formattedValue)
{
   m_pLeft->setText(formattedValue);
}

void Player::play()
{
   m_pParent->m_pRecording->play();
}
