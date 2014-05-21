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
#include "videodevice.h"
#include "dbus/videomanager.h"


Resolution::Resolution(uint _width, uint _height):QSize(_width,_height)
{
}

Resolution::Resolution() : QSize()
{
}

Resolution::Resolution(const QString& size)
{
   if (size.split('x').size() == 2) {
      setWidth(size.split('x')[0].toInt());
      setHeight(size.split('x')[1].toInt());
   }
}

Resolution::Resolution(const Resolution& res):QSize(res.width(),res.height())
{
}

Resolution::Resolution(const QSize& size):QSize(size)
{
}

const QString Resolution::toString() const
{
   return QString::number(width())+'x'+QString::number(height());
}


///Constructor
VideoDevice::VideoDevice(const QString &id) : QObject(nullptr), m_DeviceId(id),m_Resolution("0x0")
{
   
}

///Destructor
VideoDevice::~VideoDevice()
{
}

///Get the valid rates for this device
const QStringList VideoDevice::rateList(const VideoChannel& channel, const Resolution& resolution)
{
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   return interface.getDeviceRateList(m_DeviceId,channel,resolution.toString());
}

///Get the valid channel list
const QList<VideoChannel> VideoDevice::channelList()
{
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   return interface.getDeviceChannelList(m_DeviceId);
}

///Set the current device rate
void VideoDevice::setRate(const VideoRate& rate)
{
   m_Rate = rate;
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   MapStringString pref = interface.getPreferences(m_DeviceId);
   pref[PreferenceNames::RATE] = rate;
   interface.setPreferences(m_DeviceId,pref);
}

///Set the current resolution
void VideoDevice::setResolution(const Resolution& resolution) //??? No device
{
   m_Resolution = resolution;
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   MapStringString pref = interface.getPreferences(m_DeviceId);
   pref[PreferenceNames::SIZE] = resolution.toString();
   interface.setPreferences(m_DeviceId,pref);
}

///Set the current device channel
void VideoDevice::setChannel(const VideoChannel& channel) //??? No device
{
   m_Channel = channel;
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   MapStringString pref = interface.getPreferences(m_DeviceId);
   pref[PreferenceNames::CHANNEL] = channel;
   interface.setPreferences(m_DeviceId,pref);
}

///Get the current resolution
const Resolution VideoDevice::resolution()
{
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   if ((!m_Resolution.isValid()) || m_Resolution == QSize(0,0))
      m_Resolution = Resolution(QMap<QString,QString>(interface.getPreferences(m_DeviceId))[PreferenceNames::SIZE]);
   return m_Resolution;
}

///Get the current channel
const VideoChannel VideoDevice::channel() //??? No device
{
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   if (m_Channel.isEmpty())
      m_Channel = QMap<QString,QString>(interface.getPreferences(m_DeviceId))[PreferenceNames::CHANNEL];
   return m_Channel;
}

///Get the current rate
const VideoRate VideoDevice::rate()
{
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   if (m_Rate.isEmpty())
      m_Rate = QMap<QString,QString>(interface.getPreferences(m_DeviceId))[PreferenceNames::RATE];
   return m_Rate;
}

///Get a list of valid resolution
const QList<Resolution> VideoDevice::resolutionList(const VideoChannel& channel)
{
   QList<Resolution> toReturn;
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   const QStringList list = interface.getDeviceSizeList(m_DeviceId,channel);
   foreach(const QString& res,list) {
      toReturn << Resolution(res);
   }
   return toReturn;
}

///Get the device id
const QString VideoDevice::id() const
{
   return m_DeviceId;
}

///Get the device name
const QString VideoDevice::name()
{
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   return QMap<QString,QString>(interface.getPreferences(m_DeviceId))[PreferenceNames::NAME];;
}

///Is this device the default one
bool VideoDevice::isActive()
{
   return QString(DBus::VideoManager::instance().getActiveDevice()) == m_DeviceId;
}
