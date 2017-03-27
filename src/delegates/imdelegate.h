/***************************************************************************
 *   Copyright (C) 2012-2015 by Savoir-Faire Linux                         *
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

#ifndef IMDELEGATES_H
#define IMDELEGATES_H

#include <QtWidgets/QListView>
#include <QtWidgets/QStyledItemDelegate>

namespace Media {
    class TextRecording;
}

class QAbstractItemModel;
class IMTab;

class ImDelegates : public QStyledItemDelegate
{
   Q_OBJECT
public:
   explicit ImDelegates(IMTab* parent = nullptr);
protected:
   virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
   virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
private:
   IMTab* m_pParent;
};

class IMTab : public QListView
{
   Q_OBJECT
public:
   explicit IMTab(Media::TextRecording* model,QWidget* parent = nullptr);

   Media::TextRecording* textRecording() const;

private Q_SLOTS:
   void scrollBottom();
   void updateScrollBar();

private:
    Media::TextRecording* m_pTextRecording;
};

#endif // IM_MANAGER
