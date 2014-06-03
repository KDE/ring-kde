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


Resolution::Resolution(uint _width, uint _height):QSize(_width,_height),
m_pCurrentRate(nullptr),m_pChannel(nullptr)
{
}

Resolution::Resolution() : QSize(),m_pCurrentRate(nullptr),m_pChannel(nullptr)
{
}

Resolution::Resolution(const QString& size, VideoChannel* chan)
: m_pCurrentRate(nullptr),m_pChannel(chan)
{
   if (size.split('x').size() == 2) {
      setWidth(size.split('x')[0].toInt());
      setHeight(size.split('x')[1].toInt());
   }
}

Resolution::Resolution(const Resolution& res):QSize(res.width(),res.height()),
m_pCurrentRate(nullptr),m_pChannel(nullptr)
{
}

Resolution::Resolution(const QSize& size):QSize(size),
m_pCurrentRate(nullptr),m_pChannel(nullptr)
{
}

const QString Resolution::name() const
{
   return QString::number(width())+'x'+QString::number(height());
}


///Constructor
VideoDevice::VideoDevice(const QString &id) : QObject(nullptr), m_DeviceId(id),
m_pCurrentChannel(nullptr)
{
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   MapStringMapStringVectorString cap = interface.getCapabilities(id);
   QMapIterator<QString, MapStringVectorString> channels(cap);
   qDebug() << "\n\n\n\nCAP"  << id << cap.size();
   while (channels.hasNext()) {
      channels.next();

      VideoChannel* chan = new VideoChannel(this,channels.key());
      m_lChannels << chan;

      QMapIterator<QString, VectorString> resolutions(channels.value());
      while (resolutions.hasNext()) {
         resolutions.next();

         Resolution* res = new Resolution(resolutions.key(),chan);
         chan->m_lValidResolutions << res;

         foreach(const QString& rate, resolutions.value()) {
            VideoRate* r = new VideoRate(res,rate);
            res->m_lValidRates << r;
         }
      }
   }
   Q_UNUSED(cap)
}

///Destructor
VideoDevice::~VideoDevice()
{
}

///Get the valid channel list
QList<VideoChannel*> VideoDevice::channelList() const
{
   return m_lChannels;
}

///Save the current settings
void VideoDevice::save()
{
   //In case new (unsupported) fields are added, merge with existing
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   MapStringString pref = interface.getPreferences(m_DeviceId);
   pref[VideoDevice::PreferenceNames::CHANNEL] = activeChannel()->name();
   pref[VideoDevice::PreferenceNames::SIZE   ] = activeChannel()->activeResolution()->name();
   pref[VideoDevice::PreferenceNames::RATE   ] = activeChannel()->activeResolution()->activeRate()->name();
   interface.setPreferences(m_DeviceId,pref);
}

///Get the device id
const QString VideoDevice::id() const
{
   return m_DeviceId;
}

///Get the device name
const QString VideoDevice::name() const
{
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   return QMap<QString,QString>(interface.getPreferences(m_DeviceId))[PreferenceNames::NAME];;
}

///Is this device the default one
bool VideoDevice::isActive() const
{
   return QString(DBus::VideoManager::instance().getActiveDevice()) == m_DeviceId;
}

Resolution* VideoChannel::activeResolution()
{
   //If it is the current device, then there is "current" resolution
   if ((!m_pCurrentResolution) && m_pDevice->isActive()) {
      VideoManagerInterface& interface = DBus::VideoManager::instance();
      const QString res = QMap<QString,QString>(interface.getPreferences(m_pDevice->id()))[VideoDevice::PreferenceNames::SIZE];
      foreach(Resolution* r, validResolutions()) {
         if (r->name() == res) {
            m_pCurrentResolution = r;
            break;
         }
      }
   }
   //If it isn't the current _or_ the current res is invalid, pick the first valid one
   if (!m_pCurrentResolution && validResolutions().size()) {
      m_pCurrentResolution = validResolutions()[0];
   }

   return m_pCurrentResolution;
}

bool VideoChannel::setActiveResolution(Resolution* res) {
   if ((!res) || m_lValidResolutions.indexOf(res) == -1 || res->name().isEmpty()) {
      qWarning() << "Invalid active resolution" << (res?res->name():"NULL");
      return false;
   }
   m_pCurrentResolution = res;
   m_pDevice->save();
   return true;
}

VideoRate* Resolution::activeRate()
{
   if (!m_pChannel) {
      qWarning() << "Trying to get the active rate of an unattached resolution";
      return nullptr;
   }
   if (!m_pCurrentRate && m_pChannel && m_pChannel->device()->isActive()) {
      VideoManagerInterface& interface = DBus::VideoManager::instance();
      const QString rate = QMap<QString,QString>(
         interface.getPreferences(m_pChannel->device()->id()))[VideoDevice::PreferenceNames::RATE];
      foreach(VideoRate* r, m_lValidRates) {
         if (r->name() == rate) {
            m_pCurrentRate = r;
            break;
         }
      }
   }
   if ((!m_pCurrentRate) && m_lValidRates.size())
      m_pCurrentRate = m_lValidRates[0];

   return m_pCurrentRate;
}

int Resolution::index() const
{
   return m_pChannel?m_pChannel->validResolutions().indexOf(const_cast<Resolution*>(this)):-1;
}

int VideoChannel::index() {
   return m_pDevice->channelList().indexOf(this);
}

int VideoRate::index() {
   return m_pResolution->validRates().indexOf(this);
}

bool VideoDevice::setActiveChannel(VideoChannel* chan)
{
   if (!chan || !m_lChannels.indexOf(chan)) {
      qWarning() << "Trying to set an invalid channel" << (chan?chan->name():"NULL") << "for" << id();
      return false;
   }
   m_pCurrentChannel = chan;
   save();
   return true;
}

VideoChannel::VideoChannel(VideoDevice* dev,const QString& name) :
   m_Name(name),m_pCurrentResolution(nullptr),m_pDevice(dev)
{
   m_pCurrentResolution = nullptr;
   m_pCurrentResolution = nullptr;
   m_pCurrentResolution = nullptr;
   m_pCurrentResolution = nullptr;
   m_pCurrentResolution = nullptr;
}

VideoChannel* VideoDevice::activeChannel() const
{
   if (!m_pCurrentChannel) {
      VideoManagerInterface& interface = DBus::VideoManager::instance();
      const QString chan = QMap<QString,QString>(interface.getPreferences(m_DeviceId))[VideoDevice::PreferenceNames::CHANNEL];
      foreach(VideoChannel* c, m_lChannels) {
         if (c->name() == chan) {
            const_cast<VideoDevice*>(this)->m_pCurrentChannel = c;
            break;
         }
      }
   }
   if (!m_pCurrentChannel && m_lChannels.size()) {
      const_cast<VideoDevice*>(this)->m_pCurrentChannel = m_lChannels[0];
   }
   return m_pCurrentChannel;
}