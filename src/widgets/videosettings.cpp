/***************************************************************************
 *   Copyright (C) 2011-2015 by Savoir-Faire Linux                         *
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
#include "videosettings.h"

#include <video/devicemodel.h>
#include <video/channel.h>
#include <video/resolution.h>
#include <video/rate.h>
#include <video/configurationproxy.h>

VideoSettings::VideoSettings(QWidget* parent) : QWidget(parent)
{
   setupUi(this);

   // Work around a race condition in LRC
   Video::ConfigurationProxy::rateModel();
   Video::ConfigurationProxy::resolutionModel();
   Video::ConfigurationProxy::channelModel();
   Video::ConfigurationProxy::deviceModel();

   m_pDevice     -> bindToModel(&Video::ConfigurationProxy::deviceModel    () , &Video::ConfigurationProxy::deviceSelectionModel    ());
   m_pChannel    -> bindToModel(&Video::ConfigurationProxy::channelModel   () , &Video::ConfigurationProxy::channelSelectionModel   ());
   m_pResolution -> bindToModel(&Video::ConfigurationProxy::resolutionModel() , &Video::ConfigurationProxy::resolutionSelectionModel());
   m_pRate       -> bindToModel(&Video::ConfigurationProxy::rateModel      () , &Video::ConfigurationProxy::rateSelectionModel      ());

   if (!Video::ConfigurationProxy::deviceModel().rowCount()) {
      m_pChannel    ->setDisabled(true);
      m_pResolution ->setDisabled(true);
      m_pRate       ->setDisabled(true);
      connect(&Video::ConfigurationProxy::deviceModel(), &QAbstractItemModel::rowsInserted, this, &VideoSettings::slotInserted);
   }
}

void VideoSettings::slotInserted()
{
   m_pChannel    ->setDisabled(false);
   m_pResolution ->setDisabled(false);
   m_pRate       ->setDisabled(false);

   emit videoEnabled(true);
}

void VideoSettings::setDevice(Video::Device* dev)
{
   Video::DeviceModel::instance().setActive(dev);
   m_pDevice->selectionModel()->setCurrentIndex(m_pDevice->model()->index(Video::DeviceModel::instance().activeIndex(),0), QItemSelectionModel::ClearAndSelect);
}

Video::Device* VideoSettings::device() const
{
   const QList<Video::Device*> devices = Video::DeviceModel::instance().devices();
   return m_pDevice->currentIndex() < devices.size() && m_pDevice->currentIndex() >= 0 ? devices[m_pDevice->currentIndex()]:nullptr;
}

void VideoSettings::slotChannelChanged(int idx)
{
   Q_UNUSED(idx)
   emit settingsChanged();
}

void VideoSettings::slotResolutionChanged(int idx)
{
   Q_UNUSED(idx)
   emit settingsChanged();
}

void VideoSettings::slotRateChanged(int idx)
{
   Q_UNUSED(idx)
   emit settingsChanged();
}

void VideoSettings::slotDeviceChanged(int idx)
{
   Q_UNUSED(idx)
   emit settingsChanged();
}

void VideoSettings::hideDevices()
{
   m_pDevice->setVisible(false);
   label_4->setVisible(false);
}
