/***************************************************************************
 *   Copyright (C) 2015 by Savoir-Faire Linux                              *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
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
#ifndef AUTOCOMBOBOX_H
#define AUTOCOMBOBOX_H

#include <QtWidgets/QComboBox>

#include <QtCore/QAbstractItemModel>

class QItemSelectionModel;

class AutoComboBox : public QComboBox
{
   Q_OBJECT

public:
   AutoComboBox(QWidget* parent = nullptr);
   virtual ~AutoComboBox();

   void bindToModel(QAbstractItemModel* m, QItemSelectionModel* s);

   QItemSelectionModel* selectionModel();

private:
   QItemSelectionModel* m_pSelectionModel;

private Q_SLOTS:
   void slotComboBoxSelectionChanged(int idx);
   void slotModelSelectionChanged(const QModelIndex& idx);
};

#endif
