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
class Resolution;
class VideoRate;
class VideoChannel;
class VideoDevice;

///@typedef VideoChannel A channel available in a Device
class LIB_EXPORT VideoChannel
{
   //Only VideoDevice can add resolutions
   friend class VideoDevice;
public:
   QString name() const {
      return m_Name;
   }
   Resolution* activeResolution();
   QList<Resolution*> validResolutions() const {
      return m_lValidResolutions;
   }
   VideoDevice* device() const {
      return m_pDevice;
   }
   int index();

   bool setActiveResolution(Resolution* res);

private:
   VideoChannel(VideoDevice* dev,const QString& name);
   virtual ~VideoChannel() {}
   QString m_Name;
   QList<Resolution*> m_lValidResolutions;
   Resolution*        m_pCurrentResolution;
   VideoDevice*       m_pDevice;
};

///@typedef VideoRate The rate for a device
class LIB_EXPORT VideoRate
{
   //Can only be created by VideoDevice
   friend class VideoDevice;

public:
   virtual ~VideoRate() {}
   QString name() const {
      return m_Name;
   }
   int index();
private:
   VideoRate(const Resolution* res,const QString& name) :
      m_Name(name),m_pResolution(res) {}
   QString m_Name;
   const Resolution* m_pResolution;
};

///@struct Resolution Equivalent of "640x480"
class LIB_EXPORT Resolution : public QSize {
   //Only VideoDevice can add validated rates
   friend class VideoDevice;
public:
   //Constructor
   Resolution(uint _width, uint _height);
   Resolution(const QString& size = QString(), VideoChannel* chan = nullptr);
   Resolution(const Resolution& res);
   Resolution(const QSize& size);
   explicit Resolution();
   //Getter
   const QString name() const;
   const QList<VideoRate*> validRates() const {
      return m_lValidRates;
   }
   int index() const;
   VideoRate* activeRate();
   bool setActiveRate(VideoRate* rate) {
      if (!rate || (m_lValidRates.indexOf(rate) != -1)) {
         qWarning() << "Trying to set an invalid rate";
         return false;
      }
      return true;
   }
private:

   //Attributes
   QList<VideoRate*> m_lValidRates;
   VideoRate*        m_pCurrentRate;
   VideoChannel*     m_pChannel;
};

class VideoModel;

///VideoDevice: V4L devices used to record video for video call
class LIB_EXPORT VideoDevice : public QObject {
   Q_OBJECT
   friend class VideoModel;
   friend class VideoDeviceModel;

   //Need to access the PreferenceNames table
   friend class VideoChannel;
   friend class Resolution;
   public:
      //Constants
      constexpr static const char* NONE = "";

      //Getter
      QList<VideoChannel*> channelList      () const;
      VideoChannel*        activeChannel    () const;
      const QString        id               () const;
      const QString        name             () const;
      bool  isActive                        () const;

      //Static getter

      //Setter
      bool setActiveChannel(VideoChannel* chan);

   private:
      //Constructor
      explicit VideoDevice(const QString &id);
      ~VideoDevice();

      //Attributes
      QString       m_DeviceId          ;
      VideoChannel* m_pCurrentChannel   ;
      QList<VideoChannel*> m_lChannels  ;

      //Helper
      void save();

      class PreferenceNames {
      public:
         constexpr static const char* RATE    = "rate"   ;
         constexpr static const char* NAME    = "name"   ;
         constexpr static const char* CHANNEL = "channel";
         constexpr static const char* SIZE    = "size"   ;
      };

   Q_SIGNALS:
      void renderingStarted(VideoRenderer*);
      void renderingStopped(VideoRenderer*);
      void renderStateChanged(bool state);
};
#endif
