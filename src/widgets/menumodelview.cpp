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
#include "menumodelview.h"

//Qt
#include <QtCore/QAbstractItemModel>
#include <QtCore/QItemSelectionModel>
#include <QtCore/QDebug>
#include <QtGui/QIcon>
#include <QtGui/QPixmap>

//Recursively build the menu, please note this doesn't use any kind of lazy-loading optimizations
void MenuModelView::drill(QAbstractItemModel* model, QItemSelectionModel* s, const QModelIndex& parent)
{
   const int rc = model->rowCount(parent);

   if (!rc)
      return;

   for (int i=0; i < rc;i++) {
      const QModelIndex idx = model->index(i,0);

      QIcon icon;
      const QVariant icn = idx.data(Qt::DecorationRole);

      if (icn.canConvert<QIcon>())
         icon = qvariant_cast<QIcon>(icn);
      else if (icn.canConvert<QPixmap>())
         icon = QIcon(qvariant_cast<QPixmap>(icn));

      QAction* action = addAction(icon, idx.data(Qt::DisplayRole).toString());

      action->setCheckable(idx.flags() & Qt::ItemIsUserCheckable);
      action->setEnabled(idx.flags() & (Qt::ItemIsEnabled | Qt::ItemIsSelectable));

      QPersistentModelIndex pidx(idx);

      if (idx == s->currentIndex()) {
         action->setChecked(true);

         const QMetaObject::Connection conn = connect(s, &QItemSelectionModel::currentChanged, [this, action, pidx](const QModelIndex& idx) {
            if (idx == pidx)
               emit actionChanged(action);
         });

         //Make sure the slot isn't called after a reload, the action pointer would be invalid
         connect(action, &QObject::destroyed, [conn]() {disconnect(conn);});
      }

      connect(action, &QAction::triggered, [action,pidx,s,this](bool) {
         if (pidx.isValid()) {
            s->setCurrentIndex(pidx, QItemSelectionModel::ClearAndSelect);
         }
      });

      drill(model, s, idx);
   }
}

MenuModelView::MenuModelView(QAbstractItemModel* model, QItemSelectionModel* s, QWidget* parent) : QMenu(parent), m_pModel(model)
{
   drill(model, s);

   //TODO check for dataChanged and rowInserted/removed more efficiently
   const auto reload = [this, model, s]() {
      clear();
      foreach (QAction* a, m_lActions)
         delete a;
      m_lActions.clear();
      drill(model,s);
   };

   connect(model, &QAbstractItemModel::layoutChanged, reload);
   connect(model, &QAbstractItemModel::rowsInserted , reload);
   connect(model, &QAbstractItemModel::rowsRemoved  , reload);
   connect(model, &QAbstractItemModel::dataChanged  , reload);
}

MenuModelView::~MenuModelView()
{
   foreach (QAction* a, m_lActions)
      delete a;
   m_lActions.clear();
}
