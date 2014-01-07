/****************************************************************************
 *   Copyright (C) 2012-2014 by Savoir-Faire Linux                          *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
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
#ifndef REMOVECONFERENCETIP_H
#define REMOVECONFERENCETIP_H
//Base
#include "../../klib/tip.h"

//Qt
#include <QtGui/QWidget>


//Qt
class QPainter;

///A tip to be passed to the TipLoader
class RemoveConferenceTip : public Tip
{
   Q_OBJECT
public:
   explicit RemoveConferenceTip(QWidget* parent = nullptr);
   virtual ~RemoveConferenceTip();
   virtual QSize reload(const QRect& availableSize,bool force = false);

protected:
   virtual QRect getDecorationRect();
   virtual void  paintDecorations(QPainter& p, const QRect& textRect);
   
private:
   QRect m_DecoRect;
};

#endif
