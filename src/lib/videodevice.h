/****************************************************************************
 *   Copyright (C) 2012-2014 by Savoir-Faire Linux                          *
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

//SFLPhone
class VideoRenderer;

///@typedef VideoChannel A channel available in a Device
typedef QString VideoChannel;

///@typedef VideoRate The rate for a device
typedef QString VideoRate;

///@struct Resolution Equivalent of "640x480"
class LIB_EXPORT Resolution : public QSize {
public:
   //Constructor
   Resolution(uint _width, uint _height);
   Resolution(const QString& size = QString());
   Resolution(const Resolution& res);
   Resolution(const QSize& size);
   explicit Resolution();
   //Getter
   const QString toString() const;

};

class VideoModel;

///VideoDevice: V4L devices used to record video for video call
class LIB_EXPORT VideoDevice : public QObject {
   Q_OBJECT
   friend class VideoModel;
   friend class VideoDeviceModel;
   public:
      //Constants
      constexpr static const char* NONE = "";

      //Getter
      const QStringList         rateList(const VideoChannel& channel, const Resolution& resolution);
      const QList<Resolution>   resolutionList(const VideoChannel& channel);
      const QList<VideoChannel> channelList ();
      const Resolution          resolution  ();
      const VideoChannel        channel     ();
      const VideoRate           rate        ();
      const QString             id          () const;
      const QString             name        ();
      bool  isActive                        ();

      //Static getter

      //Setter
      void setRate       ( const VideoRate&    rate       );
      void setResolution ( const Resolution&   resolution );
      void setChannel    ( const VideoChannel& channel    );

   private:
      //Constructor
      explicit VideoDevice(const QString &id);
      ~VideoDevice();

      //Attributes
      QString m_DeviceId;

      class PreferenceNames {
      public:
         constexpr static const char* RATE    = "rate"   ;
         constexpr static const char* NAME    = "name"   ;
         constexpr static const char* CHANNEL = "channel";
         constexpr static const char* SIZE    = "size"   ;
      };

   signals:
      void renderingStarted(VideoRenderer*);
      void renderingStopped(VideoRenderer*);
      void renderStateChanged(bool state);
};
#endif
