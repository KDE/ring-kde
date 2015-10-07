/***************************************************************************
 *   Copyright (C) 2015 by Emmanuel Lepage Vallee                          *
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
#ifndef TOOLBUTTONMODELVIEW_H
#define TOOLBUTTONMODELVIEW_H

#include <QtWidgets/QToolButton>

class QAbstractItemModel;
class QItemSelectionModel;

class MenuModelView;

class ToolButtonModelView : public QToolButton
{
   Q_OBJECT

public:
   explicit ToolButtonModelView(QWidget* parent = nullptr);
   ToolButtonModelView(QAbstractItemModel* model, QItemSelectionModel* s, QWidget* parent = nullptr);
   virtual ~ToolButtonModelView() {}

   void setModel(QAbstractItemModel* model, QItemSelectionModel* s);

};

#endif
