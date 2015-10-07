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
#include "toolbuttonmodelview.h"

//Qt
#include <QtWidgets/QAction>
#include <QtCore/QItemSelectionModel>
#include <QtCore/QDebug>

//Ring
#include "menumodelview.h"

ToolButtonModelView::ToolButtonModelView(QWidget* parent) : QToolButton(parent)
{
   setPopupMode(QToolButton::InstantPopup);
}

ToolButtonModelView::ToolButtonModelView(QAbstractItemModel* model, QItemSelectionModel* s, QWidget* parent) :
QToolButton(parent)
{
   setPopupMode(QToolButton::InstantPopup);
   setModel(model,s);
}

void ToolButtonModelView::setModel(QAbstractItemModel* model, QItemSelectionModel* s)
{
   if (QMenu* m = menu()) {
      setMenu(nullptr);
      delete m;
   }

   setMenu(new MenuModelView(model, s, this));

   const auto update = [this,s,model]() {
      QModelIndex currentIndex = s->currentIndex();

      if (!currentIndex.isValid())
         currentIndex = model->index(0,0);

      if (currentIndex.isValid()) {
         setText(currentIndex.data(Qt::DisplayRole).toString());

         const QVariant icn = currentIndex.data(Qt::DecorationRole);

         QIcon icon;
         if (icn.canConvert<QIcon>())
            icon = qvariant_cast<QIcon>(icn);
         else if (icn.canConvert<QPixmap>())
            icon = QIcon(qvariant_cast<QPixmap>(icn));

         setIcon(icon);
      }
   };

   update();
   connect(s, &QItemSelectionModel::currentChanged, update);
}
