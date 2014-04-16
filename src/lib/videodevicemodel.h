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
#ifndef VIDEODEVICEMODEL_H
#define VIDEODEVICEMODEL_H

#include "typedefs.h"
#include <QtCore/QAbstractListModel>
#include "videodevice.h"

//Qt

//SFLPhone
class VideoDevice;

///Abstract model for managing account video codec list
class LIB_EXPORT VideoDeviceResolutionModel : public QAbstractListModel {
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop

public:
   //Private constructor, can only be called by 'Account'
   explicit VideoDeviceResolutionModel();
   ~VideoDeviceResolutionModel();

   //Model functions
   QVariant      data     ( const QModelIndex& index, int role = Qt::DisplayRole     ) const;
   int           rowCount ( const QModelIndex& parent = QModelIndex()                ) const;
   Qt::ItemFlags flags    ( const QModelIndex& index                                 ) const;
   virtual bool  setData  ( const QModelIndex& index, const QVariant &value, int role)      ;

   Resolution activeResolution() const;
   int currentIndex() const;

private:
   //Attrbutes
   QHash<QString,Resolution*>   m_hResolutions  ;
   QList<Resolution*> m_lResolutions;
   static VideoDeviceResolutionModel* m_spInstance;

public Q_SLOTS:
   void setActive(const QModelIndex& idx);
   void setActive(const int idx);
   void reload();

Q_SIGNALS:
   void changed();
   void currentIndexChanged(int);
};
Q_DECLARE_METATYPE(VideoDeviceResolutionModel*)

///Abstract model for managing account video codec list
class LIB_EXPORT VideoDeviceChannelModel : public QAbstractListModel {
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop

public:
   //Private constructor, can only be called by 'Account'
   explicit VideoDeviceChannelModel();
   ~VideoDeviceChannelModel();

   //Model functions
   QVariant      data     ( const QModelIndex& index, int role = Qt::DisplayRole     ) const;
   int           rowCount ( const QModelIndex& parent = QModelIndex()                ) const;
   Qt::ItemFlags flags    ( const QModelIndex& index                                 ) const;
   virtual bool  setData  ( const QModelIndex& index, const QVariant &value, int role)      ;

   static VideoDeviceChannelModel* instance();

   QString activeChannel() const;
   int currentIndex() const;

private:
   //Attrbutes
   QList<QString> m_lChannels;


public Q_SLOTS:
   void setActive(const QModelIndex& idx);
   void setActive(const int idx);
   void reload();

Q_SIGNALS:
   void changed();
   void currentIndexChanged(int);
};
Q_DECLARE_METATYPE(VideoDeviceChannelModel*)

///Abstract model for managing account video codec list
class LIB_EXPORT VideoDeviceRateModel : public QAbstractListModel {
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop

public:
   //Private constructor, can only be called by 'Account'
   explicit VideoDeviceRateModel();
   ~VideoDeviceRateModel();

   //Model functions
   QVariant      data     ( const QModelIndex& index, int role = Qt::DisplayRole     ) const;
   int           rowCount ( const QModelIndex& parent = QModelIndex()                ) const;
   Qt::ItemFlags flags    ( const QModelIndex& index                                 ) const;
   virtual bool  setData  ( const QModelIndex& index, const QVariant &value, int role)      ;

   QString activeRate() const;
   int currentIndex() const;

private:
   //Attrbutes
   QList<QString> m_lRates;

public Q_SLOTS:
   void setActive(const QModelIndex& idx);
   void setActive(const int idx);
   void reload();

Q_SIGNALS:
   void changed();
   void currentIndexChanged(int);
};
Q_DECLARE_METATYPE(VideoDeviceRateModel*)

///Abstract model for managing account video codec list
class LIB_EXPORT VideoDeviceModel : public QAbstractListModel {
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop

public:
   //Private constructor, can only be called by 'Account'
   explicit VideoDeviceModel();
   ~VideoDeviceModel();

   //Model functions
   QVariant      data     ( const QModelIndex& index, int role = Qt::DisplayRole     ) const;
   int           rowCount ( const QModelIndex& parent = QModelIndex()                ) const;
   Qt::ItemFlags flags    ( const QModelIndex& index                                 ) const;
   virtual bool  setData  ( const QModelIndex& index, const QVariant &value, int role)      ;

   static VideoDeviceModel* instance();

   VideoDeviceRateModel*       rateModel       () const;
   VideoDeviceChannelModel*    channelModel    () const;
   VideoDeviceResolutionModel* resolutionModel () const;

   VideoDevice* activeDevice() const;
   int currentIndex() const;

private:
   //Attrbutes
   QHash<QString,VideoDevice*> m_hDevices        ;
   QList<VideoDevice*>         m_lDevices        ;
   static VideoDeviceModel*    m_spInstance      ;
   VideoDeviceResolutionModel* m_pResolutionModel;
   VideoDeviceChannelModel*    m_pChannelModel   ;
   VideoDeviceRateModel*       m_pRateModel      ;

public Q_SLOTS:
   void setActive(const QModelIndex& idx);
   void setActive(const int idx);
   void setActive(const VideoDevice* device);
   void reload();

Q_SIGNALS:
   void changed();
   void currentIndexChanged(int);

};
Q_DECLARE_METATYPE(VideoDeviceModel*)
#endif
