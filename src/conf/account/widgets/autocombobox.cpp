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
#include "autocombobox.h"

#include <QtCore/QItemSelectionModel>
#include <QtCore/QDebug>
#include <QtCore/QAbstractProxyModel>

AutoComboBox::AutoComboBox(QWidget* parent) : QComboBox(parent), m_pSelectionModel(nullptr)
{
   connect(this,SIGNAL(currentIndexChanged(int)),this,SLOT(slotComboBoxSelectionChanged(int)));
}

AutoComboBox::~AutoComboBox()
{
}

void AutoComboBox::bindToModel(QAbstractItemModel* m, QItemSelectionModel* s)
{
   if (m_pSelectionModel)
      disconnect(m_pSelectionModel,&QItemSelectionModel::currentChanged,this,&AutoComboBox::slotModelSelectionChanged);

   blockSignals(true);
   setModel(m);
   m_pSelectionModel = s;
   setCurrentIndex(s->currentIndex().row());
   blockSignals(false);

   connect(m_pSelectionModel,&QItemSelectionModel::currentChanged,this,&AutoComboBox::slotModelSelectionChanged);
}

void AutoComboBox::slotComboBoxSelectionChanged(int idx)
{
   if (!m_pSelectionModel)
      return;

   const QModelIndex i = model()->index(idx,0,QModelIndex());

   if (i.isValid() && m_pSelectionModel)
      m_pSelectionModel->setCurrentIndex(i,QItemSelectionModel::ClearAndSelect);
}

void AutoComboBox::slotModelSelectionChanged(const QModelIndex& idx)
{
   if (!idx.isValid())
      return;

   setCurrentIndex(idx.row());
}

QItemSelectionModel* AutoComboBox::selectionModel()
{
   return m_pSelectionModel;
}

// kate: space-indent on; indent-width 3; replace-tabs on;
