/****************************************************************************
 *   Copyright (C) 2016 by Emmanuel Lepage-Vallee                           *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                    *
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
#include "contextlistview.h"

// Qt
#include <QtGui/QContextMenuEvent>
#include <QtCore/QDebug>

// Ring
#include <itemdataroles.h>

ContextListView::ContextListView(QWidget* parent) : QListView(parent)
{
}

void ContextListView::contextMenuEvent(QContextMenuEvent* e)
{
   const QModelIndex index = indexAt(e->pos());

   if (index.isValid()) {
      const QVariant objTv = index.data(static_cast<int>(Ring::Role::ObjectType));

      //Be sure the model support the UAM abstraction
      if (!objTv.canConvert<Ring::ObjectType>()) {
         qWarning() << "Cannot determine object type";
         return;
      }

      const auto objT = qvariant_cast<Ring::ObjectType>(objTv);

      // Deduce each kind of objects from the relations
      switch(objT) {
         case Ring::ObjectType::Person         :
         case Ring::ObjectType::ContactMethod  :
         case Ring::ObjectType::Call           :
         case Ring::ObjectType::Media          :
         case Ring::ObjectType::Certificate    :
         case Ring::ObjectType::ContactRequest :
         case Ring::ObjectType::Event          :
         case Ring::ObjectType::Individual     :
         case Ring::ObjectType::COUNT__        :
            break;
      }
   }
}

// kate: space-indent on; indent-width 3; replace-tabs on;
