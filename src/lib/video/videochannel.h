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

class VideoResolution;
class VideoDevice;

class VideoChannelPrivate;

///@typedef VideoChannel A channel available in a Device
class LIB_EXPORT VideoChannel : public QAbstractListModel
{
   //Only VideoDevice can add resolutions
   friend class VideoDevice;
public:
   QString name() const;
   VideoResolution* activeResolution();
   QList<VideoResolution*> validResolutions() const;
   VideoDevice* device() const;
   int relativeIndex();

   bool setActiveResolution(VideoResolution* res);
   bool setActiveResolution(int idx);

   //Model
   virtual QVariant      data     ( const QModelIndex& index, int role = Qt::DisplayRole     ) const override;
   virtual int           rowCount ( const QModelIndex& parent = QModelIndex()                ) const override;
   virtual Qt::ItemFlags flags    ( const QModelIndex& index                                 ) const override;
   virtual bool          setData  ( const QModelIndex& index, const QVariant &value, int role)       override;

private:
   VideoChannel(VideoDevice* dev,const QString& name);
   virtual ~VideoChannel();

   VideoChannelPrivate* d_ptr;
};

#endif
