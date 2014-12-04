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
#ifndef VIDEOCHANNEL_H
#define VIDEOCHANNEL_H

#include "../typedefs.h"
#include <QtCore/QAbstractListModel>

namespace Video {
   class Resolution;
   class Device;
}

class VideoChannelPrivate;

namespace Video {

///@typedef Channel A channel available in a Device
class LIB_EXPORT Channel : public QAbstractListModel
{
   //Only Video::Device can add resolutions
   friend class Video::Device;
public:
   QString name() const;
   Video::Resolution* activeResolution();
   QList<Video::Resolution*> validResolutions() const;
   Video::Device* device() const;
   int relativeIndex();

   bool setActiveResolution(Video::Resolution* res);
   bool setActiveResolution(int idx);

   //Model
   virtual QVariant      data     ( const QModelIndex& index, int role = Qt::DisplayRole     ) const override;
   virtual int           rowCount ( const QModelIndex& parent = QModelIndex()                ) const override;
   virtual Qt::ItemFlags flags    ( const QModelIndex& index                                 ) const override;
   virtual bool          setData  ( const QModelIndex& index, const QVariant &value, int role)       override;

private:
   Channel(Video::Device* dev,const QString& name);
   virtual ~Channel();

   VideoChannelPrivate* d_ptr;
};

}

#endif
