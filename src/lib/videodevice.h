/****************************************************************************
 *   Copyright (C) 2012-2013 by Savoir-Faire Linux                          *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Lesser General Public             *
 *   License as published by the Free Software Foundation; either           *
 *   version 2.1 of the License, or (at your option) any later version.     *
 *                                                                          *
 *   This library is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#ifndef VIDEO_DEVICE_H
#define VIDEO_DEVICE_H

#include "typedefs.h"

//Qt
#include <QStringList>
#include <QtCore/QSize>

///@typedef VideoChannel A channel available in a Device
typedef QString VideoChannel;

///@typedef VideoRate The rate for a device
typedef QString VideoRate;

///@struct Resolution Equivalent of "640x480"
class LIB_EXPORT Resolution : public QSize {
public:
   //Constructor
   Resolution(uint _width, uint _height);
   Resolution(const QString& size);
   Resolution(const Resolution& res);
   Resolution(const QSize& size);
   //Getter
   const QString toString() const;

};

///VideoDevice: V4L devices used to record video for video call
class LIB_EXPORT VideoDevice {
   public:
      //Singleton
      static VideoDevice* getDevice(QString id);

      //Getter
      const QStringList         rateList(VideoChannel channel, Resolution resolution);
      const QList<Resolution>   resolutionList(VideoChannel channel);
      const QList<VideoChannel> channelList ();
      const Resolution          resolution  ();
      const VideoChannel        channel     ();
      const VideoRate           rate        ();
      const QString             deviceId    () const;
      
      //Static getter
      static const QList<VideoDevice*> deviceList();

      //Setter
      void setRate       ( VideoRate rate        );
      void setResolution ( Resolution resolution );
      void setChannel    ( VideoChannel channel  );
   private:
      //Constructor
      explicit VideoDevice(QString id);

      //Attributes
      QString m_DeviceId;
      static QHash<QString,VideoDevice*> m_slDevices;
      static bool m_sInit;
};
#endif
