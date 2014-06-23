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
#ifndef VIDEO_SETTINGS_H
#define VIDEO_SETTINGS_H

#include <QtGui/QWidget>
#include "ui_videodevicesetting.h"

class VideoDevice;

class VideoSettings : public QWidget, public Ui_VideoSettings
{
   Q_OBJECT
public:
   explicit VideoSettings(QWidget* parent);

   void setDevice(VideoDevice* dev);

   VideoDevice* device() const;

   void hideDevices();

public Q_SLOTS:
   void slotReloadDevices();

private Q_SLOTS:
   void slotDeviceChanged(int idx = -1);
   void slotChannelChanged(int idx = -1);
   void slotResolutionChanged(int idx = -1);
   void slotRateChanged(int idx = -1);
Q_SIGNALS:
   void settingsChanged();
};

#endif
