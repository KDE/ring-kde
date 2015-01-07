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
#ifndef VIDEOSOURCESMODEL_H
#define VIDEOSOURCESMODEL_H
#include <QtCore/QAbstractListModel>
#include <QtCore/QRect>
#include "../typedefs.h"

//SFLPhone

namespace Video {

class Device;
class SourcesModelPrivate;

//TODO qt5, use QIdentityProxyModel
class LIB_EXPORT SourcesModel : public QAbstractListModel {
   Q_OBJECT
public:
   enum ExtendedDeviceList {
      NONE   ,
      SCREEN ,
      FILE   ,
      __COUNT
   };
   virtual QVariant      data     ( const QModelIndex& index, int role = Qt::DisplayRole     ) const override;
   virtual int           rowCount ( const QModelIndex& parent = QModelIndex()                ) const override;
   virtual Qt::ItemFlags flags    ( const QModelIndex& index                                 ) const override;
   virtual bool          setData  ( const QModelIndex& index, const QVariant &value, int role)       override;

   Device* deviceAt(const QModelIndex& idx) const;

   int activeIndex() const;

   //Singleton
   static Video::SourcesModel* instance();
private:
   explicit SourcesModel();

   Video::SourcesModelPrivate* d_ptr;
   static Video::SourcesModel* m_spInstance;

public Q_SLOTS:
   void switchTo(const QModelIndex& idx);
   void switchTo(const int idx);
   void switchTo(Video::Device* device);
   void setFile(const QUrl& url);
   void setDisplay(int index, QRect rect = QRect(0,0,0,0));
};

}

#endif