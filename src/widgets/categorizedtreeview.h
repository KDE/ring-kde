/***************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                         *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>         *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
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
#ifndef CATEGORIZEDTREEVIEW_H
#define CATEGORIZEDTREEVIEW_H

#include <QtGui/QTreeWidget>
#include <QtCore/QDebug>

class QTreeWidgetItem;
class QStyledItemDelegate;

///CategorizedTreeView: A better looking widget than the plain QListWidget
class CategorizedTreeView : public QTreeView
{
  Q_OBJECT

  public:
   enum ViewType {
      Other,
      Contact,
      History,
      BookMark,
      Call
   };
   explicit CategorizedTreeView(QWidget *parent = nullptr);
   void setDelegate(QStyledItemDelegate* delegate);
   void setViewType(ViewType type) {m_Type = type;}

  protected:
   virtual void contextMenuEvent ( QContextMenuEvent * e );
   virtual void dragLeaveEvent   ( QDragLeaveEvent   * e );
   virtual void dragEnterEvent   ( QDragEnterEvent   * e );
   virtual void dragMoveEvent    ( QDragMoveEvent    * e );
   virtual void drawBranches(QPainter* painter, const QRect& rect, const QModelIndex& index) const;
   virtual void mouseDoubleClickEvent(QMouseEvent* event);
   virtual void startDrag ( Qt::DropActions supportedActions );
   virtual bool edit(const QModelIndex& index, EditTrigger trigger, QEvent* event);

  private:
   QModelIndex m_HoverIdx;
   ViewType m_Type;

  Q_SIGNALS:
   void contextMenuRequest(QModelIndex);
   void itemDoubleClicked(QModelIndex);
};

#endif
