/***************************************************************************
 *   Copyright (C) 2009 by Rafael Fernández López <ereslibre@kde.org>      *
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

#include "categorizedtreeview.h"

//Qt
#include <QtGui/QStyledItemDelegate>
#include <QtGui/QPainter>
#include <QtGui/QContextMenuEvent>
#include <QEvent>

//KDE
#include <klocale.h>
#include <kdebug.h>

///Constructor
CategorizedTreeView::CategorizedTreeView(QWidget *parent)
  : QTreeView(parent)
{
  setHeaderHidden(true);
  setRootIsDecorated(false);
  //setUniformRowHeights(false);
  setIndentation(25);
  setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  setHorizontalScrollMode(QAbstractItemView::ScrollPerItem);
}

///Do not draw branches
void CategorizedTreeView::drawBranches(QPainter* painter, const QRect& rect, const QModelIndex& index) const
{
  Q_UNUSED( painter )
  Q_UNUSED( rect    )
  Q_UNUSED( index   )
}

void CategorizedTreeView::contextMenuEvent ( QContextMenuEvent * e ) {
  QModelIndex index = indexAt(e->pos());
  emit contextMenuRequest(index);
  e->accept();
}

void CategorizedTreeView::setDelegate(QStyledItemDelegate* delegate)
{
   setItemDelegate(delegate);
}