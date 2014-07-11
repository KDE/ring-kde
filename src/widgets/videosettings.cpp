/***************************************************************************
 *   Copyright (C) 2011-2014 by Savoir-Faire Linux                         *
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
#include "videosettings.h"

#include <lib/video/videodevicemodel.h>
#include <lib/video/videochannel.h>
#include <lib/video/videoresolution.h>
#include <lib/video/videorate.h>

VideoSettings::VideoSettings(QWidget* parent) : QWidget(parent)
{
   setupUi(this);
   slotReloadDevices();
   m_pChannel->blockSignals(true);
   m_pChannel->setModel(VideoDeviceModel::instance()->activeDevice());
   m_pChannel->blockSignals(false);
}

void VideoSettings::setDevice(VideoDevice* dev)
{
   VideoDeviceModel::instance()->setActive(dev);
   m_pDevice->setCurrentIndex(VideoDeviceModel::instance()->activeIndex());
}

VideoDevice* VideoSettings::device() const
{
   const QList<VideoDevice*> devices = VideoDeviceModel::instance()->devices();
   return devices.size() > m_pDevice->currentIndex()? devices[m_pDevice->currentIndex()]:nullptr;
}

void VideoSettings::slotReloadDevices()
{
   m_pDevice->blockSignals(true);
   m_pDevice->setModel(VideoDeviceModel::instance());
   m_pDevice->blockSignals(false);
   m_pDevice->setCurrentIndex(VideoDeviceModel::instance()->activeIndex());
   slotChannelChanged   ();
   slotResolutionChanged();
   slotRateChanged      ();
   setEnabled(m_pDevice->count());
}

void VideoSettings::slotChannelChanged(int idx)
{
   if (m_pChannel->count() == 0 && (idx != -1)) return;

   if (idx != -1 && idx < VideoDeviceModel::instance()->activeDevice()->channelList().size()) {
      VideoDeviceModel::instance()->activeDevice()->setActiveChannel(idx);
      emit settingsChanged();
   }
   else
      m_pChannel->setCurrentIndex(VideoDeviceModel::instance()->activeDevice()->activeChannel()->relativeIndex());
   m_pResolution->blockSignals(true);
   m_pResolution->setModel(VideoDeviceModel::instance()->activeDevice()->activeChannel());
   m_pResolution->blockSignals(false);
   slotResolutionChanged();
}

void VideoSettings::slotResolutionChanged(int idx)
{
   if (m_pResolution->count() == 0 && (idx != -1)) return;

   if (idx >= 0 && VideoDeviceModel::instance()->activeDevice()->activeChannel()->validResolutions().size() >idx) {
      VideoDeviceModel::instance()->activeDevice()->activeChannel()->setActiveResolution(idx);
      emit settingsChanged();
   }
   else {
      m_pResolution->setCurrentIndex(VideoDeviceModel::instance()->activeDevice()->activeChannel()->activeResolution()->relativeIndex());
   }

   m_pRate->blockSignals(true);
   m_pRate->setModel(VideoDeviceModel::instance()->activeDevice()->activeChannel()->activeResolution());
   m_pRate->blockSignals(false);
   slotRateChanged();
}

void VideoSettings::slotRateChanged(int idx)
{
   if (m_pRate->count() == 0 && (idx != -1)) return;

   if (idx == -1 || idx >= VideoDeviceModel::instance()->activeDevice()->activeChannel()->activeResolution()->validRates().size()) {
      m_pRate->setCurrentIndex(
         VideoDeviceModel::instance()->activeDevice()->activeChannel()->activeResolution()->activeRate()->relativeIndex()
      );
   }
   else {
      VideoDeviceModel::instance()->activeDevice()->activeChannel()->activeResolution()->setActiveRate(idx);
      emit settingsChanged();
   }
}

void VideoSettings::slotDeviceChanged(int idx)
{
   if (m_pDevice->count() == 0 && (idx != -1)) return;

   VideoDeviceModel::instance()->setActive(VideoDeviceModel::instance()->index(idx,0));
   m_pChannel->blockSignals(true);
   m_pChannel->setModel(VideoDeviceModel::instance()->activeDevice());
   m_pChannel->blockSignals(false);
   slotChannelChanged();
   emit settingsChanged();
}

void VideoSettings::hideDevices()
{
   m_pDevice->setVisible(false);
   label_4->setVisible(false);
}
