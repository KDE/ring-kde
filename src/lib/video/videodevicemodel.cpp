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
m_pResolutionModel(nullptr),m_pChannelModel(nullptr),m_pRateModel(nullptr),
m_pDummyDevice(nullptr),m_pActiveDevice(nullptr)
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
      m_pActiveDevice = m_lDevices[idx.row()];
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
   m_pActiveDevice = const_cast<VideoDevice*>(device);
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
   if (!m_pActiveDevice) {
      VideoManagerInterface& interface = DBus::VideoManager::instance();
      const QString deId = interface.getActiveDevice();
      if (!m_lDevices.size())
         const_cast<VideoDeviceModel*>(this)->reload();
      VideoDevice* dev =  m_hDevices[deId];

      //Handling null everywhere is too long, better create a dummy device and
      //log the event
      if (!dev) {
         if (!deId.isEmpty())
            qWarning() << "Requested unknown device" << deId;
         if (!m_pDummyDevice)
            const_cast<VideoDeviceModel*>(this)->m_pDummyDevice = new VideoDevice("None");
         return m_pDummyDevice;
      }
      const_cast<VideoDeviceModel*>(this)->m_pActiveDevice = dev;
   }
   return m_pActiveDevice;
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
      return QVariant(VideoDeviceModel::instance()->activeDevice()->activeChannel()->validResolutions()[idx.row()]->name());
   return QVariant();
}

///The number of codec
int VideoDeviceResolutionModel::rowCount( const QModelIndex& par ) const
{
   Q_UNUSED(par)
   return VideoDeviceModel::instance()->activeDevice()->activeChannel()->validResolutions().size();
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


Resolution* VideoDeviceResolutionModel::activeResolution() const
{
   return VideoDeviceModel::instance()->activeDevice()->activeChannel()->activeResolution();
}

///Save the current model over dbus
void VideoDeviceResolutionModel::setActive(const QModelIndex& idx)
{
   if (idx.isValid()) {
      Resolution* r = VideoDeviceModel::instance()->activeDevice()->activeChannel()->validResolutions()[idx.row()];
      QSize s1(*r),s2(*VideoDeviceModel::instance()->activeDevice()->activeChannel()->activeResolution());
      if (s1 == s2)
         return;
      VideoDeviceModel::instance()->activeDevice()->activeChannel()->setActiveResolution(r);
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
   VideoDevice* active = VideoDeviceModel::instance()->activeDevice();
   if (active) {

      emit layoutChanged();

      VideoDeviceModel::instance()->rateModel()->reload();

      emit changed();
      emit currentIndexChanged(activeResolution()->index());
   }
   else {
      m_hResolutions.clear();
      emit layoutChanged();
   }
}


int VideoDeviceResolutionModel::currentIndex() const
{
   return VideoDeviceModel::instance()->activeDevice()->activeChannel()->activeResolution()->index();
}





//Camera


VideoDeviceChannelModel* VideoDeviceModel::channelModel() const
{
   if (!m_pChannelModel)
      const_cast<VideoDeviceModel*>(this)->m_pChannelModel = new VideoDeviceChannelModel();
   return m_pChannelModel;
}


VideoChannel* VideoDeviceChannelModel::activeChannel() const
{
   return VideoDeviceModel::instance()->activeDevice()->activeChannel();
}

///Get data from the model
QVariant VideoDeviceChannelModel::data( const QModelIndex& idx, int role) const
{
   if(idx.column() == 0 && role == Qt::DisplayRole)
      return QVariant(activeChannel()->name());
   return QVariant();
}

///The number of codec
int VideoDeviceChannelModel::rowCount( const QModelIndex& par ) const
{
   Q_UNUSED(par)
   return VideoDeviceModel::instance()->activeDevice()->channelList().size();
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
      VideoChannel* c = VideoDeviceModel::instance()->activeDevice()->channelList()[idx.row()];
      VideoDeviceModel::instance()->activeDevice()->setActiveChannel(c);
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
   VideoDevice* active = VideoDeviceModel::instance()->activeDevice();
   if (active) {

      emit layoutChanged();

      VideoDeviceModel::instance()->resolutionModel()->reload();

      setActive(activeChannel()->index());
   }
   else {
      emit layoutChanged();
   }
}

int VideoDeviceChannelModel::currentIndex() const
{
   return activeChannel()->index();
}




//Rate


VideoDeviceRateModel* VideoDeviceModel::rateModel() const
{
   if (!m_pRateModel)
      const_cast<VideoDeviceModel*>(this)->m_pRateModel = new VideoDeviceRateModel();
   return m_pRateModel;
}

VideoRate* VideoDeviceRateModel::activeRate() const
{
   return VideoDeviceModel::instance()->activeDevice()->activeChannel()->activeResolution()->activeRate();
}

///Get data from the model
QVariant VideoDeviceRateModel::data( const QModelIndex& idx, int role) const
{
   if(idx.isValid() && idx.column() == 0 && role == Qt::DisplayRole)
      return QVariant(VideoDeviceModel::instance()->activeDevice()->activeChannel()->activeResolution()->validRates()[idx.row()]->name());
   return QVariant();
}

///The number of codec
int VideoDeviceRateModel::rowCount( const QModelIndex& par ) const
{
   Q_UNUSED(par)
   return VideoDeviceModel::instance()->activeDevice()->activeChannel()->activeResolution()->validRates().size();
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
      VideoDeviceModel::instance()->activeDevice()->activeChannel()->activeResolution()->setActiveRate(
         VideoDeviceModel::instance()->activeDevice()->activeChannel()->activeResolution()->validRates()[idx.row()]);
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
   VideoDevice* active = VideoDeviceModel::instance()->activeDevice();
   if (active) {
      emit layoutChanged();
      setActive(activeRate()->index());
   }
   else {
      emit layoutChanged();
   }
}

int VideoDeviceRateModel::currentIndex() const
{
   return activeRate()->index();
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
               return tr("NONE");
         };
         break;
      case ExtendedDeviceList::SCREEN:
         switch(role) {
            case Qt::DisplayRole:
               return tr("SCREEN");
         };
         break;
      case ExtendedDeviceList::FILE:
         switch(role) {
            case Qt::DisplayRole:
               return tr("FILE");
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
         DBus::VideoManager::instance().switchInput(ProtocolPrefix::NONE);
         break;
      case ExtendedDeviceList::SCREEN:
         DBus::VideoManager::instance().switchInput( QString(ProtocolPrefix::DISPLAY)+"0:100x100");
         break;
      case ExtendedDeviceList::FILE:
         DBus::VideoManager::instance().switchInput(
            !m_CurrentFile.isEmpty()?+ProtocolPrefix::FILE+m_CurrentFile.path():ProtocolPrefix::NONE
         );
         break;
      default:
         DBus::VideoManager::instance().switchInput(ProtocolPrefix::V4L2 +
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
