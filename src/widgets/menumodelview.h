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
#ifndef MENUMODELVIEW_H
#define MENUMODELVIEW_H

#include <QtWidgets/QMenu>
#include <QtCore/QAbstractItemModel>
class QItemSelectionModel;
class QAction;

/**
 * Basic bindings between a QMenu and a QAbstractItemModel
 *
 * This can be used along with a selection model or some helper function
 * to share logic between clients without support for QAction
 */
class MenuModelView : public QMenu
{
   Q_OBJECT

public:
   explicit MenuModelView(QAbstractItemModel* model, QItemSelectionModel* s, QWidget* parent = nullptr);
   virtual ~MenuModelView();

private:
   //Attributes
   QAbstractItemModel* m_pModel;
   QList<QAction*> m_lActions;

   //Helpers
   void drill(QAbstractItemModel* model, QItemSelectionModel* s, const QModelIndex& parent = QModelIndex());

Q_SIGNALS:
   void actionChanged(QAction* a);
   void itemClicked(const QModelIndex& idx);
};

#endif