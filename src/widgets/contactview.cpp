/***************************************************************************
 *   Copyright (C) 2013 Savoir-Faire Linux                                 *
 *   @author: Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
 *                                                                         *
 * This library is free software; you can redistribute it and/or           *
 * modify it under the terms of the GNU Library General Public             *
 * License version 2 as published by the Free Software Foundation.         *
 *                                                                         *
 * This library is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * Library General Public License for more details.                        *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/
#include "contactview.h"

#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>

#include <lib/contactproxymodel.h>
#include <lib/abstractitembackend.h>

ContactView::ContactView(QWidget* parent) : CategorizedTreeView(parent)
{
   
}

QRect ContactView::phoneRect(const QModelIndex& index) const
{
   QRect origin = CategorizedTreeView::visualRect(index);
   const QRect parentRect = visualRect(index.parent());
   origin.setX(4 + 48 + parentRect.x() + 4);
   origin.setWidth(parentRect.width()-48-4);
   return origin;
}

QRect ContactView::visualRect(const QModelIndex& index) const
{
   //Detect if it is a phone number
   if (index.parent().parent().isValid()) {
      return phoneRect(index);
   }
   return CategorizedTreeView::visualRect(index);
}

bool ContactView::viewportEvent( QEvent * event ) {
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wswitch-enum"
   switch (event->type()) {
      case QEvent::HoverEnter:
      case QEvent::HoverLeave:
      case QEvent::HoverMove: {
         const QHoverEvent* he     = static_cast<QHoverEvent*>(event);
         const QModelIndex  oldIdx = indexAt(he->oldPos());
         const QModelIndex  newIdx = indexAt(he->pos());
      } break;
      case QEvent::Drop: {
         const QDropEvent* de      = static_cast<QDropEvent*>(event);
         const QModelIndex newIdx  = indexAt(de->pos());

         //Clear drop state
         cancelHoverState();

         //HACK For some reasons, the proxy model fail to forward a valid index to dropMimeData()
         //force it
         model()->dropMimeData(de->mimeData(), Qt::TargetMoveAction, newIdx.row(), newIdx.column(), newIdx.parent());
         return true;
      } break;
      default:
         break;
   }
   #pragma GCC diagnostic pop
   return CategorizedTreeView::viewportEvent(event);
}
