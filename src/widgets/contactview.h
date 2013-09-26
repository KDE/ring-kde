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
#ifndef CONTACTVIEW_H
#define CONTACTVIEW_H

#include "categorizedtreeview.h"

class ContactView : public CategorizedTreeView
{
   Q_OBJECT
public:
   explicit ContactView(QWidget* parent = nullptr);
   virtual QRect visualRect(const QModelIndex& index) const;

protected:
   virtual bool viewportEvent ( QEvent * event );
//    virtual void drawRow( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
private:
   QRect phoneRect(const QModelIndex & index) const;
};

#endif
