/***************************************************************************
 *   Copyright (C) 2015 by Savoir-Faire Linux                              *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/
#ifndef EXTENDEDPROTOCOLMODEL_H
#define EXTENDEDPROTOCOLMODEL_H

#include <QtCore/QIdentityProxyModel>
#include <QtCore/QItemSelectionModel>

#include <protocolmodel.h>

class ExtendedProtocolModel : public QIdentityProxyModel
{
   Q_OBJECT

public:
   explicit ExtendedProtocolModel(QObject* parent = nullptr);
   virtual ~ExtendedProtocolModel();

   //Model
   virtual QVariant    data     ( const QModelIndex& index, int role = Qt::DisplayRole ) const override;
   virtual int         rowCount ( const QModelIndex& parent = QModelIndex()            ) const override;
   virtual QModelIndex index    ( int row, int column, const QModelIndex& parent       ) const override;

   QItemSelectionModel* selectionModel() const;

private:
   ProtocolModel* m_pSource {new ProtocolModel()};
};

#endif
