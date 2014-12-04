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
#include "devicemodel.h"

namespace Video {
class SourcesModelPrivate
{
public:
   SourcesModelPrivate();

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
}

Video::SourcesModelPrivate::SourcesModelPrivate() : m_CurrentSelection(-1)
{
   
}

Video::SourcesModel* Video::SourcesModel::m_spInstance = nullptr;

Video::SourcesModel::SourcesModel() : QAbstractListModel(QCoreApplication::instance()),
d_ptr(new Video::SourcesModelPrivate())
{
   d_ptr->m_Display.rect = QRect(0,0,0,0);
}

Video::SourcesModel* Video::SourcesModel::instance()
{
   if (!m_spInstance)
      m_spInstance = new Video::SourcesModel();
   return m_spInstance;
}

QVariant Video::SourcesModel::data( const QModelIndex& index, int role ) const
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
         return Video::DeviceModel::instance()->data(Video::DeviceModel::instance()->index(index.row()-ExtendedDeviceList::__COUNT,0),role);
   };
   return QVariant();
}

int Video::SourcesModel::rowCount( const QModelIndex& parent ) const
{
   Q_UNUSED(parent)
   return Video::DeviceModel::instance()->rowCount() + ExtendedDeviceList::__COUNT;
}

Qt::ItemFlags Video::SourcesModel::flags( const QModelIndex& idx ) const
{
   switch (idx.row()) {
      case ExtendedDeviceList::NONE  :
      case ExtendedDeviceList::SCREEN:
      case ExtendedDeviceList::FILE  :
         return QAbstractItemModel::flags(idx) | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
         break;
      default:
         return Video::DeviceModel::instance()->flags(Video::DeviceModel::instance()->index(idx.row()-ExtendedDeviceList::__COUNT,0));
   };
}

bool Video::SourcesModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

void Video::SourcesModel::switchTo(const QModelIndex& idx)
{
   switchTo(idx.row());
}

///This model is designed for "live" switching rather than configuration
void Video::SourcesModel::switchTo(const int idx)
{
   switch (idx) {
      case ExtendedDeviceList::NONE:
         DBus::VideoManager::instance().switchInput(Video::SourcesModelPrivate::ProtocolPrefix::NONE);
         break;
      case ExtendedDeviceList::SCREEN:
         DBus::VideoManager::instance().switchInput( QString(Video::SourcesModelPrivate::ProtocolPrefix::DISPLAY)+QString(":%1 %2x%3")
            .arg(d_ptr->m_Display.index)
            .arg(d_ptr->m_Display.rect.width())
            .arg(d_ptr->m_Display.rect.height()));
         break;
      case ExtendedDeviceList::FILE:
         DBus::VideoManager::instance().switchInput(
            !d_ptr->m_CurrentFile.isEmpty()?+Video::SourcesModelPrivate::ProtocolPrefix::FILE+d_ptr->m_CurrentFile.path():Video::SourcesModelPrivate::ProtocolPrefix::NONE
         );
         break;
      default:
         DBus::VideoManager::instance().switchInput(Video::SourcesModelPrivate::ProtocolPrefix::V4L2 +
            Video::DeviceModel::instance()->index(idx-ExtendedDeviceList::__COUNT,0).data(Qt::DisplayRole).toString());
         break;
   };
   d_ptr->m_CurrentSelection = (ExtendedDeviceList) idx;
}

void Video::SourcesModel::switchTo(Video::Device* device)
{
   DBus::VideoManager::instance().switchInput(Video::SourcesModelPrivate::ProtocolPrefix::V4L2 + device->id());
}

Video::Device* Video::SourcesModel::deviceAt(const QModelIndex& idx) const
{
   if (!idx.isValid()) return nullptr;
   switch (idx.row()) {
      case ExtendedDeviceList::NONE:
      case ExtendedDeviceList::SCREEN:
      case ExtendedDeviceList::FILE:
         return nullptr;
      default:
         return Video::DeviceModel::instance()->devices()[idx.row()-ExtendedDeviceList::__COUNT];
   };
}

int Video::SourcesModel::activeIndex() const
{
   if (d_ptr->m_CurrentSelection == -1) {
      return ExtendedDeviceList::__COUNT + Video::DeviceModel::instance()->activeIndex();
   }
   return d_ptr->m_CurrentSelection;
}

void Video::SourcesModel::setFile(const QUrl& url)
{
   d_ptr->m_CurrentFile = url;
   switchTo(ExtendedDeviceList::FILE);
}

void Video::SourcesModel::setDisplay(int index, QRect rect)
{
   d_ptr->m_Display.index  = index ;
   d_ptr->m_Display.rect   = rect  ;
   switchTo(ExtendedDeviceList::SCREEN);
}
