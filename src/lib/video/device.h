/****************************************************************************
 *   Copyright (C) 2012-2015 by Savoir-Faire Linux                          *
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

#include "../typedefs.h"
#include <QtCore/QAbstractListModel>

//Qt
#include <QStringList>
#include <QtCore/QSize>

//SFLPhone
namespace Video {
   class Renderer;
   class Resolution;
   class Rate;
   class Channel;
   class Device;
   class Model;
   class Manager;
   class DeviceModel;
   class ManagerPrivate;
}

class VideoDevicePrivate;

namespace Video {


///Device: V4L devices used to record video for video call
class LIB_EXPORT Device : public QAbstractListModel {
   Q_OBJECT
   friend class Video::Manager;
   friend class Video::ManagerPrivate;
   friend class Video::DeviceModel;
   friend class VideoDevicePrivate;

   //Need to access the PreferenceNames table
   friend class Video::Channel;
   friend class Video::Resolution;
   public:

      //Constants
      constexpr static const char* NONE = "";

      //Model
      virtual QVariant      data     ( const QModelIndex& index, int role = Qt::DisplayRole     ) const override;
      virtual int           rowCount ( const QModelIndex& parent = QModelIndex()                ) const override;
      virtual Qt::ItemFlags flags    ( const QModelIndex& index                                 ) const override;
      virtual bool          setData  ( const QModelIndex& index, const QVariant &value, int role)       override;

      //Getter
      QList<Channel*> channelList      () const;
      Video::Channel* activeChannel    () const;
      const QString   id               () const;
      const QString   name             () const;
      bool  isActive                   () const;

      //Static getter

      //Setter
      bool setActiveChannel(Video::Channel* chan);
      bool setActiveChannel(int idx);

      //Mutator
      void save();

   private:
      //Constructor
      explicit Device(const QString& id);
      virtual ~Device();

      QScopedPointer<VideoDevicePrivate> d_ptr;


   Q_SIGNALS:
      void renderingStarted(Video::Renderer*);
      void renderingStopped(Video::Renderer*);
      void renderStateChanged(bool state);
};

}
#endif
