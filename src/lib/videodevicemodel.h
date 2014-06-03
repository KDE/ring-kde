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
#include <QtCore/QUrl>
#include <QtCore/QPoint>
#include "videodevice.h"

//Qt

//SFLPhone
class VideoDevice;

//DEPRECATED
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

   Resolution* activeResolution() const;
   int currentIndex() const;

private:

   //Attrbutes
   QHash<QString,Resolution*>   m_hResolutions  ;
//    QList<Resolution*> m_lResolutions;
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

//DEPRECATED
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

   VideoChannel* activeChannel() const;
   int currentIndex() const;

private:
   //Attrbutes
//    QList<QString> m_lChannels;


public Q_SLOTS:
   void setActive(const QModelIndex& idx);
   void setActive(const int idx);
   void reload();

Q_SIGNALS:
   void changed();
   void currentIndexChanged(int);
};
Q_DECLARE_METATYPE(VideoDeviceChannelModel*)



//TODO qt5, use QIdentityProxyModel
class LIB_EXPORT ExtendedVideoDeviceModel : public QAbstractListModel {
   Q_OBJECT
public:
   enum ExtendedDeviceList {
      NONE   ,
      SCREEN ,
      FILE   ,
      __COUNT
   };
   QVariant      data     ( const QModelIndex& index, int role = Qt::DisplayRole     ) const;
   int           rowCount ( const QModelIndex& parent = QModelIndex()                ) const;
   Qt::ItemFlags flags    ( const QModelIndex& index                                 ) const;
   virtual bool  setData  ( const QModelIndex& index, const QVariant &value, int role)      ;

   //Singleton
   static ExtendedVideoDeviceModel* instance();
private:
   //Constants
   class ProtocolPrefix {
   public:
      constexpr static const char* NONE    = ""          ;
      constexpr static const char* DISPLAY = "display://";
      constexpr static const char* FILE    = "file://"   ;
      constexpr static const char* V4L2    = "v4l2://"   ;
   };

   struct Display {
      Display() : res("0x0"),point(0,0),index(0){}
      Resolution res ; /* Resolution 0x0 for native */
      QPoint point   ; /* Origin, usually x:0,y:0   */
      int index      ; /* X11 display ID, usually 0 */
   };
   explicit ExtendedVideoDeviceModel();
   static ExtendedVideoDeviceModel* m_spInstance;
   QUrl m_CurrentFile;
   Display m_Display;

public Q_SLOTS:
   void switchTo(const QModelIndex& idx);
   void switchTo(const int idx);
   void setFile(const QUrl& url);
   void setDisplay(int index, Resolution res = Resolution("0x0"), QPoint point = QPoint(0,0));
};

//DEPRECATED
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

   VideoRate* activeRate() const;
   int currentIndex() const;

private:
   //Attrbutes
//    QList<QString> m_lRates;

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
   VideoDevice*                m_pDummyDevice    ;

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
