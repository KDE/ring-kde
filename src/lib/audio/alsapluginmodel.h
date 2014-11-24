/****************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                          *
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
#ifndef ALSAPLUGINMODEL_H
#define ALSAPLUGINMODEL_H

#include <QtCore/QAbstractListModel>

//Qt
#include <QtCore/QStringList>

//SFLPhone
#include "../typedefs.h"

class AlsaPluginModelPrivate;

namespace Audio {

class LIB_EXPORT AlsaPluginModel : public QAbstractListModel {
   Q_OBJECT
public:
   explicit AlsaPluginModel(const QObject* parent);
   virtual ~AlsaPluginModel();

   //Models function
   virtual QVariant      data    ( const QModelIndex& index, int role = Qt::DisplayRole ) const override;
   virtual int           rowCount( const QModelIndex& parent = QModelIndex()            ) const override;
   virtual Qt::ItemFlags flags   ( const QModelIndex& index                             ) const override;
   virtual bool          setData ( const QModelIndex& index, const QVariant &value, int role) override;

   //Getters
   QModelIndex currentPlugin() const;

   //Setters
   void setCurrentPlugin(const QModelIndex& idx);
   void setCurrentPlugin(int idx);

   //Mutator
   void reload();

private:
   QScopedPointer<AlsaPluginModelPrivate> d_ptr;
   Q_DECLARE_PRIVATE(AlsaPluginModel)
};

}

#endif