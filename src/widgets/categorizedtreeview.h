/***************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                         *
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
#ifndef CATEGORIZEDTREEVIEW_H
#define CATEGORIZEDTREEVIEW_H

#include <QtGui/QTreeWidget>
#include <QtCore/QDebug>

class QStyledItemDelegate;

//SFLPhone
#include <lib/callmodel.h>

///CategorizedTreeView: A better looking widget than the plain QListWidget
class CategorizedTreeView : public QTreeView
{
  Q_OBJECT

  public:
   friend class ConferenceDelegate;

   enum class ViewType {
      Other,
      Contact,
      History,
      BookMark,
      Call
   };

   explicit CategorizedTreeView(QWidget *parent = nullptr);
   virtual ~CategorizedTreeView();
   void setDelegate(QStyledItemDelegate* delegate);
   void setViewType(ViewType type) {m_Type = type;}

   void setHoverState(const QModelIndex& idx);
   
   virtual void setModel ( QAbstractItemModel * model );

  protected:
   virtual void contextMenuEvent ( QContextMenuEvent * e );
   virtual void dragLeaveEvent   ( QDragLeaveEvent   * e );
   virtual void dragEnterEvent   ( QDragEnterEvent   * e );
   virtual void dragMoveEvent    ( QDragMoveEvent    * e );
   virtual void dropEvent        ( QDropEvent        * e );
   virtual void drawBranches(QPainter* painter, const QRect& rect, const QModelIndex& index) const;
   virtual void mouseDoubleClickEvent(QMouseEvent* event);
   virtual void startDrag ( Qt::DropActions supportedActions );
   virtual bool edit(const QModelIndex& index, EditTrigger trigger, QEvent* event);

   QModelIndex m_HoverIdx;

  private:
   ViewType m_Type;
   bool     m_InitSignals;

   //Helper
   static CallModel::DropPayloadType payloadType(const QMimeData* data);
   void initSignals();

  Q_SIGNALS:
   void contextMenuRequest(QModelIndex);
   void itemDoubleClicked(QModelIndex);

public Q_SLOTS:
   void cancelHoverState();
   void setDirty(const QRect &rect);

private Q_SLOTS:
   void slotExpandInserted(const QModelIndex& parentIdx,int start, int end);
};

#endif
