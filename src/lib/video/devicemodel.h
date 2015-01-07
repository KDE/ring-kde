/****************************************************************************
 *   Copyright (C) 2014-2015 by Savoir-Faire Linux                          *
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
#ifndef VIDEODEVICEMODEL_H
#define VIDEODEVICEMODEL_H

#include "../typedefs.h"
#include <QtCore/QAbstractListModel>
#include <QtCore/QUrl>
#include <QtCore/QRect>
#include "device.h"


//Qt


namespace Video {

class DeviceModelPrivate;

///Abstract model for managing account video codec list
class LIB_EXPORT DeviceModel : public QAbstractListModel {
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop

public:
   //Private constructor, can only be called by 'Account'
   explicit DeviceModel();
   virtual ~DeviceModel();

   //Model functions
   virtual QVariant      data     ( const QModelIndex& index, int role = Qt::DisplayRole     ) const override;
   virtual int           rowCount ( const QModelIndex& parent = QModelIndex()                ) const override;
   virtual Qt::ItemFlags flags    ( const QModelIndex& index                                 ) const override;
   virtual bool          setData  ( const QModelIndex& index, const QVariant &value, int role)       override;

   static DeviceModel* instance();


   Video::Device* activeDevice() const;
   int activeIndex() const;
   Video::Device* getDevice(const QString& devId) const;
   QList<Video::Device*> devices() const;

private:
   const QScopedPointer<DeviceModelPrivate> d_ptr;
   static DeviceModel* m_spInstance;

public Q_SLOTS:
   void setActive(const QModelIndex& idx);
   void setActive(const int idx);
   void setActive(const Video::Device* device);
   void reload();

Q_SIGNALS:
   void changed();
   void currentIndexChanged(int);

};

}
Q_DECLARE_METATYPE(Video::DeviceModel*)
#endif
