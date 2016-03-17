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
#include "extendedprotocolmodel.h"

#include <account.h>

ExtendedProtocolModel::ExtendedProtocolModel(QObject* parent) : QIdentityProxyModel(parent)
{
   setSourceModel(m_pSource);
}

ExtendedProtocolModel::~ExtendedProtocolModel()
{
   delete m_pSource;
}

QItemSelectionModel* ExtendedProtocolModel::selectionModel() const
{
   return m_pSource->selectionModel();
}

QVariant ExtendedProtocolModel::data( const QModelIndex& index, int role ) const
{
   if (index.isValid() && index.row() == static_cast<int>(Account::Protocol::COUNT__) && role == Qt::DisplayRole)
      return tr("Profile");
   else
      return m_pSource->data(mapToSource(index), role);
}

int ExtendedProtocolModel::rowCount( const QModelIndex& parent ) const
{
   return parent.isValid() ? 0 : m_pSource->rowCount() + 1;
}

QModelIndex ExtendedProtocolModel::index( int row, int column, const QModelIndex& parent) const
{
   if ( row == static_cast<int>(Account::Protocol::COUNT__))
      return createIndex(row, column, static_cast<int>(Account::Protocol::COUNT__));
   else
      return mapFromSource(m_pSource->index(row, column, mapToSource(parent)));
}
