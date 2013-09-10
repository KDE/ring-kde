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

void ContactView::drawRow(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   if (index.parent().parent().isValid()) {
      //Hacky, but better than the other 2 alternative I tried
      // * repainting "clear" orver the parent drawRow
      // * forcing the delegate to ignore option.rect
      const int ind = indentation();
      const_cast<ContactView*>(this)->setIndentation((ind+48+12)/2);
      CategorizedTreeView::drawRow(painter,option,index);
      const_cast<ContactView*>(this)->setIndentation(ind);
   }
   else {
      CategorizedTreeView::drawRow(painter,option,index);
   }
}