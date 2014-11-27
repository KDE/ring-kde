/****************************************************************************
 *   Copyright (C) 2014 by Savoir-Faire Linux                               *
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
#include "sourcesmodel.h"
#include <QtCore/QUrl>
#include <QtCore/QCoreApplication>
#include "../dbus/videomanager.h"
#include "videodevicemodel.h"

class VideoSourcesModelPrivate
{
public:
   VideoSourcesModelPrivate();

   //Constants
   class ProtocolPrefix {
   public:
      constexpr static const char* NONE    = ""          ;
      constexpr static const char* DISPLAY = "display://";
      constexpr static const char* FILE    = "file://"   ;
      constexpr static const char* V4L2    = "v4l2://"   ;
   };

   struct Display {
      Display() : rect(0,0,0,0),index(0){}
      QRect rect;
      int index ; /* X11 display ID, usually 0 */
   };

   QUrl m_CurrentFile;
   Display m_Display;
   int m_CurrentSelection;
};

VideoSourcesModelPrivate::VideoSourcesModelPrivate() : m_CurrentSelection(-1)
{
   
}

VideoSourcesModel* VideoSourcesModel::m_spInstance = nullptr;

VideoSourcesModel::VideoSourcesModel() : QAbstractListModel(QCoreApplication::instance()),
d_ptr(new VideoSourcesModelPrivate())
{
   d_ptr->m_Display.rect = QRect(0,0,0,0);
}

VideoSourcesModel* VideoSourcesModel::instance()
{
   if (!m_spInstance)
      m_spInstance = new VideoSourcesModel();
   return m_spInstance;
}

QVariant VideoSourcesModel::data( const QModelIndex& index, int role ) const
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

int VideoSourcesModel::rowCount( const QModelIndex& parent ) const
{
   Q_UNUSED(parent)
   return VideoDeviceModel::instance()->rowCount() + ExtendedDeviceList::__COUNT;
}

Qt::ItemFlags VideoSourcesModel::flags( const QModelIndex& idx ) const
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

bool VideoSourcesModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

void VideoSourcesModel::switchTo(const QModelIndex& idx)
{
   switchTo(idx.row());
}

///This model is designed for "live" switching rather than configuration
void VideoSourcesModel::switchTo(const int idx)
{
   switch (idx) {
      case ExtendedDeviceList::NONE:
         DBus::VideoManager::instance().switchInput(VideoSourcesModelPrivate::ProtocolPrefix::NONE);
         break;
      case ExtendedDeviceList::SCREEN:
         DBus::VideoManager::instance().switchInput( QString(VideoSourcesModelPrivate::ProtocolPrefix::DISPLAY)+QString(":%1 %2x%3")
            .arg(d_ptr->m_Display.index)
            .arg(d_ptr->m_Display.rect.width())
            .arg(d_ptr->m_Display.rect.height()));
         break;
      case ExtendedDeviceList::FILE:
         DBus::VideoManager::instance().switchInput(
            !d_ptr->m_CurrentFile.isEmpty()?+VideoSourcesModelPrivate::ProtocolPrefix::FILE+d_ptr->m_CurrentFile.path():VideoSourcesModelPrivate::ProtocolPrefix::NONE
         );
         break;
      default:
         DBus::VideoManager::instance().switchInput(VideoSourcesModelPrivate::ProtocolPrefix::V4L2 +
            VideoDeviceModel::instance()->index(idx-ExtendedDeviceList::__COUNT,0).data(Qt::DisplayRole).toString());
         break;
   };
   d_ptr->m_CurrentSelection = (ExtendedDeviceList) idx;
}

void VideoSourcesModel::switchTo(VideoDevice* device)
{
   DBus::VideoManager::instance().switchInput(VideoSourcesModelPrivate::ProtocolPrefix::V4L2 + device->id());
}

VideoDevice* VideoSourcesModel::deviceAt(const QModelIndex& idx) const
{
   if (!idx.isValid()) return nullptr;
   switch (idx.row()) {
      case ExtendedDeviceList::NONE:
      case ExtendedDeviceList::SCREEN:
      case ExtendedDeviceList::FILE:
         return nullptr;
      default:
         return VideoDeviceModel::instance()->devices()[idx.row()-ExtendedDeviceList::__COUNT];
   };
}

int VideoSourcesModel::activeIndex() const
{
   if (d_ptr->m_CurrentSelection == -1) {
      return ExtendedDeviceList::__COUNT + VideoDeviceModel::instance()->activeIndex();
   }
   return d_ptr->m_CurrentSelection;
}

void VideoSourcesModel::setFile(const QUrl& url)
{
   d_ptr->m_CurrentFile = url;
   switchTo(ExtendedDeviceList::FILE);
}

void VideoSourcesModel::setDisplay(int index, QRect rect)
{
   d_ptr->m_Display.index  = index ;
   d_ptr->m_Display.rect   = rect  ;
   switchTo(ExtendedDeviceList::SCREEN);
}
