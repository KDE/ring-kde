/****************************************************************************
 *   Copyright (C) 2015 by Savoir-Faire Linux                               *
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
#ifndef TOOLBARDELEGATE_H
#define TOOLBARDELEGATE_H

#include <QtWidgets/QStyledItemDelegate>

class QWidget;

class ToolbarDelegate : public QStyledItemDelegate
{
   Q_OBJECT
public:
   explicit ToolbarDelegate(QWidget* view);
   virtual ~ToolbarDelegate();

   virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
//    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
   QWidget* m_pParent;
};

#endif //TOOLBARDELEGATE_H
