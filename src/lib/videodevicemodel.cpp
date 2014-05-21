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
#include "videodevicemodel.h"
#include "videodevice.h"
#include "call.h"
#include "account.h"
#include "videocodec.h"
#include "dbus/videomanager.h"

#include <QtCore/QCoreApplication>

VideoDeviceModel* VideoDeviceModel::m_spInstance = nullptr;

VideoDeviceModel* VideoDeviceModel::instance()
{
   if (!m_spInstance)
      m_spInstance = new VideoDeviceModel();
   return m_spInstance;
}

///Get data from the model
QVariant VideoDeviceModel::data( const QModelIndex& idx, int role) const
{
   if(idx.column() == 0 && role == Qt::DisplayRole)
      return QVariant(m_lDevices[idx.row()]->id());
   return QVariant();
}

///The number of codec
int VideoDeviceModel::rowCount( const QModelIndex& par ) const
{
   Q_UNUSED(par)
   return m_lDevices.size();
}

///Items flag
Qt::ItemFlags VideoDeviceModel::flags( const QModelIndex& idx ) const
{
   if (idx.column() == 0)
      return QAbstractItemModel::flags(idx) | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   return QAbstractItemModel::flags(idx);
}

///Set the codec data (codecs can't be added or removed that way)
bool VideoDeviceModel::setData(const QModelIndex& idx, const QVariant &value, int role)
{
   Q_UNUSED(idx)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

///Constructor
VideoDeviceModel::VideoDeviceModel() : QAbstractListModel(QCoreApplication::instance()),
m_pResolutionModel(nullptr),m_pChannelModel(nullptr),m_pRateModel(nullptr)
{
   connect(this             ,SIGNAL(changed()) , channelModel   () , SLOT(reload()));
   connect(channelModel()   ,SIGNAL(changed()) , resolutionModel() , SLOT(reload()));
   connect(resolutionModel(),SIGNAL(changed()) , rateModel      () , SLOT(reload()));
   m_spInstance = this;
   reload();
   channelModel   ()->reload();
   rateModel      ()->reload();
   resolutionModel()->reload();
}

///Destructor
VideoDeviceModel::~VideoDeviceModel()
{
   while (m_lDevices.size()) {
      VideoDevice* c = m_lDevices[0];
      m_lDevices.removeAt(0);
      delete c;
   }
}

///Save the current model over dbus
void VideoDeviceModel::setActive(const QModelIndex& idx)
{
   if (idx.isValid()) {
      VideoManagerInterface& interface = DBus::VideoManager::instance();
      interface.setActiveDevice(m_lDevices[idx.row()]->id());
      emit changed();
      emit currentIndexChanged(idx.row());
   }
}

///Convenience
void VideoDeviceModel::setActive(const int idx)
{
   setActive(index(idx,0,QModelIndex()));
}


void VideoDeviceModel::setActive(const VideoDevice* device)
{
   VideoManagerInterface& interface = DBus::VideoManager::instance();

   interface.setActiveDevice(device?device->id():VideoDevice::NONE);
   emit changed();
   const int idx = m_lDevices.indexOf((VideoDevice*)device);
   emit currentIndexChanged(idx);
}

void VideoDeviceModel::reload()
{
   QHash<QString,VideoDevice*> devicesHash;
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   const QStringList deviceList = interface.getDeviceList();
   if (deviceList.size() == m_hDevices.size()) {
      m_lDevices = m_hDevices.values();
   }

   foreach(const QString& deviceName,deviceList) {
      if (!m_hDevices[deviceName]) {
         devicesHash[deviceName] = new VideoDevice(deviceName);
      }
      else {
         devicesHash[deviceName] = m_hDevices[deviceName];
      }
   }
   foreach(VideoDevice* dev,m_hDevices) {
      if (dev && devicesHash.key(dev).isEmpty()) {
         delete dev;
      }
   }
   m_hDevices.clear();
   m_hDevices = devicesHash;
   m_lDevices = m_hDevices.values();

   emit layoutChanged();
   channelModel   ()->reload();
   setActive(activeDevice());
}


VideoDevice* VideoDeviceModel::activeDevice() const
{
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   const QString deId = interface.getActiveDevice();
   if (!m_lDevices.size())
      const_cast<VideoDeviceModel*>(this)->reload();
   return m_hDevices[deId];
}


int VideoDeviceModel::currentIndex() const
{
   return m_lDevices.indexOf(activeDevice());
}





//Resolution

VideoDeviceResolutionModel* VideoDeviceModel::resolutionModel() const
{
   if (!m_pResolutionModel)
      const_cast<VideoDeviceModel*>(this)->m_pResolutionModel = new VideoDeviceResolutionModel();
   return m_pResolutionModel;
}

///Get data from the model
QVariant VideoDeviceResolutionModel::data( const QModelIndex& idx, int role) const
{
   if(idx.column() == 0 && role == Qt::DisplayRole)
      return QVariant(m_lResolutions[idx.row()]->toString());
   return QVariant();
}

///The number of codec
int VideoDeviceResolutionModel::rowCount( const QModelIndex& par ) const
{
   Q_UNUSED(par)
   return m_lResolutions.size();
}

///Items flag
Qt::ItemFlags VideoDeviceResolutionModel::flags( const QModelIndex& idx ) const
{
   if (idx.column() == 0)
      return QAbstractItemModel::flags(idx) | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   return QAbstractItemModel::flags(idx);
}

///Set the codec data (codecs can't be added or removed that way)
bool VideoDeviceResolutionModel::setData(const QModelIndex& idx, const QVariant &value, int role)
{
   Q_UNUSED(idx)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

///Constructor
VideoDeviceResolutionModel::VideoDeviceResolutionModel() : QAbstractListModel(QCoreApplication::instance())
{
}

///Destructor
VideoDeviceResolutionModel::~VideoDeviceResolutionModel()
{

}


Resolution VideoDeviceResolutionModel::activeResolution() const
{
   return VideoDeviceModel::instance()->activeDevice()->resolution();
}

///Save the current model over dbus
void VideoDeviceResolutionModel::setActive(const QModelIndex& idx)
{
   if (idx.isValid()) {
      QSize s1(*m_lResolutions[idx.row()]),s2(VideoDeviceModel::instance()->activeDevice()->resolution());
      if (s1 == s2)
         return;
      VideoDeviceModel::instance()->activeDevice()->setResolution(m_lResolutions[idx.row()]->toString());
      emit changed();
      emit currentIndexChanged(idx.row());
   }
   else
      qDebug() << "INVALID RESOLUTION INDEX" << idx.row();
}

///Convenience
void VideoDeviceResolutionModel::setActive(const int idx)
{
   setActive(index(idx,0,QModelIndex()));
}

void VideoDeviceResolutionModel::reload()
{
   QHash<QString,Resolution*> devicesHash;
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   VideoDevice* active = VideoDeviceModel::instance()->activeDevice();
   if (active) {
      const QStringList deviceList = interface.getDeviceSizeList(active->id(),
                                                               VideoDeviceModel::instance()->rateModel()->activeRate());
      if (deviceList.size() == m_hResolutions.size()) {
         m_lResolutions = m_hResolutions.values();
      }

      foreach(const QString& deviceName,deviceList) {
         if (!m_hResolutions.contains(deviceName)) {
            devicesHash[deviceName] = new Resolution(deviceName);
         }
         else {
            devicesHash[deviceName] = m_hResolutions[deviceName];
         }
      }
      m_hResolutions.clear();
      m_hResolutions = devicesHash;
      m_lResolutions = m_hResolutions.values();
      emit layoutChanged();

      VideoDeviceModel::instance()->rateModel()->reload();

      emit changed();
      emit currentIndexChanged(m_lResolutions.indexOf(m_hResolutions[activeResolution().toString()]));
   }
   else {
      m_hResolutions.clear();
      emit layoutChanged();
   }
}


int VideoDeviceResolutionModel::currentIndex() const
{
   const Resolution& res = activeResolution();
   for (int i=0;i<m_lResolutions.size();i++) {
      Resolution* availableRes  = m_lResolutions[i];
      if (res.width() == availableRes->width() && res.height() == availableRes->height()) {
         return i;
      }
   }
   qWarning() << "Invalid resolution";
   return -1;
}





//Camera


VideoDeviceChannelModel* VideoDeviceModel::channelModel() const
{
   if (!m_pChannelModel)
      const_cast<VideoDeviceModel*>(this)->m_pChannelModel = new VideoDeviceChannelModel();
   return m_pChannelModel;
}


QString VideoDeviceChannelModel::activeChannel() const
{
   return VideoDeviceModel::instance()->activeDevice()->channel();
}

///Get data from the model
QVariant VideoDeviceChannelModel::data( const QModelIndex& idx, int role) const
{
   if(idx.column() == 0 && role == Qt::DisplayRole)
      return QVariant(m_lChannels[idx.row()]);
   return QVariant();
}

///The number of codec
int VideoDeviceChannelModel::rowCount( const QModelIndex& par ) const
{
   Q_UNUSED(par)
   return m_lChannels.size();
}

///Items flag
Qt::ItemFlags VideoDeviceChannelModel::flags( const QModelIndex& idx ) const
{
   if (idx.column() == 0)
      return QAbstractItemModel::flags(idx) | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   return QAbstractItemModel::flags(idx);
}

///Set the codec data (codecs can't be added or removed that way)
bool VideoDeviceChannelModel::setData(const QModelIndex& idx, const QVariant &value, int role)
{
   Q_UNUSED(idx)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

///Constructor
VideoDeviceChannelModel::VideoDeviceChannelModel() : QAbstractListModel(QCoreApplication::instance())
{
}

///Destructor
VideoDeviceChannelModel::~VideoDeviceChannelModel()
{
}

///Save the current model over dbus
void VideoDeviceChannelModel::setActive(const QModelIndex& idx)
{
   if (idx.isValid()) {
      VideoDeviceModel::instance()->activeDevice()->setChannel(m_lChannels[idx.row()]);
      emit changed();
      emit currentIndexChanged(idx.row());
   }
}

///Convenience
void VideoDeviceChannelModel::setActive(const int idx)
{
   setActive(index(idx,0,QModelIndex()));
}

void VideoDeviceChannelModel::reload()
{
   QHash<QString,QString> devicesHash;
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   VideoDevice* active = VideoDeviceModel::instance()->activeDevice();
   if (active) {
      const QStringList deviceList = interface.getDeviceChannelList(active->id());

      m_lChannels = deviceList;
      emit layoutChanged();

      VideoDeviceModel::instance()->resolutionModel()->reload();

      setActive(m_lChannels.indexOf(activeChannel()));
   }
   else {
      //There is no channel when there is no devices
      m_lChannels.clear();
      emit layoutChanged();
   }
}

int VideoDeviceChannelModel::currentIndex() const
{
   return m_lChannels.indexOf(activeChannel());
}




//Rate


VideoDeviceRateModel* VideoDeviceModel::rateModel() const
{
   if (!m_pRateModel)
      const_cast<VideoDeviceModel*>(this)->m_pRateModel = new VideoDeviceRateModel();
   return m_pRateModel;
}

QString VideoDeviceRateModel::activeRate() const
{
   return VideoDeviceModel::instance()->activeDevice()->rate();
}

///Get data from the model
QVariant VideoDeviceRateModel::data( const QModelIndex& idx, int role) const
{
   if(idx.isValid() && idx.column() == 0 && role == Qt::DisplayRole && idx.row() < m_lRates.size())
      return QVariant(m_lRates[idx.row()]);
   return QVariant();
}

///The number of codec
int VideoDeviceRateModel::rowCount( const QModelIndex& par ) const
{
   Q_UNUSED(par)
   return m_lRates.size();
}

///Items flag
Qt::ItemFlags VideoDeviceRateModel::flags( const QModelIndex& idx ) const
{
   if (idx.column() == 0)
      return QAbstractItemModel::flags(idx) | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   return QAbstractItemModel::flags(idx);
}

///Set the codec data (codecs can't be added or removed that way)
bool VideoDeviceRateModel::setData(const QModelIndex& idx, const QVariant &value, int role)
{
   Q_UNUSED(idx)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

///Constructor
VideoDeviceRateModel::VideoDeviceRateModel() : QAbstractListModel(QCoreApplication::instance())
{
}

///Destructor
VideoDeviceRateModel::~VideoDeviceRateModel()
{
}

///Save the current model over dbus
void VideoDeviceRateModel::setActive(const QModelIndex& idx)
{
   if (idx.isValid()) {
      VideoDeviceModel::instance()->activeDevice()->setRate(m_lRates[idx.row()]);
      emit changed();
      emit currentIndexChanged(idx.row());
   }
   else
      qDebug() << "INVALID RATE INDEX" << idx.row() << rowCount();
}

///Convenience
void VideoDeviceRateModel::setActive(const int idx)
{
   setActive(index(idx,0,QModelIndex()));
}

void VideoDeviceRateModel::reload()
{
   QHash<QString,VideoDevice*> devicesHash;
   VideoManagerInterface& interface = DBus::VideoManager::instance();
   VideoDevice* active = VideoDeviceModel::instance()->activeDevice();
   if (active) {
      const QStringList deviceList = interface.getDeviceRateList(active->id                                                       (),
         VideoDeviceModel::instance()->channelModel()->activeChannel      (),
         VideoDeviceModel::instance()->resolutionModel()->activeResolution().toString()
      );
      m_lRates = deviceList;
      emit layoutChanged();
      const int currentRate = m_lRates.indexOf(activeRate());
      setActive(currentRate==-1?0:currentRate);
   }
   else {
      m_lRates.clear();
      emit layoutChanged();
   }
}

int VideoDeviceRateModel::currentIndex() const
{
   return m_lRates.indexOf(activeRate());
}


// Extended Device list


ExtendedVideoDeviceModel* ExtendedVideoDeviceModel::m_spInstance = nullptr;

ExtendedVideoDeviceModel::ExtendedVideoDeviceModel() : QAbstractListModel(QCoreApplication::instance())
{
   
}

ExtendedVideoDeviceModel* ExtendedVideoDeviceModel::instance()
{
   if (!m_spInstance)
      m_spInstance = new ExtendedVideoDeviceModel();
   return m_spInstance;
}

QVariant ExtendedVideoDeviceModel::data( const QModelIndex& index, int role ) const
{
   switch (index.row()) {
      case ExtendedDeviceList::NONE:
         switch(role) {
            case Qt::DisplayRole:
               return "NONE";
         };
         break;
      case ExtendedDeviceList::SCREEN:
         switch(role) {
            case Qt::DisplayRole:
               return "SCREEN";
         };
         break;
      case ExtendedDeviceList::FILE:
         switch(role) {
            case Qt::DisplayRole:
               return "FILE";
         };
         break;
      default:
         return VideoDeviceModel::instance()->data(VideoDeviceModel::instance()->index(index.row()-ExtendedDeviceList::__COUNT,0),role);
   };
   return QVariant();
}

int ExtendedVideoDeviceModel::rowCount( const QModelIndex& parent ) const
{
   Q_UNUSED(parent)
   return VideoDeviceModel::instance()->rowCount() + ExtendedDeviceList::__COUNT;
}

Qt::ItemFlags ExtendedVideoDeviceModel::flags( const QModelIndex& idx ) const
{
   switch (idx.row()) {
      case ExtendedDeviceList::NONE  :
      case ExtendedDeviceList::SCREEN:
      case ExtendedDeviceList::FILE  :
         return QAbstractItemModel::flags(idx) | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
         break;
      default:
         return VideoDeviceModel::instance()->flags(VideoDeviceModel::instance()->index(idx.row()-ExtendedDeviceList::__COUNT,0));
   };
}

bool ExtendedVideoDeviceModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

void ExtendedVideoDeviceModel::switchTo(const QModelIndex& idx)
{
   switchTo(idx.row());
}

///This model is designed for "live" switching rather than configuration
void ExtendedVideoDeviceModel::switchTo(const int idx)
{
   switch (idx) {
      case ExtendedDeviceList::NONE:
         DBus::VideoManager::instance().switchInput("");
         break;
      case ExtendedDeviceList::SCREEN:
         DBus::VideoManager::instance().switchInput("display://0:100x100");
         break;
      case ExtendedDeviceList::FILE:
         DBus::VideoManager::instance().switchInput(!m_CurrentFile.isEmpty()?"file://"+m_CurrentFile.path():"");
         break;
      default:
         DBus::VideoManager::instance().switchInput("v4l2://"+
            VideoDeviceModel::instance()->index(idx-ExtendedDeviceList::__COUNT,0).data(Qt::DisplayRole).toString());
         break;
   };
}

void ExtendedVideoDeviceModel::setFile(const QUrl& url)
{
   m_CurrentFile = url;
}

void ExtendedVideoDeviceModel::setDisplay(int index, Resolution res, QPoint point)
{
   m_Display.index  = index ;
   m_Display.res    = res   ;
   m_Display.point  = point ;
}
